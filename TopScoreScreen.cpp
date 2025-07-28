#include "TopScoreScreen.h"

TopScoreScreen::TopScoreScreen(HighScoreManager& hsm) 
    : highScoreManager(hsm), finished(false), next(0) {
    font.loadFromFile("BitcountPropDouble.ttf");
    topScoresText.setFont(font);
    topScoresText.setCharacterSize(24);
    topScoresText.setFillColor(sf::Color::Yellow);
    updateTopScoresText();
}

void TopScoreScreen::processEvent(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                finished = true;
                next = 1; // Back to Main Menu
            }
        }
    }
}

void TopScoreScreen::render(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    window.draw(topScoresText);
    window.display();
}

void TopScoreScreen::updateTopScoresText() {
    highScoreManager.load();
    const auto& scores = highScoreManager.getHighScores();
    std::string topScoresString = "Top Scores:\n\n";
    for (size_t i = 0; i < scores.size(); ++i) {
        topScoresString += std::to_string(i + 1) + ". " +
            std::to_string(scores[i].score) + " - " +
            scores[i].time + "\n";
    }
    topScoresString += "\nPress Esc to return to menu";
    topScoresText.setString(topScoresString);
    topScoresText.setOrigin(topScoresText.getLocalBounds().width / 2, 0);
    topScoresText.setPosition(490, 100);
}