#ifndef INVISIBLEMAN_HPP
#define INVISIBLEMAN_HPP

#include "monster.hpp"

class InvisibleMan : public Monster {
public:
    InvisibleMan(std::shared_ptr<Town> startingTown);

};

#endif