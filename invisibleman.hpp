#ifndef INVISIBLEMAN_HPP
#define INVISIBLEMAN_HPP

#include <string>
#include <memory>
#include "monster.hpp"

class Location;

class InvisibleMan : public Monster {
private:
std::string powerName;

public:
    InvisibleMan(std::shared_ptr<Location> startingLocation);

    void power(Hero* hero) override;
};

#endif