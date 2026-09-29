//ship.cpp
#include "ship.hpp"
#include <cstdlib>
#include "bullet.hpp"
#include "game_parameters.hpp"
#include "game_system.hpp"

//renaming with using holds only for the current scope (here the whole file)
using param = Parameters;
using gs = GameSystem;

//================================================================ Ship
Ship::Ship() {}

//": sf::Sprite(s), _sprite(...)" is the initialiser list: it builds the parent part
//and the members before the body {} runs. Without sf::Sprite(s) the position/texture would be lost.
Ship::Ship(const Ship &s) :
    sf::Sprite(s), _sprite(s._sprite), _exploded(s._exploded), _explosion_timer(s._explosion_timer) {}

Ship::Ship(sf::IntRect ir) : sf::Sprite() {
    _sprite = ir;
    setTexture(*gs::spritesheet); //use the whole sheet as texture...
    setTextureRect(_sprite);      //...but only show this 32x32 rectangle of it
}

void Ship::update(const float &dt) {
    //Logic common to every ship: once the explosion has lasted long enough, make the ship invisible.
    //(The ship is not deleted, it just becomes fully transparent.)
    if (_exploded && _explosion_timer > 0.f) {
        _explosion_timer -= dt;
        if (_explosion_timer <= 0.f)
            setColor(sf::Color::Transparent);
    }
}

void Ship::move_down() {} //by default a ship does not move down (the Player keeps this version)

bool Ship::is_exploded() const { return _exploded; }

void Ship::explode() {
    //the explosion picture is in the sprite sheet at column 4, row 1 -> (128, 32)
    setTextureRect(sf::IntRect(sf::Vector2i(param::sprite_size * 4, param::sprite_size),
                               sf::Vector2i(param::sprite_size, param::sprite_size)));
    _exploded = true;
    _explosion_timer = param::explosion_time; //start the countdown before it disappears
}

//Define the ship deconstructor.
//Although we set this to pure virtual, we still have to define it.
Ship::~Ship() = default;

//================================================================ Invader
//Static members must be defined once in a .cpp (here with their starting values)
bool Invader::direction = true;
float Invader::speed = param::invader_speed;
float Invader::fire_timer = 0.f;

Invader::Invader() : Ship() {}
Invader::Invader(const Invader &inv) : Ship(inv) {} //nothing new to copy, the Ship part does everything
Invader::Invader(sf::IntRect ir, sf::Vector2f pos) : Ship(ir) {
    //origin in the middle of the sprite, so getPosition() is the centre of the invader
    setOrigin(param::sprite_size / 2.f, param::sprite_size / 2.f);
    setPosition(pos);
}

void Invader::update(const float &dt) {
    Ship::update(dt); //run the common ship logic first (explosion fade)

    //move left or right: (direction ? 1 : -1) gives the sign, speed * dt gives the distance this frame
    move(dt * (direction ? 1.0f : -1.0f) * speed, 0.0f);

    //dead invaders keep flying with the formation (so the block stays aligned)
    //but they don't take part in the logic below
    if (_exploded)
        return;

    //Did this invader reach the side it is moving towards?
    //Checking "direction" too avoids a feedback loop: after the flip, the next invaders
    //in the same frame won't flip it back, because they are now moving away from that side.
    if ((direction && getPosition().x > param::game_width - param::sprite_size / 2.f) ||
        (!direction && getPosition().x < param::sprite_size / 2.f)) {
        direction = !direction;          //turn around (for ALL invaders, it's static)
        speed += param::invader_acc;     //go a bit faster
        for (std::shared_ptr<Ship> &ship : gs::ships)
            ship->move_down();           //only invaders really move, the player ignores it
    }

    //the invaders reached the player's height: game over
    const std::shared_ptr<Ship> &player = gs::ships[0];
    if (getPosition().y + param::sprite_size / 2.f >= player->getPosition().y - param::sprite_size / 2.f)
        player->explode();

    //Shooting: the cooldown is shared (GameSystem::update lowers it once per frame).
    //When it is ready, each alive invader has a 1% chance per frame to be the one that shoots.
    if (fire_timer <= 0.f && std::rand() % 100 == 0) {
        Bullet::fire(getPosition(), false); //false = enemy bullet (goes down)
        //next shot in invader_fire_min .. invader_fire_min + 1 seconds
        fire_timer = param::invader_fire_min + (std::rand() % 100) / 100.f;
    }
}

void Invader::move_down() {
    move(0.f, param::invader_drop);
}

void Invader::explode() {
    Ship::explode(); //do the common part (explosion picture + timer)
    speed += param::invader_kill_acc; //the remaining invaders speed up
}

//================================================================ Player
//The player picture is in the sprite sheet at column 5, row 1 -> (160, 32)
Player::Player() :
    Ship(sf::IntRect(sf::Vector2i(param::sprite_size * 5, param::sprite_size),
                     sf::Vector2i(param::sprite_size, param::sprite_size))) {
    setOrigin(param::sprite_size / 2.f, param::sprite_size / 2.f);
    //start in the middle, at the bottom of the screen
    setPosition(param::game_width / 2.f, param::game_height - static_cast<float>(param::sprite_size));
}

void Player::update(const float &dt) {
    Ship::update(dt); //common ship logic (explosion fade)
    if (_exploded)
        return; //a dead player can't move or shoot

    //direction: -1 = left, +1 = right, 0 = no key (or both keys) pressed
    float direction = 0.f;
    //Move left, only if we are not already at the left edge
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && getPosition().x > param::sprite_size / 2.f)
        direction -= 1.f;
    //Move Right, only if we are not already at the right edge
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && getPosition().x < param::game_width - param::sprite_size / 2.f)
        direction += 1.f;
    move(direction * param::player_speed * dt, 0.f);

    //Fire, with a cooldown: the timer goes down every frame,
    //we can only shoot when it reaches 0, and shooting puts it back up.
    _fire_timer -= dt;
    if (_fire_timer <= 0.f && sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        Bullet::fire(getPosition(), true); //true = player bullet (goes up)
        _fire_timer = param::player_fire_cooldown;
    }
}
