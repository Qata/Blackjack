enum SUITS
{
	SUIT_SPADES = 0,
	SUIT_HEARTS,
	SUIT_DIAMONDS,
	SUIT_CLUBS
};

char * getSuitChar(unsigned int suit);
int play(int * playerScore, int * dealerScore);
int lowestHand(int, int *);
