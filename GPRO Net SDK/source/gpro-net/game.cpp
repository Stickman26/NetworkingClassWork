
#include "game.h"
#include<string>

vector<Dealer*>dealerSet;
vector<Player*>playerSet;


void addDealer() {
	Dealer* d = new Dealer();
	dealerSet.push_back(d);
}

void addPlayer() {
	Player* d = new Player();
	playerSet.push_back(d);
}

void start() {
	cout << "******Game Start!******"<<endl;
	init_deck();
	print_deck();
	shuffler();
	print_deck();
	Dealer* d = dealerSet.front();
	d->setPlayerSet(playerSet);
	d->Distribute();
}

void checkWin() {
	cout << "******Check Win******" << endl;
	Dealer* d = dealerSet.front();
	int dealerHand=d->CheckHand();
	int player = 0;

	string res = "";
	for (auto p : d->getPlayerSet()) {
		player++;
		cout << "Player" << player << "'s Hand" << endl;
		int playerHand = p->CheckHand();
		if (playerHand == 21 && dealerHand == 21) {
			res += "Player";
			res += (char)(player + '0');
			res+=" Blackjack Push!\n";
		}else
		if (playerHand == 21 && dealerHand != 21) {
			res += "Player";
			res += (char)(player + '0');
			res += " Wins, Blackjack!\n";
		}else
		if(playerHand != 21 && dealerHand == 21) {
			res+= "Dealer Wins, Blackjack!\n";
		}else
		if (playerHand <21 && playerHand > dealerHand) {
			res+= "Player";
			res+= (char)(player + '0');
			res += " Wins!\n";
		}else
		if (playerHand > 21) {
			res+= "Player";
			res+= (char)(player + '0');
			res += " Bust, lose!\n";
		}else
		if (dealerHand > 21) {
			res+= "Dealer Bust, Player Wins!\n";
		}else
		if (playerHand == dealerHand) {
			res += "Player";
			res += (char)(player + '0');
			res+= " Push!\n";
		}else
		if (playerHand < dealerHand) {
			res += "Player";
			res += (char)(player + '0');
			res += " lose!\n";
		}
	}
		cout << res << endl;
}

void turn() {
	Dealer* d = dealerSet.front();
	int player = 0;
	for (auto p : d->getPlayerSet()) {
		player++;
		cout << endl;
		cout << "******Player" << player << "'s move*******" << endl;
		cout << "Player" << player << "'s Hand" << endl;
		int playerHand = p->CheckHand();
		char c = ' ';
		while (c != 'S' && playerHand < 21) {
			c = p->choice();
			if (c == 'H') 
				p->Hit();
			else if (c == 'S') 
				/*Stand, do nothing*/;
			else 
				cout << "Invalid input, try again." << endl;
			playerHand = p->CheckHand();
		}
	}
	cout << "******Turns end!******" << endl;
	while (d->CheckHand() < 17) 
		d->Hit();
}