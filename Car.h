#pragma once
#include <SFML/Graphics.hpp>

class Car {
	sf::Sprite sprite;
	sf::Texture texture;
	float speed;
	float slipTimer;

public:
	Car(const std::string& file);

	void steer(float dx);
	void changeSpeed(float amount);
	float getSpeed() const { return speed; }
	void setSpeed(float newSpeed);

	void applySlip(float slipForce);
	void update(float dt);
	float getX() const { return sprite.getPosition().x; }
	sf::FloatRect getBounds() const;

	void setTexture(sf::Texture& newTexture);
	void draw(sf::RenderWindow& window);
};