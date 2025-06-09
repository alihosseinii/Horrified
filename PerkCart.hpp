#ifndef PERKCARD_HPP
#define PERKCARD_HPP


#include <string>

using namespace std;

enum class PerkType {
    VisitFromDetective,
    BreakOfDawn,
    Overstock,
    LateIntoTheNight,
    Repel,
    Hurry
};

class PerkCard {
private:
    PerkType type;
    string description;

public:
    PerkCard(PerkType type);

    PerkType getType() const;
    string getDescription() const;

    static string perkTypeToString(PerkType type);
};
#endif
