#ifndef SCIENTIST_HPP
#define SCIENTIST_HPP

#include "hero.hpp"

class Scientist : public Hero {
public:
    Scientist(const std::string& playerName, std::shared_ptr<Location> startingLocation);

    void specialAction() override;
    void ability(size_t index) override;
};

#endif