#ifndef COURIER_HPP
#define COURIER_HPP

#include "hero.hpp"

class Courier : public Hero {
public:
    Courier(const std::string& playerName, std::shared_ptr<Location> startingLocation);

    void setOtherHero(Hero* otherHero) override;
    void specialAction() override;
    void ability(size_t index) override;
private:
    Hero* otherHero = nullptr;
};

#endif