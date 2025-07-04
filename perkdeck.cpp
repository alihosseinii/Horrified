#include "perkdeck.hpp"
#include <algorithm>
#include <iostream>

using namespace std;

PerkDeck::PerkDeck() {
    rng.seed(static_cast<unsigned>(chrono::steady_clock::now().time_since_epoch().count()));
    initializeDefaultCards();
    shuffle();
}

void PerkDeck::initializeDefaultCards() {
    for (int i = 0; i < 3; ++i) {
        cards.emplace_back(PerkCard(PerkType::VisitFromTheDetective));
        cards.emplace_back(PerkCard(PerkType::BreakOfDawn));
        cards.emplace_back(PerkCard(PerkType::Repel));
        cards.emplace_back(PerkCard(PerkType::Hurry));
    }
    for (int i = 0; i < 4; ++i) {
        cards.emplace_back(PerkCard(PerkType::Overstock));
        cards.emplace_back(PerkCard(PerkType::LateIntoTheNight));
    }
}

void PerkDeck::shuffle() {
    std::shuffle(cards.begin(), cards.end(), rng);
}

PerkCard PerkDeck::drawRandomCard() {
    if (isEmpty()) {
        throw runtime_error("No perk cards left!");
    }
    shuffle();
    PerkCard card = cards.back();
    cards.pop_back();
    return card;
}

bool PerkDeck::isEmpty() const {
    return cards.empty();
}
