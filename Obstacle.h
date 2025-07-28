#pragma once
#include <SFML/Graphics.hpp>

class Obstacle {
    sf::Sprite sprite;
    sf::Texture texture;
    bool isSameDirection;

public:
    Obstacle(const sf::Texture& texture, float x, float startY, bool sameDir);

    void update(float carSpeed, float dt);

    void draw(sf::RenderWindow& window);

    sf::FloatRect getBounds();

    bool isOutOfScreen();
};