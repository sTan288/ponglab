//ship.hpp
#pragma once
#include <SFML/Graphics.hpp>

class Ship : public sf::Sprite {
public:
    Ship();
    //Copy constructor
    Ship(const Ship &s);
    //Constructor that takes a sprite
    Ship(sf::IntRect ir);
    //Pure virtual deconstructor -- makes this an abstract class and avoids undefined behaviour!
    virtual ~Ship() = 0;
    //Update, virtual so can be overridden, but not pure virtual
    virtual void update(const float &dt);
    //Only invaders drop down, the player ignores it
    virtual void move_down();
    bool is_exploded() const;
    virtual void explode();
protected:
    sf::IntRect _sprite;
    bool _exploded = false;
    float _explosion_timer = 0.f; //time left before the explosion sprite disappears
};

class Invader : public Ship {
public:
    //shared by all invaders
    static bool direction; //true = right, false = left
    static float speed;
    static float fire_timer; //cooldown shared by all invaders

    Invader();
    Invader(const Invader &inv);
    Invader(sf::IntRect ir, sf::Vector2f pos);
    void update(const float &dt) override;
    void move_down() override;
    void explode() override;
};

class Player : public Ship {
public:
    Player();
    void update(const float &dt) override;
private:
    float _fire_timer = 0.f;
};
