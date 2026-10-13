//components.hpp
#pragma once
#include "ecm.hpp"

// Компоненты игры Pacman. Каждый — маленький кусочек поведения,
// который можно "прикрепить" к любой сущности через add_component<...>().

// ShapeComponent — сущность рисуется SFML-фигурой (кругом, квадратом...).
class ShapeComponent : public Component {
public:
  ShapeComponent() = delete;
  explicit ShapeComponent(Entity *const p);

  void update(const float &dt) override; //двигает фигуру туда, где сейчас сущность
  void render() override;                //кладёт фигуру в очередь Renderer-а

  sf::Shape &get_shape() const; //доступ к фигуре, чтобы задать цвет, origin...

  // Заменить фигуру на фигуру типа T, параметры уходят в её конструктор.
  // Пример: set_shape<sf::CircleShape>(12.f) -> круг радиуса 12.
  // Так одним компонентом можно сделать любую фигуру, а не писать CircleComponent, RectComponent...
  template <typename T, typename... Targs>
  void set_shape(Targs... params) {
    _shape.reset(new T(params...));
  }

protected:
  std::shared_ptr<sf::Shape> _shape;
};

// ActorMovementComponent — общая основа для всего, что ходит по лабиринту.
// Умеет: скорость, move() с проверкой стен. Сам по себе ничего не делает в update —
// решать, КУДА идти, будут наследники: игрок (клавиши) и призрак (ИИ).
class ActorMovementComponent : public Component {
public:
  explicit ActorMovementComponent(Entity *p);
  ActorMovementComponent() = delete;

  float get_speed() const;
  void set_speed(float _speed);

  //сдвинуть сущность, если на новом месте нет стены. Возвращает true, если сдвинули
  bool move(const sf::Vector2f &);
  bool move(float x, float y);

  void render() override {} //рисовать нечего, этим занимается ShapeComponent
  void update(const float &dt) override;

protected:
  bool _valid_move(const sf::Vector2f &); //true, если круг в этой точке не задевает стены
  float _speed;                          //скорость (px/сек)
};

// PlayerMovementComponent — pacman ходит стрелками / WASD.
class PlayerMovementComponent : public ActorMovementComponent {
public:
  explicit PlayerMovementComponent(Entity *p);
  PlayerMovementComponent() = delete;
  void update(const float &dt) override;

private:
  //сдвиг по одной оси; если упёрлись, но рядом поворот — помогаем "вписаться" в коридор
  bool _move_axis(const sf::Vector2f &delta, bool allow_assist);
};

// EnemyAIComponent — призрак ходит сам.
// Едет прямо, пока не упрётся в стену или не доедет до развилки (клетка WAYPOINT '+'),
// там выбирает случайное новое направление, но никогда не разворачивается назад (если есть выбор).
class EnemyAIComponent : public ActorMovementComponent {
public:
  explicit EnemyAIComponent(Entity *p);
  EnemyAIComponent() = delete;
  void update(const float &dt) override;
  void reset(); //забыть направление (после респауна выбрать новое)

protected:
  sf::Vector2f _direction; //куда едем сейчас: (1,0) вправо, (0,1) вниз, (0,-1) вверх, (-1,0) влево
  //"Конечный автомат" (state machine) из трёх состояний:
  //  ROAMING  — едем прямо, развилки ещё нет;
  //  ROTATING — выбираем новое направление (длится один кадр);
  //  ROTATED  — уже повернули, едем, пока не выедем из клетки развилки
  //             (иначе на следующем кадре мы всё ещё "на развилке" и повернём снова).
  enum state { ROAMING, ROTATING, ROTATED };
  state _state;
};

// PickupComponent — сущность можно съесть (это "nibble", еда).
// Каждый кадр смотрит, нет ли рядом того, кто умеет ходить (у кого есть ActorMovementComponent).
// Если есть — ускоряет его и удаляет себя.
class PickupComponent : public Component {
public:
  PickupComponent(Entity *p, bool big); //big = большая синяя точка
  PickupComponent() = delete;
  void update(const float &dt) override;
  void render() override {}

  //сколько очков набрал ИГРОК (не призраки) с прошлого вызова; счётчик обнуляется
  static int take_player_points();

private:
  float _points; //насколько ускорить того, кто съел
  int _score;    //сколько очков даёт игроку
  static int _player_points; //очки, накопленные игроком (одни на все точки)
};
