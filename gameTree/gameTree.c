/*
*	Author: Yikan Chen
*	Purpose: Primary functions of the game tree
*	Date Created:	11/02/26
*/

#include "gameTree.h"
#include "../handValue/handValue.h"
#include "../opp.h"

#define MAXNODE 450		//larger than 243+81+...
#define MAXDEGREE 6		//At most 6 depth tree

void printTree(Gametree* temptree, int numRaise)
{	
	Gametree *nodeindex[MAXNODE];	
	int maxlevel = numRaise + 3;	
	int level = 0;	
	int front = 0;	
	int	rear = 1;	
	int i = 0;	
	int temprear = rear;	
	for (i = 0; i < MAXNODE; i++)		
		nodeindex[i] = NULL;		
	nodeindex[0] = temptree;	
	level++;	
	while (level <=maxlevel)	
	{		
		while (front < rear)		
		{			
			if ((nodeindex[front]->call) && (nodeindex[front]->fold))			
			{				
				nodeindex[temprear] =  nodeindex[front]->fold;				
				nodeindex[temprear+1] =  nodeindex[front]->call;					
				temprear +=2;				
				if (nodeindex[front]->raise)				
				{					
					nodeindex[temprear] =  nodeindex[front]->raise;					
					temprear ++;				
				}			
			}	
			front++;		
		}		
		rear = temprear;		
		level++;	
	}	
	rear--;												//rear will point to the last node in the node list		
	for (i = 0; i <= rear; i++)	
	{		
		fprintf(stderr,"%d\t%f\n",nodeindex[i]->nodeType,nodeindex[i]->data);	
	}	
	fprintf(stderr,"\n Over.");
}

DataType winningProb(Game *game, State *state, int myHandStrength, 
			int opponentID, Action* actionList, int actLen)
{
	uint8_t round = state->round;
	//uint8_t playerID = currentPlayer(game,state);
	//int loopvar;
	//fprintf(stderr,"\nPrint here: actLen(%d)", actLen);
	#if 0
	for (loopvar = 0; loopvar < actLen; loopvar++)
	{
		fprintf(stderr,"%d",actionList[loopvar].type);
	}
		fprintf(stderr,"\n");
	#endif
	int j;
	fprintf(stderr, "getNode actionList(%d):\t", actLen);
	for(j=0; j<actLen; j++) {
			fprintf(stderr, "%d", actionList[j].type);
		}
	fprintf(stderr, "\n");
		
	struct Node* node = getNode(actionList, actLen, round, opponentID);
	if (!node)
	{
		fprintf(stderr, "%s:%d\t Warning: getNode return NULL\t", __FILE__, __LINE__);
		int i;
		fprintf(stderr, "round:%d\t oppID:%d\t actionList(%d):\t", round, opponentID, actLen);
		for(i=0; i<actLen; i++) {
			fprintf(stderr, "%d", actionList[i].type);
			}
		fprintf(stderr, "\n");
		//printModel(game);
		printRound(game, round, opponentID);
		return 0.5;
	}
	DataType winningP = 0;
	int i = 0;
	Bucket total = 0;
	for (; i<MAX_NUM_BUCKETS; i++)
	{
		total += node->data.bucket[i];
	}

#if 0	
	if (total == 0)
	{
		fprintf(stderr, "%s:%d\t Warning: WinningProb return default(total == 0)\n", __FILE__, __LINE__);
		printNode(node, stderr);
		int i;
		fprintf(stderr, "round:%d\t oppID:%d\t actionList(%d):\t", round, opponentID, actLen);
		for(i=0; i<actLen; i++) {
			fprintf(stderr, "%d", actionList[i].type);
			}
		fprintf(stderr, "\n");

		return 0.5;
	}
	
#endif
	if (total < 200) {
		return (double)myHandStrength / 5;
	}

	//fprintf(stderr,"\nBucket: %d %d %d %d %d. hand: %d",node->data.bucket[0],node->data.bucket[1],node->data.bucket[2],node->data.bucket[3],node->data.bucket[4],myHandStrength);
	for (i=0;i<=myHandStrength;i++)
	{
		winningP += (node->data.bucket[i])/(double)total;
		if (i == myHandStrength) winningP -= (node->data.bucket[i])/((double)total*2); 		//Yuchen: this accounts the probability of winning the opponent within the same bucket.
	}
	//fprintf(stderr,"\nShowing WinningP: %f",winningP);
	return winningP;
}

void getOpponentAction(Game *game, State *state, Action* actionList, int actLen, uint8_t opponentID,DataType *action)
{
	int round = state->round;
	//int playerID = currentPlayer(game,state);
	
	int j;
	fprintf(stderr, "getNode actionList(%d):\t", actLen);
	for(j=0; j<actLen; j++) {
			fprintf(stderr, "%d", actionList[j].type);
		}
	fprintf(stderr, "\n");
	struct Node* node = getNode(actionList, actLen, round, opponentID);
	unsigned total = 0;
	int i;
	//DataType* action = (DataType*) malloc(sizeof(DataType)*3);
	if (!node)
	{
		fprintf(stderr, "%s:%d\t Warning: getNode return NULL\t", __FILE__, __LINE__);
		int i;
		fprintf(stderr, "round:%d\t oppID:%d\t actionList(%d):\t", round, opponentID, actLen);
		for(i=0; i<actLen; i++) {
			fprintf(stderr, "%d", actionList[i].type);
			}
		fprintf(stderr, "\n");
		action[0] = 0.33;
		action[1] = 0.33;
		action[2] = 0.33;
		
		printRound(game, round, opponentID);

	//	return action;
		return;
	}
	for(i=0; i<3; i++)
		total += node->data.actionDist[i];
	if (total == 0)
	{
		fprintf(stderr,"%s:%d\t Warning: getOpponentAction return default(total == 0)\n", __FILE__, __LINE__);
		
		int i;
		fprintf(stderr, "round:%d\t oppID:%d\t actionList(%d):\t", round, opponentID, actLen);
		for(i=0; i<actLen; i++) {
			fprintf(stderr, "%d", actionList[i].type);
		}
		fprintf(stderr, "\n");

		action[0] = 0.33;
		action[1] = 0.33;
		action[2] = 0.33;
		//return action;
		return;
		
	}
//	fprintf("\naction:%f %f %f,%f",(float)node->data.actionDist[0],(float)node->data.actionDist[1],(float)node->data.actionDist[2],(float)total);

	for(i=0; i<3; i++)
		*(action+i) = ((DataType)(node->data.actionDist[i]))/((DataType)total);
	return;
	//return action;
}
Gametree* constructTree(Game *game, State *state,int opponentID, int selfID)		
//Construct a game tree in certain stages. If the current stage does not change, we can always determine next action with this tree
{
	uint8_t currentRound;		//current round
	uint8_t numRaise;		//number of raises allowed in this game
	int handStrength;
	int isFirst = 0;
	Gametree *thisGametree = (Gametree *)malloc(sizeof(Gametree));

	
	currentRound = state->round;
	numRaise = game->maxRaises[currentRound];
	initTree(numRaise,thisGametree);
	handStrength = computeHandStrength(state, selfID) - 1;	//fuck yuchen
	if ((int)(game->firstPlayer[currentRound]) == selfID)
		isFirst = 1;

	thisGametree = computeTreevalue(game, state, thisGametree, 
									numRaise, handStrength, opponentID, isFirst);
	return thisGametree;
}

void releaseTree(Gametree* rootnode)
{
	if (!rootnode)
		return;
	releaseTree(rootnode->fold);
	releaseTree(rootnode->call);
	releaseTree(rootnode->raise);
	free(rootnode);
}

/*
void main()
{

	Gametree* ok = (Gametree *)malloc(sizeof(Gametree));

	Game testgame;
	State teststate;
	int test = 0;

	testgame.maxRaises[0] = 2;
	testgame.maxRaises[1] = 2;
	testgame.firstPlayer[0] = 1;
	testgame.firstPlayer[1] = 1;
	testgame.blind[0] = 10;
	testgame.blind[1] = 5;
	testgame.raiseSize[0] = 10;
	testgame.raiseSize[1] = 20;

	teststate.round = 1 ;
	teststate.spent[0] = 30;
	teststate.spent[1] = 30;







	ok = constructTree(&testgame, &teststate, 1, 0);


	test++;
	test++;

}
*/


void initTree(int numRaise,Gametree *rootnode)
//Generate the structure of the tree
{
	//First generate a full tree and then cut it to the gametree.

	//---------------------------------------------------------------------------------------
	//NOTICE: Current logic about the depth of the tree:
	//Two player game, one player can only raise ceil(numRaise/2) times and from that we know 
	//the max possible depth of the tree should be: crrc for numRaise = 2, generally, it should
	//be numRaise+2 operations and numRaise+3 levels
	//---------------------------------------------------------------------------------------

	int maxlevel = (int)numRaise+3;		//maxlevel of the tree is combining calls/raises
	int level = 1;					//the level of the tree
	int front = 0;
	int	rear = 1;
	int i = 0;
	int j = 0;
	int temprear = rear;
	//Gametree* rootnode = (Gametree *)malloc(sizeof(Gametree));	
	Gametree* temp;
	
	Gametree *nodeindex[MAXNODE];
	for (i = 0; i < MAXNODE; i++)
		nodeindex[i] = NULL;

	nodeindex[0] = rootnode;
	rootnode->data = 0;
	rootnode->parent = NULL;
	rootnode->raise = NULL;
	rootnode->fold = NULL;
	rootnode->call = NULL;

	level++;		//start by generating the 2nd level

	while (level <= maxlevel)
	{
		while (front < rear)
		{
			Gametree* foldnode = (Gametree *)malloc(sizeof(Gametree));
			Gametree* callnode = (Gametree *)malloc(sizeof(Gametree));
			Gametree* raisenode = (Gametree *)malloc(sizeof(Gametree));

			foldnode->data = 0;
			foldnode->nodeType = 0;
			foldnode->raise = NULL;
			foldnode->call = NULL;
			foldnode->fold = NULL;
			foldnode->parent = nodeindex[front];
			(*(nodeindex[front])).fold = foldnode;
			nodeindex[temprear] = foldnode;
			temprear++;


			callnode->data = 0;
			callnode->nodeType = 1;
			callnode->raise = NULL;
			callnode->call = NULL;
			callnode->fold = NULL;
			(*(nodeindex[front])).call = callnode;
			callnode->parent = nodeindex[front];
			nodeindex[temprear] = callnode;
			temprear++;


			raisenode->data = 0;
			raisenode->nodeType = 2;
			raisenode->raise = NULL;
			raisenode->call = NULL;
			raisenode->fold = NULL;
			(*(nodeindex[front])).raise = raisenode;
			raisenode->parent = nodeindex[front];
			nodeindex[temprear] = raisenode;
			temprear++;

			front++;
		}
		rear = temprear;
		level++;
	}

	//Begin to delete irreasonable nodes

	for (i = 0; i < rear; i++)		//0~rear will be all the nodes
	{
		if (nodeindex[i]->fold)
		{		
			if ((nodeindex[i]->fold)->fold)	//all childs of fold child should be deleted
			{
				nodeindex[i]->fold->fold = NULL;
				nodeindex[i]->fold->call = NULL;
				nodeindex[i]->fold->raise = NULL;
			}
		}
		
		if (nodeindex[i]->raise)							//calls after a raise must terminate the game
		{
			if ((nodeindex[i]->raise)->call)
			{
				(nodeindex[i]->raise)->call->fold = NULL;
				(nodeindex[i]->raise)->call->call = NULL;
				(nodeindex[i]->raise)->call->raise = NULL;
			}

			j = 1;
			temp = nodeindex[i]->raise;
			while((j < numRaise) && temp)					//there will be at most numRaise raises
			{
				temp = temp->raise;
				j++;
			}
			if (temp)
				temp->raise = NULL;
		}



		if	(nodeindex[i]->call)
		{
			if ((nodeindex[i]->call)->call)
			{
				if (((nodeindex[i]->call)->call)->fold)		//all tree nodes after 2 consecutive calls should be deleted
				{
					((nodeindex[i]->call)->call)->fold = NULL;
					((nodeindex[i]->call)->call)->call = NULL;
					((nodeindex[i]->call)->call)->raise = NULL;
				}
			}
		}
	}
//	free(nodeindex);
	//return rootnode;
}

Gametree* computeTreevalue(Game* game, State* state, 
									Gametree* emptyTree, int numRaise, 
									DataType handStrength, int opponentID, 
									int isFirst)
//Give all the values in the tree
{
	int nodeDegree;											//the degree of certain node
	int totalSpent, playerSpent;								//total number of chips in the pot and the chip player A spent
	int maxlevel = numRaise+3;
	int level = 1;
	int front = 0;
	int	rear = 1;
	int i = 0;
	int temprear = rear;
	//DataType* opponentAction;
	Gametree *nodeindex[MAXNODE];
	for (i = 0; i < MAXNODE; i++)
		nodeindex[i] = NULL;
	
	nodeindex[0] = emptyTree;
	level++;

	while (level <=maxlevel)
	{
		while (front < rear)
		{
			if ((nodeindex[front]->call) && (nodeindex[front]->fold))
			{
				nodeindex[temprear] =  nodeindex[front]->fold;
				nodeindex[temprear+1] =  nodeindex[front]->call;	
				temprear +=2;
				if (nodeindex[front]->raise)
				{
					nodeindex[temprear] =  nodeindex[front]->raise;
					temprear ++;
				}
			}
			front++;
		}
		rear = temprear;
		level++;
	}

	rear--;												//rear will point to the last node in the node list
	
	for(i = 0; i < MAXNODE; i++)
	{
		if (!(nodeindex[i]))							//Arrives the end of the node list
			break;
		if (!(nodeindex[i]->fold))						//This node has no child
		{
			nodeDegree = getDegree(nodeindex[i]);
			totalSpent = totalSpentChips(game, state, nodeindex[i], &playerSpent, isFirst);
			//fprintf(stderr,"%d",isFirst);
			if ((!(nodeDegree%2) && (isFirst)) ||	((nodeDegree%2) && (!isFirst)))			
			//If player A will decide first, then all nodes in even level will represent results of A's move, otherwise all nodes in odd level will represent results of  A's move
			{
				if (nodeindex[i]->nodeType == 0)	//a fold node
					nodeindex[i]->data = 0 - playerSpent;
				if (nodeindex[i]->nodeType == 1)	//After two calls
				{
					//Action* actionList = getActionList(nodeindex[i]);
					Action* actionList = (Action *)malloc(sizeof(Action)*MAXDEGREE);
					int actLen;
					//if (0 == (getDegree(nodeindex[i])%2))
					//{
						getActionList((nodeindex[i])->parent,actionList);
						//fprintf(stderr,"\n nowrong1 %d %d\n",getDegree(nodeindex[i]),getDegree(nodeindex[i]->parent));
						actLen = getDegree(nodeindex[i]) - 2;
					//}
					//else
					//{
					//	getActionList(nodeindex[i],actionList);
					//	fprintf(stderr,"\n nowrong1 %d %d\n",getDegree(nodeindex[i]),getDegree(nodeindex[i]->parent));
					//	actLen = getDegree(nodeindex[i])-1;
					//}

					#if 0
					int loopvar;
					fprintf(stderr,"\nwinningP para actlen(%d):  ", actLen);
					for (loopvar = 0; loopvar<actLen;loopvar++)
					{
						fprintf(stderr,"%d",(actionList+loopvar)->type);
					}
					#endif
					nodeindex[i]->data = winningProb(game, state, handStrength, opponentID, actionList,actLen)*totalSpent-playerSpent;
					free(actionList);
				}
			}
			else								//B's move
			{
				if (nodeindex[i]->nodeType == 0)	//a fold node
					nodeindex[i]->data = totalSpent - playerSpent;
				if (nodeindex[i]->nodeType == 1)	//After two calls
				{
					//Action* actionList = getActionList(nodeindex[i]);
					Action* actionList = (Action *)malloc(sizeof(Action)*MAXDEGREE);
					int actLen;
					//if (0 == (getDegree(nodeindex[i])%2))
					//{
						getActionList(nodeindex[i],actionList);
						//fprintf(stderr,"\n nowrong %d %d\n",getDegree(nodeindex[i]),getDegree(nodeindex[i]->parent));
						actLen = getDegree(nodeindex[i]) - 1;
					//}
					//else
					//{
					//	fprintf(stderr,"\n nowrong %d %d\n",getDegree(nodeindex[i]),getDegree(nodeindex[i]->parent));
				//		getActionList((nodeindex[i])->parent,actionList);
			//			actLen = getDegree(nodeindex[i])-2;
				//	}
				#if 0
					int loopvar;
					fprintf(stderr,"\nwinningP para actlen(%d):  ", actLen);
					for (loopvar = 0; loopvar<actLen;loopvar++)
					{
						fprintf(stderr,"%d",(actionList+loopvar)->type);
					}
					#endif
					nodeindex[i]->data = winningProb(game, state, handStrength, opponentID ,actionList,actLen)*totalSpent-playerSpent;
					free(actionList);
				}
			}
		}
	}

	for (i = rear; i >= 0;)
	{
		nodeDegree = getDegree(nodeindex[i]);
		if (nodeDegree == 1)					//The data for the root node should always be computed later
			break;

		if ((nodeindex[i]->parent == nodeindex[i-1]->parent) && (nodeindex[i-1]->parent == nodeindex[i-2]->parent))
			//Their parents will be determined by all the fold, call and raise operation values
		{
			if ((!(nodeDegree%2) && (isFirst)) ||	((nodeDegree%2) && (!isFirst)))			
				nodeindex[i]->parent->data = findMax(nodeindex[i]->data,nodeindex[i-1]->data,nodeindex[i-2]->data);
			else
			{
				//Action* actionList = getActionList(nodeindex[i]->parent);
				Action* actionList = (Action *)malloc(sizeof(Action)*MAXDEGREE);
				getActionList(nodeindex[i]->parent,actionList);
				int actLen = getDegree(nodeindex[i]->parent)-1;
				DataType *opponentAction = (DataType *)malloc(sizeof(DataType)*3);
				getOpponentAction(game,state,actionList,actLen,opponentID,opponentAction);
				//fprintf(stderr,"\nOpponentActionProb:%f,%f,%f",*(opponentAction+2),*(opponentAction+1),*(opponentAction));
				nodeindex[i]->parent->data = (*(opponentAction+2)) * nodeindex[i]->data + (*(opponentAction+1)) * nodeindex[i-1]->data + (*opponentAction) * nodeindex[i-2]->data;
				free(actionList);
			}
			i -= 3;
		}
		else
			//Their parents will only be determined by fold and call operations
		{
			if ((!(nodeDegree%2) && (isFirst)) ||	((nodeDegree%2) && (!isFirst)))			
				nodeindex[i]->parent->data = ((nodeindex[i]->data) > (nodeindex[i-1]->data)) ? (nodeindex[i]->data) :(nodeindex[i-1]->data);
			else
			{
				Action* actionList = (Action *)malloc(sizeof(Action)*MAXDEGREE);
				getActionList(nodeindex[i]->parent,actionList);
				int actLen = getDegree(nodeindex[i]->parent)-1;
				DataType *opponentAction = (DataType *)malloc(sizeof(DataType)*3);
				getOpponentAction(game,state,actionList,actLen,opponentID,opponentAction);
				//fprintf(stderr,"\nOpponentActionProb:%f,%f",*(opponentAction+1),*(opponentAction));
				nodeindex[i]->parent->data = (*(opponentAction+1)) * nodeindex[i]->data + (*(opponentAction)) * nodeindex[i-1]->data;
				free(actionList);
			}
			i -= 2;
		}
	}
	return emptyTree;
}

int getDegree(Gametree* testnode)				//return the degree of any node, root node will be in level 1
{
	int degree = 1;
	Gametree *tempnode;
	tempnode = testnode;
	if (!(tempnode->parent))
		return 1;
	else
	{
		while (tempnode->parent)
		{
			tempnode = tempnode->parent;
			degree++;
		}
		return degree;
	}
}

int totalSpentChips(Game *game, State *state, Gametree *testnode, int* playerSpent, int isFirst)
{
	int totalSpent = state->spent[0]+state->spent[1];				//Chips already in the pot, TWO PLAYERS ONLY!
	int currentCall = 0;											//Like check, no more chips needed to check
	int countchips[MAXDEGREE];
	int i = 0;
	int j = 0;
	int temp = 0;
	Gametree* tempnode;
	tempnode = testnode;
	//for (i=0;i<MAXDEGREE;i++)
	//	countchips[i] = 10;					//Irreasonable number 

	while (tempnode->parent)						//Find how this node is reached
	{
		countchips[i] = tempnode->nodeType;
		tempnode = tempnode->parent;
		i++;
	}	
	i--;						//countchips[i] means the first action (from root)

	if (isFirst)				//Player A plays first, j = 0, then all j%2==0 implies it is A's move
	{
		j = 0;
		(*playerSpent) = state->spent[0];
	}
	else
	{
		j = 1;
		(*playerSpent) = state->spent[1];
	}
	
	if (state->round == 0)		//Containing blind
	{
		if (getDegree(testnode) == 1)
			return game->blind[0] + game->blind[1];
		else
		{
			if (i == 0)								//fold directly because all the testnodes in this function must be leaf nodes
			{
				if (j == 0)							//Player A folds, then Player A pays the small blind
					(*playerSpent) = game->blind[1];
				else								//Player B folds, then Player A pays the big blind
					(*playerSpent) = game->blind[0];
				return game->blind[0] + game->blind[1];

			}
			totalSpent = game->blind[0] * 2;						//big blind
			currentCall = game->blind[0];
			*playerSpent = game->blind[0];
			while (i>=0)
			{
				if (countchips[i] == 0)				//fold, exit
				{
					if (getDegree(testnode) == 2)
					{
						if (isFirst)
							*playerSpent = game->blind[1];
						else
							*playerSpent = game->blind[0];
						totalSpent = game->blind[0] + game->blind[1];
					}
					return totalSpent;	
				}
				if (countchips[i] == 1)				//call
				{
					if (j%2 == 0)					//Player A calls
						(*playerSpent) = currentCall;
					if (countchips[i+1] == 2)		//just raised, MAYBE A PROBLEM when numRaise != 2
						totalSpent = totalSpent - (currentCall - game->raiseSize[state->round]) + currentCall;

				}
				if (countchips[i] == 2)				//raise
				{
					currentCall += game->raiseSize[state->round];				//after one raise, to call the player must pay extra raiseSize(currentround) chips
					if (j%2 == 0)					//Player A raises
						(*playerSpent) = currentCall;
					if (countchips[i+1] == 2)		//just raised
						totalSpent = totalSpent - (currentCall - 2 * game->raiseSize[state->round]) + currentCall;
					else
						totalSpent = totalSpent - (currentCall - game->raiseSize[state->round]) + currentCall;
				}
				i--;
				j++;
			}
		return totalSpent;
		}
	}
	else						//After community cards are dealt
	{
		temp = *playerSpent;
		*playerSpent = 0;
		while (i>=0)
		{
			if (countchips[i] == 0)				//fold, exit
			{
				*playerSpent = *playerSpent + temp;
				return totalSpent;
			}
			if (countchips[i] == 1)				//call
			{
				if (j%2 == 0)					//Player A calls
					(*playerSpent) = currentCall;
				if (countchips[i+1] == 2)		//just raised
					totalSpent = totalSpent - (currentCall - game->raiseSize[state->round]) + currentCall;
			}
			if (countchips[i] == 2)				//raise
			{
				currentCall += game->raiseSize[state->round];				//after one raise, to call the player must pay extra raiseSize(currentround) chips
				if (j%2 == 0)					//Player A raises
					(*playerSpent) = currentCall;
				if (countchips[i+1] == 2)		//just raised
					totalSpent = totalSpent - (currentCall - 2 * game->raiseSize[state->round]) + currentCall;
				else
					totalSpent = totalSpent - (currentCall - game->raiseSize[state->round]) + currentCall;
			}
			i--;
			j++;
		}
		*playerSpent = *playerSpent + temp;
		return totalSpent;
	}
}

void getActionList(Gametree *testnode, Action *actionList)
{
	//Action *actionList = (Action *)malloc(sizeof(Action)*MAXDEGREE);
	Action *temp = (Action *)malloc(sizeof(Action)*MAXDEGREE);
	Gametree *tempnode;
	int i = 0;
	int j = 0;

	tempnode = testnode;

	if (getDegree(testnode) == 1)
	{
		actionList = NULL;
		//return actionList;
	}
//	fprintf(stderr,"\nDebug:");
	while (tempnode->parent)
	{
		(*(temp+i)).type = tempnode->nodeType;
//		fprintf(stderr,"%d ", tempnode->nodeType);
		i++;
		tempnode = tempnode->parent;
	}
	i--;
	int tempi = i;
//	fprintf(stderr,"\ncompare:%d %d",i,getDegree(testnode) - 1);
	for (j = 0; j <= tempi; j++)
	{
		(*(actionList + j)).type = (*(temp + i)).type;
//		fprintf(stderr,"\nAfterdebug: %d",(*(temp+i)).type);
		i--;
	}
	//return actionList;
	free(temp);
}

DataType findMax(DataType x1, DataType x2, DataType x3)
{
	DataType maxNo = (x1 > x2) ? x1 : x2;
	maxNo = (maxNo > x3) ? maxNo : x3;
	return maxNo;
}

void decideAction(Gametree* thisGametree, Action* actionList, int actionLen, Action* action)
{
	//TODO: from the actionList, return the best action.
	Gametree* temptree = thisGametree;
	if (!thisGametree)
		fprintf(stderr,"\nerror:No game tree");

	int i = 0;
	fprintf(stderr,"\nUse decideAction: length: %d\n ",actionLen);
	for (i = 0; i<actionLen; i++)
	{
		fprintf(stderr,"%d ", actionList[i].type);
	}
	for(i = 0; i < actionLen; i++)
	{
		if (actionList[i].type == 0)
		{
			if (temptree->fold)
				temptree = temptree->fold;
			else
			{
				fprintf(stderr,"\n Fatal error: No more folds!\n");
				action->type = 1;
				return;
			}
		}
		else if (actionList[i].type == 1)
		{
			if (temptree->call)
				temptree = temptree->call;
			else
			{
				fprintf(stderr,"\n Fatal error: No more calls!\n");
				action->type = 1;
				return;
			}
		}
		else if (actionList[i].type == 2)
		{
			if (temptree->raise)
				temptree = temptree->raise;
			else
			{
				fprintf(stderr,"\n Fatal error: No more raises!\n");
				action->type = 1;
				return;
			}
		}
	}
	if (temptree->data == temptree->call->data)
		action->type = 1;
	else if (temptree->data == temptree->fold->data)
		action->type = 0;
	else if (temptree->raise)
		{
			if (temptree->data == temptree->raise->data)
				action->type = 2;
		}
	else {
		fprintf(stderr,"\nWRONG decideAction call\n");
	}
	
}


//Naive functions for debugging
/*
DataType computeHandStrength(Game *game, State *state)
{
	DataType k = 0.45;
	return k;
}

DataType winningProb(Game* game, State* state, DataType handStrength,int opponentID)
{
	return 0.2;
}

DataType* getOpponentaction(Game* game,State* state)
{
	DataType *opponentAction = (DataType *)malloc(sizeof(DataType *) * 3);
	*opponentAction = 0.2;
	*(opponentAction + 1) = 0.3;
	*(opponentAction + 2) = 0.5;
	return opponentAction;

}
*/
