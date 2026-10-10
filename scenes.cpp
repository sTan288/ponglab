//scenes.cpp
#include "scenes.hpp"
#include <iostream>
#include "game_parameters.hpp"
#include "level_system.hpp"
#include "player.hpp"

//короткие имена, только для этого файла
using ls = LevelSystem;
using gs = GameSystem;
using param = Parameters;

//статические поля структуры Scenes нужно определить в .cpp (сами сцены создаются в main)
std::shared_ptr<Scene> Scenes::maze;
std::shared_ptr<Scene> Scenes::end;

//---------------------------------------------------------------- MazeScene

void MazeScene::set_file_path(const std::string &file_path){
  _file_path = file_path;
}

void MazeScene::load(){
  //создаём игрока и кладём его в список сущностей сцены.
  //Он всегда первый -> дальше обращаемся к нему как _entities[0]
  std::shared_ptr<Entity> player = std::make_shared<Player>();
  _entities.push_back(player);
  reset();
}

void MazeScene::reset(){
  //загружаем лабиринт из файла
  ls::load_level(_file_path, param::tile_size);
  //для проверки печатаем уровень в консоль числами (0 пусто, 1 старт, 2 финиш, 3 стена)
  for (int y = 0; y < ls::get_height(); ++y) {
    for (int x = 0; x < ls::get_width(); ++x) {
      std::cout << ls::get_tile({x, y});
    }
    std::cout << std::endl;
  }
  //get_start_position() — левый верхний угол клетки старта, а позиция игрока — центр круга,
  //поэтому сдвигаем на половину клетки, чтобы круг стоял ровно в середине клетки
  const float half_tile = ls::get_tile_size() / 2.f;
  _entities[0]->set_position(ls::get_start_position() + sf::Vector2f(half_tile, half_tile));
}

void MazeScene::update(const float &dt){
  //Игрок дошёл до финиша? (смотрим, какая клетка под центром игрока)
  if (ls::get_tile_at(_entities[0]->get_position()) == ls::END) {
    if (_file_path == std::string(param::maze_1)) {
      //это был первый лабиринт -> переключаемся на второй и начинаем заново
      _file_path = param::maze_2;
      reset();
    } else {
      //это был последний лабиринт -> победа
      unload(); //выгружаем сцену лабиринта (удаляем игрока)
      gs::set_active_scene(Scenes::end); //делаем активной сцену победы
    }
    return; //в этом кадре больше ничего не делаем
  }
  Scene::update(dt); //обычное обновление: вызываем update() у всех сущностей (у игрока)
}

void MazeScene::render(sf::RenderWindow &window){
  ls::render(window);    //сначала уровень...
  Scene::render(window); //...потом игрок, чтобы он был нарисован поверх уровня
}

//---------------------------------------------------------------- EndScene

void EndScene::load(){
  //загружаем шрифт из файла (без шрифта SFML не может нарисовать текст)
  if (!font.loadFromFile(param::font)) {
    std::cerr << "Failed to load font " << param::font << std::endl;
  }
  win_text.setFont(font);
  win_text.setString("You escaped the mazes!");
  win_text.setCharacterSize(48);              //размер букв в пикселях
  win_text.setFillColor(sf::Color::White);
  //Ставим origin текста в его середину, тогда setPosition задаёт ЦЕНТР текста.
  //getLocalBounds() — прямоугольник, который занимает текст
  const sf::FloatRect bounds = win_text.getLocalBounds();
  win_text.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
  //центр текста = центр окна
  win_text.setPosition(param::game_width / 2.f, param::game_height / 2.f);
}

void EndScene::render(sf::RenderWindow &window){
  window.draw(win_text);
  Scene::render(window); //сущностей тут нет, но так сцена будет работать, если их добавить
}
