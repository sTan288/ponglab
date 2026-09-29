//ship.cpp
#include "ship.hpp"
#include <cstdlib>
#include "bullet.hpp"
#include "game_parameters.hpp"
#include "game_system.hpp"

//renaming with using holds only for the current scope (here the whole file)
using param = Parameters;
using gs = GameSystem;

//---------------------------------------------------------------- Ship
Ship::Ship() {}

Ship::Ship(const Ship &s) :
    sf::Sprite(s), _sprite(s._sprite), _exploded(s._exploded), _explosion_timer(s._explosion_timer) {}

Ship::Ship(sf::IntRect ir) : sf::Sprite() {
    _sprite = ir;
    setTexture(*gs::spritesheet);
    setTextureRect(_sprite);
}

void Ship::update(const float &dt) {
    //once the explosion has lasted long enough, make the ship invisible
    if (_exploded && _explosion_timer > 0.f) {
        _explosion_timer -= dt;
        if (_explosion_timer <= 0.f)
            setColor(sf::Color::Transparent);
    }
}

void Ship::move_down() {}

bool Ship::is_exploded() const { return _exploded; }

void Ship::explode() {
    setTextureRect(sf::IntRect(sf::Vector2i(param::sprite_size * 4, param::sprite_size),
                               sf::Vector2i(param::sprite_size, param::sprite_size)));
    _exploded = true;
    _explosion_timer = param::explosion_time;
}

//Define the ship deconstructor.
//Although we set this to pure virtual, we still have to define it.
Ship::~Ship() = default;

//---------------------------------------------------------------- Invader
bool Invader::direction = true;
float Invader::speed = param::invader_speed;
float Invader::fire_timer = 0.f;

Invader::Invader() : Ship() {}
Invader::Invader(const Invader &inv) : Ship(inv) {}
Invader::Invader(sf::IntRect ir, sf::Vector2f pos) : Ship(ir) {
    setOrigin(param::sprite_size / 2.f, param::sprite_size / 2.f);
    setPosition(pos);
}

void Invader::update(const float &dt) {
    Ship::update(dt);

    move(dt * (direction ? 1.0f : -1.0f) * speed, 0.0f);

    //dead invaders keep flying with the formation but don't take part in the logic
    if (_exploded)
        return;

    if ((direction && getPosition().x > param::game_width - param::sprite_size / 2.f) ||
        (!direction && getPosition().x < param::sprite_size / 2.f)) {
        direction = !direction;
        speed += param::invader_acc;
        for (std::shared_ptr<Ship> &ship : gs::ships)
            ship->move_down();
    }

    //the invaders reached the player: game over
    const std::shared_ptr<Ship> &player = gs::ships[0];
    if (getPosition().y + param::sprite_size / 2.f >= player->getPosition().y - param::sprite_size / 2.f)
        player->explode();

    //the cooldown is shared, each alive invader gets a small chance to take the shot
    if (fire_timer <= 0.f && std::rand() % 100 == 0) {
        Bullet::fire(getPosition(), false);
        fire_timer = param::invader_fire_min + (std::rand() % 100) / 100.f;
    }
}

void Invader::move_down() {
    move(0.f, param::invader_drop);
}

void Invader::explode() {
    Ship::explode();
    speed += param::invader_kill_acc; //the remaining invaders speed up
}

//---------------------------------------------------------------- Player
Player::Player() :
    Ship(sf::IntRect(sf::Vector2i(param::sprite_size * 5, param::sprite_size),
                     sf::Vector2i(param::sprite_size, param::sprite_size))) {
    setOrigin(param::sprite_size / 2.f, param::sprite_size / 2.f);
    setPosition(param::game_width / 2.f, param::game_height - static_cast<float>(param::sprite_size));
}

void Player::update(const float &dt) {
    Ship::update(dt);
    if (_exploded)
        return;

    float direction = 0.f;
    //Move left
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && getPosition().x > param::sprite_size / 2.f)
        direction -= 1.f;
    //Move Right
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && getPosition().x < param::game_width - param::sprite_size / 2.f)
        direction += 1.f;
    move(direction * param::player_speed * dt, 0.f);

    //Fire, with a cooldown
    _fire_timer -= dt;
    if (_fire_timer <= 0.f && sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        Bullet::fire(getPosition(), true);
        _fire_timer = param::player_fire_cooldown;
    }
}
