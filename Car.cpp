#include "Car.h"

Car::Car(const std::string& file) : speed(200.f), slipTimer(0) {
    texture.loadFromFile(file);
    sprite.setTexture(texture);
    sprite.scale(0.8f, 0.8f);
    sprite.setPosition(300, 620); // Vị trí bắt đầu gần cuối màn hình
}

void Car::steer(float dx) {
    sf::Vector2f pos = sprite.getPosition();
    if (slipTimer > 0)
        dx *= 2.f;
    // Giới hạn di chuyển của xe trong phần đường
    if ((dx < 0 && pos.x + dx > 0) || (dx > 0 && pos.x + dx < 710)) {
        sprite.move(dx, 0);
    }
}

void Car::changeSpeed(float amount) {
    speed += amount;
    if (speed < 100.f) speed = 100.f;
    if (speed > 1500.f) speed = 1500.f;
}

void Car::setSpeed(float newSpeed) {
    speed = newSpeed;
}

void Car::update(float dt) {
    if (slipTimer > 0)
        slipTimer -= dt;
}

void Car::applySlip(float slipForce) {
    float posx = sprite.getPosition().x;
    posx += slipForce * 30.f;

    posx = std::max(0.f, std::min(posx, 710.f));

    sprite.setPosition(posx, 620);
    slipTimer = 1.f;
}

sf::FloatRect Car::getBounds() const {
    return sprite.getGlobalBounds();
}

void Car::setTexture(sf::Texture& newTexture) {
    sprite.setTexture(newTexture);
}

void Car::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}