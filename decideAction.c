//not sure if this is what we want
//Parameter assumptions:
//position=0: Me, Player1, Player2 I am dealer
//position=1: Player1,Me,Player2 I am small blind
//position=2: Player1.Player2.Me I am big blind

//Interfaces:
//need:
//int getplayerType(round, playerID)
//handStrength object

#include "decideAction.h"
//add necessary head files


int decideAction(Opponents *opp, State* state, int position, int selfID, int player1ID, int player2ID, int handStrength)
{
	int round = state->round;
	int player1Type = getOpponentModel(opp, player1ID, round);//getplayerType(round,player1ID);
	int player2Type = getOpponentModel(opp, player2ID, round);//getplayerType(round,player2ID);
	int revisedHS;
	int player1Raise = 0;
	int player1Call = 0;
	int player2Raise = 0;
	int player2Call = 0;
	int isFirstaction;					//when it is 1, dealerpenalty will be considered, otherwise not
	//double randNumber;
	int i,j;

	for (i=0;i<=round;i++)
	{
		//TOCONFIRM: numActions-1 or not?
		for (j=0;j<state->numActions[i];j++)
		{
			if (state->actingPlayer[i][j] == player1ID)
			{
				if ((state->action[i][j]).type == 2)
					player1Raise++;
				
				if ((state->action[i][j]).type == 1)
					player1Call++;
			}
			if (state->actingPlayer[i][j] == player2ID)
			{
				if ((state->action[i][j]).type == 2)
					player2Raise++;
				
				if ((state->action[i][j]).type == 1)
					player2Call++;
			}
		}
	}

	if (state->numActions[round] <= 2)
		isFirstaction = 0;
	else
		isFirstaction = 1;

	if (position == 0)
	{
		if (round == 0)				//preflop
		{
			if (isFirstaction)
				revisedHS = handStrength - DS_DP - player1Type * (player1Raise * DS_Cr + player1Call * DS_Cc) - player2Type * (player2Raise * DS_Cr + player2Call * DS_Cr) - (player1Type + player2Type) * DS_Cundo;
			else
				revisedHS = handStrength - player1Type * (player1Raise * DS_Cr + player1Call * DS_Cc) - player2Type * (player2Raise * DS_Cr + player2Call * DS_Cr) - (player1Type + player2Type) * DS_Cundo;
		}
		else						//postflop
		{
			revisedHS = handStrength - player1Type * (player1Raise * DS_Cr + player1Call * DS_Cc) - player2Type * (player2Raise * DS_Cr + player2Call * DS_Cr);
		}

	}
	else if (position == 1)			//SB
	{

		if (round == 0)				//preflop
		{
			if (isFirstaction)
				revisedHS = handStrength - DS_SP - player1Type * (player1Raise * DS_Cr + player1Call * DS_Cc) - player2Type * (player2Raise * DS_Cr + player2Call * DS_Cr) - player2Type * DS_Cundo;
			else
				revisedHS = handStrength - player1Type * (player1Raise * DS_Cr + player1Call * DS_Cc) - player2Type * (player2Raise * DS_Cr + player2Call * DS_Cr) - player2Type * DS_Cundo;
		}
		else						//postflop
		{
			revisedHS = handStrength - player1Type * (player1Raise * DS_Cr + player1Call * DS_Cc) - player2Type * (player2Raise * DS_Cr + player2Call * DS_Cr) - (player1Type + player2Type) * DS_Cundo;
		}

	}
	else if (position == 2)			//BB
	{
		
		if (round == 0)				//preflop
		{
			if (isFirstaction)
				revisedHS = handStrength - DS_BP - player1Type * (player1Raise * DS_Cr + player1Call * DS_Cc) - player2Type * (player2Raise * DS_Cr + player2Call * DS_Cr);
			else
				revisedHS = handStrength - player1Type * (player1Raise * DS_Cr + player1Call * DS_Cc) - player2Type * (player2Raise * DS_Cr + player2Call * DS_Cr);
		}
		else						//postflop
		{
			revisedHS = handStrength - player1Type * (player1Raise * DS_Cr + player1Call * DS_Cc) - player2Type * (player2Raise * DS_Cr + player2Call * DS_Cr) - player1Type * DS_Cundo;
		}
	}
	else
		fprintf(stderr,"Wrong position input!\n");
	
	if (revisedHS > DS_THr)
		return 2;
	else if (revisedHS < DS_THc)
		return 0;
	else
		return 1;	

}
