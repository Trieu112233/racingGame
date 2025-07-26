#include <SFML/Graphics.hpp>
#include <vector>
#include <SFML/Audio.hpp>
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

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

struct HighScoreEntry {
    int score;
    std::string time;
};

bool compareHighScores(const HighScoreEntry& a, const HighScoreEntry& b) {
    return a.score > b.score; // So sánh điểm số
}
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
    std::vector<Obstacle> obstacles;
	std::vector <HighScoreEntry> highScores;

    sf::Clock clock;
    float spawnTimer = 0;
    int score;

	sf::Texture obstacleTexture[4];
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
	std::vector<std::string> menuOptions = { "Start Game", "Top Scores", "Exit" };
	std::vector<std::string> gameOverOptions = { "Play Again", "Main Menu", "Exit" };

    sf::Music mainMenuMusic;
	sf::Music gameMusic;
	sf::Music carEngineMusic;

	sf::SoundBuffer car_accelerate_buffer, car_brake_buffer, car_collision_buffer;
	sf::Sound car_accelerate_sound, car_brake_sound, car_collision_sound;

public:
    // Game Window: 600, 800
    Game() : window(sf::VideoMode(800, 800), "Racing Game"), car("mainCar.png"), score(0) {
        currentState = MAIN_MENU;
		selectMenuIndex = 0;
        
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
        scoreText.setPosition(620, 10);

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
            case TOP_SCORES:
                processTopScoresEvents();
                renderTopScores();
                break;
            }
        }
    }

private:
    std::string getCurrentDateTime() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::tm tm;
        localtime_s(&tm, &time_t); 
        std::stringstream ss;
        ss << std::put_time(&tm, "%d/%m/%Y %H:%M:%S");
        return ss.str();
    }

    void resetGame() {
        score = 0;
        car.reset();
        obstacles.clear();
        clock.restart();
        spawnTimer = 0;
    }

    void loadHighScores() {
        highScores.clear();
        std::ifstream fin("high_score.txt");
        std::string line;
        while (std::getline(fin, line)) {
            if (line.empty()) continue;
            size_t pos = line.find(',');
            if (pos != std::string::npos) {
                int score = std::stoi(line.substr(0, pos));
                std::string datetime = line.substr(pos + 1);
                highScores.push_back({ score, datetime });
            }
        }
        fin.close();
        std::sort(highScores.begin(), highScores.end(), compareHighScores);
        if (highScores.size() > 10) highScores.resize(10);
    }

    void saveHighScores() {
        std::ofstream fout("high_score.txt");
        for (const auto& entry : highScores) {
            fout << entry.score << "," << entry.time << std::endl;
        }
        fout.close();
    }

    void tryAddHighScore(int newScore) {
        loadHighScores();
        highScores.push_back({ newScore, getCurrentDateTime() });
        std::sort(highScores.begin(), highScores.end(), [](const HighScoreEntry& a, const HighScoreEntry& b) {
            return a.score > b.score;
            });
        if (highScores.size() > 10) highScores.resize(10);
        saveHighScores();
    }

    void updateTopScoresText() {
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
    void processMenuEvent() {
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
                if (event.key.code == sf::Keyboard::Enter) {
                    if (selectMenuIndex == 0) {
                        currentState = PLAYING;
                        resetGame();
                        mainMenuMusic.stop();
                        gameMusic.play();
                        carEngineMusic.play();
                    } else if (selectMenuIndex == 1) currentState = TOP_SCORES;
                    else if (selectMenuIndex == 2) window.close();
                }
            }
        }
    }

    void renderMenu() {
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
                if (event.key.code == sf::Keyboard::Up) {
                    selectGameOverIndex = (selectGameOverIndex - 1 + gameOverOptions.size()) % gameOverOptions.size();
                }
                if (event.key.code == sf::Keyboard::Down) {
                    selectGameOverIndex = (selectGameOverIndex + 1) % gameOverOptions.size();
                }
                if (event.key.code == sf::Keyboard::Enter) {
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

    void renderGameOver() {
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
    void processTopScoresEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Escape) {
                    currentState = MAIN_MENU;
                    mainMenuMusic.play();
                }
            }
        }
    }

    void renderTopScores() {
        window.clear(sf::Color::Black);
        updateTopScoresText();
        window.draw(topScoresText);
        window.display();
	}
};

int main() {
    srand((int) time(0));
    Game game;
    game.run();
    return 0;
}