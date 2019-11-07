//
// Created by mbdin on 11/2/2019.
//

#ifndef PAIR_WAR_PLAYER_H
#define PAIR_WAR_PLAYER_H


class Player {

public:
    Player(){

    }

    void draw_card();
    void discard();

    void get_card(int card) {
        this->card = card;

    }
private:
    int card;

};


#endif //PAIR_WAR_PLAYER_H
