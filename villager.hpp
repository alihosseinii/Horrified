#ifndef VILLAGER_HPP
#define VILLAGER_HPP

#include <string>
#include <vector>
#include <memory>
#include "location.hpp"

class Hero;
class PerkDeck;

class Villager {
public:
    Villager(const std::string& name, std::shared_ptr<Location> startingLocation);

    std::string getVillagerName() const;
    void setVillagerName(std::string villagerName);
    std::shared_ptr<Location> getCurrentLocation() const;
    void setCurrentLocation(std::shared_ptr<Location> currentLocation);

    void move(std::shared_ptr<Location> newLocation, Hero* guidingHero = nullptr, PerkDeck* perkDeck = nullptr);
    void moveByMonster(std::shared_ptr<Location> newLocation, PerkDeck* perkDeck = nullptr, Hero* hero1 = nullptr, Hero* hero2 = nullptr);
    void checkSafePlace(PerkDeck* perkDeck = nullptr, Hero* hero1 = nullptr, Hero* hero2 = nullptr);
private:
    std::string villagerName;
    std::shared_ptr<Location> currentLocation;
};

#endif