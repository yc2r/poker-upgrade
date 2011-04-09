#include "../game.h"
#include "../MACRO.h"

typedef struct {
	int history[OM_rounds];		//the history of not folding
	int historyFold[OM_rounds];	//the history of folding
	int currentPointer;			//used to implement circular buffer
} OM;

typedef struct{
	OM om[2][4];		//2: number of other players; 4:number of rounds
						//om[0][x] indicates the player sitting right to me
						//om[1][x] indicates the player sitting left to me
} Opponents;

void updateModel(Opponents *opp, Game *game, State *state);

