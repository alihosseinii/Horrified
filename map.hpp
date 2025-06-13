#ifndef MAP_HPP
#define MAP_HPP

class Location;

#include <unordered_map>
#include <memory>
#include <string>
#include <functional>

struct StringHash {
    std::size_t operator()(const std::string& str) const noexcept {
        return std::hash<std::string>{}(str);
    }
};

class Map {
public:
    std::unordered_map<std::string, std::shared_ptr<Location>, StringHash> locations;

    Map();

    std::shared_ptr<Location> getLocation(const std::string& locationName) const;
    
    void addLocation(std::shared_ptr<Location> location);
    void addNeighbor(const std::string& locationName1, const std::string& locationName2);
};

#endif