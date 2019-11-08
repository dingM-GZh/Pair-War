#include <iostream>
#include <vector>
#include <stack>
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

void *poop(void *); // delete this when done
void *dealer_moves(void *);

void init();
void end();
void display_deck();
void deck_setup();
void shuffle_deck();
void show_stack();
void stack2deck();

struct temp_player{
    int card;
};

pthread_mutex_t dealer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t player1_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t player2_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t player3_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t players_mutex;

pthread_t dealer;
pthread_t players[3];

Player player1;
Player player2;
Player player3;


vector <int> deck;
stack <int> shuffle;

int main() {

    init();

    //fout.open();
    cout << endl;
    stack2deck();

    pthread_create(&dealer, NULL, dealer_moves, NULL);
    pthread_join(dealer, NULL);

    end();
    ofstream fout("pair_war.txt");
    fout << "poop";
    fout.flush();
    fout.close();
    return 0;
}

void init() {
    deck_setup();
    //fout.open("pair_war.txt");
    //cout << "created fout" << endl << endl;
    srand(time(NULL));

    pthread_mutex_init(&dealer_mutex, NULL);
    pthread_mutex_init(&players_mutex, NULL);
}

void end() {
    pthread_mutex_destroy(&dealer_mutex);
    pthread_mutex_destroy(&players_mutex);
    pthread_exit(0);
    //exit(0);
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

void *poop(void *pee){
    cout << "deez nuts 1" << endl << endl;
    return NULL;
}

void *dealer_moves(void *){
    int top_card;
    shuffle_deck();
    top_card = shuffle.top();
    player1.set_card(top_card);
    deck.push_back(top_card);
    shuffle.top();
    cout << "PLAYER 1: draws "  << player1.get_card();
    //fout << "PLAYER 1: draws "  << player1.get_card();

    return NULL;
}

void shuffle_deck() {
    int random, counter = 0;
    vector<int>::iterator it;
    cout << "shuffle_deck" << endl;
    while (!deck.empty()){
        random = rand() % deck.size();

        shuffle.push(deck.at(random));
        it = deck.begin() + random;
        deck.erase(it);

        //cout << counter << endl;
        counter ++;
    }
}

void show_stack() {
    //
}

void stack2deck(){
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