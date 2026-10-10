//entity.hpp
#pragma once

#include <SFML/Graphics.hpp>
#include <memory>

// Entity (сущность) — базовый класс для всего, что живёт в сцене (игрок, враги...).
// В отличие от Space Invaders мы НЕ наследуемся от класса SFML (там было от sf::Sprite).
// Здесь Entity ВЛАДЕЕТ фигурой sf::Shape (круг, прямоугольник...) и рисует её.
// Конструктор по умолчанию удалён (= delete), поэтому каждый наследник обязан
// передать свою фигуру при создании.
// render() чисто виртуальная (= 0) -> Entity абстрактный, создать его напрямую нельзя.
class Entity {
public:
  Entity(std::unique_ptr<sf::Shape> shp); //единственный способ создать сущность — дать ей фигуру
  Entity() = delete;
  virtual ~Entity() = default;

  virtual void update(const float &dt);                    //логика за один кадр (по умолчанию ничего)
  virtual void render(sf::RenderWindow &window) const = 0; //как рисовать — решает наследник

  const sf::Vector2f get_position();          //где сейчас сущность
  void set_position(const sf::Vector2f &pos); //поставить в точку pos
  void move(const sf::Vector2f &pos);         //сдвинуть на pos относительно текущего места

protected:
  //unique_ptr — умный указатель с единственным владельцем: фигура принадлежит только этой сущности
  //и удалится вместе с ней. Указатель нужен, потому что sf::Shape абстрактный класс.
  std::unique_ptr<sf::Shape> _shape;
  sf::Vector2f _position; //позиция сущности на экране (px)
};
