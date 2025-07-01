#ifndef PERKCARD_HPP
#define PERKCARD_HPP

#include <string>

enum class PerkType {
    VisitFromTheDetective,
    BreakOfDawn,
    Overstock,
    LateIntoTheNight,
    Repel,
    Hurry
};

class PerkCard {
private:
    PerkType type;
    std::string description;

public:
    PerkCard(PerkType type);

    PerkType getType() const;
    std::string getDescription() const;

    static std::string perkTypeToString(PerkType type);
};
#endif
