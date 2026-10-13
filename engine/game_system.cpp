//game_system.cpp
#include "game_system.hpp"
#include "renderer.hpp"
#include <algorithm> //std::min

//Статические поля в .hpp только ОБЪЯВЛЕНЫ, а здесь их нужно один раз ОПРЕДЕЛИТЬ,
//иначе линкер выдаст ошибку "unresolved external symbol".
std::shared_ptr<Scene> GameSystem::_active_scene;

//---------------------------------------------------------------- Scene

//Сцена сама ничего не перебирает — она просто передаёт работу своему менеджеру сущностей
void Scene::update(const float &dt){
  _entities.update(dt);
}

void Scene::render(){
  _entities.render();
}

void Scene::unload(){
  _entities.list.clear(); //очищаем список, shared_ptr сами освобождают память сущностей
}

//---------------------------------------------------------------- GameSystem

void GameSystem::start(unsigned int width, unsigned int height,
                       const std::string& name, const float &time_step){
  //создаём окно нужного размера с заголовком name
  sf::RenderWindow window(sf::VideoMode(width, height), name);
  //отдаём окно Renderer-у: дальше рисует только он
  Renderer::initialise(window);
  _init();
  sf::Event event;
  sf::Clock clock; //часы: считают время между двумя кадрами
  //Игровой цикл: каждый кадр = события -> обновление -> отрисовка
  while(window.isOpen()){
    //dt = сколько секунд прошло с прошлого кадра (restart() ещё и сбрасывает часы)
    //Ограничиваем dt сверху (0.05 с = 20 FPS): если окно "подвисло" (например, его тащат мышкой),
    //dt был бы огромным и за один шаг персонажи перепрыгнули бы через стену.
    const float dt = std::min(clock.restart().asSeconds(), 0.05f);
    //1. события: нажали крестик окна -> выходим
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
        clean();
        return;
      }
    }
    window.clear();    //стираем прошлый кадр
    //sf::Keyboard::isKeyPressed читает клавиатуру ГЛОБАЛЬНО — даже если ты печатаешь в другом окне.
    //Поэтому клавиши и обновление игры работают только когда окно игры активно (в фокусе),
    //иначе игра стоит на паузе и Esc, нажатый в другой программе, её не закроет.
    if (window.hasFocus()) {
      //Escape закрывает игру
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        window.close();
      }
      _update(dt);     //2. двигаем всё
    }
    _render();         //3. собираем очередь и рисуем её (рисуем всегда, даже на паузе)
    sf::sleep(sf::seconds(time_step)); //небольшая пауза, чтобы не грузить процессор на 100%
    //показываем нарисованный кадр на экране (ждём Vsync)
    window.display();
  }
  window.close();
  clean();
}

void GameSystem::clean(){
  //выгружаем активную сцену, "отпускаем" указатель на неё и чистим очередь Renderer-а
  if (_active_scene) {
    _active_scene->unload();
  }
  _active_scene.reset();
  Renderer::shutdown();
}

void GameSystem::reset(){
  //перезапуск активной сцены: выгрузить и загрузить заново
  if (_active_scene) {
    _active_scene->unload();
    _active_scene->load();
  }
}

std::shared_ptr<Scene> GameSystem::get_active_scene(){
  return _active_scene;
}

void GameSystem::set_active_scene(const std::shared_ptr<Scene>& act_sc){
  //смена экрана игры — это просто замена одного указателя
  _active_scene = act_sc;
}

void GameSystem::_init(){
  //пока настраивать нечего: каждая сцена загружает своё сама в load()
}

void GameSystem::_update(const float &dt){
  //сначала логика активной сцены, потом обновление Renderer-а
  if (_active_scene) {
    _active_scene->update(dt);
  }
  Renderer::update(dt);
}

void GameSystem::_render(){
  //1. активная сцена кладёт в очередь всё, что нужно нарисовать
  if (_active_scene) {
    _active_scene->render();
  }
  //2. Renderer рисует всю очередь разом
  Renderer::render();
}
