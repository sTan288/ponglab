//bullet.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include "game_parameters.hpp"

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
    Bullet();
    //true=player bullet, false=Enemy bullet
    bool _mode = false;
    //Called by the static update()
    void _update(const float &dt);
    static unsigned char _bullet_pointer;
    static Bullet _bullets[Parameters::bullet_pool_size];
};
