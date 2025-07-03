#ifndef HERO_HPP
#define HERO_HPP

#include <string>
#include <vector>
#include <memory>
#include "location.hpp"
#include "map.hpp"
#include "villagermanager.hpp"
#include "item.hpp"
#include "perkcard.hpp"
#include "TaskBoard.hpp"

class PerkDeck;
class InvisibleMan;
class Dracula;

std::string toSentenceCase(std::string name);

class Hero {
public:
    Hero(const std::string& playerName, const std::string& heroName, int maxActions, std::shared_ptr<Location> startingLocation);
    virtual ~Hero() = default;

    virtual void move(std::shared_ptr<Location> newLocation, VillagerManager& villagerManager, PerkDeck* perkDeck = nullptr);
    virtual void guide(VillagerManager& villagerManager, Map& map, PerkDeck* perkDeck = nullptr);
    virtual void pickUp();
    virtual void advance(Dracula& dracula, TaskBoard& taskBoard);
    virtual void defeat(Dracula& dracula, TaskBoard& taskBoard);
    virtual void specialAction() = 0; 

    const std::string& getHeroName() const;
    const std::string& getPlayerName() const;

    int getRemainingActions() const;
    void setRemainingActions(int remainingActions);
    void resetActions(); 

    std::vector<Item> getItems() const;
    void removeItem(size_t index);

    void addPerkCard(const PerkCard& card);
    const std::vector<PerkCard>& getPerkCards() const;
    void displayPerkCards() const;
    bool usePerkCard(size_t index, Map& map, VillagerManager& villagerManager, PerkDeck* perkDeck = nullptr, InvisibleMan* invisibleMan = nullptr, ItemBag* itemBag = nullptr, Hero* otherHero = nullptr, Dracula* dracula = nullptr);
    void removePerkCard(size_t index);
    bool shouldSkipNextMonsterPhase() const;
    void setSkipNextMonsterPhase(bool skip);

    std::shared_ptr<Location> getCurrentLocation() const;
    void setCurrentLocation(std::shared_ptr<Location> currentLocation);
    
    void setMaxActions(int maxActions);
    int getMaxActions() const;

protected:
    std::vector<Item> items;
    std::vector<PerkCard> perkCards;
    std::shared_ptr<Location> currentLocation;
    std::string heroName;
    std::string playerName;
    int maxActions;
    int remainingActions;
    bool skipNextMonsterPhase;

    void setHeroName(std::string heroName);
    void setPlayerName(std::string playerName);
    void moveTwoSteps();
};

#endif