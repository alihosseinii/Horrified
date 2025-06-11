#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include "item.hpp"

class Location {
private:
    std::string name;
    std::vector<std::shared_ptr<Location>> neighbors;
    std::vector<std::string> characters;
    std::vector<Item> items;
    
public:
    Location(const std::string& name);
    
    std::string getName() const;

    void addNeighbor(std::shared_ptr<Location> neighbor);
    const std::vector<std::shared_ptr<Location>>& getNeighbors() const;
    void addCharacter(const std::string& character);
    void removeCharacter(const std::string& character);
    const std::vector<std::string>& getCharacters() const;

    void addItem(const Item& item);
    void removeItem(const Item& item);
    const std::vector<Item>& getItems() const;
    void clearItems();
};

#endif