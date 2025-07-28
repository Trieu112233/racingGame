#pragma once
#include <SFML/Audio.hpp>
#include "CompareScore.h"
#include "Car.h"
#include "Obstacle.h"

class Game {
    enum GameState {
        MAIN_MENU,
        PLAYING,
        GAME_OVER,
        TOP_SCORES
    };

    GameState currentState;
    sf::RenderWindow window;
    Car car;
    std::vector<std::unique_ptr<Obstacle>> obstacles;    
    std::vector<float> lanePos;
    std::vector <HighScoreEntry> highScores;

    sf::Clock clock;
    float spawnTimer;
    int score, middleObstacleTimer;

    sf::Texture obstacleTexture[5];
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite1;
    sf::Sprite backgroundSprite2;

    sf::Font font;
    sf::Text titleText;
    sf::Text scoreText;
    sf::Text gameOverText;
    sf::Text topScoresText;

    int selectMenuIndex;
    int selectGameOverIndex;
    std::vector<std::string> menuOptions;
    std::vector<std::string> gameOverOptions;

    sf::Music mainMenuMusic;
    sf::Music gameMusic;
    sf::Music carEngineMusic;

    sf::SoundBuffer car_accelerate_buffer, car_brake_buffer, car_collision_buffer;
    sf::Sound car_accelerate_sound, car_brake_sound, car_collision_sound;

public:
    // Game Window: 600, 800
    Game();

    void run();

    std::string getCurrentDateTime();

    void resetGame();

    void loadHighScores();

    void saveHighScores();

    void tryAddHighScore(int newScore);

    void updateTopScoresText();

    // State: MAIN_MENU
    void processMenuEvent();

    void renderMenu();

    //State: PLAYING
    void processGameEvents();

    void spawnObstacle();

    void updateGame();

    void renderGame();

    //State: GAME_OVER
    void processGameOverEvents();

    void renderGameOver();

    // State: TOP_SCORES
    void processTopScoresEvents();

    void renderTopScores();

};