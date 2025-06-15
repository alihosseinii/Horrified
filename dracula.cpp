#include "dracula.hpp"
#include "hero.hpp"

using namespace std;

Dracula::Dracula(shared_ptr<Location> startingLocation) : Monster("Dracula", startingLocation) {
    powerName = "Dark Charm";
}

void Dracula::power(Hero* hero) {
    if (currentLocation == hero->getCurrentLocation()) return;

    auto heroLocation = hero->getCurrentLocation();
    hero->setCurrentLocation(currentLocation);
    heroLocation->removeCharacter(hero->getHeroName());
    currentLocation->addCharacter(hero->getHeroName());
    cout << hero->getPlayerName() << " (" << hero->getHeroName() << ") moved to " << hero->getCurrentLocation()->getName() << ".\n";
}