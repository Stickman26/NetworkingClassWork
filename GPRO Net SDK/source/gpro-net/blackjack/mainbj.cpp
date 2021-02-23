
#include<iostream>
#include "gpro-net/blackjack/game.h"


void main() {
	//Add a Dealer
	addDealer();

	//Add players
	addPlayer();
	addPlayer();

	//Start game
	start();
	turn();
	checkWin();
	
	system("pause");
}