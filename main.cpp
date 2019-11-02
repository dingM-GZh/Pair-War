#include <iostream>
#include <pthread.h>

#define DECK_SIZE 52
#define JACK 11
#define QUEEN 12
#define KING 13
#define ACE 14

struct Card{
    int value;
    struct Card* next;
    struct Card* prev;
};

int deck[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, JACK, QUEEN, KING, ACE,
              2, 3, 4, 5, 6, 7, 8, 9, 10, JACK, QUEEN, KING, ACE,
              2, 3, 4, 5, 6, 7, 8, 9, 10, JACK, QUEEN, KING, ACE,
              2, 3, 4, 5, 6, 7, 8, 9, 10, JACK, QUEEN, KING, ACE};

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}

void setup() {

}