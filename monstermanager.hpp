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
class PerkDeck;
class Hero;

class MonsterManager {
private:
    vector<MonsterCard> cards;
    std::mt19937 rng;
    MonsterCard currentCard;  
    bool hasCurrentCard;     
    bool pendingHeroAttack = false;
    std::string pendingAttackMonsterName;
    bool awaitingResume = false;
    size_t resumeStrikeIndex = 0;
    std::vector<std::string> resumeDiceRemaining;
    int resumeInvisibleManPowerDiceAccumulated = 0;
public:
    MonsterManager();

    void initializeDefaultCards();
    void shuffle();
    MonsterCard drawCard();
    bool isEmpty() const;
    void MonsterPhase(Map& map, ItemBag& itemBag, Dracula* dracula, InvisibleMan* invisibleMan, FrenzyMarker& frenzyMarker, Hero* currentHero, TerrorTracker& terrorTracker, Archeologist* archeologist, Mayor* mayor, Courier* courier, Scientist* scientist, VillagerManager& villagerManager, std::vector<std::string>& diceResults, PerkDeck* perkDeck = nullptr, Hero* hero1 = nullptr, Hero* hero2 = nullptr, bool interactive = true);
    void moveVillagersCloserToSafePlaces(Map& map, VillagerManager& villagerManager, PerkDeck* perkDeck = nullptr, Hero* hero1 = nullptr, Hero* hero2 = nullptr);
    
    const vector<MonsterCard>& getCards() const;
    
    void setCards(const vector<MonsterCard>& newCards);
    
    std::string getCurrentCardName() const;
    MonsterCard getCurrentCard() const;

    bool hasPendingHeroAttack() const { return pendingHeroAttack; }
    std::string getPendingAttackMonsterName() const { return pendingAttackMonsterName; }
    void clearPendingHeroAttack() { pendingHeroAttack = false; pendingAttackMonsterName.clear(); }

    void ResumeMonsterPhaseAfterDefense(Map& map, ItemBag& itemBag, Dracula* dracula, InvisibleMan* invisibleMan, FrenzyMarker& frenzyMarker, Hero* currentHero, TerrorTracker& terrorTracker, Archeologist* archeologist, Mayor* mayor, Courier* courier, Scientist* scientist, VillagerManager& villagerManager, std::vector<std::string>& diceResults, PerkDeck* perkDeck = nullptr, Hero* hero1 = nullptr, Hero* hero2 = nullptr);
};

#endif 