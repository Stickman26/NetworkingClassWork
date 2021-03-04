/*
Code implemented by:
Lansingh Freeman
Jason Gold
*/
#include "gpro-net/blackjack/deck.h"

std::string Card::toString()
{
	std::string cardStr = "";

	switch (cardValue)
	{
	case '0':
		cardStr += "10";
		break;
	case 'J':
		cardStr += "Jack";
		break;
	case 'Q':
		cardStr += "Queen";
		break;
	case 'K':
		cardStr += "King";
		break;
	case 'A':
		cardStr += "Ace";
		break;
	default:
		cardStr.push_back(cardValue);
		break;
	}

	cardStr += " of ";

	switch (cardSuit)
	{
	case Suit::Spade:
		cardStr += "Spades";
		break;
	case Suit::Diamond:
		cardStr += "Diamonds";
		break;
	case Suit::Heart:
		cardStr += "Hearts";
		break;
	case Suit::Club:
		cardStr += "Clubs";
		break;
	}

	return cardStr;
}

Deck::Deck()
{
	resetDeck();
}

Deck::~Deck()
{
	deck.clear();
	deck.shrink_to_fit();
}

void Deck::shuffle()
{
	srand((unsigned)time(0));
	for (int i = 0; i < 52; i++)
		std::swap(deck[i], deck[rand() % 52]);
}

Card Deck::drawCard()
{
	Card drew = deck.front();
	deck.erase(deck.begin());
	return drew;
}

void Deck::resetDeck()
{
	deck.clear();
	deck.shrink_to_fit();

	for (int suit = 0 ; suit < 4 ; ++suit)
	{
		for (int i = 0; i < 13; ++i)
		{
			switch (i)
			{
			case 1:
				deck.push_back(Card(Suit(suit), 'A'));
				break;
			case 10:
				deck.push_back(Card(Suit(suit), 'J'));
				break;
			case 11:
				deck.push_back(Card(Suit(suit), 'Q'));
				break;
			case 12:
				deck.push_back(Card(Suit(suit), 'K'));
				break;
			default:
				deck.push_back(Card(Suit(suit),char(i + '0')));
				break;
			}
		}
	}

	shuffle();
}
