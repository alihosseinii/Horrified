#ifndef FRENZYMARKER_HPP
#define FRENZYMARKER_HPP

#include <vector>
#include "Monster.hpp"

class FrenzyMarker {
private:
    Monster* currentFrenzied;
    vector<Monster*> monsterOrder;

public:
    FrenzyMarker(Monster* dracula, Monster* invisibleMan);

    Monster* getCurrentFrenzied() const;
    void advance(Monster* dracula, Monster* invisibleMan); 
};
#endif
