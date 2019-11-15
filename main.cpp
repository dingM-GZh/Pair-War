#include <iostream>
#include <stack>
#include <pthread.h>
#include <fstream>
#include <stdlib.h>
#include <deque>
#include "Player.h"

using namespace std;

#define MAX_ROUNDS 3
#define JACK 11
#define QUEEN 12
#define KING 13
#define ACE 14
#define SRAND_SEED 12 // change as needed

pthread_cond_t dealer_cond;
pthread_cond_t player_cond[MAX_ROUNDS];
pthread_mutex_t dealer_mutex;
pthread_mutex_t player_mutex;

void initialise(); // set up
void terminator(); // termination

void *dealer_moves(void *); /*** dealer thread function(s) ***/
void deal_process(); // deals cards
void party_rock_shuffle(); // shuffling cards
void gen_deque(); // generate deck
void shuffle_deque(); // shuffle deck
void recreate_deque(); // recreate deck (after shuffling cards)

void *player_moves(void *); /*** player thread function(s) ***/
void round_complete(int);
void print_deque(); // print deck
bool compare_cards(int, int); // card comparison

void *square_moves(void *);

deque<int> deck;
stack<int> shuffle;
Player players[MAX_ROUNDS];

ofstream fout;
bool pair_found = false;
int pair_counter = 0, cond_sig_counter = 0, escapeCounter = 0, current_round = 0, current_player = 0;

int main()
{
    initialise();

    pthread_t dealer_t; // declare dealer thread
    pthread_t players_t[MAX_ROUNDS]; // declare an array of threads for players (size == 3)

    pthread_cond_init(&dealer_cond, NULL); // initialising dealer condition
    for (int i = 0; i < MAX_ROUNDS; i++)
        pthread_cond_init(&player_cond[i], NULL); // initialising player condition (x3)

    pthread_mutex_init(&dealer_mutex, NULL); // initialising dealer mutex
    pthread_mutex_init(&player_mutex, NULL);   // initialising player mutex (One For All)

    pthread_create(&dealer_t, NULL, dealer_moves, NULL); // dealer thread creation
    for (int i = 0; i < MAX_ROUNDS; i++)
        pthread_create(&players_t[i], NULL, player_moves, (void *) i); // player thread creation (x3)

    pthread_join(dealer_t, NULL);
    for (int i = 0; i < 3; i++)
        pthread_join(players_t[i], NULL);

    terminator(); // destroy and exit
    return 0;
}

/***
 * Operations to set up the program
 *     - open text file
 *     - set names of players
 *
 * P.S. Read the name, that's what it does...
 */
void initialise() {
    for (int i = 0; i < MAX_ROUNDS; i++) // set the names of each player
        players[i].set_name(i+1);

    fout.open("pair_war.log");
    srand(SRAND_SEED);
}

/***
 * # Clean up! Clean up! Everybody, everywhere! #
 * # Clean up! Clean up! Everybody do your share! #
 */
 void terminator() {
    fout.flush();
    fout.close();

    pthread_mutex_destroy(&player_mutex);
    pthread_mutex_destroy(&dealer_mutex);

    pthread_cond_destroy(&dealer_cond);
    for (int i = 0; i < MAX_ROUNDS; i++)
        pthread_cond_destroy(&player_cond[i]);

    pthread_exit(NULL);
 }


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////// Dealer Actions and Methods Needed /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/***
 * The actions the dealer thread are going to perform; mainly deal cards & signal which player to go first
 * @return NULL because of a void pointer type
 */
void *dealer_moves(void *) {
    do {
        pthread_mutex_lock(&dealer_mutex); // locks the mutex for the dealer

        fout << "DEALER: shuffles deck" << endl;
        pair_found = false;

        party_rock_shuffle(); // generating deck, shuffling cards, reinserting cards into deck
        deal_process(); // dealing cards to the players

        current_player = current_round;
        current_round++;
        cond_sig_counter = 0;
        pthread_cond_signal(&player_cond[current_player]);

        pthread_cond_wait(&dealer_cond, &dealer_mutex);
        pthread_mutex_unlock(&dealer_mutex);

        if (pair_counter == 3) // begin terminating (find Sarah Connor)
            break;

    } while (current_round < 4);
    return NULL;
 }

 /***
  * Deals the first card to each player
  */
void deal_process() {
    for(int i = 0; i < MAX_ROUNDS; i++) {
        players[i].set_card(deck.front());
        deck.pop_front();
        fout << players[i].get_name() << ": hand " << players[i].get_card() << endl;
    }
}

/***
 * Generates the deque, shuffles the cards, and places cards back into deque
 *
 * The name is just something fun
 */
void party_rock_shuffle() {
    gen_deque();
    shuffle_deque();
    recreate_deque();
}

/***
 * Generates the deque that contains the deck
 */
void gen_deque() {
    deque<int> reset;
    deck.swap(reset);

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

/***
 * Shuffles deck by placing a random card into a stack
 * Continues until deque is empty
 */
void shuffle_deque() {
    int random;
    while (!deck.empty()) {
        random = rand() % deck.size();

        shuffle.push(deck.at(random));
        deck.erase(deck.begin() + random);
    }
}

/***
 * Recreates the deck after shuffling has happened
 * Inserts cards from the stack into the deque
 */
void recreate_deque() {
    while (!shuffle.empty()) {
        deck.push_back(shuffle.top());
        shuffle.pop();
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////// Player Actions and Methods Needed /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/***
 * The actions of the player threads
 * @param player_id the index of the player thread
 * @return NULL because of a void pointer type
 */
void *player_moves(void *player_id) {
    while (current_round < 4) {
        pthread_mutex_lock(&player_mutex);
        int id = *((int *) (&player_id)); // recasting player_id back into integer
        pthread_cond_wait(&player_cond[id], &player_mutex);

        if(pair_counter == 3) { // determine when to begin terminating
            if(escapeCounter == 2) { // signalling dealer to stop executing
                pthread_mutex_unlock(&player_mutex);
                pthread_cond_signal(&dealer_cond);
                break;
            }

            escapeCounter++;
            fout << players[id].get_name() << ": round completed" << endl;
            pthread_cond_signal(&player_cond[(id + 1) % 3]);
            pthread_mutex_unlock(&player_mutex);
            break;
        }

        if (pair_found) {
            round_complete(id);
        }
        else {
            int temp = deck.front(); // drawing the second card
            deck.pop_front();

            cout << players[id].get_name() << ":" << endl ;
            cout << "HAND " << players[id].get_card()  << " " << temp << endl; // displays the player's current hand

            fout << players[id].get_name() << ": hand " << players[id].get_card() << endl
                 << players[id].get_name() << ": draws " << temp << endl;

            if (compare_cards(players[id].get_card(), temp)) { // if pair found
                cout << "WIN: YES" << endl;
                cout << "----------------------------" << endl;

                fout << players[id].get_name() << ": wins" << endl
                     << players[id].get_name() << ": round completed" << endl;

                pair_counter++;
                pair_found = true;
            }
            else { // if pair not found
                cout << "WIN: NO" << endl;

                int discard = rand() % 2; // randomly discard from the player's hand
                if (discard == 0) {
                    deck.push_back(players[id].get_card());
                    fout << players[id].get_name() << ": discards " << players[id].get_card() << endl;
                    players[id].set_card(temp);
                }
                else {
                    deck.push_back(temp);
                    fout << players[id].get_name() << ": discards " << temp << endl;
                }

                current_player = (current_player + 1) % 3;
                print_deque();
            }
            pthread_cond_signal(&player_cond[(id + 1) % 3]);
            pthread_mutex_unlock(&player_mutex);
        }
    }
    return NULL;
}

/***
 * Action to execute after a round is completed
 * @param id the index of the player object
 */
void round_complete(int id) {
    fout << players[id].get_name() << ": round completed" << endl;
    cond_sig_counter++;

    if (cond_sig_counter == 2) { // signal to dealer to deal cards
        pthread_cond_signal(&dealer_cond);
        pthread_mutex_unlock(&player_mutex);
    }
    else { // signal other players to notify completion of round
        pthread_cond_signal(&player_cond[(id + 1) % 3]);
        pthread_mutex_unlock(&player_mutex);
    }
}

/***
 * Prints the deque to the file
 * IOW - Writes contents of deck to the log file
 */
void print_deque() {
    for (int i = 0; i < deck.size(); i++)
        fout << deck[i] << ' ';
    fout << endl;
}

/***
 * Compares two integer values (cards) to determine if they are equal
 * @param p_card the current player card
 * @param temp the newly drawn card
 * @return whether or not the two cards are equal
 */
bool compare_cards(int p_card, int temp){
    return (p_card == temp);
}

/***
 * "fun" easter egg
 * @return
 */
void *square_moves(void *) {
    cout << "Square moves?" << endl << endl;
    exit(1);
}