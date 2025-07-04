#include "dracula.hpp"
#include "hero.hpp"
#include "terrorteracker.hpp"

using namespace std;

Dracula::Dracula(shared_ptr<Location> startingLocation) : Monster("Dracula", startingLocation) {
    powerName = "Dark Charm";
}

void Dracula::power(Hero* hero, TerrorTracker& terrorTracker) {
    if (currentLocation == hero->getCurrentLocation()) return;

    try {
        auto heroLocation = hero->getCurrentLocation();
        heroLocation->removeCharacter(hero->getHeroName());
        currentLocation->addCharacter(hero->getHeroName());
        hero->setCurrentLocation(currentLocation);
    } catch (const exception& e) {
        cout << e.what() << endl;
    }
    cout << hero->getPlayerName() << " (" << hero->getHeroName() << ") moved to " << hero->getCurrentLocation()->getName() << ".\n";
}