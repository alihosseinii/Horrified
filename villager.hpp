#ifndef VILLAGER_HPP
#define VILLAGER_HPP

#include <string>
#include <vector>
#include <memory>
#include "town.hpp"

class Villager {
public:
    Villager(const std::string& name, std::shared_ptr<Town> startingTown);

    std::string getVillagerName() const;
    void setVillagerName(std::string villagerName);
    std::shared_ptr<Town> getCurrentTown() const;
    void setCurrentTown(std::shared_ptr<Town> currentTown);

    void move(std::shared_ptr<Town> newTown);
private:
    std::string villagerName;
    std::shared_ptr<Town> currentTown;
};

#endif