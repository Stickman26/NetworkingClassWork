#pragma once
#include "deck.h"

struct Player
{
	std::string playerName;
	std::vector<Card> hand;
};

class BlackJack
{
private:
	Deck gameDeck;
	std::vector<Card> dealerHand;
	std::vector<Player> players;

	int numberOfPlayers = 0;
	int playerTurnIndex = 0;
	bool dealerReveal = false;

	//Helper Functions
	int cardScore(std::vector<Card>& hand);

	//Visual Functions
	std::string displayDealerHand();
	std::string displayPlayerHand(int index);

public:
	BlackJack();
	~BlackJack();

	//Game functions
	void resetBlackJackGame(std::vector<std::string> names);
	std::string displayGameState();

	//Player Functions
	void currentPlayerHit();
	void currentPlayerStand();

	//Back End
	bool handCheck(std::vector<Card>& hand);
};