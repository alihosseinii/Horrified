#ifndef TERRORTRACKER_HPP
#define TERRORTRACKER_HPP

#include <iostream>
#include <string>

class TerrorTracker {
private:
    int level;

public:
    TerrorTracker();

    void reset();
    void increase();  
    int getLevel() const;

    bool lost() const;
    std::string getStatus() const;
};

#endif 
