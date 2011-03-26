/*
* Creation Date : 24-02-2011

* Purpose :

* Creation Date :

* Last Modified : Tue 08 Mar 2011 08:43:57 PM EST

* Created By :

*/

#include "handValue.h"
#include "../game.h"
#include "../evalHandTables"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char cardToChar (int rank)
{
	char *ret = (char *) malloc(sizeof(char)*2);
	if ((rank>0)&&(rank < 10)) sprintf(ret, "%d", rank);
	else if (rank == 10) return 'T';
	else if (rank == 11) return 'J';
	else if (rank == 12) return 'Q';
	else if (rank == 13) return 'K';
	else if (rank == 14) return 'A';
	return ret[0];
}

float computePreFlop(Card *myCards)
{
	//This value does not count what the opponent model is.
	//Using Billing's thesis' table:
	int suited = (myCards[0].suite == myCards[1].suite);
	//int pair = (myCards[0].rank == myCards[1].rank);
	char *cards = (char *) malloc(sizeof(char)*3);
	//Represent the cards using string.
	if (myCards[0].rank < myCards[1].rank)		//sort the hands
	{
		cards[0] = cardToChar(myCards[1].rank+2);
		cards[1] = cardToChar(myCards[0].rank+2);
		cards[2] = suited ? 's' : 'o';
	}
	else
	{
		cards[0] = cardToChar(myCards[0].rank+2);
		cards[1] = cardToChar(myCards[1].rank+2);
		cards[2] = suited ? 's' : 'o';
	}
	//Group1 hands:
	if ((strcmp(cards,"AAo"))==0||(strcmp(cards,"KKo"))==0||(strcmp(cards,"QQo"))==0||(strcmp(cards,"JJo"))==0||(strcmp(cards,"AKs"))==0)
	{
		return 1.0;		//maximum group
	}
	//Group2 hands:
	if ((strcmp(cards,"TTo"))==0||(strcmp(cards,"AQs"))==0||(strcmp(cards,"AJs"))==0||(strcmp(cards,"KQs"))==0||(strcmp(cards,"AKo"))==0)
	{
		return 0.95;
	}
	//Group3 hands:
	if ((strcmp(cards,"99o"))==0||(strcmp(cards,"JTs"))==0||(strcmp(cards,"QJs"))==0||(strcmp(cards,"KJs"))==0||(strcmp(cards,"ATs"))==0||(strcmp(cards,"ATo"))==0)
	{
		return 0.9;
	}
	//Group4 hands:
	if ((strcmp(cards,"T9s"))==0||(strcmp(cards,"KQo"))==0||(strcmp(cards,"88o"))==0||(strcmp(cards,"QTs"))==0||(strcmp(cards,"98s"))==0||(strcmp(cards,"J9s"))==0||(strcmp(cards,"AJo"))==0||(strcmp(cards,"KTs"))==0)
	{
		return 0.82;
	}
	//Group5 hands:
	if ((strcmp(cards,"77o"))==0||(strcmp(cards,"87s"))==0||(strcmp(cards,"Q9s"))==0||(strcmp(cards,"T8s"))==0||(strcmp(cards,"KJo"))==0||(strcmp(cards,"QJo"))==0||(strcmp(cards,"JTo"))==0||(strcmp(cards,"76s"))==0 \
	||(strcmp(cards,"97s"))==0||(strcmp(cards,"A9s"))==0||(strcmp(cards,"A8s"))==0||(strcmp(cards,"A7s"))==0||(strcmp(cards,"A6s"))==0||(strcmp(cards,"A5s"))==0||(strcmp(cards,"A4s"))==0||(strcmp(cards,"A3s"))==0|| \
	(strcmp(cards,"A2s"))==0||(strcmp(cards,"65s"))==0)
	{
		return 0.75;
	}
	//Group6 hands:
	if ((strcmp(cards,"66o"))==0||(strcmp(cards,"ATo"))==0||(strcmp(cards,"55o"))==0||(strcmp(cards,"86s"))==0||(strcmp(cards,"KTo"))==0||(strcmp(cards,"QTo"))==0||(strcmp(cards,"54s"))==0||(strcmp(cards,"K9s"))==0 \
	||(strcmp(cards,"J8s"))==0)
	{
		return 0.68;
	}
	//Group7 hands:
	if ((strcmp(cards,"44o"))==0||(strcmp(cards,"J9o"))==0||(strcmp(cards,"43s"))==0||(strcmp(cards,"75s"))==0||(strcmp(cards,"T9o"))==0||(strcmp(cards,"33o"))==0||(strcmp(cards,"98o"))==0||(strcmp(cards,"64s"))==0 \
	||(strcmp(cards,"22o"))==0||(strcmp(cards,"Q8s"))==0||(strcmp(cards,"K8s"))==0||(strcmp(cards,"K7s"))==0||(strcmp(cards,"K6s"))==0||(strcmp(cards,"K5s"))==0||(strcmp(cards,"K4s"))==0||(strcmp(cards,"K3s"))==0|| \
	(strcmp(cards,"K2s"))==0)
	{
		return 0.6;
	}
	//Group8 hands:
	if ((strcmp(cards,"87o"))==0||(strcmp(cards,"53s"))==0||(strcmp(cards,"A9o"))==0||(strcmp(cards,"Q9o"))==0||(strcmp(cards,"76o"))==0||(strcmp(cards,"42s"))==0||(strcmp(cards,"32s"))==0||(strcmp(cards,"96s"))==0 \
	||(strcmp(cards,"85s"))==0||(strcmp(cards,"J8o"))==0||(strcmp(cards,"J7s"))==0||(strcmp(cards,"65o"))==0||(strcmp(cards,"54o"))==0||(strcmp(cards,"74s"))==0||(strcmp(cards,"K9o"))==0||(strcmp(cards,"T8o"))==0)
	{
		return 0.51;
	}
	if ((strcmp(cards,"72o"))==0||(strcmp(cards,"62o"))==0||(strcmp(cards,"82o"))==0||(strcmp(cards,"83o"))==0||(strcmp(cards,"92o"))==0||(strcmp(cards,"62s"))==0||(strcmp(cards,"72s"))==0||(strcmp(cards,"82s"))==0||(strcmp(cards,"92s"))==0||(strcmp(cards,"83s"))==0){
		return 0.1;
	}
	else return 0.3;
}

int rankMyHand(Card* myCards, int numberOfCards)
{
	int i;
	Cardset c = emptyCardset();
	for(i = 0;i < numberOfCards;++i) 
	{
		addCardToCardset( &c, myCards[i].suite, myCards[i].rank);
	}
	return rankCardset(c);
}

static int isCardChoiceLegal(int rank, int suite, Card* myCards, int maxNumberOfCards)
{
	int i = 0;
	while (i < maxNumberOfCards)
	{
		if ((myCards[i].suite == suite)&&(myCards[i].rank == rank))
		{
			return 1;		//collision!
		}
		i++;
	}
	return 0;
}

int computeHandStrength(State *state, int currentPlayer)
{
	unsigned bucket = 0;				//final return value
	int i = 0;					//loop vars
	int j = 0;
	int k = 0;
	int l = 0;
	int win = 0;				//simulated number of wins
	int lose = 0;				//simulated number of loses
	int tie = 0;				//simulated number of ties
	int maxNumberOfCards = 7;	//number of all cards
	int handValue = -1;			//computing the current best 5 out of 7 and its rank
	int oppoValue = -1;
	float IHS = 0;				//Immediate Hand Strength
	Card *myCards = (Card *) (malloc(sizeof(Card)*maxNumberOfCards));		//counting post-flop maximum
	Card *oppoCards = (Card *) (malloc(sizeof(Card)*maxNumberOfCards));		//counting post-flop maximum
	for (i = 0; i < 2; i++)
	{
		myCards[i].rank = rankOfCard(state->holeCards[currentPlayer][i]);
		myCards[i].suite = suitOfCard(state->holeCards[currentPlayer][i]);
	}
	if (state->round == 0)			//pre-flop
	{
		IHS = computePreFlop(myCards);
		if (IHS>0.94)
		{
			bucket = 5;
		}
		else if (IHS>0.7)
		{
			bucket = 4;
		}
		else if (IHS>0.5)
		{
			bucket = 3;
		}
		else if (IHS>0.15)
		{
			bucket = 2;
		}
		else bucket = 1;
	}
	else							//post-flop
	{
		
		//Assign the community cards to respective slots
		for (i = 0; i < 5; i++)
		{
			myCards[i+2].rank = rankOfCard(state->boardCards[i]);
			myCards[i+2].suite = suitOfCard(state->boardCards[i]);
			oppoCards[i+2].rank = rankOfCard(state->boardCards[i]);
			oppoCards[i+2].suite = suitOfCard(state->boardCards[i]);
		}
		//what's my current rank?
		handValue = rankMyHand(myCards, 7);
		
		//traverse through all possible opponent hands:
		for (i = 0; i < 13; i++){
			for (j = 0; j < 4; j++){		//suit number is 4
				//check if the card is different from revealed cards
				if (isCardChoiceLegal(i,j,myCards,maxNumberOfCards)) continue;
				//assign this card to the first opponent slot
				oppoCards[0].rank = i;
				oppoCards[0].suite = j;
				//do it again, we acutally didn't use the combination (/2), so that performance might be improved by doing that.
				for (k = 0; k < 13; k++){
					for (l = 0; l < 4; l++){
						//check if the card is different from revealed cards
						if (isCardChoiceLegal(k,l,myCards,maxNumberOfCards)) continue;
						if ((k==i)&&(l==j)) continue;				//Opponent cannot have two same cards!
						//assign this card to the first opponent slot
						oppoCards[1].rank = k;
						oppoCards[1].suite = l;
						//by the end of this loop we simulated the opponent cards.
						oppoValue = rankMyHand(oppoCards, 7);
						if (oppoValue > handValue) lose++;
						else if (oppoValue < handValue) win++;
						else tie++;
					}
				}
			}
		}
		IHS = (float)(win + (tie/2)) / (float)(win + tie + lose);
		if (IHS > 0.87) bucket = 5;
		else if (IHS > 0.72) bucket = 4;
		else if (IHS > 0.53) bucket = 3;
		else if (IHS > 0.3) bucket = 2;
		else bucket = 1;
	}
	/*if (state->round>0)
	{
		fprintf(fp,"This is from player %d:, betting round %d.\n", currentPlayer, state->round);
		fprintf(fp,"My first hole card is %d, suite is %d; Second hole card is %d, suite is %d\n", myCards[0].rank, myCards[0].suite, myCards[1].rank, myCards[1].suite);
		fprintf(fp, "community cards are: %d, %d, %d, %d, %d\n", rankOfCard(state->boardCards[0]), rankOfCard(state->boardCards[1]), rankOfCard(state->boardCards[2]), rankOfCard(state->boardCards[3]), rankOfCard(state->boardCards[4]));
		fprintf(fp, "suits are: %d, %d, %d, %d, %d\n", suitOfCard(state->boardCards[0]),suitOfCard(state->boardCards[1]),suitOfCard(state->boardCards[2]),suitOfCard(state->boardCards[3]),suitOfCard(state->boardCards[4]));
		fprintf(fp,"Post-flop hand strength is: %f\n\n", bucket);
	}
	fclose(fp);
	//IHS is derived. Now we compute EHS:
	//For wv limit poker, we don't have to worry about EHS.
	FILE *fp;
	fp = fopen("output.txt","a+");
	fprintf(fp,"This is from player %d:, betting round %d.\n", currentPlayer, state->round);
	fprintf(fp,"My first hole card is %d, suite is %d; Second hole card is %d, suite is %d\n", myCards[0].rank, myCards[0].suite, myCards[1].rank, myCards[1].suite);
	if (state->round == 0)
	{
		fprintf(fp,"Pre-flop hand strength is: %d\n", bucket);
	}
	if (state->round > 0)
	{
		fprintf(fp, "community cards are: %d, %d, %d, %d, %d\n", rankOfCard(state->boardCards[0]), rankOfCard(state->boardCards[1]), rankOfCard(state->boardCards[2]), rankOfCard(state->boardCards[3]), rankOfCard(state->boardCards[4]));
		fprintf(fp,"Post-flop hand strength is: %d\n", bucket);
	}
	fprintf(fp, "--------------------------------\n");
	fclose(fp);*/
	return bucket;
}

