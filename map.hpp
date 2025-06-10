#ifndef MAP_HPP
#define MAP_HPP

#include "town.hpp"
#include <unordered_map>

class Map {
public:
    std::unordered_map<std::string, std::shared_ptr<Town>> towns;

    Map();

    std::shared_ptr<Town> getTown(const std::string& townName) const;
    
    void addTown(std::shared_ptr<Town> town);
    void addNeighbor(const std::string& townName1, const std::string& townName2);
};

#endif