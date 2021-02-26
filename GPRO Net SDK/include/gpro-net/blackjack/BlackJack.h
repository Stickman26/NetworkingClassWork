#pragma once
#include "player.h"

class BlackJack
{
private:
	Deck gameDeck;
	std::vector<Card> dealerHand;
	std::vector<Player*> players;

	int playerTurnIndex = 0;
	bool dealerReveal = false;

public:
	BlackJack();
	~BlackJack();

	//Visual Functions
	std::string displayDealerHand();
	std::string displayPlayerHand(int index);

	//Player Functions
	void playerHit(int index);
	void playerStand(int index);

	//Back End
	bool handCheck(std::vector<Card>& hand);
};