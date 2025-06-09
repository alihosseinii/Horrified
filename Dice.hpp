#include <iostream>
#include <random>
#include <string>

using namespace std;

enum class DiceFace {
    Empty,
    Power,
    Strike
};

class Dice {
private:
    mt19937 rng;
    uniform_int_distribution<int> dist;

public:
    Dice();

    DiceFace roll();
    static string faceToString(DiceFace face);
};
