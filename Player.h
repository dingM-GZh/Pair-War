//
// Created by mbdin on 11/2/2019.
//
#include <iostream>

#ifndef PAIR_WAR_PLAYER_H
#define PAIR_WAR_PLAYER_H


class Player {
private:
    int card, id;
    std::string name;

public:
    void set_card(int card) {
        this->card = card;
    }

    int get_card() {
       return this->card;
    }

    void set_name(std::string name) {
        this->name = name;
    }

    std::string get_name() {
        return name;
    }

    void set_id(int id) {
        this->id = id;
    }

    int get_id() {
        return id;
    }
};


#endif //PAIR_WAR_PLAYER_H
