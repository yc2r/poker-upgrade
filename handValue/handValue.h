/*
* Creation Date : 24-02-2011

* Purpose : Compute Hand Value

* Creation Date :

* Last Modified : Mon 28 Mar 2011 12:14:56 PM EDT

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
	float potOdds;			//pot odds: (the number of chips I need to put in the pot/total number of chips in the pot)/winning probability
	int bucket;				//five bucket abstractions, return value from 1 to 5, with 1 being the worst hand.
} Strength;

int rankHand(Card* myCards, int numberOfCards);
		     
Strength computeHandStrength(State *state, int currentPlayer);

#endif
