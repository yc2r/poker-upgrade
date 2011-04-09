/*
* Creation Date : 24-02-2011

* Purpose : Compute Hand Value

* Creation Date :

* Last Modified : Wed 30 Mar 2011 11:57:33 PM EDT

* Created By : Yuchen Zhou

*/
#ifndef __handValue_H
#define __handValue_H

#include <stdio.h>
#include "../game.h"

enum SUITE{
	club,
	diamond,
	heart,
	spade
};

typedef struct{
	int rank;
	enum SUITE suite;
} Card;

typedef struct{
	float potOdds;			//pot odds: (the number of chips I need to put in the pot/total number of chips in the pot)/winning probability.
	int bucket;				//five current hand strength bucket abstractions, return value from 1 to 5, with 1 being the worst hand.
							//this actually already took into account of potentials. (EHS)
	float potential;		//pPot in the 22nd reference paper in poker review paper..
} Strength;

int rankHand(Card* myCards, int numberOfCards);
Strength computeHandStrength(State *state, int currentPlayer, int round);

#endif
