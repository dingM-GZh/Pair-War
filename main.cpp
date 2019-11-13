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
#define JACK 11
#define QUEEN 12
#define KING 13
#define ACE 14

void *dealer_moves(void *);
void *player_moves(void *);
void *square_moves(void *);

void initialise();
void end();
void display_deck();
void print_deck();
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
pthread_t players_thread[3];

pthread_cond_t  dealer_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t  players_cond[3] = {PTHREAD_COND_INITIALIZER};

pthread_mutex_t dealer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t players_mutex = PTHREAD_MUTEX_INITIALIZER;
/*-------------------------------------------------------*/
pthread_mutex_t player1_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t player2_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t player3_mutex = PTHREAD_MUTEX_INITIALIZER;


Player players[3];
deque <int> deck;
stack <int> shuffle;

ofstream fout;
int current_player = 0, current_round = 0, hot_potato = 0;
bool pair_found = false;

/***
 * It's main. What do you think?
 * @return 0
 */
int main() {
    initialise();

    pthread_create(&dealer_thread, NULL, dealer_moves, NULL);
    //_sleep(1);
    pthread_create(&players_thread[0], NULL, player_moves, (void *) 0);
    pthread_create(&players_thread[1], NULL, player_moves, (void *) 1);
    pthread_create(&players_thread[2], NULL, player_moves, (void *) 2);

    pthread_join(dealer_thread, NULL);
    pthread_join(players_thread[0],NULL);
    pthread_join(players_thread[1],NULL);
    pthread_join(players_thread[2],NULL);

    end();
    return 0;
}

/***
 * Operations to set up the program
 *     - open text file
 *     - set names of players
 *     - set ID nums of players
 *
 * P.S. Read the name, that's what it does...
 */
void initialise() {
    deck_setup();
    fout.open("pair_war_log.txt");
    fout << "PAIR WAR - LOG FILE" << endl << endl;
    srand(time(NULL));

    for (int i = 0; i < MAX_ROUNDS; i++) // set the names of each player
        players[i].set_name(i+1);

    pthread_mutex_init(&dealer_mutex, NULL);
    pthread_mutex_init(&players_mutex, NULL);
}

/***
 * # Clean up! Clean up! Everybody, everywhere! #
 * # Clean up! Clean up! Everybody do your share! #
 */
void end() {
    pthread_cond_destroy(&dealer_cond);

    pthread_cond_destroy(&players_cond[0]);
    pthread_cond_destroy(&players_cond[1]);
    pthread_cond_destroy(&players_cond[2]);

    pthread_mutex_destroy(&dealer_mutex);
    pthread_mutex_destroy(&players_mutex);
    pthread_exit(NULL);

    fout.flush();
    fout.close();
}

/***
 * Writes deck to the console
 */
void display_deck() {
    cout << "Display deck " << deck.size() << endl;
    for (int i = 0; i < deck.size(); i++)
        cout << deck[i] << ' ';
    cout << endl << endl;
}

/***
 * Writes the deck to the log file
 */
void print_deck() {
    for (int i = 0; i < deck.size(); i++)
        fout << deck[i] << ' ';
    cout << endl;
}

/***
 * Creates the deck by placing all cards into the deque
 */
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
    cout << "DICK SIZE " << deck.size() << endl;
    display_deck();
}

/***
 * Shuffles deck by placing a random card into a stack
 * Then the stack is loaded back into a deque
 */
void shuffle_deck() {
    int random;
    cout << "shuffle_deck - in progress" << endl;
    cout << deck.size() << "\t";

    while (!deck.empty()) {
        random = rand() % deck.size();

        shuffle.push(deck.at(random));
        deck.erase(deck.begin() + random);

    }
    cout << endl;

    cout << "stack2deck" << endl;
    while (!shuffle.empty()) {
        //card = shuffle.top();
        deck.push_back(shuffle.top()); //card);
        shuffle.pop();
        //cout << counter << endl;
    }
    cout << "shuffle_deck - completed"
         << endl << endl;
}

/***
 * Taking cards from stack and placing them back into the deque
 */
void stack2deck() {
    cout << "stack2deck" << endl;
    while (!shuffle.empty()) {
        //card = shuffle.top();
        deck.push_back(shuffle.top()); //card);
        shuffle.pop();
        //cout << counter << endl;
    }
}

/***
 * Actions the dealer (shuffles cards and hand them out)
 * @return NULL because the pointer isn't connected to anything
 */
void *dealer_moves(void *) {
    pthread_mutex_lock(&dealer_mutex);
    shuffle_deck();

    cout << "DEALER: shuffles deck" << endl;
    fout << "DEALER: shuffles deck" << endl;

    display_deck();

    for (int i = 0; i < MAX_ROUNDS; i++) {
        deal_process(players[i]);
    }

    //pthread_cond_wait(&dealer_cond, &dealer_mutex);
    pthread_cond_signal(&players_cond[current_round]);
    pthread_cond_wait(&dealer_cond, &dealer_mutex);
    cout << "ending dealer moves" << endl;

    pthread_mutex_unlock(&dealer_mutex);
    return NULL;
}

/***
 * The actions a player makes during the game
 * @param player_id the name is self explanatory
 * @return NULL because the pointer isn't connected to anything
 */
void *player_moves(void *player_id) {
    pthread_mutex_lock(&players_mutex);
    pthread_cond_wait(&players_cond[*((int*)(&player_id))], &players_mutex);

    int temp = deck.front(); // the second that is to be drawn
    deck.pop_front(); // removes new card from top of deck

    int id = *((int*)(&player_id));
    Player thread_player = players[id]; // thread specific player;

    if (current_round == id) {
        cout << "player " << id << endl;
        cout << thread_player.get_name() << ": draws " << temp << endl;
        fout << thread_player.get_name() << ": draws " << temp << endl;
        pair_found = compare_cards(thread_player, temp);

        if (pair_found == false) {
            cout << thread_player.get_name() << ": discard " << temp << endl;
            fout << thread_player.get_name() << ": discard " << temp << endl;
            deck.push_back(temp);
            display_deck();
            print_deck();

            pthread_cond_signal(&players_cond[id]);
        }
        else {
            // stuff to terminate the program bc pair is found
        }
        //cout << "player 1" << endl;
    }
    else {
        cout << "this is player " << id << endl;
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
    cout << "ending player " << id << endl;
    pthread_mutex_unlock(&players_mutex);
    return NULL;
}

/***
 * "fun" easter egg
 * @return
 */
void *square_moves(void *) {
    cout << "Square moves?" << endl << endl;
    exit(1);
}

/***
 * Deals a card to the player
 * @param player object that is receiving the card
 */
void deal_process(Player &player) {
    int top_card = deck.front();

    string name = player.get_name(); // easy access to player name
    player.set_card(top_card); // assign the main card (doesn't change)

    ////////////////////////////////////////////////////////
    cout << name << ": hand "  << player.get_card() << endl; // delete when complete
    ////////////////////////////////////////////////////////

    fout << name << ": hand "  << top_card << endl;
    deck.pop_front(); // deletes card from top of deck

    ////////////////////////////////////////////////////////
    display_deck(); // delete when complete
    ////////////////////////////////////////////////////////
}

/***
 * Determines if the hand is a pair
 * @param player object to access the card dealt by the dealer
 * @param temp second card that will be discarded if not a pair
 * @return truth value of card comparison
 */
bool compare_cards(Player player, int temp) {
    ////////////////////////////////////////////////////////
    cout << "compare_cards" << endl // delete when complete
    << "card " << player.get_card() << "\t temp " << temp << endl; // delete when complete
    ////////////////////////////////////////////////////////

    return (player.get_card() == temp);
}