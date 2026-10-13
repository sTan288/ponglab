//ecm.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>
#include <memory>
#include <type_traits>
#include <typeinfo>
#include <vector>

// ECM = Entity Component Model (модель "сущность — компонент").
//
// Раньше каждый тип объекта был своим классом-наследником Entity (Player, Ghost...).
// С ростом игры это превращается в огромное дерево наследования, которое тяжело менять.
// В ECM всё наоборот:
//  - Entity — это просто "коробка": позиция, поворот, флаги и СПИСОК КОМПОНЕНТОВ;
//  - Component — маленький кусочек поведения: "рисуется кругом", "ходит по клавишам",
//    "ходит сам", "его можно съесть"...
// Игрок = Entity + ShapeComponent + PlayerMovementComponent.
// Призрак = Entity + ShapeComponent + EnemyAIComponent.
// Компоненты можно добавлять и убирать прямо во время игры.

class Component; //forward declaration: "такой класс есть, полностью опишем ниже"

class Entity {
public:
  Entity();
  virtual ~Entity();
  virtual void update(const float &dt); //обновляет все компоненты (если сущность "жива")
  virtual void render();                //рисует все компоненты (если сущность видима)

  const sf::Vector2f &get_position() const;
  void set_position(const sf::Vector2f &_position);
  bool is_for_deletion() const;
  float get_rotation() const;
  void set_rotation(float _rotation);
  bool is_alive() const;
  void set_alive(bool _alive);
  void set_for_delete();  //пометить на удаление: EntityManager удалит сущность в конце update
  bool is_visible() const;
  void set_visible(bool _visible);

  // Создать компонент типа T, прикрепить к этой сущности и вернуть указатель на него.
  // template: T — "переменная-тип". При вызове add_component<ShapeComponent>() компилятор
  // подставит ShapeComponent вместо T и создаст отдельную версию функции под этот тип.
  // Targs... params — любое количество любых параметров, они передаются в конструктор компонента.
  // Пример: auto s = ghost->add_component<ShapeComponent>();
  template <typename T, typename... Targs>
  std::shared_ptr<T> add_component(Targs... params) {
    //проверка при КОМПИЛЯЦИИ: T обязан быть наследником Component, иначе ошибка сборки
    static_assert(std::is_base_of<Component, T>::value, "T != component");
    //создаём компонент; первым параметром передаём this — его "родителя" (эту сущность)
    std::shared_ptr<T> sp(std::make_shared<T>(this, params...));
    _components.push_back(sp);
    return sp;
  }

  // Найти все компоненты РОВНО типа T.
  // typeid(*c) — настоящий тип объекта во время работы программы (RTTI).
  // Наследники не подходят: get_components<ActorMovementComponent>() НЕ найдёт PlayerMovementComponent.
  template <typename T>
  const std::vector<std::shared_ptr<T>> get_components() const {
    static_assert(std::is_base_of<Component, T>::value, "T != component");
    std::vector<std::shared_ptr<T>> ret;
    for (const auto &c : _components) {
      if (typeid(*c) == typeid(T)) {
        ret.push_back(std::dynamic_pointer_cast<T>(c)); //приводим указатель Component -> T
      }
    }
    return ret;
  }

  // Найти все компоненты типа T ИЛИ его наследников.
  // dynamic_cast пробует превратить Component* в T*: получилось -> компонент подходит.
  // get_compatible_components<ActorMovementComponent>() найдёт и PlayerMovement, и EnemyAI.
  // dynamic_cast медленный, поэтому каждый кадр лучше его не вызывать без нужды.
  template <typename T>
  const std::vector<std::shared_ptr<T>> get_compatible_components() {
    static_assert(std::is_base_of<Component, T>::value, "T != component");
    std::vector<std::shared_ptr<T>> ret;
    for (const auto &c : _components) {
      auto dd = dynamic_cast<T *>(&(*c));
      if (dd) {
        ret.push_back(std::dynamic_pointer_cast<T>(c));
      }
    }
    return ret;
  }

protected:
  std::vector<std::shared_ptr<Component>> _components; //все компоненты этой сущности
  sf::Vector2f _position; //позиция на экране (px)
  float _rotation;        //поворот (градусы)
  bool _alive;            //обновлять ли (update) компоненты
  bool _visible;          //рисовать ли (render) компоненты
  bool _for_deletion;     //пора ли удалить сущность
};

// Component — базовый класс для всех компонентов.
// Абстрактный: update и render чисто виртуальные, каждый компонент пишет их сам.
class Component {
public:
  Component() = delete; //компонент без родителя не бывает
  bool is_for_deletion() const;
  virtual void update(const float &dt) = 0;
  virtual void render() = 0;
  virtual ~Component();

protected:
  //Сущность, к которой прикреплён компонент. Обычный указатель, а не shared_ptr:
  //сущность владеет компонентом, а не наоборот (иначе была бы "петля" и утечка памяти).
  //Entity *const — сам указатель менять нельзя: родитель задаётся один раз навсегда.
  Entity *const _parent;
  bool _for_deletion; //пора ли удалить компонент
  //explicit + protected: создать можно только из наследника и только явно
  explicit Component(Entity *const p);
};

// EntityManager — "менеджер сущностей": хранит список сущностей одной сцены
// и умеет обновить/нарисовать их все разом. У каждой сцены свой менеджер.
struct EntityManager {
  std::vector<std::shared_ptr<Entity>> list; //все сущности
  void update(const float &dt); //update у каждой + удаление помеченных на удаление
  void render();                //render у каждой
};
