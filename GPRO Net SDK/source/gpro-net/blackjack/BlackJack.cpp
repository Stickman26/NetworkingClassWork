#include "gpro-net/blackjack/BlackJack.h"

BlackJack::BlackJack()
{
	gameDeck.resetDeck();
}

BlackJack::~BlackJack()
{
	for (int i = 0 ; i < players.size() ; ++i)
	{
		Player* temp = players[i];
		delete temp;
		players[i] = nullptr;
	}

	players.clear();
	players.shrink_to_fit();
}

std::string BlackJack::displayDealerHand()
{
	if (dealerReveal)
	{
		std::string cardStr = "";
		cardStr = dealerHand[0].toString();

		for (int i = 1; i < dealerHand.size(); ++i)
		{
			cardStr += ", " + dealerHand[i].toString();
		}

		return cardStr;
	}
	else
	{
		return dealerHand[0].toString() + ", X\n";
	}
}

std::string BlackJack::displayPlayerHand(int index)
{
	std::string cardStr = "";
	//cardStr = players[index].[0].toString();

	for (int i = 1; i < dealerHand.size(); ++i)
	{
		cardStr += ", " + dealerHand[i].toString();
	}

	return cardStr;
}

void BlackJack::playerHit(int index)
{

}

void BlackJack::playerStand(int index)
{

}

bool BlackJack::handCheck(std::vector<Card>& hand)
{

}