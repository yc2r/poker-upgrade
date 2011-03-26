/*
* File Name : opp.c

* Purpose :

* Creation Date : 26-02-2011

* Last Modified : Sun 06 Mar 2011 03:05:25 AM EST

* Created By : Weikeng Qin (weikqin@gmail.com)

*/

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <getopt.h>
#include "game.h"
#include "opp.h"
#include "handValue/handValue.h"

	
OppBase flopsBase[MAX_ROUNDS];

/* ========= All the following functions are now only for two people, limit game =================*/
void initNode(struct Node* node, enum NodeType type)
{
	int i;

	node->type = type;
	for(i=0; i<3; i++) {
		node->child[i] = NULL;
		}
#ifdef DEBUG
	node->actionNum = 0;
	for(i=0; i<MAX_NUM_ACTIONS; i++) node->actionList[i] = invalid;
#endif

#ifdef NODEPAR
	node->parent = NULL;
#endif

	if (strength == type) {
		for(i=0; i<3; i++)
			node->data.actionDist[i] = 0;
	}
	else {
		memset(node->data.bucket, 0, sizeof(Bucket)*MAX_NUM_BUCKETS);
	}
}

struct Node* initBase(OppBase *base, bool isPlayFirst)
{
	if (false == isPlayFirst) {
		base->nonDealerRoot  = (struct Node*)malloc(sizeof(struct Node));
		assert(NULL != base->nonDealerRoot);
		initNode(base->nonDealerRoot, strength);
		return base->nonDealerRoot;
		}
	else {
		base->dealerRoot  = (struct Node*)malloc(sizeof(struct Node));
		assert(NULL != base->dealerRoot);
		initNode(base->dealerRoot, prob);
		return base->dealerRoot;
	}
}

void initModel(Game *game)
{
	int i;
	for (i=0; i<MAX_ROUNDS; i++) {
		flopsBase[i].dealerRoot = NULL;
		flopsBase[i].nonDealerRoot = NULL;
	}
}

void updateBase(uint8_t round, uint8_t pos, State *state, bool isFold)
{
	int k;
	int i;
	Action localActionList[MAX_NUM_ACTIONS];
	uint8_t localActionNum;
	struct Node *pt = NULL;
	OppBase *base;

	if (state->numActions[round] == 0) return;
	
	if (round < MAX_ROUNDS) {
		base = &flopsBase[round];
	}
	else {
		fprintf(stderr, "%s:%d\t updateBase: invalid round argument %d\n", __FILE__, __LINE__, round);
		exit(EXIT_FAILURE);
	}
	
	for (i=0; i<state->numActions[round]; i++) {
		localActionList[i].type = state->action[round][i].type;
		localActionList[i].size = 0;
		
		if (localActionList[i].type >= 3) {
			fprintf(stderr, "%s:%d\t updateBase: invalid action %d, #actions %d\n", 
							 __FILE__, __LINE__, localActionList[i].type, state->numActions[round]);
			exit(EXIT_FAILURE);
		}
	}
 	localActionNum = state->numActions[round];

	if (0 == round) {/* preflop */
		if (pos == 0) { /* non-first hand*/
			pt = ((NULL == base->nonDealerRoot) ? initBase(base, false): base->nonDealerRoot);
		}
		else if (pos == 1) { /* first hand*/
			pt = ((NULL == base->dealerRoot) ? initBase(base, true): base->dealerRoot);
		}
		else {
			fprintf(stderr, "%s:%d\t updateBase: invalid player position\n", __FILE__, __LINE__);
			exit(EXIT_FAILURE);
		}
	}
	else {
		if (pos == 0) { /* first player*/
			pt = ((NULL == base->dealerRoot) ? initBase(base, true): base->dealerRoot);
		}
		else if (pos == 1) { /* non-first player*/
			pt = ((NULL == base->nonDealerRoot) ? initBase(base, false): base->nonDealerRoot);
		}
		else {
			fprintf(stderr, "%s:%d\t updateBase: invalid player position\n", __FILE__, __LINE__);
			exit(EXIT_FAILURE);
		}
	}
	
	for(k=0; k<localActionNum; k++) {
		enum ActionType type = localActionList[k].type;
		if (type >= 3) {
			fprintf(stderr, "%s:%d\t type %d abnormal\n", __FILE__, __LINE__, type);
			exit(EXIT_FAILURE);
		}
		else {
			if (pt->type == prob) {(pt->data.actionDist[type])++ ;}
			else {
				if (false == isFold) {
					unsigned idx = computeHandStrength(state, pos);
					assert(idx <= MAX_NUM_BUCKETS);
					(pt->data.bucket[idx-1]) ++;
				}
			}
			if (pt->child[type] == NULL) {
				pt->child[type] = (struct Node*)malloc(sizeof(struct Node));
				assert(NULL != pt->child[type]);
				initNode(pt->child[type], (pt->type == prob)? strength: prob);
				pt->child[type]->parent = pt;
				#ifdef DEBUG
				memcpy(pt->child[type]->actionList, pt->actionList, sizeof(enum ActionType) * (pt->actionNum));
				pt->child[type]->actionNum = pt->actionNum + 1;
				pt->child[type]->actionList[pt->child[type]->actionNum-1] = type;
				#endif
				
				/* in case the last strength node can't get updated since no action is following */
				if((pt->type == prob) && (k == localActionNum-1) && (false == isFold)) {
					unsigned idx = computeHandStrength(state, pos);
					(pt->child[type]->data.bucket[idx-1]) ++;
				}
			}
			pt = pt->child[type];
		}
	}
}

void updateModel(Game *game, uint8_t pos, State *state, bool isFold)
{
	int i;
	for(i=0; i<game->numRounds; i++) {
		updateBase((uint8_t)i, pos, state, isFold);
	}
}
	
struct Node *getNode(Action *act, uint8_t actLen, uint8_t round, uint8_t pos) 
{
	int i = 0;
	struct Node *node = NULL;

	if (round >= MAX_ROUNDS) {
		fprintf(stderr, "%s:%d\t getNode: invalid pos: %d\n", __FILE__, __LINE__, pos); 
		exit(EXIT_FAILURE);
	}
	else if (0 == round) {
		if (0 == pos) { node = flopsBase[0].nonDealerRoot; }
		else if(1 == pos) { node = flopsBase[0].dealerRoot; }
		else { 
			fprintf(stderr, "invalid pos: %d\n", pos); 
			exit(EXIT_FAILURE);
		}
	}
	else {
		if (1 == pos) { node = flopsBase[round].nonDealerRoot; }
		else if(0 == pos) { node = flopsBase[round].dealerRoot; }
		else { 
			fprintf(stderr, "%s:%d\t getNode: invalid pos: %d\n", __FILE__, __LINE__, pos); 
			exit(EXIT_FAILURE);
			}
	}
		
	for(i=0; i<actLen; i++) {
		if (NULL == node) return NULL;

		if (act[i].type >= 3) {
			fprintf(stderr, "%s:%d\t getNode: invalid action: %d\n", __FILE__, __LINE__, act[i].type);
			exit(EXIT_FAILURE);
		}
		else {
			node = node->child[act[i].type];
		}
	}
	return node;
}
			
		
void releaseNode(struct Node *node)
{
	if (NULL == node) return;

	int i;
	for(i=0; i<3; i++) 
		releaseNode(node->child[i]);
	free(node);
}

void releaseBase(OppBase *base)
{
	if (NULL != base) {
		releaseNode(base->nonDealerRoot);
		releaseNode(base->dealerRoot);
	}
}

#ifdef DEBUG
void printNode(struct Node *node, FILE *file)
{
	if (NULL == node) return;

	fprintf(file, "Betting: ");
	fflush(file);
	int i;
	for(i=0; i<node->actionNum; i++) {
		switch(node->actionList[i]) 
			{
			case fold:
				fprintf(file, "f");
				break;
			case call:
				fprintf(file, "c");
				break;
			case raise:
				fprintf(file, "r");
				break;
			default:
				fprintf(stderr, "%s:%d\t printNode: abnormal action %d\n", __FILE__, __LINE__, node->actionList[i]);
				int k;
				fprintf(stderr, "node action(%d):\t", node->actionNum);
				for(k=0; k<node->actionNum; k++) {
					fprintf(stderr, "%d", node->actionList[i]);
				}
				fprintf(stderr, "\n");
				/* print parent node's actionlist */
				struct Node *parent = node->parent;
				fprintf(stderr, "parent node action ");
				if (parent == NULL) {
					fprintf(stderr, "NULL");
					}
				else {
					fprintf(stderr, "(%d):\t", node->parent->actionNum);
					for (k=0; k<node->parent->actionNum; k++) {
						fprintf(stderr, "%d", node->parent->actionList[k]);
					}
				}
				fprintf(stderr, "\n");
				exit(EXIT_FAILURE);
			}
		if (node->actionList[i] == invalid) break;
	}
	fprintf(file, "\t");
	if (node->type == strength) {
		/* debug */
		Bucket total = 0;
		for (i=0; i<MAX_NUM_BUCKETS; i++) {
			fprintf(file, "strength: %d\t", node->data.bucket[i]);
			total += node->data.bucket[i];
		}
		fprintf(file, "\n");
		//assert(total > 0);

	}
	else {
		fprintf(file, "Flop: %d\t Call: %d\t Raise: %d\n", 
				node->data.actionDist[0], 
				node->data.actionDist[1], 
				node->data.actionDist[2]);
			
	}
	for(i=0; i<3; i++) {
		printNode(node->child[i], file);
	}
}

void printBase(OppBase *base)
{
	fprintf(stdout, "As 1st Player\n");
	printNode(base->dealerRoot, stdout);
	fprintf(stdout, "As non-1st Player\n");
	printNode(base->nonDealerRoot, stdout);
	printf("\n");
}

void printModel(Game *game)
{
	int i;
	for(i=0; i<game->numRounds; i++) {
		fprintf(stdout, "%d th round:\n", i);
		printBase(&flopsBase[i]);
	}
}

void printRound(Game *game, uint8_t round, uint8_t pos)
{
	if (round >= game->numRounds) {
		fprintf(stderr, "%s:%d printRound: invalid round\n", __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	if (0 == round) { /*preflop*/
		printNode((pos == 0)?flopsBase[0].nonDealerRoot:flopsBase[0].dealerRoot, stderr);
	}
	else {
		printNode((pos == 1)?flopsBase[round].nonDealerRoot:flopsBase[round].dealerRoot, stderr);
	}
	fprintf(stderr, "\n");
}
#endif
