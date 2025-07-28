#pragma once
#include <SFML/Graphics.hpp>

class Obstacle {
public:
    virtual ~Obstacle() {}
    virtual void update(float carSpeed, float dt) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;
    virtual sf::FloatRect getBounds() const = 0;
    virtual bool isOutOfScreen() const = 0;
};

class CarObstacle : public Obstacle {
    sf::Sprite sprite;
    bool isSameDirection;
public:
    CarObstacle(const sf::Texture& texture, float x, float startY, bool sameDir);
    void update(float carSpeed, float dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    bool isOutOfScreen() const override;
};

class BarrierObstacle : public Obstacle {
    sf::Sprite sprite;
public:
    BarrierObstacle(const sf::Texture& texture, float x, float startY);
    void update(float carSpeed, float dt) override;
    void draw(sf::RenderWindow& window) override;
    sf::FloatRect getBounds() const override;
    bool isOutOfScreen() const override;
};