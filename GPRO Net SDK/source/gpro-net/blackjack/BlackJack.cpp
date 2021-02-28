#include "gpro-net/blackjack/BlackJack.h"

BlackJack::BlackJack()
{
	gameDeck.resetDeck();
}

BlackJack::~BlackJack()
{
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
	cardStr = players[index].hand[0].toString();

	for (int i = 1; i < dealerHand.size(); ++i)
	{
		cardStr += ", " + players[index].hand[i].toString();
	}

	return cardStr;
}

void BlackJack::playerHit(int index)
{
	players[index].hand.push_back(gameDeck.drawCard());
}

void BlackJack::playerStand(int index)
{
	++playerTurnIndex;
}

int BlackJack::cardScore(std::vector<Card>& hand)
{
	int handScore = 0;
	bool hasAce = false;

	for (int x = 0; x < hand.size(); ++x)
	{
		switch (hand[x].cardValue)
		{
		case 'J':
		case 'Q':
		case 'K':
			handScore += 10;
			break;
		case 'A':
			hasAce = true;
			break;
		default:
			handScore += (hand[x].cardValue - '0');
			break;
		}
	}

	if ((handScore + 11) > 21)
	{
		handScore += 1;
	}
	else
	{
		handScore += 11;
	}

	return handScore;
}

bool BlackJack::handCheck(std::vector<Card>& hand)
{
	bool var = (cardScore(hand) <= 21);
	return var;
}