#include "item.hpp"
#include "location.hpp"
#include "map.hpp"
#include <random>
#include <algorithm>
#include <stdexcept>
#include <chrono>

using namespace std;

Item::Item(string itemName, ItemColor color, int power, const shared_ptr<Location> location) : itemName(itemName), color(color), power(power) {
    setLocation(location);
}

ItemColor Item::getColor() const {
    return color;
}

int Item::getPower() const {
    return power;
}

string Item::getItemName() const {
    return itemName;
}

void Item::setItemPower(int newPower) {
    this->power = newPower;
}

shared_ptr<Location> Item::getLocation() const {
    return location;
}

void Item::setLocation(shared_ptr<Location> location) {
    this->location = location;
}

string Item::colorToString(ItemColor color) {
    switch (color) {
        case ItemColor::Red:
            return "Red";
        case ItemColor::Blue:
            return "Blue";
        case ItemColor::Yellow:
            return "Yellow";
        default:
            return "Unknown";
    }
}

ItemBag::ItemBag(Map& map) {
    rng.seed(chrono::steady_clock::now().time_since_epoch().count());

    items.emplace_back("Torch", ItemColor::Red, 5, map.getLocation("Barn"));
    items.emplace_back("Torch", ItemColor::Red, 5, map.getLocation("Barn"));
    items.emplace_back("Dart", ItemColor::Red, 2, map.getLocation("Inn"));
    items.emplace_back("Dart", ItemColor::Red, 2, map.getLocation("Inn"));
    items.emplace_back("Fire pocker", ItemColor::Red, 3, map.getLocation("Mansion"));
    items.emplace_back("Fire pocker", ItemColor::Red, 3, map.getLocation("Mansion"));
    items.emplace_back("Rapier", ItemColor::Red, 5, map.getLocation("Theatre"));
    items.emplace_back("Rapier", ItemColor::Red, 5, map.getLocation("Theatre"));
    items.emplace_back("Shovel", ItemColor::Red, 2, map.getLocation("Graveyard"));
    items.emplace_back("Shovel", ItemColor::Red, 2, map.getLocation("Graveyard"));
    items.emplace_back("Pitchfork", ItemColor::Red, 4, map.getLocation("Barn"));
    items.emplace_back("Pitchfork", ItemColor::Red, 4, map.getLocation("Barn"));
    items.emplace_back("Rifle", ItemColor::Red, 6, map.getLocation("Barn"));
    items.emplace_back("Rifle", ItemColor::Red, 6, map.getLocation("Barn"));
    items.emplace_back("Silver cane", ItemColor::Red, 6, map.getLocation("Shop"));
    items.emplace_back("Silver cane", ItemColor::Red, 6, map.getLocation("Shop"));
    items.emplace_back("Knife", ItemColor::Red, 3, map.getLocation("Docks"));
    items.emplace_back("Knife", ItemColor::Red, 3, map.getLocation("Docks"));
    items.emplace_back("Pistol", ItemColor::Red, 6, map.getLocation("Precinct"));
    items.emplace_back("Pistol", ItemColor::Red, 6, map.getLocation("Precinct"));
    items.emplace_back("Bear trap", ItemColor::Red, 4, map.getLocation("Shop"));
    items.emplace_back("Bear trap", ItemColor::Red, 4, map.getLocation("Shop"));
    items.emplace_back("Speargun", ItemColor::Red, 4, map.getLocation("Institute"));
    items.emplace_back("Speargun", ItemColor::Red, 4, map.getLocation("Institute"));

    items.emplace_back("Anatomy test", ItemColor::Blue, 1, map.getLocation("Institute"));
    items.emplace_back("Anatomy test", ItemColor::Blue, 1, map.getLocation("Institute"));
    items.emplace_back("Centrifuge", ItemColor::Blue, 1, map.getLocation("Laboratory")); 
    items.emplace_back("Centrifuge", ItemColor::Blue, 1, map.getLocation("Laboratory"));
    items.emplace_back("Kite", ItemColor::Blue, 1, map.getLocation("Tower"));
    items.emplace_back("Kite", ItemColor::Blue, 1, map.getLocation("Tower"));
    items.emplace_back("Research", ItemColor::Blue, 2, map.getLocation("Tower"));
    items.emplace_back("Research", ItemColor::Blue, 2, map.getLocation("Tower"));
    items.emplace_back("Telescope", ItemColor::Blue, 2, map.getLocation("Mansion"));
    items.emplace_back("Telescope", ItemColor::Blue, 2, map.getLocation("Mansion"));
    items.emplace_back("Searchlight", ItemColor::Blue, 2, map.getLocation("Precinct"));
    items.emplace_back("Searchlight", ItemColor::Blue, 2, map.getLocation("Precinct"));
    items.emplace_back("Experiment", ItemColor::Blue, 2, map.getLocation("Laboratory"));
    items.emplace_back("Experiment", ItemColor::Blue, 2, map.getLocation("Laboratory"));
    items.emplace_back("Analysis", ItemColor::Blue, 2, map.getLocation("Institute"));
    items.emplace_back("Analysis", ItemColor::Blue, 2, map.getLocation("Institute"));
    items.emplace_back("Rotenone", ItemColor::Blue, 3, map.getLocation("Institute"));
    items.emplace_back("Rotenone", ItemColor::Blue, 3, map.getLocation("Institute"));
    items.emplace_back("Cosmic Ray Diffuser", ItemColor::Blue, 3, map.getLocation("Tower"));
    items.emplace_back("Cosmic Ray Diffuser", ItemColor::Blue, 3, map.getLocation("Tower"));
    items.emplace_back("Nebularium", ItemColor::Blue, 3, map.getLocation("Tower"));
    items.emplace_back("Nebularium", ItemColor::Blue, 3, map.getLocation("Tower"));
    items.emplace_back("Fossil", ItemColor::Blue, 3, map.getLocation("Camp"));
    items.emplace_back("Fossil", ItemColor::Blue, 3, map.getLocation("Camp"));
    items.emplace_back("Monocane Mixture", ItemColor::Blue, 3, map.getLocation("Inn"));
    items.emplace_back("Monocane Mixture", ItemColor::Blue, 3, map.getLocation("Inn"));

    items.emplace_back("Flower", ItemColor::Yellow, 2, map.getLocation("Docks"));
    items.emplace_back("Flower", ItemColor::Yellow, 2, map.getLocation("Docks"));
    items.emplace_back("Tarot dech", ItemColor::Yellow, 3, map.getLocation("Camp"));
    items.emplace_back("Tarot dech", ItemColor::Yellow, 3, map.getLocation("Camp"));
    items.emplace_back("Garlic", ItemColor::Yellow, 2, map.getLocation("Inn"));
    items.emplace_back("Garlic", ItemColor::Yellow, 2, map.getLocation("Inn"));
    items.emplace_back("Mirrored Box", ItemColor::Yellow, 3, map.getLocation("Mansion"));
    items.emplace_back("Mirrored Box", ItemColor::Yellow, 3, map.getLocation("Mansion"));
    items.emplace_back("Stake", ItemColor::Yellow, 3, map.getLocation("Abbey"));
    items.emplace_back("Stake", ItemColor::Yellow, 3, map.getLocation("Abbey"));
    items.emplace_back("Scroll of Thoth", ItemColor::Yellow, 4, map.getLocation("Museum"));
    items.emplace_back("Scroll of Thoth", ItemColor::Yellow, 4, map.getLocation("Museum"));
    items.emplace_back("violin", ItemColor::Yellow, 3, map.getLocation("Camp"));
    items.emplace_back("violin", ItemColor::Yellow, 3, map.getLocation("Camp"));
    items.emplace_back("tablet", ItemColor::Yellow, 3, map.getLocation("Mansion"));
    items.emplace_back("tablet", ItemColor::Yellow, 3, map.getLocation("Mansion"));
    items.emplace_back("Wolfsbane", ItemColor::Yellow, 4, map.getLocation("Camp"));
    items.emplace_back("Wolfsbane", ItemColor::Yellow, 4, map.getLocation("Camp"));
    items.emplace_back("Charm", ItemColor::Yellow, 4, map.getLocation("Camp"));
    items.emplace_back("Charm", ItemColor::Yellow, 4, map.getLocation("Camp"));

    initialItems = items;
    
    shuffleItems();

    for (int i = 0; i < 12; ++i) {
        drawRandomItem(map);
    }
}

void ItemBag::shuffleItems() {
    shuffle(items.begin(), items.end(), rng);
}

void ItemBag::refillItems(Map& map) {
    vector<Item> usedItems;
    for (const auto& pair : map.locations) {
        const auto& location = pair.second;
        const auto& locItems = location->getItems();
        usedItems.insert(usedItems.end(), locItems.begin(), locItems.end());
    }

    items.clear();

    for (const auto& item : initialItems) {
        auto it = find_if(usedItems.begin(), usedItems.end(), [&](const Item& used) {
            return item.getItemName() == used.getItemName() &&
                   item.getColor() == used.getColor() &&
                   item.getPower() == used.getPower() &&
                   item.getLocation() == used.getLocation();
        });

        if (it == usedItems.end()) {
            items.push_back(item);
        }
    }

    shuffleItems();
}

Item ItemBag::drawRandomItem(Map& map) {
    if (items.empty()) {
        refillItems(map);
    }
    shuffleItems();

    Item item = items.back();
    items.pop_back();

    auto location = map.getLocation(item.getLocation()->getName());
    location->addItem(item); 

    return item;
}

const vector<Item>& ItemBag::getItems() const {
    return items;
}
