#include <iostream>
#include <queue>
#include <stack>
#include <pthread.h>
#include <time.h>
#include <fstream>
#include <stdlib.h>
#include <deque>
#include <cstdint>
#include "Player.h"

using namespace std;

#define MAX_ROUNDS 3
#define DECK_SIZE 52
#define JACK 11
#define QUEEN 12
#define KING 13
#define ACE 14

void *dealer_moves(void *);
void *player_moves(void *);
void *square_moves(void *);

void init();
void end();
void display_deck();
void deck_setup();
void shuffle_deck();
void show_stack();
void stack2deck();

bool compare_cards(Player, int);

void deal_process(Player&);

struct temp_player{
    int card;
};

pthread_t dealer_thread;
pthread_t player1_thread;
pthread_t player2_thread;
pthread_t player3_thread;
pthread_t players_thread[3];

pthread_cond_t  dealer_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t  player1_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t  player2_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t  player3_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t  players_cond[3] = {PTHREAD_COND_INITIALIZER};

pthread_mutex_t dealer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t players_mutex = PTHREAD_MUTEX_INITIALIZER;
/*-------------------------------------------------------*/
pthread_mutex_t player1_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t player2_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t player3_mutex = PTHREAD_MUTEX_INITIALIZER;

Player player1;
Player player2;
Player player3;
Player players[3];

deque <int> deck;
stack <int> shuffle;

ofstream fout;
int current_player = 0, current_round = 0, hot_potato = 0;
bool pair_found = false;

int main() {
    init();

    pthread_create(&dealer_thread, NULL, dealer_moves, NULL);
    //_sleep(1);
    pthread_create(&players_thread[0], NULL, player_moves, (void *) 0);
    pthread_create(&players_thread[1], NULL, player_moves, (void *) 1);
    pthread_create(&players_thread[2], NULL, player_moves, (void *) 2);

    //pthread_create(&player1_thread, NULL, player_moves, (void *)1);
    //pthread_create(&player2_thread, NULL, player_moves, (void *)2);
    //pthread_create(&player3_thread, NULL, player_moves, (void *)3);

    pthread_join(dealer_thread, NULL);
    pthread_join(players_thread[0],NULL);
    pthread_join(players_thread[1],NULL);
    pthread_join(players_thread[2],NULL);
    //pthread_join(player1_thread, NULL);
    //pthread_join(player2_thread, NULL);
    //pthread_join(player3_thread, NULL);

    end();

    return 0;
}

/***
 * Operations to set up the program
 *     - open text file
 *     - set names of players
 *     - set ID nums of players
 */
void init() {
    deck_setup();
    fout.open("pair_war_log.txt");
    fout << "PAIR WAR - LOG FILE" << endl << endl;
    srand(time(NULL));

    player1.set_name(1);
    player2.set_name(2);
    player3.set_name(3);

    for (int i = 0; i < MAX_ROUNDS; i++) {
        players[i].set_name(i+1);
        cout << players[i].get_name() << endl;
    }
    cout << endl;

    pthread_mutex_init(&dealer_mutex, NULL);
    pthread_mutex_init(&players_mutex, NULL);
}

/***
 * Clean up after done
 */
void end() {
    pthread_cond_destroy(&dealer_cond);
    pthread_cond_destroy(&players_cond[0]);
    //pthread_cond_destroy(&players_cond[1]);
    //pthread_cond_destroy(&players_cond[2]);

    pthread_mutex_destroy(&dealer_mutex);
    pthread_mutex_destroy(&players_mutex);
    pthread_exit(NULL);

    fout.flush();
    fout.close();
}

void display_deck() {
    cout << "Display deck" << endl;

    for (int i = 0; i < deck.size(); i++)
        cout << deck[i] << ' ';
    cout << endl << endl;
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

void shuffle_deck() {
    int random;
    cout << "shuffle_deck - in progress" << endl;

    while (!deck.empty()) {
        random = rand() % deck.size();

        shuffle.push(deck.at(random));
        deck.erase(deck.begin() + random);
    }
    stack2deck();
    cout << "shuffle_deck - completed"
         << endl << endl;
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
}

void *dealer_moves(void *) {
    pthread_mutex_lock(&dealer_mutex);
    shuffle_deck();

    cout << "DEALER: shuffles deck" << endl;
    fout << "DEALER: shuffles deck" << endl;

    display_deck();

    for (int i = 0; i < MAX_ROUNDS; i++) {
        deal_process(players[i]);
    }

    //deal_process(player1);
    //deal_process(player2);
    //deal_process(player3);

    //pthread_cond_wait(&dealer_cond, &dealer_mutex);
    pthread_cond_signal(&players_cond[current_round-1]);
    pthread_cond_wait(&dealer_cond, &dealer_mutex);
    cout << "ending dealer moves" << endl;
    pthread_mutex_unlock(&dealer_mutex);
    //exit(0);

    return NULL;
}

void *player_moves(void *player_id) {
    pthread_mutex_lock(&players_mutex);
    pthread_cond_wait(&players_cond[*((int*)(&player_id))-1], &players_mutex);

    int temp = deck.front(); // the second that is to be drawn
    deck.pop_front(); // deletes new card from top of deck
    //cout << "temp " << temp << endl;
    int id = *((int*)(&player_id));
    Player thread_player = players[id]; // thread specific player;

    if (current_round == id) {
        cout << thread_player.get_name() << ": draws " << temp << endl;
        fout << thread_player.get_name() << ": draws " << temp << endl;
        pair_found = compare_cards(thread_player, temp);
        //cout << pair_found << endl;
        if (pair_found == false) {
            cout << thread_player.get_name() << ": discard " << temp << endl;
            fout << thread_player.get_name() << ": discard " << temp << endl;
            deck.push_back(temp);
            display_deck();

            pthread_cond_signal(&players_cond[id]);
        }
        else {
            // stuff to terminate the program bc pair is found
        }
        //cout << "player 1" << endl;
    }
    else {
        cout << "not player 1" << endl;
    }

    //deck.push_back(temp);
    //temp = deck.front();

    /***
    if (*(int *) player_id == 1) {
        //
    }
    else if (*(int *) player_id == 2) {
        //
    }
    else if (*(int *) player_id == 3) {
        //
    }
    else {
        //
    }*/


    /*
    while (!pair_found) {
        break;
    } */

    pthread_cond_signal(&dealer_cond);
    cout << "ending player moves "<< endl;
    pthread_mutex_unlock(&players_mutex);
    return NULL;
}

void *square_moves(void *) { // "fun" easter egg
    cout << "Square moves?" << endl << endl;
    exit(1);
}


void deal_process(Player &player) {
    int top_card = deck.front();

    string name = player.get_name(); // easy access to player name
    player.set_card(top_card); // assign the main card (doesn't change)

    //cout << "DEALER: " << name << " is dealt " << top_card << endl << endl;
    //fout << "DEALER: " << name << " is dealt " << top_card << endl << endl;

    cout << name << ": hand "  << player.get_card() << endl; // delete when complete
    fout << name << ": hand "  << top_card << endl;

    deck.pop_front(); // deletes card from top of deck
    cout << deck.size() << endl; // delete when complete
    display_deck(); // delete when complete
}

bool compare_cards(Player player, int temp) {
    cout << "compare_cards" << endl
    << "card " << player.get_card() << "\t temp " << temp << endl;
    return (player.get_card() == temp);
}