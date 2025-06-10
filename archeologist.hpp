#ifndef ARCHEOLOGIST_HPP
#define ARCHEOLOGIST_HPP

#include "hero.hpp"
#include <iostream>

class Archeologist : public Hero {
public:
    Archeologist(const std::string& playerName, std::shared_ptr<Town> startingTown);

    void specialAction() override;
};

#endif