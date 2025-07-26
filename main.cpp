#include <SFML/Graphics.hpp>
#include <vector>
#include <SFML/Audio.hpp>

class Car {
    sf::Sprite sprite;
    sf::Texture texture;
    float speed;

public:
    Car(const std::string& file) : speed(200.f) { // Tốc độ ban đầu
        texture.loadFromFile(file);
        sprite.setTexture(texture);
        sprite.scale(0.8f, 0.8f);
        sprite.setPosition(370, 650); // Vị trí bắt đầu gần cuối màn hình
    }

    void reset() {
        sprite.setPosition(370, 650);
        speed = 200.f; // Đặt lại tốc độ về mặc định
	}

    void steer(float dx) {
        sf::Vector2f pos = sprite.getPosition();
        // Giới hạn di chuyển của xe trong phần đường
        if ((dx < 0 && pos.x + dx > 0) || (dx > 0 && pos.x + dx < 520)) {
            sprite.move(dx, 0);
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    sf::FloatRect getBounds() {
        return sprite.getGlobalBounds();
    }

    void changeSpeed(float amount) {
        speed += amount;
        if (speed < 100.f) speed = 100.f; 
        if (speed > 1500.f) speed = 1500.f; 
    }

    float getSpeed() const {
        return speed;
    }

    float getX() const {
        return sprite.getPosition().x;
	}
};

class Obstacle {
    sf::Sprite sprite;
    sf::Texture texture;
    bool isSameDirection; 

public:
    Obstacle(const sf::Texture& texture, float x, float startY, bool sameDir) : isSameDirection(sameDir) {
        sprite.setTexture(texture);

        // Đặt gốc của sprite ở giữa để nó căn giữa trên tọa độ x của làn đường
        sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.width / 2.f, 0);

        sprite.setPosition(x, startY);

        // Lật hình ảnh nếu xe đi ngược chiều (hướng xuống)
        if (isSameDirection) sprite.setScale(0.8f, 0.8f);
        else sprite.setScale(0.8f, - 0.8f);
    }

    void update(float carSpeed, float dt) {
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

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }

    sf::FloatRect getBounds() {
        return sprite.getGlobalBounds();
    }

    bool isOutOfScreen() {
        return sprite.getPosition().y > 900;
    }
};

class Game {
    enum GameState {
        MAIN_MENU,
        PLAYING,
        GAME_OVER
	};

    GameState currentState;
    sf::RenderWindow window;
    Car car;
    std::vector<Obstacle> obstacles;
    sf::Clock clock;
    float spawnTimer = 0;

	sf::Texture obstacleTexture[4];
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite1;
    sf::Sprite backgroundSprite2;

    sf::Font font;
	sf::Text scoreText;
	sf::Text gameOverText;
	sf::Text mainMenuText;
    int score;

    sf::Music mainMenuMusic;
	sf::Music gameMusic;
	sf::Music carEngineMusic;

	sf::SoundBuffer car_accelerate_buffer, car_brake_buffer, car_collision_buffer;
	sf::Sound car_accelerate_sound, car_brake_sound, car_collision_sound;

public:
    // Game Window: 600, 800
    Game() : window(sf::VideoMode(800, 800), "Racing Game"), car("mainCar.png"), score(0) {
        currentState = MAIN_MENU;
        
        if (!backgroundTexture.loadFromFile("road_map.png")) {
            throw std::runtime_error("Could not load background image");
        }
        backgroundSprite1.setTexture(backgroundTexture);
        backgroundSprite2.setTexture(backgroundTexture);
        backgroundSprite1.setPosition(0, 0);
        backgroundSprite2.setPosition(0, -800); // Đặt ảnh nền thứ hai ngay trên ảnh đầu tiên
        for(int i = 0; i < 4; ++i) {
            if (!obstacleTexture[i].loadFromFile("car" + std::to_string(i + 1) + ".png")) {
            }
		}

        if (!font.loadFromFile("BitcountPropDouble.ttf")) {};
		scoreText.setFont(font);
		scoreText.setCharacterSize(24);
		scoreText.setFillColor(sf::Color::White);
        scoreText.setPosition(620, 10);

		gameOverText.setFont(font);
		gameOverText.setCharacterSize(48);
		gameOverText.setFillColor(sf::Color::Red);

        mainMenuText.setFont(font);
		mainMenuText.setCharacterSize(48);
		mainMenuText.setFillColor(sf::Color::Blue);
		mainMenuText.setString("Press Enter to Start");
		mainMenuText.setOrigin(mainMenuText.getLocalBounds().width / 2, mainMenuText.getLocalBounds().height / 2);
		mainMenuText.setPosition(window.getSize().x / 2, window.getSize().y / 2);

		if (!mainMenuMusic.openFromFile("menu_theme_music.wav")) {};
		mainMenuMusic.setLoop(true);

        if (!gameMusic.openFromFile("game_background_music.wav")) {};
		gameMusic.setLoop(true);
        gameMusic.setVolume(50);

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

    void run() {
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
            }
        }
    }

private:
    void resetGame() {
        score = 0;
        car.reset();
        obstacles.clear();
        clock.restart();
        spawnTimer = 0;
    }

    // State: MAIN_MENU
    void processMenuEvent() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                currentState = PLAYING;
                resetGame();
                mainMenuMusic.stop();
                gameMusic.play();
                carEngineMusic.play();
            }
        }
    }

    void renderMenu() {
        window.clear(sf::Color::Black);
        window.draw(mainMenuText);
        window.display();
    }

	//State: PLAYING
    void processGameEvents() {
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

    void spawnObstacle() {
        spawnTimer = 0;

        float randomRoad = rand() % 550 + 50;
		int randomType = (int) rand() % 4; // Chọn ngẫu nhiên loại xe cản

        if (randomRoad < 320.f) obstacles.emplace_back(obstacleTexture[randomType], randomRoad - 40, -150.f, true);
        else obstacles.emplace_back(obstacleTexture[randomType], randomRoad, -150.f, false);
     
    }

    void updateGame() {
        float dt = clock.restart().asSeconds();
        spawnTimer += dt;

		int speed = (int) car.getSpeed() / 10;

        carEngineMusic.setVolume(speed / 150 * 50 + 50);

        // Cập nhật cuộn nền
        float scrollSpeed = car.getSpeed() * dt;
        backgroundSprite1.move(0, scrollSpeed);
        backgroundSprite2.move(0, scrollSpeed);
        if (backgroundSprite1.getPosition().y >= 800) backgroundSprite1.setPosition(0, backgroundSprite2.getPosition().y - 800);
        if (backgroundSprite2.getPosition().y >= 800) backgroundSprite2.setPosition(0, backgroundSprite1.getPosition().y - 800);


        if (spawnTimer > 40.0f / speed) {
            if (car.getX() < 300.f) score += 10;
            else score += 20;

            spawnObstacle();
            spawnTimer = 0;
        }

        for (auto& obs : obstacles)
            obs.update(car.getSpeed(), dt);

        // Xử lý va chạm
        for (auto& obs : obstacles) {
            if (car.getBounds().intersects(obs.getBounds())) {
                currentState = GAME_OVER;
                gameMusic.stop();
                carEngineMusic.stop();
                car_collision_sound.play();
				sf::sleep(sf::seconds(2.5f)); 

                gameOverText.setString("Game Over\nTotal Score: " + std::to_string(score) + "\nPress Enter to play again\nPress Backspace to Exit");
                mainMenuMusic.play();
            }
        }

        // Xoá vật cản đã ra khỏi màn hình
        obstacles.erase(
            std::remove_if(obstacles.begin(), obstacles.end(),
                [](Obstacle& o) { return o.isOutOfScreen(); }),
            obstacles.end());

		// Cập nhật điểm số
        scoreText.setString("Score: " + std::to_string(score) + '\n' + "Speed: " + std::to_string(speed));
    }

    void renderGame() {
        window.clear();
        window.draw(backgroundSprite1);
        window.draw(backgroundSprite2);
		window.draw(scoreText);
        car.draw(window);
        for (auto& obs : obstacles)
            obs.draw(window);
        window.display();
    }

    //State: GAME_OVER
    void processGameOverEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Enter) {
                    currentState = PLAYING;
                    resetGame();
                    mainMenuMusic.stop();
                    gameMusic.play();
                    carEngineMusic.play();
                }
                if (event.key.code == sf::Keyboard::BackSpace) {
                    window.close();
                }
            }
        }
    }

    void renderGameOver() {
        window.clear(sf::Color::Black);
        window.draw(gameOverText);
        window.display();
    }
};

int main() {
    srand((int) time(0));
    Game game;
    game.run();
    return 0;
}