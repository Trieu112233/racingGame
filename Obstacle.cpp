#include "Obstacle.h"

Obstacle::Obstacle(const sf::Texture& texture, float x, float startY, bool sameDir) : isSameDirection(sameDir) {
    sprite.setTexture(texture);
    sprite.setPosition(x, startY);

    // Lật hình ảnh nếu xe đi ngược chiều (hướng xuống)
    if (isSameDirection) sprite.setScale(1.f, 1.f);
    else sprite.setScale(1.f, -1.f);
}

void Obstacle::update(float carSpeed, float dt) {
    float scrollSpeed = carSpeed;
    float obstacleSpeed;

    if (isSameDirection) {
        // Xe cùng chiều di chuyển chậm hơn tốc độ cuộn nền -> người chơi vượt qua
        obstacleSpeed = scrollSpeed * 0.8f;
    }
    else {
        // Xe ngược chiều có tốc độ riêng cộng với tốc độ cuộn nền
        obstacleSpeed = scrollSpeed + 250.f;
    }

    sprite.move(0, obstacleSpeed * dt);
}

void Obstacle::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

sf::FloatRect Obstacle::getBounds() {
    return sprite.getGlobalBounds();
}

bool Obstacle::isOutOfScreen() {
    return sprite.getPosition().y > 900;
}