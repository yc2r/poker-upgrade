CC = gcc
#CFLAGS = -DDEBUG  -DNODEPAR -g  -Wall
CFLAGS = -Wall -g

#all:	dealer example_player my_player
#improved_player
all:	dealer example_player yuchen_bot 

dealer: game.c game.h evalHandTables rng.c rng.h dealer.c
	$(CC) $(CFLAGS) -o $@ game.c rng.c dealer.c


example_player: game.c game.h evalHandTables rng.c rng.h example_player.c 
	$(CC) $(CFLAGS) -o $@ game.c rng.c example_player.c 

yuchen_bot: game.c game.h evalHandTables rng.c rng.h handValue/handValue.h handValue/handValue.c OM/OM.h OM/OM.c decideAction.h decideAction.c yuchen_bot.c 
	$(CC) $(CFLAGS) -o $@ game.c rng.c yuchen_bot.c handValue/handValue.c OM/OM.c decideAction.c
	

#improved_player: game.c game.h evalHandTables rng.c rng.h improved_player.c handValue/handValue.h handValue/handValue.c
#	$(CC) $(CFLAGS) -o $@ game.c rng.c improved_player.c handValue/handValue.c

#my_player: game.c game.h evalHandTables rng.c rng.h my_player.c opp.c opp.h handValue/handValue.h handValue/handValue.c gameTree/gameTree.h gameTree/gameTree.c
#	$(CC) $(CFLAGS) -o $@ game.c rng.c my_player.c opp.c handValue/handValue.c gameTree/gameTree.c

#test_player: game.c game.h evalHandTables rng.c rng.h test_player.c opp.c opp.h handValue/handValue.h handValue/handValue.c 
#	$(CC) $(CFLAGS) -o $@ game.c rng.c test_player.c opp.c handValue/handValue.c 

clean:
#	rm -f dealer example_player my_player
	rm -f dealer example_player yuchen_bot
