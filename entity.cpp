//entity.cpp
#include "entity.hpp"

//std::move "передаёт" владение фигурой в поле _shape (unique_ptr нельзя скопировать, только переместить)
Entity::Entity(std::unique_ptr<sf::Shape> s) : _shape(std::move(s)) {}

const sf::Vector2f Entity::get_position() { return _position;}

void Entity::set_position(const sf::Vector2f &pos) {
  _position = pos;
  _shape->setPosition(_position); //двигаем и саму фигуру, иначе на экране она останется на старом месте
}

//сдвиг = поставить в (текущая позиция + смещение)
void Entity::move(const sf::Vector2f &pos) { set_position(_position + pos);}

//базовая сущность ничего не делает за кадр, наследники переопределяют это
void Entity::update(const float &dt) {}
