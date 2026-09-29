//bullet.cpp
#include "bullet.hpp"
#include "game_system.hpp"

using param = Parameters;
using gs = GameSystem;

//define the static members (the 256 bullets are created here, before main() starts)
unsigned char Bullet::_bullet_pointer;
Bullet Bullet::_bullets[param::bullet_pool_size];

Bullet::Bullet() {}

void Bullet::update(const float &dt) {
    //"Bullet &" = reference: we work on the real bullet in the array, not on a copy
    for (Bullet &bullet : _bullets)
        bullet._update(dt);
}

void Bullet::render(sf::RenderWindow &window) {
    //off-screen bullets are drawn too, but they are outside the window so nothing shows
    for (const Bullet &bullet : _bullets)
        window.draw(bullet);
}

void Bullet::fire(const sf::Vector2f &pos, const bool mode) {
    //take the next bullet of the pool.
    //unsigned char wraps from 255 back to 0, so we always reuse the oldest bullet
    Bullet &bullet = _bullets[++_bullet_pointer];
    if (mode) //player: white bullet, sprite sheet column 1, row 1 -> (32, 32)
        bullet.setTextureRect(sf::IntRect(sf::Vector2i(param::sprite_size, param::sprite_size),
                                          sf::Vector2i(param::sprite_size, param::sprite_size)));
    else //invader: green bullet, sprite sheet column 3, row 1 -> (96, 32)
        bullet.setTextureRect(sf::IntRect(sf::Vector2i(param::sprite_size * 3, param::sprite_size),
                                          sf::Vector2i(param::sprite_size, param::sprite_size)));
    //put it where the shooter is: from now on _update() will move it
    bullet.setPosition(pos);
    bullet._mode = mode;
}

void Bullet::init() {
    //must be called AFTER the texture is loaded (GameSystem::reset does it)
    for (Bullet &bullet : _bullets) {
        bullet.setTexture(*gs::spritesheet);
        bullet.setOrigin(param::sprite_size / 2.f, param::sprite_size / 2.f); //centre of the sprite
        bullet.setPosition(-100.f, -100.f); //parked off-screen = not in use
    }
}

void Bullet::_update(const float &dt) {
    //unused bullets (parked at -100) and bullets that left the screen are ignored
    if (getPosition().y < -param::sprite_size || getPosition().y > param::game_height + param::sprite_size) {
        //off screen - do nothing
        return;
    }
    //player bullets go up (-y), enemy bullets go down (+y)
    move(sf::Vector2f(0.f, dt * param::bullet_speed * (_mode ? -1.0f : 1.0f)));

    //Collision: SFML gives us the rectangle around each sprite (getGlobalBounds),
    //and intersects() tells if two rectangles overlap.
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
        //a dead ship can't be hit again
        if (!s->is_exploded() && s->getGlobalBounds().intersects(bounding_box)) {
            //Explode the ship (virtual: Invader::explode or Ship::explode for the player)
            s->explode();
            //warp bullet off-screen -> it goes back to the pool
            setPosition(sf::Vector2f(-100.f, -100.f));
            return;
        }
    }
}
