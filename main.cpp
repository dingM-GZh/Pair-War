#include <iostream>
#include <stack>
#include <pthread.h>
#include <time.h>
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

void *dealer_moves(void *);
void *player_moves(void *);
void *square_moves(void *);

void deal_process(Player&);
bool compare_cards(Player, int);

void initialise();
void end();
void display_deck();
void print_deck();
void deck_setup();
void shuffle_deck();
void discard_card(Player&, int&);

pthread_t dealer_t,
          players_t[3];

pthread_cond_t  dealer_cond, players_cond[3];

pthread_mutex_t dealer_mutex, players_mutex;

Player players[3];
deque <int> deck;
stack <int> shuffle;

ofstream fout;
/* Escape plan == pairs_found
 * roundOver == pair_found
 * Activeplayer & roundCounter == current round
 * OR (activePlayer == current_player and roundCounter == current_round)
 */
int current_player = 0, current_round = 0, round_counter = 0, pairs_found = 0,
esc_counter = 0, sig_count = 0;
bool pair_found = false;

/***
 * It's main. What do you think?
 * @return 0
 */
int main() {
    initialise();

    pthread_create(&dealer_t, NULL, dealer_moves, NULL);
    //_sleep(1);

    // thread player_id is same as the index of the player_thread being created
    pthread_create(&players_t[0], NULL, player_moves, (void *) 0);
    pthread_create(&players_t[1], NULL, player_moves, (void *) 1);
    pthread_create(&players_t[2], NULL, player_moves, (void *) 2);

    pthread_join(dealer_t, NULL);
    pthread_join(players_t[0],NULL);
    pthread_join(players_t[1],NULL);
    pthread_join(players_t[2],NULL);

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
    cout << "shuffle_deck - in progress" << endl /****************** delete when complete ******************/
         << deck.size() << "\t"; /****************** delete when complete ******************/

    while (!deck.empty()) {
        random = rand() % deck.size();

        shuffle.push(deck.at(random));
        deck.erase(deck.begin() + random);

    }
    cout << endl;

    cout << "stack2deck" << endl; /****************** delete when complete ******************/
    while (!shuffle.empty()) {
        // card = shuffle.top();
        deck.push_back(shuffle.top());
        shuffle.pop();
        // cout << counter << endl;
    }
    cout << "shuffle_deck - completed" /****************** delete when complete ******************/
         << endl << endl; /****************** delete when complete ******************/
}

/***
 * Actions the dealer (shuffles cards and hand them out)
 * @return NULL because the pointer isn't connected to anything
 */
void *dealer_moves(void *) {
    while (current_round < MAX_ROUNDS + 1) {
        pthread_mutex_lock(&dealer_mutex);
        cout << "DEALER: shuffles deck" << endl; /****************** delete when complete ******************/
        fout << "DEALER: shuffles deck" << endl;

        pair_found = false;
        shuffle_deck();

        display_deck(); /****************** delete when complete ******************/

        for (int i = 0; i < MAX_ROUNDS; i++) { // hands the first card to each player
            deal_process(players[i]);
        }
        current_player = current_round;
        current_round++;
        sig_count = 0;

        pthread_cond_signal(&players_cond[current_round]);
        pthread_cond_wait(&dealer_cond, &dealer_mutex);
        cout << "ending dealer moves" << endl; /****************** delete when complete ******************/

        pthread_mutex_unlock(&dealer_mutex);

        if (pairs_found == MAX_ROUNDS)
            break;
    }
    return NULL;
}

/***
 * The actions a player makes during the game
 * @param player_id the name is self explanatory
 * @return NULL because the pointer isn't connected to anything
 */
void *player_moves(void *player_id) {
    while (current_round < MAX_ROUNDS + 1) {
        pthread_mutex_lock(&players_mutex);
        pthread_cond_wait(&players_cond[*((int *) (&player_id))], &players_mutex);

        int id = *((int *) (&player_id)); // recasting player_id back into integer
        Player thread_player = players[id]; // temp player object relative to current thread

        if (pairs_found >= MAX_ROUNDS) {
            if (esc_counter == MAX_ROUNDS - 1) {
                pthread_mutex_unlock(&players_mutex);
                pthread_cond_signal(&dealer_cond);
                break;
            }
            esc_counter++;
            cout << thread_player.get_name() << ": round completed" << endl; /****************** delete when complete ******************/
            fout << thread_player.get_name() << ": round completed" << endl;
            pthread_cond_signal(&players_cond[(id + 1) % 3]);
            pthread_mutex_unlock(&players_mutex);
            break;
        }
        if (!pair_found) { /* if a pair has not been found - round keeps going */
            int temp = deck.front(); // the second that is to be drawn
            /**  std::cout << "PLAYER " << ((long)currentPlayerIndex) + 1 << ": HAND " << players[((long)currentPlayerIndex)].hand  <<" " << temp<< std::endl; **/
            deck.pop_front(); // removes new card from top of deck

            //cout << "player " << id << endl; /****************** delete when complete ******************/
            cout << thread_player.get_name() << ": draws " << temp << endl; /****************** delete when complete ******************/
            cout << thread_player.get_name() << ": hand " << thread_player.get_card() << endl; /****************** delete when complete ******************/

            fout << thread_player.get_name() << ": hand " << thread_player.get_card() << endl;
            fout << thread_player.get_name() << ": draws " << temp << endl;
            if (compare_cards(thread_player, temp)) {
                cout << "WIN: YES" << endl;
                pairs_found++;
                fout << thread_player.get_name() << ": wins" << endl
                     << thread_player.get_name() << ": round completed" << endl;
                pair_found = true;
            }
            else {
                cout << "WIN: NO" << endl;
                discard_card(thread_player, temp);
            }
            current_player = (current_player + 1) % 3;
            display_deck(); //****************** delete when complete ******************//
            print_deck();
        }
        else { /* if a pair has been found (i.o.w. the round is over) */
            /** fout << "PLAYER " << ((long)currentPlayerIndex) + 1 << ": round completed" << std::endl; **/
            sig_count++;

            if (sig_count == 2) {
                pthread_cond_signal(&dealer_cond);
                pthread_mutex_unlock(&players_mutex);
            }
            else {
                pthread_cond_signal(&players_cond[id + 1]);
                pthread_mutex_unlock(&players_mutex);
            }
        }

        pthread_cond_signal(&dealer_cond);
        cout << "ending player " << id << endl;
        pthread_mutex_unlock(&players_mutex);
    }
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

    cout << name << ": hand "  << player.get_card() << endl; /****************** delete when complete ******************/
    fout << name << ": hand "  << top_card << endl;

    deck.pop_front(); // removes card from top of deck

    display_deck(); /****************** delete when complete ******************/
}

/***
 * Determines if the hand is a pair
 * @param player object to access the card dealt by the dealer
 * @param temp second card that will be discarded if not a pair
 * @return truth value of card comparison
 */
bool compare_cards(Player player, int temp) {
    cout << "compare_cards" << endl /****************** delete when complete ******************/
         << "card " << player.get_card() << "\t temp " << temp << endl; /****************** delete when complete ******************/

    return (player.get_card() == temp);
}

void discard_card(Player &player, int &temp) {
    int discard = rand() % 2;
    if (discard == 1) {
        deck.push_back(temp);
        cout << player.get_name() << ": discard " << temp << endl; /****************** delete when complete ******************/
        fout << player.get_name() << ": discard " << temp << endl;
        temp = 0;
    }
    else {
        deck.push_back(player.get_card());
        cout << player.get_name() << ": discard " << player.get_card() << endl; /****************** delete when complete ******************/
        fout << player.get_name() << ": discard " << player.get_card() << endl;
        player.set_card(temp);
    }

}