#include <iostream>
#include "PerkCard.hpp"

using namespace std;

PerkCard::PerkCard(PerkType type) : type(type) {
    switch (type) {
        case PerkType::VisitFromTheDetective:
            description = "place the invisible man in a desired location";
            break;
        case PerkType::BreakOfDawn:
            description = "the next monster phase is skiped.add two new items.";
            break;
        case PerkType::Overstock:
            description = "each player receives one item";
            break;
        case PerkType::LateIntoTheNight:
            description = "the current player will have two additional actions.";
            break;
        case PerkType::Repel:
            description = "move each monster two spaces.";
            break;
        case PerkType::Hurry:
            description = "move each hero two spaces.";
            break;
    }
}

PerkType PerkCard::getType() const {
    return type;
}

string PerkCard::getDescription() const {
    return description;
}

string PerkCard::perkTypeToString(PerkType type) {
    switch (type) {
        case PerkType::VisitFromDetective: return "Visit from the Detective";
        case PerkType::BreakOfDawn: return "Break of Dawn";
        case PerkType::Overstock: return "Overstock";
        case PerkType::LateIntoTheNight: return "Late into the Night";
        case PerkType::Repel: return "Repel";
        case PerkType::Hurry: return "Hurry";
        default: return "Unknown";
    }
}
