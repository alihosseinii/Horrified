#ifndef TERRORTRACKER_HPP
#define TERRORTRACKER_HPP

#include <iostream>
#include <string>

class TerrorTracker {
private:
    int level;

public:
    TerrorTracker();

    void increase();  
    int getLevel() const;
};

#endif
