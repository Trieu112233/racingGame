#include "Car.h"

Car::Car(const std::string& file) : speed(200.f) { // Tốc độ ban đầu
    texture.loadFromFile(file);
    sprite.setTexture(texture);
    sprite.scale(0.8f, 0.8f);
    sprite.setPosition(370, 620); // Vị trí bắt đầu gần cuối màn hình
}

void Car::reset() {
    sprite.setPosition(370, 620);
    speed = 200.f;; // Đặt lại tốc độ về mặc định
}

void Car::steer(float dx) {
    sf::Vector2f pos = sprite.getPosition();
    // Giới hạn di chuyển của xe trong phần đường
    if ((dx < 0 && pos.x + dx > 0) || (dx > 0 && pos.x + dx < 710)) {
        sprite.move(dx, 0);
    }
}

void Car::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::FloatRect Car::getBounds() {
    return sprite.getGlobalBounds();
}

void Car::changeSpeed(float amount) {
    speed += amount;
    if (speed < 100.f) speed = 100.f;
    if (speed > 1500.f) speed = 1500.f;
}

float Car::getSpeed() const {
    return speed;
}

float Car::getX() const {
    return sprite.getPosition().x;
}