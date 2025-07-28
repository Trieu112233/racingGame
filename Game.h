#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "Screen.h"
#include "HighScoreManager.h"

class Game {
    sf::RenderWindow window;
    std::unique_ptr<Screen> currentScreen;
    HighScoreManager highScoreManager;
    sf::Music mainMenuMusic;
    int lastScore;
    
public:
    Game();
    void run();
};