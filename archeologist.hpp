#ifndef ARCHEOLOGIST_HPP
#define ARCHEOLOGIST_HPP

#include <string>
#include <memory>
#include "hero.hpp"

class Archeologist : public Hero {
public:
    Archeologist(const std::string& playerName, std::shared_ptr<Location> startingLocation);

    void specialAction() override;
    void ability(size_t index) override;
};

#endif