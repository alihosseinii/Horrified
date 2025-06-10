#ifndef VILLAGERMANAGER_HPP
#define VILLAGERMANAGER_HPP

#include <unordered_map>
#include <memory>
#include <string>
#include "villager.hpp"

class VillagerManager {
public:
    void addVillager(const std::string& villagerName, std::shared_ptr<Town> town);
    std::shared_ptr<Villager> getVillager(const std::string& villagerName) const;

private:
    std::unordered_map<std::string, std::shared_ptr<Villager>> villagerMap;

    // friend class Game;
    // friend class Monster;
};

#endif
