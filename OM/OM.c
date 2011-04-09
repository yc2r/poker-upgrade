#include "OM.h"
#include "../game.h"
#include "../MACRO.h"
#include "../handValue/handValue.h"

int getOpponentModel(int playerID, int round)
//round: 0=preflop 1=flop 2=turn 3=river
{
	
}

void updateModel(Opponents *opp, Game *game, State *state)
{
	//get general info
	int player1Fold = (state->playerFolded[(state.viewingPlayer+1)%3] == 1)?1:0;
	int player2Fold = (state->playerFolded[(state.viewingPlayer+2)%3] == 1)?1:0;
	int dealerId = ((state->actingPlayer[0][0])+2)%3;
	int numRaises = 0;
	int numCalls = 0;
	int i = 0;
	//update preflop
	//player on the right:
	if (!player1Fold)
	{
		int pos = (state.viewingPlayer+1) % 3;
		int round = 0;
		Strength oppoStr = computeHandStrength(state, pos, round);		//got oppo pre-flop strength
		for (i = 0; i < MAX_NUM_ACTIONS; i++)
		{
			if (state->actingPlayer[0][i] == pos)
			{
				if (state->action[0][i] == 2)
				{
					numRaises ++;
				}
				if (state->action[0][i] == 1)
				{
					numCalls ++;
				}
			}
		}
		/*float raiserate = (float)numRaises/(numCalls+numRaises);
		int norm_raiserate = raiserate * 40 + 10;
		int temp = norm_raiserate%10-5;
		norm_raiserate = norm_raiserate / 10;
		norm_raiserate = (temp>0)?norm_raiserate+1:norm_raiserate;	//si she wu ru*/
		if (oppoStr.bucket == 5)
		{
			if (numCalls>1) opp->om[0][0].history[opp->om[0][0].currentPointer] = 5;
			else opp->om[0][0].history[opp->om[0][0].currentPointer] = 3;	
		}
		if (oppoStr.bucket == 4)
		{
			if (numRaises == 0) opp->om[0][0].history[opp->om[0][0].currentPointer] = 4;
			else opp->om[0][0].history[opp->om[0][0].currentPointer] = 3;
		}
		if (oppoStr.bucket == 3)
		{
			if ((numCalls == 1)&&(numRaises>1)) opp->om[0][0].history[opp->om[0][0].currentPointer] = 2;	
			else opp->om[0][0].history[opp->om[0][0].currentPointer] = 3;	
		}
		if (oppoStr.bucket==2) 
		{
			if (dealerId != pos) opp->om[0][0].history[opp->om[0][0].currentPointer] = 3;		//protecting blinds.
			else opp->om[0][0].history[opp->om[0][0].currentPointer] = 2;	//dealer plays a sub-optimal hand 
			if (numRaises>0) opp->om[0][0].history[opp->om[0][0].currentPointer] = 1;		//opponent raising when he's got weak hand. He's very loose
		}
		if (oppoStr.bucket==1)
		{
			if (dealerId == (pos+2)%3) opp->om[0][0].history[opp->om[0][0].currentPointer] = 2;
			else if (dealerId == (pos+1)%3) opp->om[0][0].history[opp->om[0][0].currentPointer] = 3;
			else opp->om[0][0].history[opp->om[0][0].currentPointer] = 1;
			if (numRaises>0) opp->om[0][0].history[opp->om[0][0].currentPointer] = 1;	
		}
		opp->om[0][0].historyFold[opp->om[0][0].currentPointer] = 0;
		opp->om[0][0].currentPointer = (opp->om[0][0].currentPointer+1)%100;
	}
	else
	{
		//oppo folded pre-flop, no cards are shown
		opp->om[0][0].historyFold[opp->om[0][0].currentPointer] = 1;
		opp->om[0][0].currentPointer = (opp->om[0][0].currentPointer+1)%100;
	}
	//////////////////////////////////////////
	//player on the left:
	if (!player2Fold)
	{
		int pos = (state.viewingPlayer+2) % 3;
		int round = 0;
		Strength oppoStr = computeHandStrength(state, pos, round);		//got oppo pre-flop strength
		for (i = 0; i < MAX_NUM_ACTIONS; i++)
		{
			if (state->actingPlayer[0][i] == pos)
			{
				if (state->action[0][i] == 2)
				{
					numRaises ++;
				}
				if (state->action[0][i] == 1)
				{
					numCalls ++;
				}
			}
		}
		if (oppoStr.bucket == 5)
		{
			if (numCalls>1) opp->om[1][0].history[opp->om[1][0].currentPointer] = 5;
			else opp->om[1][0].history[opp->om[1][0].currentPointer] = 3;	
		}
		if (oppoStr.bucket == 4)
		{
			if (numRaises == 0) opp->om[1][0].history[opp->om[1][0].currentPointer] = 4;
			else opp->om[1][0].history[opp->om[1][0].currentPointer] = 3;
		}
		if (oppoStr.bucket == 3)
		{
			if ((numCalls == 1)&&(numRaises>1)) opp->om[1][0].history[opp->om[1][0].currentPointer] = 2;	
			else opp->om[1][0].history[opp->om[1][0].currentPointer] = 3;	
		}
		if (oppoStr.bucket==2) 
		{
			if (dealerId != pos) opp->om[1][0].history[opp->om[1][0].currentPointer] = 3;		//protecting blinds.
			else opp->om[1][0].history[opp->om[1][0].currentPointer] = 2;	//dealer plays a sub-optimal hand 
			if (numRaises>0) opp->om[1][0].history[opp->om[1][0].currentPointer] = 1;		//opponent raising when he's got weak hand. He's very loose
		}
		if (oppoStr.bucket==1)
		{
			if (dealerId == (pos+2)%3) opp->om[1][0].history[opp->om[1][0].currentPointer] = 2;
			else if (dealerId == (pos+1)%3) opp->om[1][0].history[opp->om[1][0].currentPointer] = 3;
			else opp->om[1][0].history[opp->om[1][0].currentPointer] = 1;
			if (numRaises>0) opp->om[1][0].history[opp->om[1][0].currentPointer] = 1;	
		}
		opp->om[1][0].historyFold[opp->om[1][0].currentPointer] = 0;
		opp->om[1][0].currentPointer = (opp->om[1][0].currentPointer+1)%100;
	}
	else
	{
		//oppo folded pre-flop, no cards are shown
		opp->om[1][0].historyFold[opp->om[1][0].currentPointer] = 1;
		opp->om[1][0].currentPointer = (opp->om[1][0].currentPointer+1)%100;
	}
	//////////////////////
	//post-flop
	int j = 0;
	for (i=1; i<4; i++)
	{
		//player on the right:
		if (!player1Fold)
		{
			int pos = (state.viewingPlayer+1) % 3;
			int round = i;
			Strength oppoStr = computeHandStrength(state, pos, round);		//got oppo pre-flop strength
			for (j = 0; j < MAX_NUM_ACTIONS; j++)
			{
				if (state->actingPlayer[i][j] == pos)
				{
					if (state->action[i][j] == 2)
					{
						numRaises ++;
					}
					if (state->action[i][j] == 1)
					{
						numCalls ++;
					}
				}
			}
			if (oppoStr.bucket == 5)
			{
				if (numCalls>1) opp->om[0][round].history[opp->om[0][round].currentPointer] = 5;
				else opp->om[0][round].history[opp->om[0][round].currentPointer] = 3;	
			}
			if (oppoStr.bucket == 4)
			{
				if (numRaises == 0) opp->om[0][round].history[opp->om[0][round].currentPointer] = 4;
				else opp->om[0][round].history[opp->om[0][round].currentPointer] = 3;
			}
			if (oppoStr.bucket == 3)
			{
				if ((numCalls == 1)&&(numRaises>1)) opp->om[0][round].history[opp->om[0][round].currentPointer] = 2;	
				else opp->om[0][round].history[opp->om[0][round].currentPointer] = 3;	
			}
			if (oppoStr.bucket==2) 
			{
				if (numRaises>0) opp->om[0][round].history[opp->om[0][round].currentPointer] = 2;		//opponent raising when he's got weak hand. He's very loose
				else opp->om[0][round].history[opp->om[0][round].currentPointer] = 3;
			}
			if (oppoStr.bucket==1)
			{
				if (numRaises>0) opp->om[0][round].history[opp->om[0][round].currentPointer] = 1;	
				else opp->om[0][round].history[opp->om[0][round].currentPointer] = 3;
			}
			opp->om[0][round].historyFold[opp->om[0][round].currentPointer] = 0;
			opp->om[0][round].currentPointer = (opp->om[0][round].currentPointer+1)%100;
		}
		else
		{
			//oppo folded pre-flop, no cards are shown
			opp->om[0][round].historyFold[opp->om[0][round].currentPointer] = 1;
			opp->om[0][round].currentPointer = (opp->om[0][round].currentPointer+1)%100;
		}
		//////////////////////////////////////////
		//player on the left:
		if (!player1Fold)
		{
			int pos = (state.viewingPlayer+2) % 3;
			int round = i;
			Strength oppoStr = computeHandStrength(state, pos, round);		//got oppo pre-flop strength
			for (j = 0; j < MAX_NUM_ACTIONS; j++)
			{
				if (state->actingPlayer[i][j] == pos)
				{
					if (state->action[i][j] == 2)
					{
						numRaises ++;
					}
					if (state->action[i][j] == 1)
					{
						numCalls ++;
					}
				}
			}
			if (oppoStr.bucket == 5)
			{
				if (numCalls>1) opp->om[1][round].history[opp->om[1][round].currentPointer] = 5;
				else opp->om[1][round].history[opp->om[1][round].currentPointer] = 3;	
			}
			if (oppoStr.bucket == 4)
			{
				if (numRaises == 0) opp->om[1][round].history[opp->om[1][round].currentPointer] = 4;
				else opp->om[1][round].history[opp->om[1][round].currentPointer] = 3;
			}
			if (oppoStr.bucket == 3)
			{
				if ((numCalls == 1)&&(numRaises>1)) opp->om[1][round].history[opp->om[1][round].currentPointer] = 2;	
				else opp->om[1][round].history[opp->om[1][round].currentPointer] = 3;	
			}
			if (oppoStr.bucket==2) 
			{
				if (numRaises>0) opp->om[1][round].history[opp->om[1][round].currentPointer] = 2;		//opponent raising when he's got weak hand. He's very loose
				else opp->om[1][round].history[opp->om[1][round].currentPointer] = 3;
			}
			if (oppoStr.bucket==1)
			{
				if (numRaises>0) opp->om[1][round].history[opp->om[1][round].currentPointer] = 1;	
				else opp->om[1][round].history[opp->om[1][round].currentPointer] = 3;
			}
			opp->om[1][round].historyFold[opp->om[1][round].currentPointer] = 0;
			opp->om[1][round].currentPointer = (opp->om[1][round].currentPointer+1)%100;
		}
		else
		{
			//oppo folded pre-flop, no cards are shown
			opp->om[1][round].historyFold[opp->om[1][round].currentPointer] = 1;
			opp->om[1][round].currentPointer = (opp->om[1][round].currentPointer+1)%100;
		}
	}
	//////////////////////////////////////////
}
