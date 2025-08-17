#include "PlayingScreen.h"
#include <algorithm>
#include <cstdlib>

PlayingScreen::PlayingScreen(HighScoreManager& hsm)
    : car("mainCar.png"), highScoreManager(hsm), score(0), finished(false), next(0),
    spawnCarTimer(0), spawnBarrierTimer(0) {

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
    backgroundSprite2.setPosition(0, -800);

    for (int i = 0; i < 4; ++i) {
        if (!obstacleTexture[i].loadFromFile("car" + std::to_string(i + 1) + ".png")) {}
    }
    if (!obstacleTexture[4].loadFromFile("barrier.png")) {}
    if (!obstacleTexture[5].loadFromFile("water_puddle.png")) {}

    if (!font.loadFromFile("BitcountPropDouble.ttf")) {}

    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(800, 10);

    if (!gameMusic.openFromFile("game_background_music.wav")) {}
    gameMusic.setLoop(true);
    gameMusic.setVolume(40);
    gameMusic.play();

    if (!carEngineMusic.openFromFile("car_engine_sound.wav")) {}
    carEngineMusic.setLoop(true);
    carEngineMusic.play();

    if (!car_accelerate_buffer.loadFromFile("car_accelerate_sound.wav")) {}
    car_accelerate_sound.setBuffer(car_accelerate_buffer);

    if (!car_brake_buffer.loadFromFile("car_brake_sound.wav")) {}
    car_brake_sound.setBuffer(car_brake_buffer);

    if (!car_collision_buffer.loadFromFile("car_collision_sound.wav")) {}
    car_collision_sound.setBuffer(car_collision_buffer);
}

void PlayingScreen::processEvent(sf::RenderWindow& window) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) window.close();
    }

    // Xử lý phím di chuyển xe
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
        car.steer(-0.5f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
        car.steer(0.5f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        car.changeSpeed(0.5f);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        if (car_brake_sound.getStatus() != sf::Sound::Playing)
            car_brake_sound.play();
        car.changeSpeed(-0.5f);
    }
}

void PlayingScreen::update(float dt) {
    spawnCarTimer += dt;
    spawnBarrierTimer += dt;

    car.update(dt);

    int speed = (int)car.getSpeed() / 10;
    carEngineMusic.setVolume(speed / 150 * 50 + 50);

    // Cập nhật cuộn nền
    float scrollSpeed = car.getSpeed() * dt;
    backgroundSprite1.move(0, scrollSpeed);
    backgroundSprite2.move(0, scrollSpeed);
    if (backgroundSprite1.getPosition().y >= 800)
        backgroundSprite1.setPosition(0, backgroundSprite2.getPosition().y - 800);
    if (backgroundSprite2.getPosition().y >= 800)
        backgroundSprite2.setPosition(0, backgroundSprite1.getPosition().y - 800);

    if (spawnCarTimer > 50.0f / speed) {
        if (car.getX() < 380.f) score += 10;
        else score += 20;

        spawnCarObstacle();
        spawnCarTimer = 0;
    }

    if (spawnBarrierTimer > 100.f / speed) {
        spawnBarrierObstacle();
        spawnBarrierTimer = 0;
    }

    for (auto& obs : obstacles)
        obs->update(car.getSpeed(), dt);

    // Xử lý va chạm
    for (auto& obs : obstacles) {
        if (car.getBounds().intersects(obs->getBounds())) {
            if (WaterPuddleObstacle* puddle = dynamic_cast<WaterPuddleObstacle*>(obs.get())) {
                if (!puddle->hasTriggeredSlip()) {
                    car.applySlip(puddle->getSlipForce());

                    continue;
                }
            }

            else if (dynamic_cast<CarObstacle*>(obs.get()) || dynamic_cast<BarrierObstacle*>(obs.get())) {
                highScoreManager.tryAdd(score, highScoreManager.getCurrentDateTime());
                gameMusic.stop();
                carEngineMusic.stop();
                car_collision_sound.play();

                sf::sleep(sf::seconds(2.5f));

                finished = true;
                next = 2;
                break;
            }
        }
    }

    // Xoá vật cản đã ra khỏi màn hình
    obstacles.erase(
        std::remove_if(obstacles.begin(), obstacles.end(),
            [](const std::unique_ptr<Obstacle>& o) { return o->isOutOfScreen(); }),
        obstacles.end());

    // Cập nhật điểm số
    scoreText.setString("Score: " + std::to_string(score) + "\nSpeed: " + std::to_string(speed));
}

void PlayingScreen::spawnCarObstacle() {
    float randomObstacle = rand() % 100;
    float randomLanePos = rand() % 6;
    int randomType = (int)rand() % 4;
    if (randomObstacle <= 70) {
        if (randomLanePos < 3.f)
            obstacles.push_back(std::unique_ptr<CarObstacle>(new CarObstacle(obstacleTexture[randomType], lanePos[randomLanePos], -150.f, true)));
        else
            obstacles.push_back(std::unique_ptr<CarObstacle>(new CarObstacle(obstacleTexture[randomType], lanePos[randomLanePos], -150.f, false)));
    }
    else {
        float randomDir = (rand() % 2 == 0) ? 1.f : -1.f;
        obstacles.push_back(std::unique_ptr<WaterPuddleObstacle>(new WaterPuddleObstacle(obstacleTexture[5], rand() % 740, -150.f, randomDir)));
    }
}

void PlayingScreen::spawnBarrierObstacle() {
    obstacles.push_back(std::unique_ptr<BarrierObstacle>(new BarrierObstacle(obstacleTexture[4], 365.f, -150.f)));
}

void PlayingScreen::render(sf::RenderWindow& window) {
    window.clear();
    window.draw(backgroundSprite1);
    window.draw(backgroundSprite2);
    window.draw(scoreText);
    car.draw(window);

    for (auto& obs : obstacles)
        obs->draw(window);

    window.display();
}
