#ifndef DRACULA_HPP
#define DRACULA_HPP

#include "monster.hpp"

class TerrorTracker;

class Dracula : public Monster {
private:
    std::string powerName;

public:
    Dracula(std::shared_ptr<Location> startingLocation);

    void power(Hero* hero, TerrorTracker& terrorTracker) override;
};

#endif