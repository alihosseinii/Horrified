#include "item.hpp"
#include <algorithm>
#include <vector>

Item::Item(string name, ItemColor color, int power, const std::string& location) :name(name), color(color), power(power), location(location) {}

string Item::getname() const{
    return name;
}

ItemColor Item::getColor() const {
    return color;
}

int Item::getPower() const {
    return power;
}

string Item::getLocation() const {
    return location;
}

void Item::setLocation(const string& newLocation) {
    location = newLocation;
}

string Item::colorToString(ItemColor color) {
    switch (color) {
        case ItemColor::Red: return "Red";
        case ItemColor::Blue: return "Blue";
        case ItemColor::Yellow: return "Yellow";
        default: return "Unknown";
    }
}

ItemBag::ItemBag() {
    rng.seed(chrono::steady_clock::now().time_since_epoch().count());

    // Red Items
    items.emplace_back("Torch", ItemColor::Red, 5, "Barn");
    items.emplace_back("Torch", ItemColor::Red, 5, "Barn");
    items.emplace_back("Dart", ItemColor::Red, 2, "Inn");
    items.emplace_back("Dart", ItemColor::Red, 2, "Inn");
    items.emplace_back("Fire pocker", ItemColor::Red, 3, "Mansion");
    items.emplace_back("Fire pocker", ItemColor::Red, 3, "Mansion");
    items.emplace_back("Rapier", ItemColor::Red, 5, "Theatre");
    items.emplace_back("Rapier", ItemColor::Red, 5, "Theatre");
    items.emplace_back("Shovel", ItemColor::Red, 2, "Graveyard");
    items.emplace_back("Shovel", ItemColor::Red, 2, "Graveyard");
    items.emplace_back("Pitchfork", ItemColor::Red, 4, "Barn");
    items.emplace_back("Pitchfork", ItemColor::Red, 4, "Barn");
    items.emplace_back("Rifle", ItemColor::Red, 6, "Barn");
    items.emplace_back("Rifle", ItemColor::Red, 6, "Barn");
    items.emplace_back("Silver cane", ItemColor::Red, 6, "Hsop");
    items.emplace_back("Silver cane", ItemColor::Red, 6, "Hsop");
    items.emplace_back("Knife", ItemColor::Red, 3, "Docks");
    items.emplace_back("Knife", ItemColor::Red, 3, "Docks");
    items.emplace_back("Pistol", ItemColor::Red, 6, "Precict");
    items.emplace_back("Pistol", ItemColor::Red, 6, "Precinct");
    items.emplace_back("Bear trap", ItemColor::Red, 4, "Shop");
    items.emplace_back("Bear trap", ItemColor::Red, 4, "Shop");
    items.emplace_back("Speargun", ItemColor::Red, 4, "Institute");
    items.emplace_back("Speargun", ItemColor::Red, 4, "Institute");


    // Blue Items
    items.emplace_back("Anatomy test", ItemColor::Blue, 1, "Institute");
    items.emplace_back("Anatomy test", ItemColor::Blue, 1, "Institute");
    items.emplace_back("Centrifuge", ItemColor::Blue, 1, "Laboratory"); 
    items.emplace_back("Centrifuge", ItemColor::Blue, 1, "Laboratory");
    items.emplace_back("Kite", ItemColor::Blue, 1, "Tower");
    items.emplace_back("Kite", ItemColor::Blue, 1, "Tower");
    items.emplace_back("Research", ItemColor::Blue, 2, "Tower");
    items.emplace_back("Research", ItemColor::Blue, 2, "Tower");
    items.emplace_back("Telescope", ItemColor::Blue, 2, "Mansion");
    items.emplace_back("Telescope", ItemColor::Blue, 2, "Mansion");
    items.emplace_back("Searchlight", ItemColor::Blue, 2, "precint");
    items.emplace_back("Searchlight", ItemColor::Blue, 2, "precint");
    items.emplace_back("Experiment", ItemColor::Blue, 2, "Laboratory");
    items.emplace_back("Experiment", ItemColor::Blue, 2, "Laboratory");
    items.emplace_back("Analysis", ItemColor::Blue, 2, "Institute");
    items.emplace_back("Analysis", ItemColor::Blue, 2, "Institute");
    items.emplace_back("Rotenone", ItemColor::Blue, 3, "Institute");
    items.emplace_back("Rotenone", ItemColor::Blue, 3, "Institute");
    items.emplace_back("Cosmic Ray Diffuser", ItemColor::Blue, 3, "Tower");
    items.emplace_back("Cosmic Ray Diffuser", ItemColor::Blue, 3, "Tower");
    items.emplace_back("Nebularium", ItemColor::Blue, 3, "Tower");
    items.emplace_back("Nebularium", ItemColor::Blue, 3, "Tower");
    items.emplace_back("Fossil", ItemColor::Blue, 3, "camp");
    items.emplace_back("Fossil", ItemColor::Blue, 3, "camp");
    items.emplace_back("Monocane Mixture", ItemColor::Blue, 3, "Inn");
    items.emplace_back("Monocane Mixture", ItemColor::Blue, 3, "Inn");

    // Yellow Items

    items.emplace_back("Flower", ItemColor::Yellow, 2, "Docks");
    items.emplace_back("Flower", ItemColor::Yellow, 2, "Docks");
    items.emplace_back("Tarot dech", ItemColor::Yellow, 3, "Camp");
    items.emplace_back("Tarot dech", ItemColor::Yellow, 3, "Camp");
    items.emplace_back("Garlic", ItemColor::Yellow, 2, "Inn");
    items.emplace_back("Garlic", ItemColor::Yellow, 2, "Inn");
    items.emplace_back("Mirrored Box", ItemColor::Yellow, 3, "Mansion");
    items.emplace_back("Mirrored Box", ItemColor::Yellow, 3, "Mansion");
    items.emplace_back("Stake", ItemColor::Yellow, 3, "Abbey");
    items.emplace_back("Stake", ItemColor::Yellow, 3, "Abbey");
    items.emplace_back("Scroll of Thoth", ItemColor::Yellow, 4, "Museum");
    items.emplace_back("Scroll of Thoth", ItemColor::Yellow, 4, "Museum");
    items.emplace_back("violin", ItemColor::Yellow, 3, "Camp");
    items.emplace_back("violin", ItemColor::Yellow, 3, "Camp");
    items.emplace_back("tablet", ItemColor::Yellow, 3, "Museum");
    items.emplace_back("tablet", ItemColor::Yellow, 3, "Museum");
    items.emplace_back("Wolfsbane", ItemColor::Yellow, 4, "Camp");
    items.emplace_back("Wolfsbane", ItemColor::Yellow, 4, "Camp");
    items.emplace_back("Charm", ItemColor::Yellow, 4, "Camp");
    items.emplace_back("Charm", ItemColor::Yellow, 4, "Camp");

    shuffleItems();
}

void ItemBag::shuffleItems() {
    shuffle(items.begin(), items.end(), rng);
}

bool ItemBag::isEmpty() const {
    return items.empty();
}

Item ItemBag::drawRandomItem() {
    if (items.empty()) {
        throw runtime_error("Item bag is empty!");
    }
    Item top = items.back();
    items.pop_back();
    return top;
}
