#ifndef MAYOR_HPP
#define MAYOR_HPP

#include "hero.hpp"

class Mayor : public Hero {
public:
    Mayor(const std::string& playerName, std::shared_ptr<Town> startingTown);

    void specialAction() override;
};

#endif
