#ifndef FRENZYMARKER_HPP
#define FRENZYMARKER_HPP

#include <vector>
#include "Monster.h"

class FrenzyMarker {
private:
    Monster* currentFrenzied;
    std::vector<Monster*> monsterOrder;

public:
    FrenzyMarker(const std::vector<Monster*>& allMonsters);

    Monster* getCurrentFrenzied() const;
    void advance();
    void reset();  
};

#endif
