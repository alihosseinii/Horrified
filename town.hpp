#ifndef TOWN_HPP
#define TOWN_HPP

#include <string>
#include <vector>
#include <memory>

class Town {
private:
    std::string name;
    std::vector<std::shared_ptr<Town>> neighbors;
    std::vector<std::string> characters;
public:
    Town(const std::string& townName);

    const std::string& getName() const;
    const std::vector<std::shared_ptr<Town>>& getNeighbors() const;
    const std::vector<std::string>& getCharacters() const;

    void addNeighbor(std::shared_ptr<Town> neighbor);

    void addCharacter(const std::string& character);
    void removeCharacter(const std::string& character);
};

#endif