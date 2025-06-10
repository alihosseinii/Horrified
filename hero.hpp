#ifndef HERO_HPP
#define HERO_HPP

#include <string>
#include <vector>
#include <memory>
#include "town.hpp"

class Hero {
public:
    Hero(const std::string& playerName, const std::string& heroName, int maxActions, std::shared_ptr<Town> startingTown);
    virtual ~Hero() = default;

    virtual void move(std::shared_ptr<Town> newTown);
    //virtual void guide(Villager);
    // virtual void pickUp();
    // virtual void advance();
    // virtual void defeat();
    virtual void specialAction() = 0; 

    const std::string& getHeroName() const;
    const std::string& getPlayerName() const;

    int getRemainingActions() const;
    void setRemainingActions(int remainingActions);
    void resetActions(); 

    // void addItem(const Item& item);
    // const std::vector<Item>& getItems() const;

    // void addPerkCard(const PerkCard& card);
    // const std::vector<PerkCard>& getPerkCards() const;

    std::shared_ptr<Town> getCurrentTown() const;
    void setCurrentTown(std::shared_ptr<Town> currentTown);
    
protected:
    std::shared_ptr<Town> currentTown;
    std::string heroName;
    std::string playerName;
    int maxActions;
    int remainingActions;
    // std::vector<Item> inventory;
    // std::vector<PerkCard> perkCards;

    void setHeroName(std::string heroName);
    void setPlayerName(std::string playerName);
    void setMaxActions(int maxActions);
    int getMaxActions() const;
};

#endif