#ifndef MAYOR_HPP
#define MAYOR_HPP

#include <string>
#include <memory>
#include "hero.hpp"

class Mayor : public Hero {
public:
    Mayor(const std::string& playerName, std::shared_ptr<Location> startingLocation);

    void specialAction() override;
    void ability(size_t index) override;
};

#endif