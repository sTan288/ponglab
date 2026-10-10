//scenes.hpp
#pragma once
#include <string>
#include "game_system.hpp"

// Все сцены игры лежат в этом файле.

// MazeScene — сцена лабиринта: уровень (его рисует LevelSystem) + игрок.
// Когда игрок доходит до финиша — грузим следующий лабиринт или показываем экран победы.
// Наследуется от Scene и переопределяет её функции.
class MazeScene: public Scene{
public:
  MazeScene() = default;
  void update(const float &dt) override;
  void render(sf::RenderWindow &window) override;
  void load() override;                           //создаёт игрока и загружает лабиринт
  void reset();                                   //загружает файл _file_path и ставит игрока на старт
  void set_file_path(const std::string &file_path); //какой лабиринт загружать
private:
  std::string _file_path; //путь к .txt файлу текущего лабиринта
};

// EndScene — экран победы: просто надпись в центре окна.
class EndScene: public Scene{
public:
  EndScene() = default;
  void load() override;                           //загружает шрифт и настраивает текст
  void render(sf::RenderWindow &window) override; //рисует текст
private:
  sf::Text win_text; //сама надпись
  sf::Font font;     //шрифт надписи (должен жить, пока жив текст, поэтому он поле класса)
};

// Все сцены игры в одном месте, доступны из любого файла: Scenes::maze, Scenes::end.
// static -> поля принадлежат структуре, объект Scenes создавать не нужно.
struct Scenes{
  static std::shared_ptr<Scene> maze;
  static std::shared_ptr<Scene> end;
};
