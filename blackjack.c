#include <stdlib.h>
#include <stdio.h>
#include <ncurses.h>
#include <time.h>
#include <math.h>
#include "blackjack.h"

const int MAX_TURNS = 5;

int getCardSuit(int card)
{

	return ceil((card + 1) / 13.) - 1;
}

char * getCardStr(int card)
{
	char val = 0;
	static char cardStr[0x10];
	switch((card % 13) + 1)
	{
		case 1:
			val = 'A';
			break;
		case 11:
			val = 'J';
			break;
		case 12:
			val = 'Q';
			break;
		case 13:
			val = 'K';
			break;
		default:
			sprintf(cardStr, "%u", (card % 13) + 1);
			return cardStr;
	}
	sprintf(cardStr, "%c", val);
	return cardStr;
}

void shuffleDeck(int * cards)
{
	for (unsigned i = 0; i < 52; ++i)
	{
		int r = rand() % 52;
		cards[i] ^= cards[r];
		cards[r] ^= cards[i];
		cards[i] ^= cards[r];
	}
}

void initDeck(int * cards)
{
	for (unsigned i = 0; i < 52; ++i)
	{
		cards[i] = i;
	}
	shuffleDeck(cards);
}

int dealCard(int * cardsInDeck, int * cards)
{
	if (cardsInDeck && *cardsInDeck > 0)
	{
		return cards[--*cardsInDeck];
	}
	return -1;
}

int getCardValue(int card)
{
	return (card % 13) + 1;
}

int getCardPoints(int card)
{
	if (card > 10)
	{
		return 10;
	}
	return card;
}

int checkHand(int nCards, int * cards)
{
	int nAces = 0;
	for (int i = 0; i < nCards; i++)
	{
		nAces += getCardValue(cards[i]) == 1;
	}

	for (int i = 0; i <= nAces; i++)
	{
		int score = (nAces - i) + i * 11;
		if (score != 21 || nAces != nCards)
		{
			for (int j = 0; j < nCards; j++)
			{
				if (getCardValue(cards[j]) != 1)
				{
					score += getCardPoints(getCardValue(cards[j]));
				}
			}
		}

		if (score == 21)
		{
			return 1;
		}
	}
	return 0 - (lowestHand(nCards, cards) > 21);
}

int highestNonBustHand(int nCards, int * cards)
{
	int nAces = 0;
	for (int i = 0; i < nCards; i++)
	{
		nAces += getCardValue(cards[i]) == 1;
	}

	int retScore = 0;
	for (int i = 0; i <= nAces; i++)
	{
		int score = (nAces - i) + i * 11;
		if (score != 21 || nAces != nCards)
		{
			for (int j = 0; j < nCards; j++)
			{
				if (getCardValue(cards[j]) != 1)
				{
					score += getCardPoints(getCardValue(cards[j]));
				}
			}
		}
		if (retScore < score && score <= 21)
		{
			retScore = score;
		}
	}
	return retScore;
}

int highestHand(int nCards, int * cards)
{
	int nAces = 0;
	for (int i = 0; i < nCards; i++)
	{
		nAces += getCardValue(cards[i]) == 1;
	}
	int score = nAces * 11;
	if (score != 21 || nAces != nCards)
	{
		for (int j = 0; j < nCards; j++)
		{
			if (getCardValue(cards[j]) != 1)
			{
				score += getCardPoints(getCardValue(cards[j]));
			}
		}
	}
	return score;
}

int lowestHand(int nCards, int * cards)
{
	int nAces = 0;
	for (int i = 0; i < nCards; i++)
	{
		nAces += getCardValue(cards[i]) == 1;
	}
	int score = nAces;
	if (score != 21 || nAces != nCards)
	{
		for (int j = 0; j < nCards; j++)
		{
			if (getCardValue(cards[j]) != 1)
			{
				score += getCardPoints(getCardValue(cards[j]));
			}
		}
	}
	return score;
}

void printCards(int nCards, int * cards)
{
	return;
	for (int i = 0; i < nCards; i++)
	{
		printf("%s  ", getCardStr(cards[i]));
	}
	puts("");
}

int dealerShouldHit(int nCards, int * cards)
{
	return nCards < MAX_TURNS && !checkHand(nCards, cards) && highestNonBustHand(nCards, cards) < 17 && highestNonBustHand(nCards, cards) > 0;
}

void dealerTurn(int * cardsInDeck, int * cards, int * nDealerCards, int * dealerCards, int * playerTurn)
{
	*playerTurn = 0;
	while (dealerShouldHit(*nDealerCards, dealerCards))
	{
		dealerCards[(*nDealerCards)++] = dealCard(cardsInDeck, cards);
	}
}

int declareWinner(int nPlayerCards, int * playerCards, int nDealerCards, int * dealerCards, int screenHeight, int * playerScore, int * dealerScore, int * scoreAmended)
{
	if (highestNonBustHand(nPlayerCards, playerCards) < highestNonBustHand(nDealerCards, dealerCards))
	{
		mvprintw(screenHeight - 1, 0, "You suddenly remember how much it sucks to lose.");
		if (!*scoreAmended)
		{
			*scoreAmended = 1;
			++(*dealerScore);
		}
	}
	else if (highestNonBustHand(nPlayerCards, playerCards) > highestNonBustHand(nDealerCards, dealerCards))
	{
		mvprintw(screenHeight - 1, 0, "You sit back and take a nice sip from your martini. It sure feels great to win.");
		if (!*scoreAmended)
		{
			*scoreAmended = 1;
			++(*playerScore);
		}
	}
	else
	{
		mvprintw(screenHeight - 1, 0, "Neither you nor the Dealer win. You get a push.");
	}
	return 0;
}

void printHeart(int y, int x)
{
	mvprintw(y, x,     " _  _");
	mvprintw(y + 1, x, "| \\/ |");
	mvprintw(y + 2, x, " \\  /");
	mvprintw(y + 3, x, "  \\/");
}

void printDiamond(int y, int x)
{
	mvprintw(y, x + 2,      "/\\");
	mvprintw(y + 1, x + 1, "/  \\");
	mvprintw(y + 2, x + 1, "\\  /");
	mvprintw(y + 3, x + 2,  "\\/");
}

void printClub(int y, int x)
{
	mvprintw(y, x + 2,      "/\\");
	mvprintw(y + 1, x ,  "/\\\\//\\");
	mvprintw(y + 2, x,  "\\/||\\/");
	mvprintw(y + 3, x + 2,  "||");
}

void printSpade(int y, int x)
{
	mvprintw(y, x + 2,      "/\\");
	mvprintw(y + 1, x + 1,  "/  \\");
	mvprintw(y + 2, x + 1,  "\\__/");
	mvprintw(y + 3, x + 1,  "_||_");
}

void drawCard(int y, int x, char * value, int type)
{
	init_pair(1, COLOR_RED, COLOR_WHITE);
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	init_pair(3, COLOR_BLACK, COLOR_GREEN);
	attron(COLOR_PAIR(3));
	mvprintw(y, x, "__________");
	attroff(COLOR_PAIR(3));
	attron(COLOR_PAIR(2));

	for (int i = 1; i < 7; i++)
	{
		mvprintw(y + i, x, "|        |");
	}
	mvprintw(y + 7, x, "|________|");

	mvprintw(y + 6, x + 4, value);
	attroff(COLOR_PAIR(2));

	switch (type)
	{
		case SUIT_HEARTS:
			attron(COLOR_PAIR(1));
			printHeart(y + 1, x + 2);
			break;
		case SUIT_DIAMONDS:
			attron(COLOR_PAIR(1));
			printDiamond(y + 1, x + 2);
			break;
		case SUIT_SPADES:
			attron(COLOR_PAIR(2));
			printSpade(y + 1, x + 2);
			break;
		case SUIT_CLUBS:
			attron(COLOR_PAIR(2));
			printClub(y + 1, x + 2);
			break;
	}
	attroff(COLOR_PAIR(1) | COLOR_PAIR(2));
}

void drawFaceDownCard(int y, int x)
{
	init_pair(2, COLOR_BLACK, COLOR_WHITE);
	init_pair(3, COLOR_BLACK, COLOR_GREEN);
	attron(COLOR_PAIR(3));
	mvprintw(y, x, "__________");
	attroff(COLOR_PAIR(3));
	attron(COLOR_PAIR(2));

	for (int i = 1; i < 7; i++)
	{
		mvprintw(y + i, x, "|        |");
	}
	mvprintw(y + 7, x, "|________|");
	attroff(COLOR_PAIR(2));
}

void incrementScore(int * score, int * scoreAmended)
{
	if (!*scoreAmended)
	{
		*scoreAmended = 1;
		++(*score);
	}
}

int play(int * playerScore, int * dealerScore)
{
	int playerTurn = 1;
	int nPlayerCards = 0, nDealerCards = 0;
	int playerCards[MAX_TURNS], dealerCards[MAX_TURNS];

	int cardsInDeck = 52;
	int cards[cardsInDeck];
	initDeck(cards);
	shuffleDeck(cards);

	WINDOW * screen = stdscr;
    curs_set(0);
	noecho();
    keypad(stdscr, TRUE);

	if (nodelay(screen, TRUE) == ERR)
	{
		return 1;
	}

	if (!start_color())
	{
		int run = 1;

		int width = 0;
		int height = 0;

		init_pair(12, COLOR_BLACK, COLOR_GREEN);
		wbkgd(screen, COLOR_PAIR(12));
		//wcolor_set(screen, COLOR_PAIR(12), NULL);
		for (int i = 0; i < 2; i++)
		{
			playerCards[nPlayerCards++] = dealCard(&cardsInDeck, cards);
		}

		for (int i = 0; i < 2; i++)
		{
			dealerCards[nDealerCards++] = dealCard(&cardsInDeck, cards);
		}

		int scoreAmended = 0;
		int character = 0;
		while (run)
		{
			getmaxyx(screen, height, width);
			if ((character = getch()) != ERR)
			{
				switch (character)
				{
					case 'q':
						return 0;
					case 'r':
						return 1;
					case 's':
						if (playerTurn)
						{
							dealerTurn(&cardsInDeck, cards, &nDealerCards, dealerCards, &playerTurn);
						}
						break;
					case 'h':
						if (playerTurn && nPlayerCards < MAX_TURNS)
						{
							playerCards[nPlayerCards++] = dealCard(&cardsInDeck, cards);
							if (nPlayerCards == 5)
							{
								if (checkHand(nPlayerCards, playerCards) >= 0)
								{
									dealerTurn(&cardsInDeck, cards, &nDealerCards, dealerCards, &playerTurn);
								}
							}
						}
						break;
				}
			}
			erase();

			for (int i = 0; i < nPlayerCards; i++)
			{
				drawCard(height - 11, i * 12, getCardStr(playerCards[i]), getCardSuit(playerCards[i]));
			}

			drawCard(3, 0, getCardStr(dealerCards[0]), getCardSuit(dealerCards[0]));
			for (int i = 1; i < nDealerCards; i++)
			{
				if ((!playerTurn && checkHand(nPlayerCards, playerCards) >= 0))
				{
					drawCard(3, i * 12, getCardStr(dealerCards[i]), getCardSuit(dealerCards[i]));
				}
				else
				{
					drawFaceDownCard(3, i * 12);
				}
			}

			mvprintw(0, 0, "Press 'q' to exit, press 'r' to reset");
			mvprintw(1, 0, "You: %i", *playerScore);
			mvprintw(2, 0, "Dealer: %i", *dealerScore);

			if (checkHand(nDealerCards, dealerCards) < 0)
			{
				mvprintw(height - 1, 0, "The Dealer has gone bust, you win.");
				playerTurn = 0;
				incrementScore(playerScore, &scoreAmended);
			}
			else if (checkHand(nPlayerCards, playerCards) > 0)
			{
				dealerTurn(&cardsInDeck, cards, &nDealerCards, dealerCards, &playerTurn);
				declareWinner(nPlayerCards, playerCards, nDealerCards, dealerCards, height, playerScore, dealerScore, &scoreAmended);
			}
			else if (checkHand(nPlayerCards, playerCards) < 0)
			{
				mvprintw(height - 1, 0, "You suddenly remember how to count and realise you've gone bust.");
				incrementScore(dealerScore, &scoreAmended);
				playerTurn = 0;
			}
			else if (!playerTurn)
			{
				declareWinner(nPlayerCards, playerCards, nDealerCards, dealerCards, height, playerScore, dealerScore, &scoreAmended);
			}
			if (playerTurn)
			{
				mvprintw(height - 2, 0, "Type 's' to stay or 'h' to hit");
			}
			refresh();
		}
	}
	return 0;
}
