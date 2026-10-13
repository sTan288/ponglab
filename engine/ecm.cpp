//ecm.cpp
#include "ecm.hpp"

//---------------------------------------------------------------- Entity

//по умолчанию: в точке (0,0), без поворота, живая, видимая, не на удаление
Entity::Entity()
    : _position(0.f, 0.f), _rotation(0.f), _alive(true), _visible(true), _for_deletion(false) {}

Entity::~Entity() = default; //компоненты удалятся сами (shared_ptr в векторе)

void Entity::update(const float &dt) {
  if (!_alive) {
    return; //"мёртвую" сущность не обновляем
  }
  //1. убираем компоненты, помеченные на удаление.
  //   remove_if сдвигает ненужные в конец вектора, erase их отрезает
  _components.erase(std::remove_if(_components.begin(), _components.end(),
                                   [](const std::shared_ptr<Component> &c) { return c->is_for_deletion(); }),
                    _components.end());
  //2. обновляем все оставшиеся. Полиморфизм: у каждого компонента своя update()
  for (auto &c : _components) {
    c->update(dt);
  }
}

void Entity::render() {
  if (!_visible) {
    return; //невидимую сущность не рисуем
  }
  for (auto &c : _components) {
    c->render(); //компонент сам кладёт свою фигуру в очередь Renderer-а (если ему есть что рисовать)
  }
}

//простые геттеры и сеттеры
const sf::Vector2f &Entity::get_position() const { return _position; }
void Entity::set_position(const sf::Vector2f &pos) { _position = pos; }
bool Entity::is_for_deletion() const { return _for_deletion; }
float Entity::get_rotation() const { return _rotation; }
void Entity::set_rotation(float rot) { _rotation = rot; }
bool Entity::is_alive() const { return _alive; }
void Entity::set_alive(bool alive) { _alive = alive; }
void Entity::set_for_delete() {
  _for_deletion = true;
  _alive = false;   //помеченная на удаление сущность больше не обновляется...
  _visible = false; //...и не рисуется
}
bool Entity::is_visible() const { return _visible; }
void Entity::set_visible(bool visible) { _visible = visible; }

//---------------------------------------------------------------- Component

//запоминаем родителя; сам компонент на удаление не помечен
Component::Component(Entity *const p) : _parent(p), _for_deletion(false) {}

Component::~Component() = default;

bool Component::is_for_deletion() const { return _for_deletion; }

//---------------------------------------------------------------- EntityManager

void EntityManager::update(const float &dt) {
  //Обычный for по индексу, а не range-for: если во время update кто-то добавит сущность
  //в список, вектор может переехать в памяти, и range-for сломается
  for (size_t i = 0; i < list.size(); ++i) {
    list[i]->update(dt);
  }
  //удаляем сущности, помеченные на удаление (например, съеденные "nibbles")
  list.erase(std::remove_if(list.begin(), list.end(),
                            [](const std::shared_ptr<Entity> &e) { return e->is_for_deletion(); }),
             list.end());
}

void EntityManager::render() {
  for (auto &e : list) {
    e->render();
  }
}
