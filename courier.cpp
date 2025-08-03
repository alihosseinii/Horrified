#include "courier.hpp"

using namespace std;

Courier::Courier(const string& playerName, shared_ptr<Location> startingLocation) : Hero(playerName, "Courier", 4, startingLocation) {}

void Courier::setOtherHero(Hero* otherHero) {
    this->otherHero = otherHero;
}

void Courier::specialAction() {
    if (remainingActions <= 0) {
        throw invalid_argument("No remaining actions.");
    }

    string answer;
    if (otherHero) {
        while (true) {
            cout << "Do you want to move to " << otherHero->getCurrentLocation()->getName() << "(Yes or No)? ";
            getline(cin, answer);
            answer = toSentenceCase(answer);
            if (answer == "No") break;
            if (answer == "Yes") {
                auto otherHeroLocation = otherHero->getCurrentLocation();
                try {
                    currentLocation->removeCharacter(heroName);
                    otherHeroLocation->addCharacter(heroName);
                    setCurrentLocation(otherHeroLocation);

                    cout << heroName << " (" << playerName << ") moved to " << currentLocation->getName() << ".\n";
                } catch (const exception& e) {
                    cout << e.what() << endl;
                }
                remainingActions--;
                break;
            }
            cout << "Invalid answer. Please try again" << endl;
        }
    } else {
        throw invalid_argument("Other hero is not set.");
    }
}

void Courier::ability(size_t index) {
    cout << "Courier has no ability." << endl;
}