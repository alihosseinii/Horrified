#ifndef FRENZYMARKER_HPP
#define FRENZYMARKER_HPP

#include "monster.hpp"
#include <vector>

class FrenzyMarker {
private:
    Monster* currentFrenzied;
    std::vector<Monster*> monsterOrder;

public:
    FrenzyMarker(Monster* dracula, Monster* invisibleMan);

    Monster* getCurrentFrenzied() const;
    void advance(Monster* dracula, Monster* invisibleMan);
};

#endif
