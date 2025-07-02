#include "villagermanager.hpp"
#include <stdexcept>

using namespace std;

void VillagerManager::addVillager(const string& villagerName, shared_ptr<Location> location) {
    villagerMap[villagerName] = make_shared<Villager>(villagerName, location);
}

shared_ptr<Villager> VillagerManager::getVillager(const string& villagerName) const {
    auto it = villagerMap.find(villagerName);
    if (it != villagerMap.end()) {
        return it->second;
    }
    throw invalid_argument(villagerName + " doesn't exists");
}
