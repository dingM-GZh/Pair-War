#include <iostream>
#include <queue>
#include <stack>
#include <queue>
#include <iterator>
#include <pthread.h>
#include <time.h>
#include <fstream>
#include "Player.h"

using namespace std;

#define DECK_SIZE 52
#define JACK 11
#define QUEEN 12
#define KING 13
#define ACE 14

void *dealer_moves(void *);

void init();
void end();
void display_deck();
void deck_setup();
void shuffle_deck();
void show_stack();
void stack2deck();

void deal_process(Player);

struct temp_player{
    int card;
};

pthread_t dealer;
pthread_t players[3];

pthread_cond_t  dealer_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t  players_cond = PTHREAD_COND_INITIALIZER;

pthread_mutex_t dealer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t player1_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t player2_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t player3_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t players_mutex = PTHREAD_MUTEX_INITIALIZER;

Player player1;
Player player2;
Player player3;
Player player[3];

ofstream fout;
vector <int> deck;
stack <int> shuffle;
int current_player, current_round, rounds;
bool pair_found = false;

int main() {
    init();
    //stack2deck();
    pthread_create(&dealer, NULL, dealer_moves, NULL);
    pthread_join(dealer, NULL);

    end();

    return 0;
}

/***
 * Operations to set up the program
 */
void init() {
    deck_setup();
    fout.open("pair_war log.txt");
    fout << "PAIR WAR - LOG FILE" << endl << endl;
    srand(time(NULL));

    player1.set_name("PLAYER 1");
    player2.set_name("PLAYER 2");
    player3.set_name("PLAYER 3");

    pthread_mutex_init(&dealer_mutex, NULL);
    pthread_mutex_init(&players_mutex, NULL);
}

/***
 * Clean up after done
 */
void end() {
    pthread_mutex_destroy(&dealer_mutex);
    pthread_mutex_destroy(&players_mutex);
    pthread_exit(0);

    fout.flush();
    fout.close();
}

void display_deck() {
    cout << "Display deck as vector" << endl;
    for (auto i = deck.begin(); i != deck.end(); i++)
        cout << '\t' << *i << endl;
    cout << endl;
}

void deck_setup() {
    for (int i = 0; i < 4; i++) {
        deck.push_back(2);
        deck.push_back(3);
        deck.push_back(4);
        deck.push_back(5);
        deck.push_back(6);
        deck.push_back(7);
        deck.push_back(8);
        deck.push_back(9);
        deck.push_back(10);
        deck.push_back(JACK);
        deck.push_back(QUEEN);
        deck.push_back(KING);
        deck.push_back(ACE);
    }
}

void *dealer_moves(void *) {
    pthread_mutex_lock(&dealer_mutex);
    shuffle_deck();

    cout << "DEALER: shuffles deck" << endl;
    fout << "DEALER: shuffles deck" << endl;

    deal_process(player1);
    deal_process(player2);
    deal_process(player3);

    //pthread_cond_wait();
    pthread_mutex_unlock(&dealer_mutex);

    return NULL;
}

void shuffle_deck() {
    int random, counter = 0;
    vector<int>::iterator it;
    cout << "shuffle_deck - in progress" << endl;

    while (!deck.empty()) {
        random = rand() % deck.size();

        shuffle.push(deck.at(random));
        it = deck.begin() + random;
        deck.erase(it);

        //cout << counter << endl;
        counter ++;
    }
    cout << "shuffle_deck - completed" << endl
         << endl;
}

void show_stack() {
    for (int i = 0; i < shuffle.size(); i++) {
        cout << shuffle.top() + 1 << "\t";
    }
}

void stack2deck() {
    int card, counter = 0;
    cout << "stack2deck" << endl;
     while (!shuffle.empty()) {
         card = shuffle.top();
         deck.push_back(shuffle.top()); //card);
         shuffle.pop();
         //cout << counter << endl;
         counter++;
     }
    cout << endl;
}

void deal_process(Player player) {
    int top_card = shuffle.top();

    string name = player.get_name();
    player.set_card(top_card);
    deck.push_back(top_card);

    //cout << "DEALER: " << name << " is dealt " << top_card << endl << endl;
    //fout << "DEALER: " << name << " is dealt " << top_card << endl << endl;

    cout << name << ": hand "  << top_card << endl;
    fout << name << ": hand "  << top_card << endl;

    shuffle.pop(); // takes top card off stack
}