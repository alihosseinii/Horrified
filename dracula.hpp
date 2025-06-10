#ifndef DRACULA_HPP
#define DRACULA_HPP

#include "monster.hpp"

class Dracula : public Monster {
public:
    Dracula(std::shared_ptr<Town> startingTown);

};

#endif