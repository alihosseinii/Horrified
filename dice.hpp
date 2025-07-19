#ifndef DICE_HPP
#define DICE_HPP

#include <iostream>
#include <random>
#include <string>

enum class DiceFace {
    Empty,
    Power,
    Strike
};

class Dice {
private:
    std::mt19937 rng;
    std::uniform_int_distribution<int> dist;
public:
    Dice();

    DiceFace roll();
    static std::string faceToString(DiceFace face);
};

#endif
