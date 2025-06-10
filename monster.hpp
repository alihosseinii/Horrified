#ifndef MONSTER_HPP
#define MONSTER_HPP

#include "town.hpp"
#include <string>
#include <memory>

class Monster {
public:
    Monster(const std::string& monsterName, std::shared_ptr<Town> startingTown);
    virtual ~Monster() = default;

    // virtual void attack() = 0;
    // virtual void specialAction() = 0;

    std::string getMonsterName() const;
    std::shared_ptr<Town> getCurrentTown() const;

    void setCurrentTown(std::shared_ptr<Town> currentTown);

    void moveToNearestCharacter(const std::string& targetCharacter, int stepNumber);
protected:
    std::string monsterName;
    std::shared_ptr<Town> currentTown;

    void setMonsterName(std::string monsterName);
};

#endif