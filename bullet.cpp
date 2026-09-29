//bullet.cpp
#include "bullet.hpp"
#include "game_system.hpp"

using param = Parameters;
using gs = GameSystem;

unsigned char Bullet::_bullet_pointer;
Bullet Bullet::_bullets[param::bullet_pool_size];

Bullet::Bullet() {}

void Bullet::update(const float &dt) {
    for (Bullet &bullet : _bullets)
        bullet._update(dt);
}

void Bullet::render(sf::RenderWindow &window) {
    for (const Bullet &bullet : _bullets)
        window.draw(bullet);
}

void Bullet::fire(const sf::Vector2f &pos, const bool mode) {
    //unsigned char wraps from 255 back to 0, so we always reuse the oldest bullet
    Bullet &bullet = _bullets[++_bullet_pointer];
    if (mode) //player: white bullet
        bullet.setTextureRect(sf::IntRect(sf::Vector2i(param::sprite_size, param::sprite_size),
                                          sf::Vector2i(param::sprite_size, param::sprite_size)));
    else //invader: green bullet
        bullet.setTextureRect(sf::IntRect(sf::Vector2i(param::sprite_size * 3, param::sprite_size),
                                          sf::Vector2i(param::sprite_size, param::sprite_size)));
    bullet.setPosition(pos);
    bullet._mode = mode;
}

void Bullet::init() {
    for (Bullet &bullet : _bullets) {
        bullet.setTexture(*gs::spritesheet);
        bullet.setOrigin(param::sprite_size / 2.f, param::sprite_size / 2.f);
        bullet.setPosition(-100.f, -100.f);
    }
}

void Bullet::_update(const float &dt) {
    if (getPosition().y < -param::sprite_size || getPosition().y > param::game_height + param::sprite_size) {
        //off screen - do nothing
        return;
    }
    move(sf::Vector2f(0.f, dt * param::bullet_speed * (_mode ? -1.0f : 1.0f)));
    const sf::FloatRect bounding_box = getGlobalBounds();
    const std::shared_ptr<Ship> &player = gs::ships[0]; //we know that the first ship is the player
    for (std::shared_ptr<Ship> &s : gs::ships) {
        if (_mode && s == player) {
            //player bullets don't collide with player
            continue;
        }
        if (!_mode && s != player) {
            //invader bullets don't collide with other invaders
            continue;
        }
        if (!s->is_exploded() && s->getGlobalBounds().intersects(bounding_box)) {
            //Explode the ship
            s->explode();
            //warp bullet off-screen
            setPosition(sf::Vector2f(-100.f, -100.f));
            return;
        }
    }
}
