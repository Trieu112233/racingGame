#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Screen {
public:
    virtual ~Screen() {}
    virtual void processEvent(sf::RenderWindow& window) = 0;
    virtual void update(float dt) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    virtual bool isFinished() const = 0;
    virtual int nextScreen() const = 0;
};
