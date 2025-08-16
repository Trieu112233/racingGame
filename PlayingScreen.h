#pragma once

#include "Screen.h"
#include "Car.h"
#include "Obstacle.h"
#include "HighScoreManager.h"
#include <vector>
#include <memory>

class PlayingScreen : public Screen {
    Car car;
    std::vector<std::unique_ptr<Obstacle>> obstacles;
    std::vector<float> lanePos;
    HighScoreManager& highScoreManager;

    sf::Clock clock;
    float spawnCarTimer, spawnBarrierTimer;
    int score;
    bool finished;
    int next;

    sf::Texture obstacleTexture[6];
    sf::Texture carCollisionTexture;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite1, backgroundSprite2;
    sf::Font font;
    sf::Text scoreText;

    sf::Music gameMusic, carEngineMusic;
    sf::SoundBuffer car_accelerate_buffer, car_brake_buffer, car_collision_buffer;
    sf::Sound car_accelerate_sound, car_brake_sound, car_collision_sound;

public:
    PlayingScreen(HighScoreManager& hsm);
    void processEvent(sf::RenderWindow& window) override;
    void update(float dt) override;
    void render(sf::RenderWindow& window) override;
    bool isFinished() const override { return finished; }
    int nextScreen() const override { return next; }
    int getScore() const { return score; }

private:
    void spawnCarObstacle();
    void spawnBarrierObstacle();
};
