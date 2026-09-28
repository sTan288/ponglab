#include <SFML/Graphics.hpp>
#include <string>
#include <stdexcept>

const sf::Keyboard::Key controls[4] = {
    sf::Keyboard::A,   // Player1 UP
    sf::Keyboard::Z,   // Player1 Down
    sf::Keyboard::Up,  // Player2 UP
    sf::Keyboard::Down // Player2 Down
};

//Parameters
const sf::Vector2f paddleSize(25.f, 100.f);
const float ballRadius = 10.f;
const int gameWidth = 800;
const int gameHeight = 600;
const float paddleSpeed = 400.f;
const float paddleOffsetWall = 10.f;
const float time_step = 0.017f; //60 fps

int player1Score = 0;
int player2Score = 0;

sf::Font font;
sf::Text text;

sf::Vector2f ball_velocity;
bool is_player_serving = true;
const float initial_velocity_x = 100.f; //horizontal velocity
const float initial_velocity_y = 60.f; //vertical velocity
const float velocity_multiplier = 1.1f; //how much the ball will speed up everytime it hits a paddle. Here, 10% every time

//Objects of the game
sf::CircleShape ball;
sf::RectangleShape paddles[2];


void reset() {
    // Возвращаем мяч в центр и задаем начальное направление
    // Reset ball
    ball.setPosition(gameWidth / 2.f, gameHeight / 2.f);

    ball_velocity = {
        (is_player_serving ? initial_velocity_x : -initial_velocity_x),
        initial_velocity_y
    };

    // Reset paddles
    paddles[0].setPosition(
        paddleOffsetWall + paddleSize.x / 2.f,
        gameHeight / 2.f
    );

    paddles[1].setPosition(
        gameWidth - paddleOffsetWall - paddleSize.x / 2.f,
        gameHeight / 2.f
    );

    // Update score text
    text.setString(
        std::to_string(player1Score) + " : " +
        std::to_string(player2Score)
    );

    // Center score text
    text.setPosition(
        (gameWidth * 0.5f) -
        (text.getLocalBounds().width * 0.5f),
        0
    );
}

void load() {
    font.loadFromFile("res/fonts/RobotoMono-Regular.ttf");
    // Set text element to use font
    text.setFont(font);
    // set the character size to 24 pixels
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::White);
    if (!font.loadFromFile("res/fonts/RobotoMono-Regular.ttf")) {
    // handle error
}
}


void update(float dt) {
    float direction = 0.0f;
    if (sf::Keyboard::isKeyPressed(controls[0])) direction -= 1.0f;
    if (sf::Keyboard::isKeyPressed(controls[1])) direction += 1.0f;
    paddles[0].move(0.0f, direction * paddleSpeed * dt);

    direction = 0.0f;
    if (sf::Keyboard::isKeyPressed(controls[2])) direction -= 1.0f;
    if (sf::Keyboard::isKeyPressed(controls[3])) direction += 1.0f;
    paddles[1].move(0.0f, direction * paddleSpeed * dt);




    // Keep paddles inside the screen

    if (paddles[0].getPosition().y < paddleSize.y / 2.f)
        paddles[0].setPosition(
            paddles[0].getPosition().x,
            paddleSize.y / 2.f
        );

    if (paddles[0].getPosition().y > gameHeight - paddleSize.y / 2.f)
        paddles[0].setPosition(
            paddles[0].getPosition().x,
            gameHeight - paddleSize.y / 2.f
        );

    if (paddles[1].getPosition().y < paddleSize.y / 2.f)
        paddles[1].setPosition(
            paddles[1].getPosition().x,
            paddleSize.y / 2.f
        );

    if (paddles[1].getPosition().y > gameHeight - paddleSize.y / 2.f)
        paddles[1].setPosition(
            paddles[1].getPosition().x,
            gameHeight - paddleSize.y / 2.f
        );








    ball.move(ball_velocity * dt);
    const float bx = ball.getPosition().x;
    const float by = ball.getPosition().y;

    if (by > gameHeight || by < 0.0f) {
        ball_velocity.y *= -velocity_multiplier;
        ball_velocity.x *= velocity_multiplier;
    } else if (bx < paddleSize.x + paddleOffsetWall &&
               by > paddles[0].getPosition().y - paddleSize.y * 0.5f &&
               by < paddles[0].getPosition().y + paddleSize.y * 0.5f) {
        ball_velocity.x = -ball_velocity.x * velocity_multiplier;
    } else if (bx > gameWidth - paddleSize.x - paddleOffsetWall &&
               by > paddles[1].getPosition().y - paddleSize.y * 0.5f &&
               by < paddles[1].getPosition().y + paddleSize.y * 0.5f) {
        ball_velocity.x = -ball_velocity.x * velocity_multiplier;
    } else if (bx > gameWidth) {
        ++player1Score;
        is_player_serving = false;
        reset();
    } else if (bx < 0.0f) {
        ++player2Score;
        is_player_serving = true;
        reset();
    }
}
    

void render(sf::RenderWindow &window) 
    {
    // Draw Everything
    window.draw(paddles[0]);
    window.draw(paddles[1]);
    window.draw(ball);
    window.draw(text);

    }


void init() {
    
    
    
    
    // Set size and origin of paddles
    for (sf::RectangleShape &p : paddles) {
        p.setSize(paddleSize);
        p.setOrigin(paddleSize / 2.f);
    }
    // Set size and origin of ball
    ball.setRadius(ballRadius);
    ball.setOrigin(ballRadius, ballRadius); // Should be half the ball width and height
    // reset paddle position
    paddles[0].setPosition(paddleOffsetWall + paddleSize.x / 2.f, gameHeight / 2.f);
    paddles[1].setPosition(gameWidth - paddleOffsetWall - paddleSize.x / 2.f, gameHeight / 2.f);
    // reset Ball Position
    ball.setPosition(gameWidth / 2.f, gameHeight / 2.f);
    load();  
    reset();
    

    

    
}

int main() {
    sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight), "PONG");

    init();

    sf::Clock clock;

    // === GAME LOOP ===
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();
        update(dt);
        render(window);
        window.display();
    }

    

    return 0;
}