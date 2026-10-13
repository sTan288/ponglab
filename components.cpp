//components.cpp
#include "components.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>
#include "game_parameters.hpp"
#include "game_system.hpp"
#include "level_system.hpp"
#include "renderer.hpp"

//короткие имена, только для этого файла
using ls = LevelSystem;
using param = Parameters;

//расстояние между двумя точками (теорема Пифагора)
static float distance(const sf::Vector2f &a, const sf::Vector2f &b) {
  return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

//центр клетки, в которой лежит точка pos (в пикселях)
static sf::Vector2f tile_centre(const sf::Vector2f &pos) {
  const float ts = ls::get_tile_size();
  //пиксели -> номер клетки (делим и отбрасываем дробную часть) -> пиксели левого верхнего угла
  const sf::Vector2i tile(static_cast<int>(pos.x / ts), static_cast<int>(pos.y / ts));
  return ls::get_tile_position(tile) + sf::Vector2f(ts / 2.f, ts / 2.f);
}

//================================================================ ShapeComponent

//передаём родителя в конструктор Component и сразу создаём фигуру — пустой круг
ShapeComponent::ShapeComponent(Entity *p) : Component(p), _shape(std::make_shared<sf::CircleShape>()) {}

void ShapeComponent::update(const float &dt) {
  //фигура "следует" за сущностью
  _shape->setPosition(_parent->get_position());
}

void ShapeComponent::render() {
  //ещё раз ставим позицию: если сущность только что создали и update ещё не было,
  //фигура не мелькнёт в точке (0,0)
  _shape->setPosition(_parent->get_position());
  Renderer::queue(_shape.get()); //.get() — достать обычный указатель из умного
}

sf::Shape &ShapeComponent::get_shape() const { return *_shape; }

//================================================================ ActorMovementComponent

ActorMovementComponent::ActorMovementComponent(Entity *p) : Component(p), _speed(100.0f) {}

void ActorMovementComponent::update(const float &dt) {} //базовый ничего не решает сам

bool ActorMovementComponent::_valid_move(const sf::Vector2f &pos) {
  //Проверяем 4 угла квадрата вокруг круга: если хоть один угол в стене — нельзя.
  //(в лабе проверяется только центр, но тогда круг наполовину залезает в стены)
  const float r = param::entity_size - 0.5f; //запас полпикселя: касаться стены можно
  const sf::Vector2f corners[] = {{pos.x - r, pos.y - r}, {pos.x + r, pos.y - r},
                                  {pos.x - r, pos.y + r}, {pos.x + r, pos.y + r}};
  try {
    for (const sf::Vector2f &c : corners) {
      if (ls::get_tile_at(c) == ls::WALL) {
        return false;
      }
    }
  } catch (const std::string &) {
    return false; //точка вне уровня (get_tile_at бросает исключение-строку)
  }
  return true;
}

bool ActorMovementComponent::move(const sf::Vector2f &p) {
  const sf::Vector2f new_pos = _parent->get_position() + p;
  if (_valid_move(new_pos)) {
    _parent->set_position(new_pos);
    return true;
  }
  return false; //там стена — стоим на месте
}

bool ActorMovementComponent::move(float x, float y) { return move(sf::Vector2f(x, y)); }

float ActorMovementComponent::get_speed() const { return _speed; }

void ActorMovementComponent::set_speed(float speed) {
  _speed = std::min(speed, param::max_speed); //не даём разогнаться больше максимума
}

//================================================================ PlayerMovementComponent

//конструктор просто передаёт родителя в конструктор ActorMovementComponent
PlayerMovementComponent::PlayerMovementComponent(Entity *p) : ActorMovementComponent(p) {}

void PlayerMovementComponent::update(const float &dt) {
  //направление по клавишам: -1, 0 или +1 по каждой оси (в SFML ось y смотрит ВНИЗ)
  float dx = 0.f, dy = 0.f;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) dx -= 1.f;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) dx += 1.f;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) dy -= 1.f;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) dy += 1.f;

  const float step = _speed * dt; //сколько пикселей пройти за этот кадр
  //каждую ось двигаем отдельно; помощь с поворотом — только если нажата одна ось,
  //иначе "подтягивание" к центру коридора мешало бы движению по второй оси
  if (dy != 0.f) _move_axis({0.f, dy * step}, dx == 0.f);
  if (dx != 0.f) _move_axis({dx * step, 0.f}, dy == 0.f);

  ActorMovementComponent::update(dt);
}

bool PlayerMovementComponent::_move_axis(const sf::Vector2f &delta, bool allow_assist) {
  if (move(delta)) {
    return true; //прошли без проблем
  }
  if (!allow_assist) {
    return false;
  }
  //Упёрлись. Коридор шириной 25 px, а pacman 20 px: чтобы свернуть, надо стоять почти ровно
  //по центру коридора — руками так не попасть. Поэтому, если из ЦЕНТРА текущей клетки
  //в эту сторону пройти можно, подтягиваем pacman-а к центру по другой оси.
  const sf::Vector2f pos = _parent->get_position();
  const sf::Vector2f c = tile_centre(pos);
  const bool horizontal = delta.x != 0.f;
  //куда попадём, если сначала встанем по центру, а потом шагнём
  const sf::Vector2f centred_target = horizontal ? sf::Vector2f(pos.x + delta.x, c.y)
                                                 : sf::Vector2f(c.x, pos.y + delta.y);
  if (!_valid_move(centred_target)) {
    return false; //в эту сторону стена даже из центра — поворота нет
  }
  //сколько не хватает до центра по другой оси и в какую сторону
  const float off = horizontal ? c.y - pos.y : c.x - pos.x;
  const float amount = std::min(std::abs(off), std::abs(horizontal ? delta.x : delta.y));
  const float sign = off > 0.f ? 1.f : -1.f;
  return horizontal ? move(0.f, sign * amount) : move(sign * amount, 0.f);
}

//================================================================ EnemyAIComponent

//четыре возможных направления: вправо, вниз, вверх, влево
static const sf::Vector2i directions[] = {{1, 0}, {0, 1}, {0, -1}, {-1, 0}};

//начинаем в состоянии ROTATING: в первом же кадре призрак выберет, куда ехать
EnemyAIComponent::EnemyAIComponent(Entity *p)
    : ActorMovementComponent(p), _direction(0.f, 0.f), _state(ROTATING) {}

void EnemyAIComponent::reset() {
  _direction = {0.f, 0.f};
  _state = ROTATING;
}

void EnemyAIComponent::update(const float &dt) {
  //на сколько пикселей сдвинуться за этот кадр
  const float mva = dt * _speed;
  //где мы сейчас
  const sf::Vector2f pos = _parent->get_position();
  //где будем, если поедем дальше прямо
  const sf::Vector2f newpos = pos + _direction * mva;
  //направление "назад" (обратное текущему) — туда поворачивать нельзя
  const sf::Vector2i baddir = -1 * sf::Vector2i(_direction);

  switch (_state) {
  case ROAMING: {
    //Едем ли мы через ЦЕНТР клетки-развилки именно в этом кадре?
    //Координаты дробные, ровно в центр мы не попадём, поэтому проверяем, что центр
    //лежит между текущей и следующей позицией (до центра мы "не доехали", а после шага — "проехали").
    const sf::Vector2f c = tile_centre(pos);
    const sf::Vector2f to_c_now = c - pos;
    const sf::Vector2f to_c_next = c - newpos;
    const float before = to_c_now.x * _direction.x + to_c_now.y * _direction.y;   //>=0: центр впереди
    const float after = to_c_next.x * _direction.x + to_c_next.y * _direction.y;  //<=0: центр позади
    const bool at_waypoint = ls::get_tile_at(pos) == ls::WAYPOINT && before >= 0.f && after <= 0.f;

    if (at_waypoint || !_valid_move(newpos)) { //развилка или стена впереди
      _parent->set_position(c); //встаём ровно в центр клетки, чтобы точно вписаться в поворот
      _state = ROTATING;        //начинаем поворот
    } else {
      move(_direction * mva);   //едем дальше
    }
    break;
  }

  case ROTATING: {
    //собираем все "хорошие" направления: не назад и не в стену
    std::vector<sf::Vector2i> options;
    const float ts = ls::get_tile_size();
    for (const sf::Vector2i &d : directions) {
      if (d == baddir) {
        continue; //не разворачиваемся
      }
      //какая клетка соседняя в этом направлении? (мы стоим в центре своей клетки)
      const sf::Vector2f next_tile = pos + sf::Vector2f(d) * ts;
      try {
        if (ls::get_tile_at(next_tile) == ls::WALL) {
          continue; //там стена
        }
      } catch (const std::string &) {
        continue; //за краем уровня
      }
      options.push_back(d);
    }
    //В лабе тут цикл while "крутить случайное направление, пока не подойдёт".
    //Но в тупике подходящих нет совсем — while крутился бы вечно и игра зависла бы.
    //Поэтому, если вариантов нет, разрешаем развернуться назад.
    sf::Vector2i newdir = options.empty() ? baddir : options[std::rand() % options.size()];
    _direction = sf::Vector2f(newdir);
    _state = ROTATED;
    break;
  }

  case ROTATED:
    //выехали из клетки-развилки?
    if (ls::get_tile_at(pos) != ls::WAYPOINT) {
      _state = ROAMING; //да -> снова просто едем
    }
    move(_direction * mva); //нет -> едем дальше, но повторно не поворачиваем
    break;
  }
  ActorMovementComponent::update(dt);
}

//================================================================ PickupComponent

int PickupComponent::_player_points = 0;

PickupComponent::PickupComponent(Entity *p, bool big)
    : Component(p),
      _points(big ? param::big_nibble_speed : param::nibble_speed),
      _score(big ? param::big_nibble_points : param::nibble_points) {}

int PickupComponent::take_player_points() {
  const int pts = _player_points;
  _player_points = 0;
  return pts;
}

void PickupComponent::update(const float &) {
  //все сущности активной сцены (pacman и призраки)
  for (std::shared_ptr<Entity> &e : GameSystem::get_active_scene()->get_entities()) {
    //достаточно близко ко мне?
    if (distance(e->get_position(), _parent->get_position()) < param::pickup_distance) {
      //у этой сущности есть компонент движения? (то есть это pacman или призрак)
      std::vector<std::shared_ptr<ActorMovementComponent>> comp =
          e->get_compatible_components<ActorMovementComponent>();
      if (!comp.empty()) {
        //ням-ням: ускоряем того, кто съел
        comp[0]->set_speed(comp[0]->get_speed() + _points);
        //очки получает только игрок, призраки просто ускоряются
        if (!e->get_compatible_components<PlayerMovementComponent>().empty()) {
          _player_points += _score;
        }
        _parent->set_for_delete(); //удаляем себя (EntityManager уберёт в конце кадра)
        break;                     //дальше не ищем — нас уже съели
      }
    }
  }
}
