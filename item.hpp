#ifndef ITEM_HPP
#define ITEM_HPP

#include <iostream>
#include <string>
#include <random>

using namespace std;

enum class ItemColor {
    Red,
    Blue,
    Yellow
};

class Location; 

class Item {
private:
    std::string itemName;
    ItemColor color;   
    int power;  
    std::shared_ptr<Location> location;
public:
    Item(std::string itemName, ItemColor color, int power, std::shared_ptr<Location> location);

    ItemColor getColor() const;
    int getPower() const;
    std::string getItemName() const;

    std::shared_ptr<Location> getLocation() const;
    void setLocation(std::shared_ptr<Location> location);

    static string colorToString(ItemColor color);
};

class Map;  

class ItemBag {
public:
    explicit ItemBag(Map& map);

    void shuffleItems();
    void refillItems(Map& map);
    Item drawRandomItem(Map& map);
    const vector<Item>& getItems() const;

private:
    vector<Item> items;
    vector<Item> initialItems;
    mt19937 rng{random_device{}()};
};


#endif


#endif
