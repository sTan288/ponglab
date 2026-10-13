//scenes.hpp
#pragma once
#include "game_system.hpp"

// Все сцены игры лежат в этом файле.
// Переменные, которые раньше лежали бы глобально в main.cpp, теперь —
// приватные поля той сцены, которой они нужны.

// MenuScene — главное меню. Space -> начать игру.
class MenuScene : public Scene {
private:
  sf::Text text; //надпись "Almost Pacman"
  sf::Text hint; //подсказка "Press SPACE to play"
  sf::Font font; //шрифт надписей (должен жить, пока живут тексты, поэтому он поле класса)

public:
  MenuScene() = default;
  void update(const float &dt) override;
  void render() override;
  void load() override; //загружает шрифт и настраивает тексты
};

// GameScene — сама игра: уровень, pacman, призраки, еда, счёт.
// Tab -> назад в меню.
class GameScene : public Scene {
private:
  sf::Text text;        //строка со счётом внизу экрана
  sf::Font font;        //шрифт этой строки
  sf::Clock scoreClock; //сколько секунд длится текущий раунд
  void respawn();       //всё с начала: позиции, скорости, еда, счёт

  //Быстрый доступ к pacman-у и призракам (они же лежат и в _entities, но так удобнее,
  //например, для проверки столкновений — не надо искать их в общем списке)
  std::shared_ptr<Entity> _player;
  std::vector<std::shared_ptr<Entity>> _ghosts;

  //Еда — в отдельном менеджере: её много, и она не должна "есть" саму себя
  EntityManager _nibbles;
  //создать одну точку еды в клетке pos (big = большая синяя)
  std::shared_ptr<Entity> _make_nibble(const sf::Vector2i &pos, bool big);

  int _score = 0;      //очки в текущем раунде
  int _high_score = 0; //рекорд (хранится в файле, переживает перезапуск игры)
  void _save_high_score(); //обновить рекорд, если текущий счёт больше, и записать в файл

public:
  GameScene() = default;
  void update(const float &dt) override;
  void render() override;
  void load() override;
};

// Все сцены игры в одном месте, доступны из любого файла: Scenes::menu, Scenes::game.
// static -> поля принадлежат структуре, объект Scenes создавать не нужно.
struct Scenes{
  static std::shared_ptr<Scene> menu;
  static std::shared_ptr<Scene> game;
};
