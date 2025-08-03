#include "scientist.hpp"

using namespace std;

Scientist::Scientist(const string& playerName, shared_ptr<Location> startingLocation) : Hero(playerName, "Scientist", 4, startingLocation) {}

void Scientist::specialAction() {
    cout << "Scientist has no special action." << endl;
}

void Scientist::ability(size_t index) {
    string answer;
    while (true) {
        cout << "Do you want to use your ability(Yes or No)? ";
        getline(cin, answer);
        answer = toSentenceCase(answer);
        if (answer == "No") break;
        if (answer == "Yes") {
            if (index >= items.size()) {
                throw out_of_range("Item index out of range");
            }
            int currentPower = items.at(index).getPower();
            items.at(index).setItemPower(currentPower + 1);
            break;
        }
        cout << "Invalid answer. Please try again" << endl;
    }
}