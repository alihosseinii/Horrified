#ifndef PERKDECK_HPP
#define PERKDECK_HPP

#include "perkcard.hpp"
#include <vector>
#include <random>
#include <chrono>

using namespace std;

class PerkDeck {
private:
    vector<PerkCard> cards;
    std::mt19937 rng;

public:
    PerkDeck();
    
    void initializeDefaultCards();
    void shuffle();
    PerkCard drawRandomCard();
    bool isEmpty() const;
    
    const vector<PerkCard>& getCards() const;
    
    void setCards(const vector<PerkCard>& newCards);
};

#endif 