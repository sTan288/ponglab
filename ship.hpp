//ship.hpp
#pragma once
#include <SFML/Graphics.hpp>

// Class hierarchy:
//
//            sf::Sprite        (SFML: a picture with a position, can be drawn with window.draw)
//                |
//              Ship            (abstract: everything common to the player and the invaders)
//             /    \
//        Invader   Player
//
// Because Ship inherits from sf::Sprite, every ship already has setPosition(), move(),
// getPosition(), getGlobalBounds()... and can be passed straight to window.draw().

class Ship : public sf::Sprite {
public:
    Ship();
    //Copy constructor: called when a ship is copied, copies ALL the data (sprite + our members)
    Ship(const Ship &s);
    //Constructor that takes the part of the sprite sheet to display
    Ship(sf::IntRect ir);
    //Pure virtual deconstructor -- makes this an abstract class and avoids undefined behaviour!
    //(abstract = we can't create a plain Ship, only a Player or an Invader)
    virtual ~Ship() = 0;
    //Update, virtual so can be overridden, but not pure virtual (Ship has its own version)
    virtual void update(const float &dt);
    //Called on ALL ships when the formation hits a side. Does nothing here,
    //only Invader overrides it -> the player is never pushed down.
    virtual void move_down();
    //getter: has this ship been hit?
    bool is_exploded() const;
    //switch to the explosion picture. virtual so Invader can add its own behaviour
    virtual void explode();
protected: //protected = visible in this class AND in the classes that inherit from it
    sf::IntRect _sprite;          //the rectangle of the sprite sheet this ship uses
    bool _exploded = false;       //true once the ship has been hit
    float _explosion_timer = 0.f; //time left before the explosion picture disappears
};

class Invader : public Ship {
public:
    //static = ONE variable shared by ALL invaders (not one per invader).
    //Change it once and every invader sees the new value -> they move as one block.
    static bool direction;   //true = moving right, false = moving left
    static float speed;      //current horizontal speed (px/s)
    static float fire_timer; //shooting cooldown shared by all invaders (s)

    Invader();
    Invader(const Invader &inv);
    Invader(sf::IntRect ir, sf::Vector2f pos);
    void update(const float &dt) override;  //override = replaces Ship::update for invaders
    void move_down() override;              //invaders really go down
    void explode() override;                //explode + make the others faster
};

class Player : public Ship {
public:
    Player();
    void update(const float &dt) override; //keyboard movement and shooting
private:
    float _fire_timer = 0.f; //time left before the player can shoot again (s)
};
