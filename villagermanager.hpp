#ifndef VILLAGERMANAGER_HPP
#define VILLAGERMANAGER_HPP

#include <unordered_map>
#include <memory>
#include <string>
#include "villager.hpp"
#include <vector>

class VillagerManager {
public:
    void addVillager(const std::string& villagerName, std::shared_ptr<Location> location);
    std::shared_ptr<Villager> getVillager(const std::string& villagerName) const;
    std::vector<std::shared_ptr<Villager>> getAllVillagers() const {
        std::vector<std::shared_ptr<Villager>> result;
        for (const auto& pair : villagerMap) {
            result.push_back(pair.second);
        }
        return result;
    }

private:
    std::unordered_map<std::string, std::shared_ptr<Villager>> villagerMap;
};

#endif
