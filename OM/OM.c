#include "OM.h"

int getOpponentModel(Opponents *opp, int playerID, int round)
//round: 0=preflop 1=flop 2=turn 3=river
{
	int i = 0;
	/*int bins[5];
	for (i = 0; i < 5; i++)
	{
		bins[i] = 0;
	}
	for (i = 0; i < OM_rounds; i++)
	{
		bins[opp->om[playerID][round].history[i]-1]++;
	}
	int returnValue;
	int temp = -1;
	for (i = 0; i < 5; i++)
	{
		if (bins[i]>temp) 
		{
			returnValue = i;
			temp = bins[i];
		}
	}
	int totalFolds = 0;
	for (i = 0; i < OM_rounds; i++)
	{
		totalFolds += opp->om[playerID][round].historyFold[i];
	}
	if (totalFolds>80) return ((returnValue+2)<=5)?returnValue+2:5;
	else if (totalFolds<20) return (returnValue>=1)?returnValue:1;
	return returnValue+1;			//return value range is 1-5*/
	int total = 0;
	for (i=0;i<OM_rounds;i++)
	{
		total+=opp->om[playerID][round].history[i];
	}
	int totalFolds = 0;
	for (i = 0; i < OM_rounds; i++)
	{
		totalFolds += opp->om[playerID][round].historyFold[i];
	}
	int temp = total*10/OM_rounds;
	int avg = temp/10+(temp%10>=5)?1:0;
	if (totalFolds>80) return ((avg+2)<=5)?avg+2:5;
	else if (totalFolds<20) return (avg>=1)?avg:1;
	return avg+1;			//return value range is 1-5*/
}

void updateModel(Opponents *opp, Game *game, State *state, int viewingPlayer)
{
	//get general info
	int player1Fold = (state->playerFolded[(viewingPlayer+1)%3] == 1)?1:0;
	int player2Fold = (state->playerFolded[(viewingPlayer+2)%3] == 1)?1:0;
	int dealerId = state->actingPlayer[0][0];
	int numRaises = 0;
	int numCalls = 0;
	int i = 0;
	//update preflop
	//player on the right:
	if (!player1Fold)
	{
		int pos = (viewingPlayer+1) % 3;
		int round = 0;
		Strength *oppoStr = (Strength*) malloc(sizeof(oppoStr));
		computeHandStrength(state, pos, round, oppoStr);		//got oppo pre-flop strength
		for (i = 0; i < MAX_NUM_ACTIONS; i++)
		{
			if (state->actingPlayer[0][i] == pos)
			{
				if (state->action[0][i].type == 2)
				{
					numRaises ++;
				}
				if (state->action[0][i].type == 1)
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
		if (oppoStr->bucket == 5)
		{
			if (numCalls>1) opp->om[0][0].history[opp->om[0][0].currentPointer] = 5;
			else opp->om[0][0].history[opp->om[0][0].currentPointer] = 3;	
		}
		if (oppoStr->bucket == 4)
		{
			if (numRaises == 0) opp->om[0][0].history[opp->om[0][0].currentPointer] = 4;
			else opp->om[0][0].history[opp->om[0][0].currentPointer] = 3;
		}
		if (oppoStr->bucket == 3)
		{
			if ((numCalls == 1)&&(numRaises>1)) opp->om[0][0].history[opp->om[0][0].currentPointer] = 2;	
			else opp->om[0][0].history[opp->om[0][0].currentPointer] = 3;	
		}
		if (oppoStr->bucket==2) 
		{
			if (dealerId != pos) opp->om[0][0].history[opp->om[0][0].currentPointer] = 3;		//protecting blinds.
			else opp->om[0][0].history[opp->om[0][0].currentPointer] = 2;	//dealer plays a sub-optimal hand 
			if (numRaises>0) opp->om[0][0].history[opp->om[0][0].currentPointer] = 1;		//opponent raising when he's got weak hand. He's very loose
		}
		if (oppoStr->bucket==1)
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
	numRaises = 0;
	numCalls = 0;
	if (!player2Fold)
	{
		int pos = (viewingPlayer+2) % 3;
		int round = 0;
		Strength *oppoStr = (Strength*) malloc(sizeof(oppoStr));
		computeHandStrength(state, pos, round, oppoStr);		//got oppo pre-flop strength
		for (i = 0; i < MAX_NUM_ACTIONS; i++)
		{
			if (state->actingPlayer[0][i] == pos)
			{
				if (state->action[0][i].type == 2)
				{
					numRaises ++;
				}
				if (state->action[0][i].type == 1)
				{
					numCalls ++;
				}
			}
		}
		if (oppoStr->bucket == 5)
		{
			if (numCalls>1) opp->om[1][0].history[opp->om[1][0].currentPointer] = 5;
			else opp->om[1][0].history[opp->om[1][0].currentPointer] = 3;	
		}
		if (oppoStr->bucket == 4)
		{
			if (numRaises == 0) opp->om[1][0].history[opp->om[1][0].currentPointer] = 4;
			else opp->om[1][0].history[opp->om[1][0].currentPointer] = 3;
		}
		if (oppoStr->bucket == 3)
		{
			if ((numCalls == 1)&&(numRaises>1)) opp->om[1][0].history[opp->om[1][0].currentPointer] = 2;	
			else opp->om[1][0].history[opp->om[1][0].currentPointer] = 3;	
		}
		if (oppoStr->bucket==2) 
		{
			if (dealerId != pos) opp->om[1][0].history[opp->om[1][0].currentPointer] = 3;		//protecting blinds.
			else opp->om[1][0].history[opp->om[1][0].currentPointer] = 2;	//dealer plays a sub-optimal hand 
			if (numRaises>0) opp->om[1][0].history[opp->om[1][0].currentPointer] = 1;		//opponent raising when he's got weak hand. He's very loose
		}
		if (oppoStr->bucket==1)
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
	int numRaised = 0;		//number of time raised by any player
	for (i=1; i<4; i++)
	{
		numRaises = 0;
		numCalls = 0;
		int round = i;
		//player on the right:
		if (!player1Fold)
		{
			int pos = (viewingPlayer+1) % 3;
			Strength *oppoStr = (Strength*) malloc(sizeof(oppoStr));
			computeHandStrength(state, pos, round, oppoStr);		//got oppo pre-flop strength
			for (j = 0; j < MAX_NUM_ACTIONS; j++)
			{
				if (state->actingPlayer[i][j] == pos)
				{
					if (state->action[i][j].type == 2)
					{
						numRaises ++;		//number of time raised by this player
					}
					if (state->action[i][j].type == 1)
					{
						numCalls ++;
					}
				}
				if (state->action[i][j].type == 2) numRaised ++;
			}
			if (oppoStr->bucket == 5)
			{
				if (numCalls>1) opp->om[0][round].history[opp->om[0][round].currentPointer] = 5;
				else opp->om[0][round].history[opp->om[0][round].currentPointer] = 3;	
			}
			if (oppoStr->bucket == 4)
			{
				if (numRaises == 0) opp->om[0][round].history[opp->om[0][round].currentPointer] = 4;
				else opp->om[0][round].history[opp->om[0][round].currentPointer] = 3;
			}
			if (oppoStr->bucket == 3)
			{
				if ((numCalls == 1)&&(numRaises>1)) opp->om[0][round].history[opp->om[0][round].currentPointer] = 2;	
				else opp->om[0][round].history[opp->om[0][round].currentPointer] = 3;	
			}
			if (oppoStr->bucket==2) 
			{
				if (numRaises>0) opp->om[0][round].history[opp->om[0][round].currentPointer] = 1;		//opponent raising when he's got weak hand. He's very loose
				else if (numRaised>0) opp->om[0][round].history[opp->om[0][round].currentPointer] = 2;	//oppponent calling with a lower hand. He's kinda loose
				else opp->om[0][round].history[opp->om[0][round].currentPointer] = 3;
			}
			if (oppoStr->bucket==1)
			{
				if (numRaised==0) opp->om[0][round].history[opp->om[0][round].currentPointer] = 3;		//Unless everybody checks, this player is considered ultra-aggresive/loose.
				else opp->om[0][round].history[opp->om[0][round].currentPointer] = 1;
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
		numRaises = 0;
		numCalls = 0;
		if (!player2Fold)
		{
			int pos = (viewingPlayer+2) % 3;
			Strength *oppoStr = (Strength*) malloc(sizeof(oppoStr));
			computeHandStrength(state, pos, round, oppoStr);		//got oppo pre-flop strength
			for (j = 0; j < MAX_NUM_ACTIONS; j++)
			{
				if (state->actingPlayer[i][j] == pos)
				{
					if (state->action[i][j].type == 2)
					{
						numRaises ++;
					}
					if (state->action[i][j].type == 1)
					{
						numCalls ++;
					}
				}
				if (state->action[i][j].type == 2) numRaised ++;
			}
			if (oppoStr->bucket == 5)
			{
				if (numCalls>1) opp->om[1][round].history[opp->om[1][round].currentPointer] = 5;
				else opp->om[1][round].history[opp->om[1][round].currentPointer] = 3;	
			}
			if (oppoStr->bucket == 4)
			{
				if (numRaises == 0) opp->om[1][round].history[opp->om[1][round].currentPointer] = 4;
				else opp->om[1][round].history[opp->om[1][round].currentPointer] = 3;
			}
			if (oppoStr->bucket == 3)
			{
				if ((numCalls == 1)&&(numRaises>1)) opp->om[1][round].history[opp->om[1][round].currentPointer] = 2;	
				else opp->om[1][round].history[opp->om[1][round].currentPointer] = 3;	
			}
			if (oppoStr->bucket==2) 
			{
				if (numRaises>0) opp->om[1][round].history[opp->om[1][round].currentPointer] = 1;		//opponent raising when he's got weak hand. He's very loose
				else if (numRaised>0) opp->om[1][round].history[opp->om[1][round].currentPointer] = 2;	//oppponent calling with a lower hand. He's kinda loose
				else opp->om[1][round].history[opp->om[1][round].currentPointer] = 3;
			}
			if (oppoStr->bucket==1)
			{
				if (numRaised==0) opp->om[1][round].history[opp->om[1][round].currentPointer] = 3;		//Unless everybody checks, this player is considered ultra-aggresive/loose.
				else opp->om[1][round].history[opp->om[1][round].currentPointer] = 1;
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
