//game_system.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <vector>
#include "ship.hpp"

// GameSystem is the "brain" of the game: it owns the shared data and runs the main steps.
// Everything is static, so there is never a GameSystem object: we call GameSystem::init() etc.
// It behaves like global variables/functions, but they are kept inside a scope (the struct).
// Other files usually write "using gs = GameSystem;" and then "gs::ships".
struct GameSystem{
    //---------------------------------------------------------------- shared data
    //The picture with all the sprites. sf::Texture must not be constructed before main() in SFML 2.x,
    //so we only hold a pointer here (empty at start-up) and create the texture in init().
    static std::shared_ptr<sf::Texture> spritesheet;

    //Every ship of the game. ships[0] is ALWAYS the player, all the others are invaders.
    //It stores pointers to the base class Ship, so it can hold Players and Invaders together
    //(an abstract class like Ship can't be stored directly, only through pointers).
    static std::vector<std::shared_ptr<Ship>> ships;

    //---------------------------------------------------------------- game system functions
    static void init();                           //called once at start: loads the texture, then reset()
    static void reset();                          //(re)builds the level: player, invaders, bullets
    static void clean();                          //called once at the end: frees the memory
    static void update(const float &dt);          //called every frame: moves everything (dt = seconds since last frame)
    static void render(sf::RenderWindow &window); //called every frame: draws everything

private:
    static float _game_over_timer; //counts up once the game is over, when it passes game_over_delay the game restarts
};
