/*
Code implemented by:
Lansingh Freeman
Jason Gold
*/
#pragma once

#include<iostream>
#include<vector>
#include<stdlib.h>
#include<time.h>

enum Suit
{
	Spade = 0,
	Diamond,
	Heart,
	Club
};

class Card
{
public:
	Suit cardSuit;
	char cardValue;

	Card(Suit suit, char val)
	{
		cardSuit = suit;
		cardValue = val;
	}

	std::string toString();
};

class Deck
{
private:
	std::vector<Card> deck;

public:
	Deck();
	~Deck();

	void shuffle();
	Card drawCard();
	void resetDeck();
};