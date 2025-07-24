#include <SFML/Graphics.hpp>
#include <vector>
#include <random>

std::random_device rd;
std::mt19937 gen(rd());

class Car {
    sf::Sprite sprite;
    sf::Texture texture;
    float speed;

public:
    Car(const std::string& file) : speed(200.f) { // Tốc độ ban đầu
        texture.loadFromFile(file);
        sprite.setTexture(texture);
        sprite.setPosition(370, 650); // Vị trí bắt đầu gần cuối màn hình
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
        if (speed > 700.f) speed = 700.f; 
    }

    float getSpeed() const {
        return speed;
    }
};

class Obstacle {
    sf::Sprite sprite;
    sf::Texture texture;
    bool isSameDirection; 

public:
    Obstacle(const std::string& file, float x, float startY, bool sameDir) : isSameDirection(sameDir) {
        texture.loadFromFile(file);
        sprite.setTexture(texture);

        // Đặt gốc của sprite ở giữa để nó căn giữa trên tọa độ x của làn đường
        sf::FloatRect bounds = sprite.getLocalBounds();
        sprite.setOrigin(bounds.width / 2.f, 0);

        sprite.setPosition(x, startY);

        // Lật hình ảnh nếu xe đi cùng chiều (hướng lên)
        if (isSameDirection) {
            sprite.setScale(1.f, -1.f);
        }
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
        return sprite.getPosition().y > 850;
    }
};

class Game {
    sf::RenderWindow window;
    Car car;
    std::vector<Obstacle> obstacles;
    sf::Clock clock;
    float spawnTimer = 0;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite1;
    sf::Sprite backgroundSprite2;

    // Định nghĩa vị trí X cho 6 làn đường
    const std::vector<float> lanePositions = { 50.f, 145.f, 235.f, 370.f, 460.f, 550.f };

public:
    Game() : window(sf::VideoMode(600, 800), "Racing Game"), car("mainCar.png") {
        if (!backgroundTexture.loadFromFile("road_map.png")) {
            throw std::runtime_error("Could not load background image");
        }
        backgroundSprite1.setTexture(backgroundTexture);
        backgroundSprite2.setTexture(backgroundTexture);
        backgroundSprite1.setPosition(0, 0);
        backgroundSprite2.setPosition(0, -800); // Đặt ảnh nền thứ hai ngay trên ảnh đầu tiên
    }

    void run() {
        while (window.isOpen()) {
            processEvents();
            update();
            render();
        }
    }

private:
    void processEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            car.steer(-0.5f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            car.steer(0.5f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            car.changeSpeed(1.f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            car.changeSpeed(-1.f);
    }

    void spawnObstacle() {
        spawnTimer = 0;

        // Tạo một số ngẫu nhiên từ 1 đến 10 để quyết định hướng
        std::uniform_int_distribution<> directionChance(1, 10);
        int chance = directionChance(gen);
        int laneIndex;

        if (chance <= 7) {
            std::uniform_int_distribution<> sameDirLaneDist(3, 5);
            laneIndex = sameDirLaneDist(gen);
            obstacles.emplace_back("car1.png", lanePositions[laneIndex], -150.f, true);
        }
        else { 
            std::uniform_int_distribution<> oppDirLaneDist(0, 2);
            laneIndex = oppDirLaneDist(gen);
            obstacles.emplace_back("car1.png", lanePositions[laneIndex], -150.f, false);
        }
    }

    void update() {
        float dt = clock.restart().asSeconds();
        spawnTimer += dt;

        // Cập nhật cuộn nền
        float scrollSpeed = car.getSpeed() * dt;
        backgroundSprite1.move(0, scrollSpeed);
        backgroundSprite2.move(0, scrollSpeed);
        if (backgroundSprite1.getPosition().y >= 800) backgroundSprite1.setPosition(0, backgroundSprite2.getPosition().y - 800);
        if (backgroundSprite2.getPosition().y >= 800) backgroundSprite2.setPosition(0, backgroundSprite1.getPosition().y - 800);


        if (spawnTimer > 300.0f / car.getSpeed()) {
            spawnObstacle();
        }

        for (auto& obs : obstacles)
            obs.update(car.getSpeed(), dt);

        // Xử lý va chạm
        for (auto& obs : obstacles) {
            if (car.getBounds().intersects(obs.getBounds())) {
                window.close(); 
            }
        }

        // Xoá vật cản đã ra khỏi màn hình
        obstacles.erase(
            std::remove_if(obstacles.begin(), obstacles.end(),
                [](Obstacle& o) { return o.isOutOfScreen(); }),
            obstacles.end());
    }

    void render() {
        window.clear();
        window.draw(backgroundSprite1);
        window.draw(backgroundSprite2);
        car.draw(window);
        for (auto& obs : obstacles)
            obs.draw(window);
        window.display();
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}