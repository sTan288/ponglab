//pacman_main.cpp
#include <cstdlib>
#include <ctime>
#include "game_parameters.hpp"
#include "game_system.hpp"
#include "scenes.hpp"

using param = Parameters;

// main() только "склеивает" всё вместе: создаёт сцены, выбирает первую и запускает движок.
// Вся логика живёт в движке (engine/), сценах и компонентах.
int main(){
  //0. "Перемешиваем" генератор случайных чисел текущим временем,
  //   иначе призраки при каждом запуске ходили бы одинаково
  std::srand(static_cast<unsigned>(std::time(nullptr)));

  //1. Создаём и загружаем обе сцены заранее.
  //   Если какую-то забыть — указатель будет пустым, и при переключении на неё игра упадёт.
  Scenes::menu = std::make_shared<MenuScene>();
  Scenes::menu->load();
  Scenes::game = std::make_shared<GameScene>();
  Scenes::game->load();

  //2. Начинаем с меню и запускаем игровой цикл (вернётся, когда окно закроют)
  GameSystem::set_active_scene(Scenes::menu);
  GameSystem::start(param::game_width,param::game_height,"pacman",param::time_step);

  //3. Освобождаем сцены, пока SFML ещё "жив" (шрифт и т.д. нельзя удалять после выхода из main)
  Scenes::menu.reset();
  Scenes::game.reset();
  return 0;
}
