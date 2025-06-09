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

class Item {
private:
    ItemColor color;   
    int power;  
    string location;

public:
    Item(ItemColor color, int power, const string& location);

    ItemColor getColor() const;
    int getPower() const;
    string getLocation() const;

    void setLocation(const string& newLocation);

    static string colorToString(ItemColor color);
};

class ItemBag {
private:
    vector<Item> items;
    mt19937 rng;

public:
    ItemBag();       
    void shuffleItems();  
    bool isEmpty() const;
    Item drawRandomItem();      
};


#endif
