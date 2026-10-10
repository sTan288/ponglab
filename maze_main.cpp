//maze_main.cpp
#include "game_parameters.hpp"
#include "game_system.hpp"
#include "scenes.hpp"

using param = Parameters;

// main() теперь совсем короткий: создаём сцены, выбираем первую и запускаем GameSystem.
// Вся логика игры живёт в сценах и сущностях.
int main(){
  //1. Создаём сцену лабиринта и говорим ей, какой файл грузить первым.
  //   Scenes::maze хранится как указатель на Scene, а set_file_path есть только у MazeScene,
  //   поэтому приводим указатель к MazeScene через static_pointer_cast.
  Scenes::maze = std::make_shared<MazeScene>();
  std::static_pointer_cast<MazeScene>(Scenes::maze)->set_file_path(param::maze_1);
  Scenes::maze->load();
  //2. Создаём и загружаем экран победы заранее.
  //   Если забыть — указатель будет пустым, и при переключении на него игра упадёт.
  Scenes::end = std::make_shared<EndScene>();
  Scenes::end->load();

  //3. Начинаем с лабиринта и запускаем игровой цикл (вернётся, когда окно закроют)
  GameSystem::set_active_scene(Scenes::maze);
  GameSystem::start(param::game_width,param::game_height,"Tile Maze",param::time_step);

  //4. Освобождаем сцены, пока SFML ещё "жив" (шрифт и т.д. нельзя удалять после выхода из main)
  Scenes::maze.reset();
  Scenes::end.reset();
  return 0;
}
