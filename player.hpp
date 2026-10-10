//player.hpp
#pragma once
#include "entity.hpp"

// Игрок — пурпурный круг, управляется стрелками (или WASD).
// Наследуется от Entity, значит у него есть позиция, фигура, move() и т.д.
// Позиция игрока — это ЦЕНТР круга (origin фигуры ставим в центр в конструкторе).
class Player : public Entity {
public:
  void update(const float &dt) override; //override: переопределяем виртуальную функцию из Entity
  Player();
  void render(sf::RenderWindow &window) const override;

private:
  //true, если круг в позиции pos целиком помещается вне стен
  bool _valid_move(const sf::Vector2f &pos) const;

  static constexpr float _radius = 25.f; //радиус круга (px)
  static constexpr float _speed = 200.f; //скорость (px/сек)
};
