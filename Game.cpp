#include "Game.h"
#include "MenuScreen.h"
#include "PlayingScreen.h"
#include "GameOverScreen.h"
#include "TopScoreScreen.h"

Game::Game() : window(sf::VideoMode(980, 800), "Racing Game"), lastScore(0) {
    currentScreen = std::make_unique<MenuScreen>();
    
    if (!mainMenuMusic.openFromFile("menu_theme_music.wav")) {};
    mainMenuMusic.setLoop(true);
    mainMenuMusic.play();
}

void Game::run() {
    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        
        currentScreen->processEvent(window);
        currentScreen->update(dt);
        currentScreen->render(window);
        
        if (currentScreen->isFinished()) {
            int next = currentScreen->nextScreen();
            
            if (dynamic_cast<MenuScreen*>(currentScreen.get())) {
                if (next == 1) {
                    currentScreen = std::make_unique<PlayingScreen>(highScoreManager);
                    mainMenuMusic.stop();
                }
                else if (next == 2) {
                    currentScreen = std::make_unique<TopScoreScreen>(highScoreManager);
                }
                else if (next == 3) {
                    window.close();
                }
            }
            else if (dynamic_cast<PlayingScreen*>(currentScreen.get())) {
                if (next == 2) { // Game Over
                    PlayingScreen* ps = static_cast<PlayingScreen*>(currentScreen.get());
                    lastScore = ps->getScore();
                    currentScreen = std::make_unique<GameOverScreen>(lastScore);
                    mainMenuMusic.play();
                }
            }
            else if (dynamic_cast<GameOverScreen*>(currentScreen.get())) {
                if (next == 1) { // Play Again
                    currentScreen = std::make_unique<PlayingScreen>(highScoreManager);
                    mainMenuMusic.stop();
                }
                else if (next == 2) { // Main Menu
                    currentScreen = std::make_unique<MenuScreen>();
                }
                else if (next == 3) { // Exit
                    window.close();
                }
            }
            else if (dynamic_cast<TopScoreScreen*>(currentScreen.get())) {
                if (next == 1) { // Back to Menu
                    currentScreen = std::make_unique<MenuScreen>();
                }
            }
        }
    }
}
