#include <iostream>
#include <vector>
#include <stack>
#include <iterator>
#include <pthread.h>
#include <time.h>

using namespace std;

#define DECK_SIZE 52
#define JACK 11
#define QUEEN 12
#define KING 13
#define ACE 14

void *poop(void *); // delete this when done

void display_deck();

void setup();
void shuffle_deck();
void show_stack();
void stack2deck();

struct Card{
    int value;
    struct Card* next;
    struct Card* prev;
};

pthread_mutex_t dealer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t player1_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t player2_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t player3_mutex = PTHREAD_MUTEX_INITIALIZER;

vector <int> deck;
stack <int> shuffle;

int dekko[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, JACK, QUEEN, KING, ACE,  // diamonds
              2, 3, 4, 5, 6, 7, 8, 9, 10, JACK, QUEEN, KING, ACE,  // clubs
              2, 3, 4, 5, 6, 7, 8, 9, 10, JACK, QUEEN, KING, ACE,  // hearts
              2, 3, 4, 5, 6, 7, 8, 9, 10, JACK, QUEEN, KING, ACE}; // spades

int main() {
    srand(time(NULL));
    setup();
    display_deck();
    shuffle_deck();
    display_deck();

    stack2deck();
    display_deck();

    pthread_t dealer, player1, player2, player3;
    pthread_create(&dealer, NULL, poop, NULL);
    pthread_join(dealer, NULL);
    exit(0);

    return 0;
}

/*
void display_deck() {
    list <int> :: iterator it;
    for(it = deck.begin(); it != deck.end(); ++it) {
        cout << '\t' << *it;

        if (*it == ACE)
            cout << endl;
    }
    cout << '\n';
}
 */

void display_deck() {
    cout << "Display deck as vector" << endl;
    for (auto i = deck.begin(); i != deck.end(); i++) {
        cout << '\t' << *i << endl;
    }
    cout << endl;
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

void shuffle_deck() {
    int random;
    vector<int>::iterator it;
int counter = 0;
    cout << "shuffle_deck" << endl;
    while (!deck.empty()){
        random = rand() % deck.size();

        shuffle.push(deck.at(random));
        it = deck.begin() + random;
        deck.erase(it);

        cout << counter << endl;
        //display_deck();
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
         cout << counter << endl;
         //card = shuffle.top();
         deck.push_back(shuffle.top()); //card);
         shuffle.pop();
         counter++;
     }
    cout << endl;
}