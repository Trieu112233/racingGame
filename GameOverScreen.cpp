#include "GameOverScreen.h"

GameOverScreen::GameOverScreen(int finalScore) 
    : score(finalScore), selected(0), finished(false), next(0) {
    options = { "Play Again", "Back to Main Menu", "Exit" };
    
    font.loadFromFile("BitcountPropDouble.ttf");
    gameOverText.setFont(font);
    gameOverText.setString("Game Over\nTotal Score: " + std::to_string(score));
    gameOverText.setCharacterSize(48);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setOrigin(gameOverText.getLocalBounds().width / 2, gameOverText.getLocalBounds().height / 2);
    gameOverText.setPosition(490, 120);
}

void GameOverScreen::processEvent(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Up)
                selected = (selected - 1 + options.size()) % options.size();
            if (event.key.code == sf::Keyboard::Down)
                selected = (selected + 1) % options.size();
            if (event.key.code == sf::Keyboard::Return) {
                finished = true;
                next = selected + 1; // 1: PlayAgain, 2: MainMenu, 3: Exit
            }
        }
    }
}

void GameOverScreen::render(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    window.draw(gameOverText);
    
    for (size_t i = 0; i < options.size(); ++i) {
        sf::Text option;
        option.setFont(font);
        option.setString(options[i]);
        option.setCharacterSize(40);
        option.setStyle(sf::Text::Bold);
        option.setOrigin(option.getLocalBounds().width / 2, option.getLocalBounds().height / 2);
        option.setPosition(490, 300 + i * 80);
        option.setFillColor((int)i == selected ? sf::Color::Red : sf::Color::White);
        window.draw(option);
    }
    window.display();
}