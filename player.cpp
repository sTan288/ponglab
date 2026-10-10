//player.cpp
#include "player.hpp"
#include "level_system.hpp" //наша библиотека уровней: нужна, чтобы узнать, где стены

using ls = LevelSystem; //короткое имя, чтобы писать ls::WALL вместо LevelSystem::WALL

void Player::update(const float &dt) {
  //1. Смотрим, какие клавиши зажаты, и собираем направление движения.
  //   Влево = -1 по x, вправо = +1, вверх = -1 по y (в SFML ось y смотрит ВНИЗ), вниз = +1.
  //   Если зажаты две клавиши (например вверх+вправо) — движемся по диагонали.
  sf::Vector2f direction(0.f, 0.f);
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    direction.x -= 1.f;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    direction.x += 1.f;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    direction.y -= 1.f;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    direction.y += 1.f;

  //2. Скорость в px/сек, поэтому скорость * dt = сколько пикселей пройти за этот кадр
  //   (игра идёт с одинаковой скоростью при любом FPS).
  //   Каждую ось проверяем ОТДЕЛЬНО: если упёрлись в стену по одной оси,
  //   по другой всё равно можно двигаться — круг "скользит" вдоль стены.
  const sf::Vector2f step_x(direction.x * _speed * dt, 0.f);
  const sf::Vector2f step_y(0.f, direction.y * _speed * dt);
  if (_valid_move(_position + step_x)) //шагаем, только если новая позиция не в стене
    move(step_x);
  if (_valid_move(_position + step_y))
    move(step_y);

  Entity::update(dt); //вызываем версию родителя (на будущее, вдруг там появится логика)
}

bool Player::_valid_move(const sf::Vector2f &pos) const {
  //Проверяем 4 угла квадрата вокруг круга: если хоть один угол в стене — двигаться нельзя.
  //Так никакая часть круга не залезет в стену (проверять только центр мало).
  const float r = _radius - 1.f; //запас 1px: касаться стены можно, залезать — нет
  const sf::Vector2f corners[] = {{pos.x - r, pos.y - r}, {pos.x + r, pos.y - r},  //верх-лево, верх-право
                                  {pos.x - r, pos.y + r}, {pos.x + r, pos.y + r}}; //низ-лево, низ-право
  try {
    for (const sf::Vector2f &c : corners)
      if (ls::get_tile_at(c) == ls::WALL) //какая клетка лежит под этой точкой экрана?
        return false;
  } catch (const std::string &) {
    //get_tile_at бросает исключение (строку), если точка вне уровня -> туда тоже нельзя
    return false;
  }
  return true;
}

//Конструктор: создаём круг радиуса _radius и отдаём его в конструктор Entity
Player::Player()
    : Entity(std::make_unique<sf::CircleShape>(_radius)) {
  _shape->setFillColor(sf::Color::Magenta); //пурпурный цвет
  //origin = точка, к которой "привязана" позиция фигуры. По умолчанию левый верхний угол,
  //ставим его в центр круга -> позиция игрока = центр круга
  _shape->setOrigin(sf::Vector2f(_radius, _radius));
}

void Player::render(sf::RenderWindow &window) const {
  window.draw(*_shape); //* превращает указатель в сам объект, который нужен draw()
}
