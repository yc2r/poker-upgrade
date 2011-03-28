//not sure if this is what we want
//Parameter assumptions:
//position=0: Me, Player1, Player2
//position=1: Player1,Me,Player2
//position=2: Player1.Player2.Me

//Interfaces:
//need:
//int getplayerType(round, playerID)
//handStrength object

#include "stdio.h"
#include "stdlib.h"
#include "time.h"
//add necessary head files


int decideAction(int round, int position, int player1ID, int player2ID,HandStrength handStrength)
{
	int player1Type = getplayerType(round,player1ID);
	int player2Type = getplayerType(round,player2ID);
	double randNumber;

	if (position == 0)
	{
		//switch player1type
		//switch player2type
		//randNumber = random number
		//our strategy
	}
	else if (position == 1)
		//TODO: same
	else if (position == 2)
		//TODO: same
	else
		fprintf(stderr,"Wrong position input!\n");

}
