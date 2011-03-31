/*
* Creation Date : 24-02-2011

* Purpose :

* Creation Date :

* Last Modified : Thu 31 Mar 2011 12:10:56 PM EDT

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

Strength computeHandStrength(State *state, int currentPlayer)
{
	unsigned bucket = 0;				//final return value
	int i = 0;					//loop vars
	int j = 0;
	int k = 0;
	int l = 0;
	int m = 0;
	int n = 0;
	int win = 0;				//simulated number of wins
	int lose = 0;				//simulated number of loses
	int tie = 0;				//simulated number of ties
	int tie2win = 0;				//potential tie to wins
	int lose2win = 0;				//potential lose to wins
	int lose2tie = 0;				//potential lose to ties
	int maxNumberOfCards = 0;	//number of all cards
	int handValue = -1;			//computing the current best 5 out of 7 and its rank
	int oppoValue = -1;
	int handValueP = -1;		//used to compute potentials
	int oppoValueP = -1;		
	int remainingCards = 48 - state->round - 2;
	float IHS = 0.0;				//Immediate Hand Strength
	float EHS = 0.0;
	float pPot = 0.0;
	int potTotal = 0;
	int betToCall = 0;
	for (i = 0; i < 3; i++)		//3 is the number of total players.
	{
		potTotal += state->spent[i];
		betToCall = (state->spent[i]>betToCall)?state->spent[i]:betToCall;		//get the max spent.
	}
	betToCall -= state->spent[currentPlayer];		//get the chips required to call, could equal to zero.
	Card *myCards = (Card *) (malloc(sizeof(Card)*7));		//counting post-flop maximum
	Card *oppoCards = (Card *) (malloc(sizeof(Card)*7));		//counting post-flop maximum
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
		maxNumberOfCards = 4 + state->round;		//total number of used cards (for opponent hole cards enumeration)
		//Assign the community cards to respective slots
		for (i = 0; i < 2 + state->round; i++)
		{
			myCards[i+2].rank = rankOfCard(state->boardCards[i]);
			myCards[i+2].suite = suitOfCard(state->boardCards[i]);
			oppoCards[i+2].rank = rankOfCard(state->boardCards[i]);
			oppoCards[i+2].suite = suitOfCard(state->boardCards[i]);
		}
		int previousWinning = 0;
		//what's my current rank?
		handValue = rankMyHand(myCards, maxNumberOfCards);
		
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
						oppoValue = rankMyHand(oppoCards, maxNumberOfCards);
						if (oppoValue > handValue) 
						{
							lose++;
							previousWinning = -1;
						}
						else if (oppoValue < handValue) 
						{
							win++;
							previousWinning = 1;
						}
						else 
						{
							tie++;
							previousWinning = 0;
						}
						//done computing IHS, now compute pPot.
						if (state->round == 1||state->round == 2)		//flop or river. we need to compute potentials
						{
							for (m = 0; m < 13; m++){
								for (n = 0; n < 4; n++){
									if (isCardChoiceLegal(m,n,myCards,maxNumberOfCards)) continue;
									if ((m==i)&&(n==j)) continue;				//Community cards cannot be the same cards as opponent's hole cards!
									if ((m==k)&&(n==l)) continue;				//same as above!
									myCards[4 + state->round].rank = m;
									myCards[4 + state->round].suite = n;
									oppoCards[4 + state->round].rank = m;
									oppoCards[4 + state->round].suite = n;
									//by the end of this loop we simulated the opponent cards.
									handValueP = rankMyHand(myCards, maxNumberOfCards+1);
									oppoValueP = rankMyHand(oppoCards, maxNumberOfCards+1);
									if (oppoValueP < handValueP && previousWinning == -1) lose2win++;
									if (oppoValueP < handValueP && previousWinning == 0) tie2win++;
									if (oppoValueP == handValueP && previousWinning == -1) lose2tie++;
									//if our bot played too many hands, we will consider adding negative potentials.
								}
							}
						}
					}
				}
			}
		}
		if (lose!=0)
		pPot = ((float)lose2win/lose)/remainingCards + (((float)lose2tie/lose)/remainingCards)/2;
		if (tie!=0) 
		pPot += (((float)tie2win/tie)/remainingCards)/2;
		IHS = (float)(win + (tie/2)) / (float)(win + tie + lose);
		EHS = IHS + (1 - IHS) * pPot;
		if (EHS > 0.89) bucket = 5;
		else if (EHS > 0.75) bucket = 4;
		else if (EHS > 0.58) bucket = 3;
		else if (EHS > 0.38) bucket = 2;
		else bucket = 1;
	}
	//bucket indicates the current hand strength.

	/*if (state->round>0)
	{
		fprintf(fp,"This is from player %d:, betting round %d.\n", currentPlayer, state->round);
		fprintf(fp,"My first hole card is %d, suite is %d; Second hole card is %d, suite is %d\n", myCards[0].rank, myCards[0].suite, myCards[1].rank, myCards[1].suite);
		fprintf(fp, "community cards are: %d, %d, %d, %d, %d\n", rankOfCard(state->boardCards[0]), rankOfCard(state->boardCards[1]), rankOfCard(state->boardCards[2]), rankOfCard(state->boardCards[3]), rankOfCard(state->boardCards[4]));
		fprintf(fp, "suits are: %d, %d, %d, %d, %d\n", suitOfCard(state->boardCards[0]),suitOfCard(state->boardCards[1]),suitOfCard(state->boardCards[2]),suitOfCard(state->boardCards[3]),suitOfCard(state->boardCards[4]));
		fprintf(fp,"Post-flop hand strength is: %f\n\n", bucket);
	}
	fclose(fp);*/
	Strength returnStrength;
	returnStrength.bucket = bucket;
	if (potTotal!=0)
	{
		returnStrength.potOdds = ((float)betToCall)/potTotal;
	}
	else
	{
		printf("error! pot size = 0, cannot get potOdds");
		exit(0);
	}
	returnStrength.potential = pPot;
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
		fprintf(fp, "community cards are: %d of %d, %d of %d, %d of %d", rankOfCard(state->boardCards[0]), suitOfCard(state->boardCards[0]), rankOfCard(state->boardCards[1]), suitOfCard(state->boardCards[1]), rankOfCard(state->boardCards[2]), suitOfCard(state->boardCards[2]));
		if (state->round == 2) fprintf(fp, ", %d of %d", rankOfCard(state->boardCards[3]), suitOfCard(state->boardCards[3]));
		if (state->round == 3) fprintf(fp, ", %d of %d", rankOfCard(state->boardCards[4]), suitOfCard(state->boardCards[4]));
		fprintf(fp, "\n");
		fprintf(fp,"Post-flop hand strength is: %d\n", bucket);
		fprintf(fp,"Post-flop lose2win is: %d\n", lose2win);
		fprintf(fp,"Post-flop hand potential is %f\n", pPot);
		fprintf(fp,"Post-flop EHS is %f\n", EHS);
		fprintf(fp,"Post-flop IHS is %f\n", IHS);
		fprintf(fp,"betToCall is %d\n", betToCall);
		fprintf(fp,"potTotal is %d\n", potTotal);
	}
	fprintf(fp, "--------------------------------\n");
	fclose(fp);
	//Now we use heuristics to compute potentials
	return returnStrength;
}

