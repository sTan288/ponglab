//game_system.cpp
#include "game_system.hpp"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "bullet.hpp"
#include "game_parameters.hpp"

using param = Parameters;

std::shared_ptr<sf::Texture> GameSystem::spritesheet;
std::vector<std::shared_ptr<Ship>> GameSystem::ships;
float GameSystem::_game_over_timer = 0.f;

void GameSystem::init() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    spritesheet = std::make_shared<sf::Texture>();
    if (!spritesheet->loadFromFile("res/invaders_sheet.png")) {
        std::cerr << "Failed to load spritesheet!" << std::endl;
    }
    reset();
}

void GameSystem::reset() {
    ships.clear();
    _game_over_timer = 0.f;
    Invader::direction = true;
    Invader::speed = param::invader_speed;
    Invader::fire_timer = 0.f;
    Bullet::init();

    //the player must be the first ship
    ships.push_back(std::make_shared<Player>());

    //centre the formation horizontally
    const float left = (param::game_width - (param::columns - 1) * param::invader_spacing) / 2.f;
    for (int r = 0; r < param::rows; ++r) {
        //top row: small invader, 2 middle rows: medium, 2 bottom rows: big (sprite sheet cells 0, 2, 4)
        const int type = r == 0 ? 0 : (r < 3 ? 2 : 4);
        const sf::IntRect rect(sf::Vector2i(param::sprite_size * type, 0),
                               sf::Vector2i(param::sprite_size, param::sprite_size));
        for (int c = 0; c < param::columns; ++c) {
            const sf::Vector2f position(left + c * param::invader_spacing,
                                        param::formation_top + r * param::invader_spacing);
            ships.push_back(std::make_shared<Invader>(rect, position));
        }
    }
}

void GameSystem::clean() {
    for (std::shared_ptr<Ship> &ship : ships)
        ship.reset(); //free up the memory of this shared pointer
    ships.clear(); //clear the vector to be sure we free up any memory left.
    spritesheet.reset();
}

void GameSystem::update(const float &dt) {
    Invader::fire_timer -= dt;
    for (std::shared_ptr<Ship> &s : ships)
        s->update(dt);
    Bullet::update(dt);

    //game over when the player is dead or all the invaders are dead
    bool invaders_alive = false;
    for (size_t i = 1; i < ships.size(); ++i)
        if (!ships[i]->is_exploded())
            invaders_alive = true;

    if (ships[0]->is_exploded() || !invaders_alive) {
        _game_over_timer += dt;
        if (_game_over_timer > param::game_over_delay)
            reset();
    }
}

void GameSystem::render(sf::RenderWindow &window) {
    for (const std::shared_ptr<Ship> &s : ships)
        window.draw(*(s.get()));
    Bullet::render(window);
}
