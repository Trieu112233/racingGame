#pragma once

#include "Screen.h"
#include <vector>
#include <string>

class GameOverScreen : public Screen {
    int score;
    std::vector<std::string> options;
    int selected;
    bool finished;
    int next;
    sf::Font font;
    sf::Text gameOverText;
    
public:
    GameOverScreen(int finalScore);
    void processEvent(sf::RenderWindow& window) override;
    void update(float dt) override {}
    void render(sf::RenderWindow& window) override;
    bool isFinished() const override { return finished; }
    int nextScreen() const override { return next; }
};
