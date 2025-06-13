#include "map.hpp"
#include "location.hpp"
#include <iostream>
#include <algorithm>

using namespace std;

Map::Map() {
    shared_ptr<Location> Cave = make_shared<Location>("Cave");
    shared_ptr<Location> Camp = make_shared<Location>("Camp");
    shared_ptr<Location> Precinct = make_shared<Location>("Precinct");
    shared_ptr<Location> Inn = make_shared<Location>("Inn");
    shared_ptr<Location> Barn = make_shared<Location>("Barn");
    shared_ptr<Location> Dungeon = make_shared<Location>("Dungeon");
    shared_ptr<Location> Theatre = make_shared<Location>("Theatre");
    shared_ptr<Location> Tower = make_shared<Location>("Tower");
    shared_ptr<Location> Docks = make_shared<Location>("Docks");
    shared_ptr<Location> Mansion = make_shared<Location>("Mansion");
    shared_ptr<Location> Abbey = make_shared<Location>("Abbey");
    shared_ptr<Location> Shop = make_shared<Location>("Shop");
    shared_ptr<Location> Crypt = make_shared<Location>("Crypt");
    shared_ptr<Location> Museum = make_shared<Location>("Museum");
    shared_ptr<Location> Church = make_shared<Location>("Church");
    shared_ptr<Location> Laboratory = make_shared<Location>("Laboratory");
    shared_ptr<Location> Hospital = make_shared<Location>("Hospital");
    shared_ptr<Location> Graveyard = make_shared<Location>("Graveyard");
    shared_ptr<Location> Institute = make_shared<Location>("Institute");

    addLocation(Cave);
    addLocation(Camp);
    addLocation(Precinct);
    addLocation(Inn);
    addLocation(Barn);
    addLocation(Dungeon);
    addLocation(Theatre);
    addLocation(Tower);
    addLocation(Docks);
    addLocation(Mansion);
    addLocation(Abbey);
    addLocation(Shop);
    addLocation(Crypt);
    addLocation(Museum);
    addLocation(Church);
    addLocation(Laboratory);
    addLocation(Hospital);
    addLocation(Graveyard);
    addLocation(Institute);

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

void Map::addLocation(shared_ptr<Location> location) {
    if (locations.find(location->getName()) != locations.end()) {
            throw invalid_argument("Location '" + location->getName() + "' already exists in map.");
    }
    if (location) {
        locations[location->getName()] = location;
    } 
    else {
        throw invalid_argument("Invalid location.");
    }
}

void Map::addNeighbor(const string& locationName1, const string& locationName2) {
    auto it1 = locations.find(locationName1);
    auto it2 = locations.find(locationName2);

    if (it1 != locations.end() && it2 != locations.end()) {
        it1->second->addNeighbor(it2->second);
        it2->second->addNeighbor(it1->second);
    } 
    else {
        throw invalid_argument("Couldn't find one or both locations to set as neighbors.");
    }
}

shared_ptr<Location> Map::getLocation(const std::string& locationName) const {
    auto it = locations.find(locationName);
    if (it != locations.end()) {
        return it->second;
    }
    throw invalid_argument(locationName + " doesn't exist.");
}