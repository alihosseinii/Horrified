#include "map.hpp"
#include <iostream>
#include <algorithm>

using namespace std;

Map::Map() {
    shared_ptr<Town> Cave = make_shared<Town>("Cave");
    shared_ptr<Town> Camp = make_shared<Town>("Camp");
    shared_ptr<Town> Precinct = make_shared<Town>("Precinct");
    shared_ptr<Town> Inn = make_shared<Town>("Inn");
    shared_ptr<Town> Barn = make_shared<Town>("Barn");
    shared_ptr<Town> Dungeon = make_shared<Town>("Dungeon");
    shared_ptr<Town> Theatre = make_shared<Town>("Theatre");
    shared_ptr<Town> Tower = make_shared<Town>("Tower");
    shared_ptr<Town> Docks = make_shared<Town>("Docks");
    shared_ptr<Town> Mansion = make_shared<Town>("Mansion");
    shared_ptr<Town> Abbey = make_shared<Town>("Abbey");
    shared_ptr<Town> Shop = make_shared<Town>("Shop");
    shared_ptr<Town> Crypt = make_shared<Town>("Crypt");
    shared_ptr<Town> Museum = make_shared<Town>("Museum");
    shared_ptr<Town> Church = make_shared<Town>("Church");
    shared_ptr<Town> Laboratory = make_shared<Town>("Laboratory");
    shared_ptr<Town> Hospital = make_shared<Town>("Hospital");
    shared_ptr<Town> Graveyard = make_shared<Town>("Graveyard");
    shared_ptr<Town> Institute = make_shared<Town>("Institute");

    addTown(Cave);
    addTown(Camp);
    addTown(Precinct);
    addTown(Inn);
    addTown(Barn);
    addTown(Dungeon);
    addTown(Theatre);
    addTown(Tower);
    addTown(Docks);
    addTown(Mansion);
    addTown(Abbey);
    addTown(Shop);
    addTown(Crypt);
    addTown(Museum);
    addTown(Church);
    addTown(Laboratory);
    addTown(Hospital);
    addTown(Graveyard);
    addTown(Institute);

    addNeighbor("Cave", "Camp");
    addNeighbor("Camp", "Precinct");
    addNeighbor("Camp", "Mansion"); 
    addNeighbor("Precinct", "Mansion"); 
    addNeighbor("Precinct", "Inn");
    addNeighbor("Precinct", "Theatre"); 
    addNeighbor("Inn", "Theatre"); 
    addNeighbor("Theatre", "Mansion"); 
    addNeighbor("Theatre", "Barn"); 
    addNeighbor("Theatre", "Tower"); 
    addNeighbor("Theatre", "Shop"); 
    addNeighbor("Tower", "Dungeon"); 
    addNeighbor("Tower", "Docks"); 
    addNeighbor("Mansion", "Inn"); 
    addNeighbor("Mansion", "Abbey"); 
    addNeighbor("Mansion", "Shop"); 
    addNeighbor("Mansion", "Museum"); 
    addNeighbor("Mansion", "Church"); 
    addNeighbor("Abbey", "Crypt"); 
    addNeighbor("Museum", "Shop"); 
    addNeighbor("Museum", "Church"); 
    addNeighbor("Shop", "Church"); 
    addNeighbor("Shop", "Laboratory"); 
    addNeighbor("Church", "Hospital"); 
    addNeighbor("Church", "Graveyard"); 
    addNeighbor("Laboratory", "Institute"); 
}

void Map::addTown(shared_ptr<Town> town) {
    if (towns.find(town->getName()) != towns.end()) {
            throw invalid_argument("Town '" + town->getName() + "' already exists in map.");
    }
    if (town) {
        towns[town->getName()] = town;
    } 
    else {
        throw invalid_argument("Invalid town.");
    }
}

void Map::addNeighbor(const string& townName1, const string& townName2) {
    auto it1 = towns.find(townName1);
    auto it2 = towns.find(townName2);

    if (it1 != towns.end() && it2 != towns.end()) {
        it1->second->addNeighbor(it2->second);
        it2->second->addNeighbor(it1->second);
    } 
    else {
        throw invalid_argument("Couldn't find one or both towns to set as neighbors.");
    }
}

shared_ptr<Town> Map::getTown(const std::string& townName) const {
    auto it = towns.find(townName);
    if (it != towns.end()) {
        return it->second;
    }
    throw invalid_argument(townName + " doesn't exist.");
}