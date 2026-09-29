//bullet.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include "game_parameters.hpp"

// Bullet POOL:
// Instead of creating a new bullet each time someone shoots (and deleting it later),
// we create 256 bullets once at the start and just reuse them.
// An unused bullet is simply parked off-screen at (-100, -100), where it is ignored.
//
// The rest of the game only uses the STATIC functions (Bullet::fire, Bullet::update...),
// it never creates a Bullet itself (the constructor is protected).
class Bullet : public sf::Sprite {
public:
    //updates all bullets (by calling _update() on all bullets in the pool)
    static void update(const float &dt);
    //Render's all bullets
    static void render(sf::RenderWindow &window);
    //Chose the next bullet of the pool and use it. mode: true = player bullet, false = enemy bullet
    static void fire(const sf::Vector2f &pos, const bool mode);
    //Set all the bullets to -100, -100, set the spritesheet, set origin
    static void init();
    ~Bullet() = default;
protected:
    //protected: nobody outside can create bullets, only the pool below
    Bullet();
    //true=player bullet (goes up), false=Enemy bullet (goes down)
    bool _mode = false;
    //Called by the static update(): moves ONE bullet and checks its collisions
    void _update(const float &dt);
    //index of the last bullet used. unsigned char goes 0..255 and then wraps back to 0,
    //so ++_bullet_pointer always stays inside the array and reuses the oldest bullet
    static unsigned char _bullet_pointer;
    //the pool itself: all the bullets, created once
    static Bullet _bullets[Parameters::bullet_pool_size];
};
