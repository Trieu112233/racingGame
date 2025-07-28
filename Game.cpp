#include"Game.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

// Game Window: 780, 800
Game::Game() : window(sf::VideoMode(980, 800), "Racing Game"), car("mainCar.png"), score(0), spawnTimer(0), middleObstacleTimer(1) {
    currentState = MAIN_MENU;
    selectMenuIndex = 0;

    menuOptions.push_back("Play");
    menuOptions.push_back("Top Scores");
    menuOptions.push_back("Exit");

    gameOverOptions.push_back("Play Again");
    gameOverOptions.push_back("Back to Main Menu");
    gameOverOptions.push_back("Exit");

    lanePos.push_back(25);
    lanePos.push_back(140);
    lanePos.push_back(260);
    lanePos.push_back(440);
    lanePos.push_back(560);
    lanePos.push_back(675);


    if (!backgroundTexture.loadFromFile("road_map.png")) {}
    backgroundSprite1.setTexture(backgroundTexture);
    backgroundSprite2.setTexture(backgroundTexture);
    backgroundSprite1.setPosition(0, 0);
    backgroundSprite2.setPosition(0, -800); // Đặt ảnh nền thứ hai ngay trên ảnh đầu tiên
    for (int i = 0; i < 4; ++i) {
        if (!obstacleTexture[i].loadFromFile("car" + std::to_string(i + 1) + ".png")) {}
    }
    if (!obstacleTexture[4].loadFromFile("barrier.png")) {}

    if (!font.loadFromFile("BitcountPropDouble.ttf")) {};

    titleText.setFont(font);
    titleText.setString("RACING GAME");
    titleText.setCharacterSize(64);
    titleText.setFillColor(sf::Color::Cyan);
    titleText.setStyle(sf::Text::Bold);
    titleText.setOrigin(titleText.getLocalBounds().width / 2, titleText.getLocalBounds().height / 2);
    titleText.setPosition(window.getSize().x / 2.f, 120);

    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(800, 10);

    topScoresText.setFont(font);
    topScoresText.setCharacterSize(24);
    topScoresText.setFillColor(sf::Color::Yellow);

    gameOverText.setFont(font);
    gameOverText.setCharacterSize(48);
    gameOverText.setFillColor(sf::Color::Red);

    if (!mainMenuMusic.openFromFile("menu_theme_music.wav")) {};
    mainMenuMusic.setLoop(true);
    if (!gameMusic.openFromFile("game_background_music.wav")) {};
    gameMusic.setLoop(true);
    gameMusic.setVolume(40);
    if (!carEngineMusic.openFromFile("car_engine_sound.wav")) {};
    carEngineMusic.setLoop(true);

    if (!car_accelerate_buffer.loadFromFile("car_accelerate_sound.wav")) {};
    car_accelerate_sound.setBuffer(car_accelerate_buffer);
    if (!car_brake_buffer.loadFromFile("car_brake_sound.wav")) {};
    car_brake_sound.setBuffer(car_brake_buffer);
    if (!car_collision_buffer.loadFromFile("car_collision_sound.wav")) {};
    car_collision_sound.setBuffer(car_collision_buffer);
    // Bắt đầu phát nhạc nền
    mainMenuMusic.play();
}

void Game::run() {
    while (window.isOpen()) {
        switch (currentState) {
        case MAIN_MENU:
            processMenuEvent();
            renderMenu();
            break;
        case PLAYING:
            processGameEvents();
            updateGame();
            renderGame();
            break;
        case GAME_OVER:
            processGameOverEvents();
            renderGameOver();
            break;
        case TOP_SCORES:
            processTopScoresEvents();
            renderTopScores();
            break;
        }
    }
}

std::string Game::getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
    localtime_s(&tm, &time_t);
    std::stringstream ss;
    ss << std::put_time(&tm, "%d/%m/%Y %H:%M:%S");
    return ss.str();
}

void Game::resetGame() {
    score = 0;
    car.reset();
    obstacles.clear();
    clock.restart();
    spawnTimer = 0;
}

void Game::loadHighScores() {
    highScores.clear();
    std::ifstream fin("high_scores.txt");
    std::string line;
    while (std::getline(fin, line)) {
        if (line.empty()) continue;
        size_t pos = line.find(',');
        if (pos != std::string::npos) {
            int score = std::stoi(line.substr(0, pos));
            std::string dateTime = line.substr(pos + 1);
            HighScoreEntry highScoreLoad;
            highScoreLoad.score = score;
            highScoreLoad.time = dateTime;
            highScores.push_back(highScoreLoad);
        }
    }
    fin.close();
}

void Game::saveHighScores() {
    std::ofstream fout("high_scores.txt");
    for (const auto& entry : highScores) {
        fout << entry.score << "," << entry.time << std::endl;
    }
    fout.close();
}

void Game::tryAddHighScore(int newScore) {
    loadHighScores();
    HighScoreEntry highScoreAdd;
    highScoreAdd.score = newScore;
    highScoreAdd.time = getCurrentDateTime();
    highScores.push_back(highScoreAdd);
    std::sort(highScores.begin(), highScores.end(), compareHighScores);
    if (highScores.size() > 10) highScores.resize(10);
    saveHighScores();
}

void Game::updateTopScoresText() {
    loadHighScores();
    std::string topScoresString = "Top Scores:\n\n";
    for (size_t i = 0; i < highScores.size(); ++i) {
        topScoresString += std::to_string(i + 1) + ". " +
            std::to_string(highScores[i].score) + " - " +
            highScores[i].time + "\n";
    }
    topScoresString += "\nPress Esc to return to menu";
    topScoresText.setString(topScoresString);
    topScoresText.setOrigin(topScoresText.getLocalBounds().width / 2, 0);
    topScoresText.setPosition(window.getSize().x / 2.f, 100);
}

// State: MAIN_MENU
void Game::processMenuEvent() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Up) {
                selectMenuIndex = (selectMenuIndex - 1 + menuOptions.size()) % menuOptions.size();
            }
            if (event.key.code == sf::Keyboard::Down) {
                selectMenuIndex = (selectMenuIndex + 1) % menuOptions.size();
            }
            if (event.key.code == sf::Keyboard::Return) {
                if (selectMenuIndex == 0) {
                    currentState = PLAYING;
                    resetGame();
                    mainMenuMusic.stop();
                    gameMusic.play();
                    carEngineMusic.play();
                }
                else if (selectMenuIndex == 1) currentState = TOP_SCORES;
                else if (selectMenuIndex == 2) window.close();
            }
        }
    }
}

void Game::renderMenu() {
    window.clear(sf::Color::Black);

    window.draw(titleText);

    // Vẽ các lựa chọn menu
    for (size_t i = 0; i < menuOptions.size(); ++i) {
        sf::Text option;
        option.setFont(font);
        option.setString(menuOptions[i]);
        option.setCharacterSize(40);
        option.setStyle(sf::Text::Bold);
        option.setOrigin(option.getLocalBounds().width / 2, option.getLocalBounds().height / 2);
        option.setPosition(window.getSize().x / 2.f, 300 + i * 80);

        if ((int)i == selectMenuIndex)
            option.setFillColor(sf::Color::Red);
        else
            option.setFillColor(sf::Color::White);

        window.draw(option);
    }

    window.display();
}

//State: PLAYING
void Game::processGameEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            window.close();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        car.steer(-0.5f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        car.steer(0.5f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        car.changeSpeed(0.5f);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        if (car_brake_sound.getStatus() != sf::Sound::Playing)
            car_brake_sound.play();
        car.changeSpeed(-0.5f);
    }
}

void Game::spawnObstacle() {
    float randomLanePos = rand() % 6;
    int randomType = (int)rand() % 4; // Chọn ngẫu nhiên loại xe cản

    if (randomLanePos < 3.f) obstacles.push_back(std::make_unique<CarObstacle>(obstacleTexture[randomType], lanePos[randomLanePos], -150.f, true));
    else obstacles.push_back(std::make_unique<CarObstacle>(obstacleTexture[randomType], lanePos[randomLanePos], -150.f, false));

    if (middleObstacleTimer--) {
        obstacles.push_back(std::make_unique<BarrierObstacle>(obstacleTexture[4], 365.f, -150.f));
    }
    else middleObstacleTimer = 3;

}

void Game::updateGame() {
    float dt = clock.restart().asSeconds();
    spawnTimer += dt;

    int speed = (int)car.getSpeed() / 10;

    carEngineMusic.setVolume(speed / 150 * 50 + 50);

    // Cập nhật cuộn nền
    float scrollSpeed = car.getSpeed() * dt;
    backgroundSprite1.move(0, scrollSpeed);
    backgroundSprite2.move(0, scrollSpeed);
    if (backgroundSprite1.getPosition().y >= 800) backgroundSprite1.setPosition(0, backgroundSprite2.getPosition().y - 800);
    if (backgroundSprite2.getPosition().y >= 800) backgroundSprite2.setPosition(0, backgroundSprite1.getPosition().y - 800);


    if (spawnTimer > 50.0f / speed) {
        if (car.getX() < 380.f) score += 10;
        else score += 20;

        spawnObstacle();
        spawnTimer = 0;
    }

    for (auto& obs : obstacles) obs->update(car.getSpeed(), dt);

    // Xử lý va chạm
    for (auto& obs : obstacles) {
        if (car.getBounds().intersects(obs->getBounds())) {
            tryAddHighScore(score);
            currentState = GAME_OVER;
            selectGameOverIndex = 0;
            gameMusic.stop();
            carEngineMusic.stop();
            car_collision_sound.play();
            sf::sleep(sf::seconds(2.5f));

            gameOverText.setString("Game Over\nTotal Score: " + std::to_string(score));
            mainMenuMusic.play();
        }
    }

    // Xoá vật cản đã ra khỏi màn hình
    obstacles.erase(
        std::remove_if(obstacles.begin(), obstacles.end(),
            [](const std::unique_ptr<Obstacle>& o) { return o->isOutOfScreen(); }),
        obstacles.end());

    // Cập nhật điểm số
    scoreText.setString("Score: " + std::to_string(score) + '\n' + "Speed: " + std::to_string(speed));
}

void Game::renderGame() {
    window.clear();
    window.draw(backgroundSprite1);
    window.draw(backgroundSprite2);
    window.draw(scoreText);
    car.draw(window);
    for (auto& obs : obstacles) obs->draw(window);
    window.display();
}

//State: GAME_OVER
void Game::processGameOverEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Up) {
                selectGameOverIndex = (selectGameOverIndex - 1 + gameOverOptions.size()) % gameOverOptions.size();
            }
            if (event.key.code == sf::Keyboard::Down) {
                selectGameOverIndex = (selectGameOverIndex + 1) % gameOverOptions.size();
            }
            if (event.key.code == sf::Keyboard::Return) {
                if (selectGameOverIndex == 0) {
                    currentState = PLAYING;
                    resetGame();
                    mainMenuMusic.stop();
                    gameMusic.play();
                    carEngineMusic.play();
                }
                else if (selectGameOverIndex == 1) {
                    selectMenuIndex = 0;
                    currentState = MAIN_MENU;
                }
                else if (selectGameOverIndex == 2) window.close();
            }
        }
    }
}

void Game::renderGameOver() {
    window.clear(sf::Color::Black);
    window.draw(gameOverText);
    for (size_t i = 0; i < gameOverOptions.size(); ++i) {
        sf::Text option;
        option.setFont(font);
        option.setString(gameOverOptions[i]);
        option.setCharacterSize(40);
        option.setStyle(sf::Text::Bold);
        option.setOrigin(option.getLocalBounds().width / 2, option.getLocalBounds().height / 2);
        option.setPosition(window.getSize().x / 2.f, 300 + i * 80);

        if ((int)i == selectGameOverIndex)
            option.setFillColor(sf::Color::Red);
        else
            option.setFillColor(sf::Color::White);

        window.draw(option);
    }
    window.display();
}

// State: TOP_SCORES
void Game::processTopScoresEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                currentState = MAIN_MENU;
            }
        }
    }
}

void Game::renderTopScores() {
    window.clear(sf::Color::Black);
    updateTopScoresText();
    window.draw(topScoresText);
    window.display();
}
