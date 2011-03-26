/*
* File Name : opp.h

* Purpose :

* Creation Date : 23-02-2011

* Last Modified : Fri 04 Mar 2011 05:45:37 PM EST

* Created By : Weikeng Qin (weikqin@gmail.com)

*/
#define MAX_NUM_BUCKETS 5

typedef enum {false = 0, true = 1} bool;
enum NodeType {null = 0, strength = 1, prob = 2}; 


typedef unsigned Bucket;

struct Node {
	enum NodeType type;
	union {
		unsigned actionDist[3];
		Bucket bucket[MAX_NUM_BUCKETS];
	} data;
#ifdef DEBUG 
	enum ActionType actionList[MAX_NUM_ACTIONS];	
	uint8_t actionNum;
#endif 

#ifdef NODEPAR
	struct Node *parent;
#endif

	struct Node *child[3];
};
		

typedef struct {
	struct Node *dealerRoot;
	struct Node *nonDealerRoot;
} OppBase;

extern OppBase flopsBase[MAX_ROUNDS];

unsigned calcStrength();
/* ========= All the following functions are now only for two people, limit game =================*/
void initNode(struct Node* node, enum NodeType type);
struct Node* initBase(OppBase *base, bool isPlayFirst);
void updateBase(uint8_t round, uint8_t pos, State *state, bool isFold);
void updateModel(Game *game, uint8_t pos, State *state, bool isFold);
struct Node *getNode(Action *act, uint8_t actLen, uint8_t round, uint8_t pos);
void releaseNode(struct Node *node);
void releaseBase(OppBase *base);
void initModel();

#ifdef DEBUG
void printNode(struct Node *node, FILE *file);
void printBase(OppBase *base);
void printModel();
void printRound(Game *game, uint8_t round, uint8_t pos);
#endif

