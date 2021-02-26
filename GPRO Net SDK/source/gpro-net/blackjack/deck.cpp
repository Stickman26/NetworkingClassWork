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
		cardStr += cardValue;
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
		cardStr += "Club";
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
		for (int i = 0; i < 14; ++i)
		{
			switch (i)
			{
			case 10:
				deck.push_back(Card(Suit(suit), 'J'));
			case 11:
				deck.push_back(Card(Suit(suit), 'Q'));
			case 12:
				deck.push_back(Card(Suit(suit), 'K'));
			case 13:
				deck.push_back(Card(Suit(suit), 'A'));
			default:
				deck.push_back(Card(Suit(suit),char(i)));
				break;
			}
		}
	}

	shuffle();
}
