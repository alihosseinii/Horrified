#include "MonsterDeck.h"
#include <algorithm>
#include <random>
#include <ctime>
#include <iostream>

MonsterDeck::MonsterDeck() {
    initializeDefaultCards();
    shuffle();
}

void MonsterDeck::initializeDefaultCards() {
    cards.clear();

    cards.emplace_back(MonsterCard(2, "Move Dracula to hero location.", {
        {MonsterType::InvisibleMan, 1, 2}, {MonsterType::FrenziedMonster, 1, 2}
    }));

    cards.emplace_back(MonsterCard(0, "Place Dracula in Crypt.", {
        {MonsterType::InvisibleMan, 1, 2}, {MonsterType::FrenziedMonster, 1, 2}
    }));

    cards.emplace_back(MonsterCard(3, "Place Wilbur & Chick in Docks.", {
        {MonsterType::FrenziedMonster, 1, 3}
    }));

    cards.emplace_back(MonsterCard(3, "Place Dr. Cranly in Laboratory.", {
        {MonsterType::InvisibleMan, 1, 2}, {MonsterType::FrenziedMonster, 1, 2}
    }));

    cards.emplace_back(MonsterCard(3, "Place Fritz in Tower.", {
        {MonsterType::Dracula, 2, 3}
    }));

    cards.emplace_back(MonsterCard(3, "Place Maria in Barn.", {
        {MonsterType::FrenziedMonster, 1, 3}, {MonsterType::Dracula, 1, 3}, {MonsterType::InvisibleMan, 1, 3}
    }));

    cards.emplace_back(MonsterCard(3, "Place Prof. Pearson in Cave.", {
        {MonsterType::Dracula, 2, 2}, {MonsterType::FrenziedMonster, 1, 2}
    }));

    cards.emplace_back(MonsterCard(3, "Place Dr. Read in Institute.", {
        {MonsterType::FrenziedMonster, 1, 2}
    }));

    cards.emplace_back(MonsterCard(3, "Place Maleva in Camp.", {
        {MonsterType::FrenziedMonster, 1, 2}
    }));

    cards.emplace_back(MonsterCard(2, "Move closest hero or villager 1 space toward Dracula.", {
        {MonsterType::InvisibleMan, 1, 2}, {MonsterType::FrenziedMonster, 1, 2}
    }));

    cards.emplace_back(MonsterCard(3, "Advance frenzy token and move all villagers 1 space toward their safe place.", {
        {MonsterType::FrenziedMonster, 3, 2}
    }));

    cards.emplace_back(MonsterCard(2, "Move Invisible Man to location with most items and remove all items there.", {
        {MonsterType::InvisibleMan, 1, 3}, {MonsterType::Dracula, 1, 3}
    }));

    cards.emplace_back(MonsterCard(1, "Place Invisible Man in location of choice.", {
        {MonsterType::FrenziedMonster, 1, 2}, {MonsterType::InvisibleMan, 1, 2}
    }));

    cards.emplace_back(MonsterCard(2, "Move Dracula to Institute.", {
        {MonsterType::InvisibleMan, 1, 2}, {MonsterType::Dracula, 1, 2}
    }));

    cards.emplace_back(MonsterCard(3, "Place Dr. Reed and Prof. Pearson in Institute.", {
        {MonsterType::FrenziedMonster, 1, 3}, {MonsterType::Dracula, 1, 2}
    }));

    cards.emplace_back(MonsterCard(0, "Place Dracula in Cave.", {
        {MonsterType::Dracula, 1, 2}
    }));

    cards.emplace_back(MonsterCard(3, "Place two villagers in Mansion.", {
        {MonsterType::FrenziedMonster, 1, 3}
    }));

    cards.emplace_back(MonsterCard(2, "Move Dracula and Invisible Man each 1 space.", {
        {MonsterType::Dracula, 1, 2}, {MonsterType::InvisibleMan, 1, 2}
    }));
}

void MonsterDeck::shuffle() {
    srand(static_cast<unsigned>(time(0)));
    std::shuffle(cards.begin(), cards.end(), default_random_engine(rand()));
}

MonsterCard MonsterDeck::drawCard() {
    if (isEmpty()) {
        throw runtime_error("No monster cards left!");
    }
    MonsterCard card = cards.back();
    cards.pop_back();
    return card;
}

bool MonsterDeck::isEmpty() const {
    return cards.empty();
}

void MonsterDeck::printDeckSummary() const {
    cout << "Monster Deck contains " << cards.size() << " cards:\n";
    for (const auto& card : cards) {
        cout << "- " << card.getEventText() << "\n";
    }
}
