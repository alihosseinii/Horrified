#include "Item.hpp"
#include <algorithm>
#include <vector>

Item::Item(ItemColor color, int power, const std::string& location) : color(color), power(power), location(location) {}

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

    for (int i = 0; i < 20; ++i) {
        items.emplace_back(ItemColor::Red, (i % 4) + 1, "Bag");
        items.emplace_back(ItemColor::Blue, (i % 4) + 1, "Bag");
        items.emplace_back(ItemColor::Yellow, (i % 4) + 1, "Bag");
    }

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
