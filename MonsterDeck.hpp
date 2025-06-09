#ifndef MONSTERDECK_H
#define MONSTERDECK_H

#include <vector>
#include "MonsterCard.hpp"

using namespace std;

class MonsterDeck {
private:
    vector<MonsterCard> cards;

public:
    MonsterDeck();

    void initializeDefaultCards();
    void shuffle();
    MonsterCard drawCard();
    bool isEmpty() const;
    void printDeckSummary() const;
};

#endif
