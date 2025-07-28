#pragma once

#include "Screen.h"
#include <vector>
#include <string>

class MenuScreen : public Screen {
    sf::Font font;
    sf::Text titleText;
    std::vector<std::string> options;
    int selected;
    bool finished;
    int next;
    
public:
    MenuScreen();
    void processEvent(sf::RenderWindow& window) override;
    void update(float dt) override {}
    void render(sf::RenderWindow& window) override;
    bool isFinished() const override { return finished; }
    int nextScreen() const override { return next; }
};
