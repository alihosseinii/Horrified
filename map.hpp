#ifndef MAP_HPP
#define MAP_HPP

#include "location.hpp"
#include <unordered_map>
#include <memory>
#include <string>
#include <functional>

class Map {
public:
    std::unordered_map<std::string, std::shared_ptr<Location>> locations;

    Map();

    std::shared_ptr<Location> getLocation(const std::string& locationName) const;
    std::shared_ptr<Location> getLocationWithMostItems() const;
    int calculateDistance(std::shared_ptr<Location> from, std::shared_ptr<Location> to) const;
    std::shared_ptr<Location> findCloserLocation(std::shared_ptr<Location> current, std::shared_ptr<Location> target) const;
    
    void addLocation(std::shared_ptr<Location> location);
    void addNeighbor(const std::string& locationName1, const std::string& locationName2);
};

#endif