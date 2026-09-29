//game_system.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "ship.hpp"

struct GameSystem{
    //The "global" variables go here
    //sf::Texture must not be constructed before main() in SFML 2.x, so we only hold a pointer
    //and create the texture in init().
    static std::shared_ptr<sf::Texture> spritesheet;
    static std::vector<std::shared_ptr<Ship>> ships; //vector of shared pointers to Ships. ships[0] is the player

    //game system functions
    static void init();
    static void reset();
    static void clean();
    static void update(const float &dt);
    static void render(sf::RenderWindow &window);

private:
    static float _game_over_timer; //counts down once the game is over, then the game restarts
};
