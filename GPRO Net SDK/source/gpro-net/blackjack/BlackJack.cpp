/*
Code implemented by:
Lansingh Freeman
Jason Gold
*/
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

void BlackJack::resetBlackJackGame(std::vector<std::string> names)
{
	//reload deck
	gameDeck.resetDeck();

	//clear up dealer
	dealerHand.clear();
	dealerHand.shrink_to_fit();
	dealerReveal = false;

	//clear up players
	players.clear();
	players.shrink_to_fit();

	//reset turn count and number of turns
	numberOfPlayers = names.size();
	playerTurnIndex = 0;

	//assign all players & deal first card to each player
	for (int i = 0; i < names.size(); ++i)
	{
		Player temp;
		temp.playerName = names[i];
		temp.hand.push_back(gameDeck.drawCard());
		players.push_back(temp);
	}

	//deal first card to dealer
	dealerHand.push_back(gameDeck.drawCard());

	//deal second card to each player
	for (int i = 0; i < players.size(); ++i)
	{
		players[i].hand.push_back(gameDeck.drawCard());
	}

	//deal second card to dealer, placed at front to ensure its revealed
	dealerHand.insert(dealerHand.begin(), gameDeck.drawCard());
}

std::string BlackJack::displayGameState()
{
	std::string output = "";

	//Checks for if last player has passed
	if (playerTurnIndex >= numberOfPlayers)
	{
		//Dealer Details
		dealerReveal = true;
		output += "Dealer's Hand: " + displayDealerHand() + "\n";
		output += "Hand Score: " + (cardScore(dealerHand));
		output += "\n\n";

		while (cardScore(dealerHand) < 17)
		{
			dealerHand.push_back(gameDeck.drawCard());
			output += "Dealer's Hand: " + displayDealerHand() + "\n";
			output += "Hand Score: " + (cardScore(dealerHand));
			output += "\n\n";
		}

		if (handCheck(dealerHand))
		{
			output += "Dealer Stands.\n";

			//Compare hands and check for busts
			for (int i = 0; i < players.size(); ++i)
			{
				output += players[i].playerName;
				if (handCheck(players[i].hand))
				{
					output += " Busts!";
				}
				else if (cardScore(players[i].hand) < cardScore(dealerHand))
				{
					output += " Lost!";
				}
				else if (cardScore(players[i].hand) > cardScore(dealerHand))
				{
					output += " Won!";
				}
				else
				{
					output += " Drew!";
				}
				output += "\n P:" + cardScore(players[i].hand);
				output += " to D:" + cardScore(dealerHand);
				output += "\n\n";
			}
		}
		else
		{
			output += "Dealer has busted! \n";

			//Award victories and check for busts
			for (int i = 0; i < players.size(); ++i)
			{
				output += players[i].playerName;
				if (handCheck(players[i].hand))
				{
					output += " Wins!\n";
				}
				else
				{
					output += " Busted!\n";
				}
			}
		}
	}
	else 
	{
		//Dealer Details
		output += "Dealer's Hand: " + displayDealerHand() + "\n\n";

		//Player Details
		output += players[playerTurnIndex].playerName + "'s Hand: " + displayPlayerHand(playerTurnIndex) + "\n";
		output += "Hand Score: " + (cardScore(players[playerTurnIndex].hand));
		output += "\n\n";
	}

	return output;
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
		return dealerHand[0].toString() + ", XXX";
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

void BlackJack::currentPlayerHit()
{
	players[playerTurnIndex].hand.push_back(gameDeck.drawCard());
}

void BlackJack::currentPlayerStand()
{
	++playerTurnIndex;
}

int BlackJack::cardScore(std::vector<Card>& hand)
{
	int handScore = 0;
	bool hasAce = false;

	//Check hands and assigns score based on value
	for (int x = 0; x < hand.size(); ++x)
	{
		switch (hand[x].cardValue)
		{
		case '0':
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