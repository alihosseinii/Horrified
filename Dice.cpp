#include "dice.hpp"
#include <chrono>

using namespace std;

Dice::Dice() : rng(chrono::steady_clock::now().time_since_epoch().count()), dist(0, 5) {}

DiceFace Dice::roll() {
    int roll = dist(rng);
    if (roll == 0) return DiceFace::Power; 
    else if (roll == 5) return DiceFace::Strike; 
    else return DiceFace::Empty;
}

string Dice::faceToString(DiceFace face) {
    switch (face) {
        case DiceFace::Power: return "!";
        case DiceFace::Strike: return "*";
        case DiceFace::Empty: return " ";
        default: return "?";
    }
}
