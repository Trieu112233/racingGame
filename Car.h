#pragma once
#include <SFML/Graphics.hpp>

class Car {
    sf::Sprite sprite;
    sf::Texture texture;
    float speed;

public:
    Car(const std::string& file);

    void reset();

    void steer(float dx);

    void draw(sf::RenderWindow& window);

    sf::FloatRect getBounds();

    void changeSpeed(float amount);

    float getSpeed() const;

    float getX() const;
};