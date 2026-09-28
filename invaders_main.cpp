#include <SFML/Graphics.hpp>
#include <iostream>

//main.cpp
// NOTE: sf::Texture must not be a global in SFML 2.x — its constructor touches
// SFML's OpenGL context machinery, which isn't initialised yet during static
// initialisation, and the program crashes before main() runs.

void init(sf::Texture& spritesheet, sf::Sprite& invader) {
    if (!spritesheet.loadFromFile("res/invaders_sheet.png")) {
        std::cerr << "Failed to load spritesheet!" << std::endl;
    }
    invader.setTexture(spritesheet);
    invader.setTextureRect(sf::IntRect(0, 0, 32, 32));
}

void render(sf::RenderWindow& window, const sf::Sprite& invader) {
    window.draw(invader);
}

int main() {
    std::cout << "start" << std::endl;
    sf::RenderWindow window(sf::VideoMode(800, 600), "Space Invaders");

    sf::Texture spritesheet;
    sf::Sprite invader;
    init(spritesheet, invader);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();
        render(window, invader);
        window.display();
        sf::sleep(sf::seconds(1.f / 60.f));
    }

    return 0;
}
