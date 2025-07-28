#include "Obstacle.h"

// CarObstacle
CarObstacle::CarObstacle(const sf::Texture& texture, float x, float startY, bool sameDir)
    : isSameDirection(sameDir) {
    sprite.setTexture(texture);
    sprite.setPosition(x, startY);
    if (isSameDirection) sprite.setScale(1.f, 1.f);
    else sprite.setScale(1.f, -1.f);
}

void CarObstacle::update(float carSpeed, float dt) {
    float scrollSpeed = carSpeed;
    float obstacleSpeed = isSameDirection ? scrollSpeed * 0.8f : scrollSpeed + 250.f;
    sprite.move(0, obstacleSpeed * dt);
}

void CarObstacle::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::FloatRect CarObstacle::getBounds() const {
    return sprite.getGlobalBounds();
}

bool CarObstacle::isOutOfScreen() const {
    return sprite.getPosition().y > 900;
}

// BarrierObstacle
BarrierObstacle::BarrierObstacle(const sf::Texture& texture, float x, float startY) {
    sprite.setTexture(texture);
    sprite.setPosition(x, startY);
    sprite.setScale(1.f, 1.f);
}

void BarrierObstacle::update(float carSpeed, float dt) {
    float scrollSpeed = carSpeed;
    sprite.move(0, scrollSpeed * dt);
}

void BarrierObstacle::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::FloatRect BarrierObstacle::getBounds() const {
    return sprite.getGlobalBounds();
}

bool BarrierObstacle::isOutOfScreen() const {
    return sprite.getPosition().y > 900;
}