//scenes.cpp
#include "scenes.hpp"
#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include "components.hpp"
#include "game_parameters.hpp"
#include "level_system.hpp"
#include "renderer.hpp"

//короткие имена, только для этого файла
using gs = GameSystem;
using ls = LevelSystem;
using param = Parameters;

//статические поля структуры Scenes нужно определить в .cpp (сами сцены создаются в main)
std::shared_ptr<Scene> Scenes::menu;
std::shared_ptr<Scene> Scenes::game;

//поставить origin текста в его середину, тогда setPosition задаёт ЦЕНТР текста
static void centre_origin(sf::Text &t) {
  const sf::FloatRect bounds = t.getLocalBounds(); //прямоугольник, который занимает текст
  t.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
}

//================================================================ MenuScene

void MenuScene::update(const float &dt) {
  //Space -> переключаемся на сцену игры (просто меняем активную сцену)
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
    gs::set_active_scene(Scenes::game);
  }
  Scene::update(dt); //обновляем сущности меню (их нет, но так правильно)
  text.setString("Almost Pacman");
}

void MenuScene::render() {
  //не рисуем сами, а кладём тексты в очередь Renderer-а (передаём их АДРЕСА)
  Renderer::queue(&text);
  Renderer::queue(&hint);
  Scene::render();
}

void MenuScene::load() {
  //без шрифта SFML не может нарисовать текст (как в Pong)
  if (!font.loadFromFile(param::font)) {
    std::cerr << "Failed to load font " << param::font << std::endl;
  }
  text.setFont(font);
  text.setString("Almost Pacman");
  text.setCharacterSize(56);
  text.setFillColor(sf::Color::Yellow);
  centre_origin(text);
  text.setPosition(param::game_width / 2.f, param::game_height / 2.f - 40.f);

  hint.setFont(font);
  hint.setString("SPACE - play    TAB - menu    ESC - quit");
  hint.setCharacterSize(22);
  hint.setFillColor(sf::Color::White);
  centre_origin(hint);
  hint.setPosition(param::game_width / 2.f, param::game_height / 2.f + 40.f);
}

//================================================================ GameScene

void GameScene::load() {
  //1. Уровень. Стены красим в синий, как в настоящем Pacman
  ls::load_level(param::level, param::tile_size);
  ls::set_color(ls::WALL, sf::Color(33, 33, 222));

  //2. Pacman: пустая сущность + компонент-фигура (жёлтый круг) + компонент движения по клавишам
  {
    std::shared_ptr<Entity> player = std::make_shared<Entity>();
    std::shared_ptr<ShapeComponent> shape = player->add_component<ShapeComponent>();
    shape->set_shape<sf::CircleShape>(param::entity_size);
    shape->get_shape().setFillColor(sf::Color::Yellow);
    //origin в центр круга -> позиция сущности = центр круга
    shape->get_shape().setOrigin(sf::Vector2f(param::entity_size, param::entity_size));
    player->add_component<PlayerMovementComponent>();
    _entities.list.push_back(player); //pacman всегда первый в списке
    _player = player;
  }

  //3. Призраки: та же сущность + фигура, но вместо клавиш — компонент ИИ
  const sf::Color ghost_cols[]{{208, 62, 25},    // красный Blinky
                               {219, 133, 28},   // оранжевый Clyde
                               {70, 191, 238},   // голубой Inky
                               {234, 130, 229}}; // розовый Pinky
  for (int i = 0; i < param::ghost_count; i++) {
    std::shared_ptr<Entity> ghost = std::make_shared<Entity>();
    std::shared_ptr<ShapeComponent> shape = ghost->add_component<ShapeComponent>();
    shape->set_shape<sf::CircleShape>(param::entity_size);
    shape->get_shape().setFillColor(ghost_cols[i % 4]); //% 4 — если призраков больше, цвета повторяются
    shape->get_shape().setOrigin(sf::Vector2f(param::entity_size, param::entity_size));
    ghost->add_component<EnemyAIComponent>();
    _ghosts.push_back(ghost);
    _entities.list.push_back(ghost);
  }

  //4. Текст счёта внизу экрана
  if (!font.loadFromFile(param::font)) {
    std::cerr << "Failed to load font " << param::font << std::endl;
  }
  text.setFont(font);
  text.setCharacterSize(22);
  text.setFillColor(sf::Color::White);
  text.setPosition(10.f, ls::get_height() * ls::get_tile_size() + 3.f); //сразу под уровнем

  //5. Рекорд из файла (если файла ещё нет — рекорд 0)
  std::ifstream f(param::highscore_file);
  if (!(f >> _high_score)) {
    _high_score = 0;
  }

  //6. Всех на стартовые позиции и разложить еду
  respawn();
}

void GameScene::respawn() {
  const float half_tile = ls::get_tile_size() / 2.f;
  //get_start_position / get_tile_position дают ЛЕВЫЙ ВЕРХНИЙ угол клетки, а позиция
  //сущности — центр круга, поэтому прибавляем полклетки

  //pacman — на клетку старта 's', скорость сбрасываем на стартовую
  _player->set_position(ls::get_start_position() + sf::Vector2f(half_tile, half_tile));
  _player->get_compatible_components<ActorMovementComponent>()[0]->set_speed(param::player_speed);

  //призраки — на случайные клетки 'n' (дом призраков в центре)
  std::vector<sf::Vector2i> ghost_spawns = ls::find_tiles(ls::ENEMY);
  for (std::shared_ptr<Entity> &ghost : _ghosts) {
    ghost->set_position(ls::get_tile_position(ghost_spawns[std::rand() % ghost_spawns.size()]) +
                        sf::Vector2f(half_tile, half_tile));
    ghost->get_compatible_components<ActorMovementComponent>()[0]->set_speed(param::ghost_speed);
    ghost->get_compatible_components<EnemyAIComponent>()[0]->reset(); //заново выбрать направление
  }

  //убираем всю старую еду
  for (auto n : _nibbles.list) {
    n->set_for_delete();
    n.reset();
  }
  _nibbles.list.clear();
  //белые точки — на каждую пустую клетку
  std::vector<sf::Vector2i> nibbleLoc = ls::find_tiles(ls::EMPTY);
  for (const sf::Vector2i &nl : nibbleLoc) {
    _nibbles.list.push_back(_make_nibble(nl, false));
  }
  //синие точки — на каждую развилку
  nibbleLoc = ls::find_tiles(ls::WAYPOINT);
  for (const sf::Vector2i &nl : nibbleLoc) {
    _nibbles.list.push_back(_make_nibble(nl, true));
  }

  //счёт и таймер раунда — с нуля
  PickupComponent::take_player_points(); //выбросить очки, если что-то накопилось
  _score = 0;
  scoreClock.restart();
}

std::shared_ptr<Entity> GameScene::_make_nibble(const sf::Vector2i &pos, bool big) {
  //точка еды — тоже сущность: фигура (кружок) + компонент "меня можно съесть"
  std::shared_ptr<Entity> cherry = std::make_shared<Entity>();
  std::shared_ptr<ShapeComponent> s = cherry->add_component<ShapeComponent>();
  const float r = big ? param::big_nibble_size : param::nibble_size;
  s->set_shape<sf::CircleShape>(r);
  s->get_shape().setFillColor(big ? sf::Color(70, 120, 255) : sf::Color::White); //синяя или белая
  s->get_shape().setOrigin(sf::Vector2f(r, r));
  cherry->add_component<PickupComponent>(big);
  //в центр клетки
  const float half_tile = ls::get_tile_size() / 2.f;
  cherry->set_position(ls::get_tile_position(pos) + sf::Vector2f(half_tile, half_tile));
  return cherry;
}

void GameScene::_save_high_score() {
  if (_score > _high_score) {
    _high_score = _score;
    std::ofstream f(param::highscore_file); //перезаписываем файл
    f << _high_score;
  }
}

void GameScene::update(const float &dt) {
  //Tab -> назад в меню (игра встаёт на паузу: эта сцена просто перестаёт обновляться)
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab)) {
    gs::set_active_scene(Scenes::menu);
  }
  Scene::update(dt);    //pacman и призраки
  _nibbles.update(dt);  //еда: проверяет, не съели ли её

  //очки, которые pacman набрал в этом кадре
  _score += PickupComponent::take_player_points();

  //Столкновение с призраком. Лямбда-функция — маленькая функция прямо внутри кода:
  //[](аргументы) -> тип_результата { тело }. Считает расстояние между двумя точками.
  auto vect_distance = [](sf::Vector2f a, sf::Vector2f b) -> float {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
  };
  for (const std::shared_ptr<Entity> &ghost : _ghosts) {
    if (vect_distance(ghost->get_position(), _player->get_position()) < param::collision_distance) {
      _save_high_score(); //поймали — проверяем рекорд...
      respawn();          //...и начинаем раунд заново
      break;              //остальных призраков уже не проверяем
    }
  }

  //всю еду съели (pacman и призраки вместе) — новый раунд
  if (_nibbles.list.empty()) {
    _save_high_score();
    respawn();
  }

  //строка счёта
  text.setString("Score: " + std::to_string(_score) +
                 "    High score: " + std::to_string(_high_score) +
                 "    Time: " + std::to_string(static_cast<int>(scoreClock.getElapsedTime().asSeconds())) + "s");
}

void GameScene::render() {
  //Уровень рисуем сразу в окно (LevelSystem — наша старая библиотека, она не знает про Renderer).
  //Это происходит ДО Renderer::render(), поэтому уровень окажется под всем остальным.
  ls::render(Renderer::get_window());
  _nibbles.render();      //еда — в очередь первой, чтобы pacman и призраки рисовались поверх
  Scene::render();        //pacman и призраки
  Renderer::queue(&text); //счёт
}
