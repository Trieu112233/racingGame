#include "MenuScreen.h"

MenuScreen::MenuScreen() : selected(0), finished(false), next(0) {
    font.loadFromFile("BitcountPropDouble.ttf");
    titleText.setFont(font);
    titleText.setString("RACING GAME");
    titleText.setCharacterSize(64);
    titleText.setFillColor(sf::Color::Cyan);
    titleText.setStyle(sf::Text::Bold);
    titleText.setOrigin(titleText.getLocalBounds().width / 2, titleText.getLocalBounds().height / 2);
    titleText.setPosition(490, 120);
    options = { "Play", "Top Scores", "Exit" };
}

void MenuScreen::processEvent(sf::RenderWindow& window) {
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
                next = selected + 1; // 1: Play, 2: TopScores, 3: Exit
            }
        }
    }
}

void MenuScreen::render(sf::RenderWindow& window) {
    window.clear(sf::Color::Black);
    window.draw(titleText);
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