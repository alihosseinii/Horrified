#ifndef MONSTERMANAGER_HPP
#define MONSTERMANAGER_HPP

#include "monstercard.hpp"
#include "dracula.hpp"
#include "invisibleman.hpp"
#include "hero.hpp"
#include "frenzymarker.hpp"
#include "archeologist.hpp"
#include "mayor.hpp"
#include <vector>
#include <random>
#include <chrono>

using namespace std;

class TerrorTracker;
class Archeologist;
class Mayor;
class VillagerManager;

class MonsterManager {
private:
    vector<MonsterCard> cards;
    std::mt19937 rng;
public:
    MonsterManager();

    void initializeDefaultCards();
    void shuffle();
    MonsterCard drawCard();
    bool isEmpty() const;
    void MonsterPhase(Map& map, ItemBag& itemBag, Dracula* dracula, InvisibleMan* invisibleMan, FrenzyMarker& frenzyMarker, Hero* hero, TerrorTracker& terrorTracker, Archeologist* archeologist, Mayor* mayor, VillagerManager& villagerManager);
    void moveVillagersCloserToSafePlaces(Map& map, VillagerManager& villagerManager);
};

#endif 