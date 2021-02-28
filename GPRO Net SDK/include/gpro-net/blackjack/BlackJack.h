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
	bool handCheck(std::vector<Card>& hand) { return (cardScore(hand) <= 21); };
	bool checkBlackJack(std::vector<Card>& hand) { return (cardScore(hand) == 21); };

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

	//Public Helpers
	bool currentHandCheck() { return handCheck(players[playerTurnIndex].hand); };
	bool currentHandBlackJackCheck() { return checkBlackJack(players[playerTurnIndex].hand); };

	std::string getCurrentPlayerName() { return players[playerTurnIndex].playerName; }
};