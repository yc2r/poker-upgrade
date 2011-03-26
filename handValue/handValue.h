/*
* Creation Date : 24-02-2011

* Purpose : Compute Hand Value

* Creation Date :

* Last Modified : Wed 02 Mar 2011 05:04:28 PM EST

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


int rankHand(Card* myCards, int numberOfCards);
		     
int computeHandStrength(State *state, int currentPlayer);

#endif
