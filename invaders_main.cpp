#include <SFML/Graphics.hpp>
#include "game_parameters.hpp"
#include "game_system.hpp"

//main.cpp
// NOTE: sf::Texture must not be a global in SFML 2.x — its constructor touches
// SFML's OpenGL context machinery, which isn't initialised yet during static
// initialisation, and the program crashes before main() runs.
// That's why GameSystem only keeps a pointer to the spritesheet and creates it in init().

using param = Parameters;
using gs = GameSystem;

int main() {
    sf::RenderWindow window(sf::VideoMode(param::game_width, param::game_height), "Space Invaders");
    window.setFramerateLimit(60);

    gs::init();

    sf::Clock clock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            window.close();

        const float dt = clock.restart().asSeconds();
        gs::update(dt);

        window.clear();
        gs::render(window);
        window.display();
    }

    gs::clean();
    return 0;
}
