#include <iostream>
#include <list>
#include <iterator>
#include <pthread.h>

using namespace std;

#define DECK_SIZE 52
#define JACK 11
#define QUEEN 12
#define KING 13
#define ACE 14

void *poop(void *); // delete this when done

void display_deck();
void setup();

struct Card{
    int value;
    struct Card* next;
    struct Card* prev;
};


pthread_mutex_t dealer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t player1_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t player2_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t player3_mutex = PTHREAD_MUTEX_INITIALIZER;

list <int> deck;
int deck[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, JACK, QUEEN, KING, ACE,  // diamonds
              2, 3, 4, 5, 6, 7, 8, 9, 10, JACK, QUEEN, KING, ACE,  // clubs
              2, 3, 4, 5, 6, 7, 8, 9, 10, JACK, QUEEN, KING, ACE,  // hearts
              2, 3, 4, 5, 6, 7, 8, 9, 10, JACK, QUEEN, KING, ACE}; // spades
>>>>>>> 852e99c5f672bdb997182327022179b286dc58f3

int main() {
    setup();
    //display_deck();

    pthread_t dealer, player1, player2, player3;
    pthread_create(&dealer, NULL, poop, NULL);
    pthread_join(dealer, NULL);
    exit(0);

    return 0;
}

void display_deck() {
    list <int> :: iterator it;
    for(it = deck.begin(); it != deck.end(); ++it) {
        cout << '\t' << *it;

        if (*it == ACE)
            cout << endl;
    }
    cout << '\n';
}

void setup() {
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