#pragma once
#include "Screen.h"
#include "HighScoreManager.h"

class TopScoreScreen : public Screen {
    HighScoreManager& highScoreManager;
    bool finished;
    int next;
    sf::Font font;
    sf::Text topScoresText;
    
public:
    TopScoreScreen(HighScoreManager& hsm);
    void processEvent(sf::RenderWindow& window) override;
    void update(float dt) override {}
    void render(sf::RenderWindow& window) override;
    bool isFinished() const override { return finished; }
    int nextScreen() const override { return next; }
    
private:
    void updateTopScoresText();
};
