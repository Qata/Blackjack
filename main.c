#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include <ncurses.h>
#include "blackjack.h"

#define ever ;;

int main(int argc, char ** argv)
{
	long * seed1 = malloc(1);
	long seed2;
	srand((unsigned)((long)seed1 ^ seed2 ^ time(NULL)));
	free(seed1);
	int playerScore = 0;
	int dealerScore = 0;
	initscr();
	while(play(&playerScore, &dealerScore));
	endwin();
	return 0;
}
