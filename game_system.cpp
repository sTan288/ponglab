//game_system.cpp
#include "game_system.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "bullet.hpp"
#include "game_parameters.hpp"

using param = Parameters; //short name, only for this file

//Static member variables are only DECLARED in the header, they must be DEFINED once in a .cpp,
//otherwise the linker complains with "unresolved external symbol".
std::shared_ptr<sf::Texture> GameSystem::spritesheet;
std::vector<std::shared_ptr<Ship>> GameSystem::ships;
float GameSystem::_game_over_timer = 0.f;

void GameSystem::init() {
    //seed the random generator with the current time, so invaders don't shoot the same way every run
    std::srand(static_cast<unsigned>(std::time(nullptr)));

    //the texture has to exist BEFORE any ship or bullet is created, because they use it
    spritesheet = std::make_shared<sf::Texture>();
    if (!spritesheet->loadFromFile("res/invaders_sheet.png")) {
        std::cerr << "Failed to load spritesheet!" << std::endl;
    }
    reset();
}

void GameSystem::reset() {
    //throw away the old ships (shared_ptr frees the memory automatically)
    ships.clear();
    _game_over_timer = 0.f;

    //the invaders' shared values go back to their starting state
    Invader::direction = true;
    Invader::speed = param::invader_speed;
    Invader::fire_timer = 0.f;

    //put every bullet of the pool back off-screen
    Bullet::init();

    //the player must be the first ship (bullets and invaders rely on ships[0] being the player)
    ships.push_back(std::make_shared<Player>());

    //x of the first column, chosen so the whole formation is centred horizontally
    const float left = (param::game_width - (param::columns - 1) * param::invader_spacing) / 2.f;
    for (int r = 0; r < param::rows; ++r) {
        //which invader picture to use for this row (column in the sprite sheet):
        //top row: small invader (0), 2 middle rows: medium (2), 2 bottom rows: big (4)
        const int type = r == 0 ? 0 : (r < 3 ? 2 : 4);
        //the part of the sprite sheet to show: top row of the sheet (y = 0), column "type"
        const sf::IntRect rect(sf::Vector2i(param::sprite_size * type, 0),
                               sf::Vector2i(param::sprite_size, param::sprite_size));
        for (int c = 0; c < param::columns; ++c) {
            const sf::Vector2f position(left + c * param::invader_spacing,
                                        param::formation_top + r * param::invader_spacing);
            //make_shared creates the Invader on the heap and gives back a smart pointer to it
            ships.push_back(std::make_shared<Invader>(rect, position));
        }
    }
}

void GameSystem::clean() {
    for (std::shared_ptr<Ship> &ship : ships)
        ship.reset(); //free up the memory of this shared pointer
    ships.clear(); //clear the vector to be sure we free up any memory left.
    spritesheet.reset(); //free the texture while SFML is still alive
}

void GameSystem::update(const float &dt) {
    //the invaders' shooting cooldown goes down once per frame (not once per invader!)
    Invader::fire_timer -= dt;

    //Polymorphism: s is a pointer to Ship, but update() is virtual, so C++ calls
    //Player::update() for the player and Invader::update() for the invaders.
    for (std::shared_ptr<Ship> &s : ships)
        s->update(dt);

    //move all the bullets and check if they hit something
    Bullet::update(dt);

    //is at least one invader still alive? (start at 1 to skip the player)
    bool invaders_alive = false;
    for (size_t i = 1; i < ships.size(); ++i)
        if (!ships[i]->is_exploded())
            invaders_alive = true;

    //game over (player dead) or victory (no invader left): wait a bit, then restart
    if (ships[0]->is_exploded() || !invaders_alive) {
        _game_over_timer += dt;
        if (_game_over_timer > param::game_over_delay)
            reset();
    }
}

void GameSystem::render(sf::RenderWindow &window) {
    //Ship inherits from sf::Sprite, so SFML can draw a ship directly.
    //s.get() gives the raw pointer, * turns it into the object that draw() wants.
    for (const std::shared_ptr<Ship> &s : ships)
        window.draw(*(s.get()));
    Bullet::render(window);
}
