//
// Created by mbdin on 11/15/2019.
//
#include <iostream>
#include <string>

using namespace std;

#ifndef PAIR_WAR_PLAYER_H
#define PAIR_WAR_PLAYER_H

class Player {
private:
    int card;
    std::string name;

public:
    void set_card(int card) {
        this->card = card;
    }

    int get_card() {
        return this->card;
    }

    void set_name(int id) {
        this->name = "PLAYER " + to_string(id);
    }

    std::string get_name() {
        return name;
    }
};

#endif //PAIR_WAR_PLAYER_H
