#ifndef MONSTER_HPP
#define MONSTER_HPP

#include <string>
#include <vector>
#include <memory>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include "location.hpp"
#include "hero.hpp"

class Monster {
public:
    Monster(const std::string& monsterName, std::shared_ptr<Location> startingLocation);
    virtual ~Monster() = default;

    // virtual void attack() = 0;
    virtual void power(Hero* hero) = 0;

    std::string getMonsterName() const;
    std::shared_ptr<Location> getCurrentLocation() const;

    void setCurrentLocation(std::shared_ptr<Location> currentLocation);

    void moveToNearestCharacter(const std::string& targetCharacter, int stepNumber);
protected:
    std::string monsterName;
    std::shared_ptr<Location> currentLocation;

    void setMonsterName(std::string monsterName);
};

#endif