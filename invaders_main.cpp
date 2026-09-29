#include <SFML/Graphics.hpp>
#include "game_parameters.hpp"
#include "game_system.hpp"

//main.cpp
// NOTE: sf::Texture must not be a global in SFML 2.x — its constructor touches
// SFML's OpenGL context machinery, which isn't initialised yet during static
// initialisation, and the program crashes before main() runs.
// That's why GameSystem only keeps a pointer to the spritesheet and creates it in init().
//
// main() stays tiny: it only opens the window and runs the game loop.
// All the game logic is in GameSystem, Ship and Bullet.

using param = Parameters;
using gs = GameSystem;

int main() {
    sf::RenderWindow window(sf::VideoMode(param::game_width, param::game_height), "Space Invaders");
    window.setFramerateLimit(60); //SFML waits a bit each frame so we run at max 60 FPS

    gs::init(); //load the texture and build the level

    sf::Clock clock; //measures the time between two frames
    //Game loop: every frame = handle events -> update -> render
    while (window.isOpen()) {
        //1. events: closing the window with the X button
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            window.close();

        //2. update: dt = seconds since the last frame (about 1/60 = 0.016).
        //Speeds are in px/s, so "speed * dt" = distance to move this frame,
        //and the game runs at the same speed whatever the FPS is.
        //restart() returns the elapsed time AND resets the clock for the next frame.
        const float dt = clock.restart().asSeconds();
        gs::update(dt);

        //3. render: erase the old frame, draw everything, show it on screen
        window.clear();
        gs::render(window);
        window.display();
    }

    gs::clean(); //free the memory before leaving
    return 0;
}
