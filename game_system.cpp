//game_system.cpp
#include "game_system.hpp"

//Статические поля в .hpp только ОБЪЯВЛЕНЫ, а здесь их нужно один раз ОПРЕДЕЛИТЬ,
//иначе линкер выдаст ошибку "unresolved external symbol".
std::shared_ptr<Scene> GameSystem::_active_scene;

//---------------------------------------------------------------- Scene

void Scene::update(const float &dt){
  //Полиморфизм: ent — указатель на Entity, но update() виртуальная,
  //поэтому для игрока вызовется Player::update()
  for(std::shared_ptr<Entity> &ent: _entities){
    ent->update(dt);
  }
}

void Scene::render(sf::RenderWindow &window){
  //то же самое для отрисовки: у каждой сущности своя render()
  for(std::shared_ptr<Entity> &ent: _entities){
    ent->render(window);
  }
}

void Scene::unload(){
  _entities.clear(); //очищаем список, shared_ptr сами освобождают память сущностей
}

//---------------------------------------------------------------- GameSystem

void GameSystem::start(unsigned int width, unsigned int height,
                       const std::string& name, const float &time_step){
  //создаём окно нужного размера с заголовком name
  sf::RenderWindow window(sf::VideoMode(width, height), name);
  _init();
  sf::Event event;
  sf::Clock clock; //часы: считают время между двумя кадрами
  //Игровой цикл: каждый кадр = события -> обновление -> отрисовка
  while(window.isOpen()){
    //dt = сколько секунд прошло с прошлого кадра (restart() ещё и сбрасывает часы)
    //скорости у нас в px/сек, поэтому "скорость * dt" = сколько пройти за этот кадр
    const float dt = clock.restart().asSeconds();
    //1. события: нажали крестик окна -> выходим
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
        clean();
        return;
      }
    }
    //Escape тоже закрывает игру
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
      window.close();
    }
    window.clear();    //стираем прошлый кадр
    _update(dt);       //2. двигаем всё
    _render(window);   //3. рисуем всё
    sf::sleep(sf::seconds(time_step)); //небольшая пауза, чтобы не грузить процессор на 100%
    //показываем нарисованный кадр на экране (ждём Vsync)
    window.display();
  }
  window.close();
  clean();
}

void GameSystem::clean(){
  //выгружаем активную сцену и "отпускаем" указатель на неё
  if (_active_scene) {
    _active_scene->unload();
  }
  _active_scene.reset();
}

void GameSystem::reset(){
  //перезапуск активной сцены: выгрузить и загрузить заново
  if (_active_scene) {
    _active_scene->unload();
    _active_scene->load();
  }
}

void GameSystem::set_active_scene(const std::shared_ptr<Scene>& act_sc){
  //смена экрана игры — это просто замена одного указателя
  _active_scene = act_sc;
}

void GameSystem::_init(){
  //пока настраивать нечего: каждая сцена загружает своё сама в load()
}

void GameSystem::_update(const float &dt){
  //if (_active_scene) — проверка, что сцена вообще задана (указатель не пустой)
  if (_active_scene) {
    _active_scene->update(dt);
  }
}

void GameSystem::_render(sf::RenderWindow &window){
  if (_active_scene) {
    _active_scene->render(window);
  }
}
