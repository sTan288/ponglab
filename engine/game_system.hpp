//game_system.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include "ecm.hpp" //Entity, Component, EntityManager

// Scene (сцена) = один "экран" игры: меню, сама игра...
// По сути это менеджер сущностей (EntityManager) + общая логика этого экрана.
// Сцена сама загружает то, что ей нужно (load), и освобождает это (unload).
// Класс абстрактный (есть чисто виртуальная load() = 0), поэтому создать
// просто Scene нельзя — только наследников: MenuScene, GameScene.
class Scene {
public:
  Scene() = default;

  //виртуальный деструктор: при удалении через указатель на Scene вызовется деструктор наследника
  virtual ~Scene() = default;
  //virtual = наследник может переопределить (override) эти функции
  virtual void update(const float &dt); //обновляет все сущности сцены (dt = секунды с прошлого кадра)
  virtual void render();                //кладёт все сущности сцены в очередь Renderer-а (окно не нужно!)
  virtual void load() = 0;              //= 0 -> каждая сцена ОБЯЗАНА сама написать свою загрузку
  virtual void unload();                //удаляет все сущности сцены
  //доступ к списку сущностей снаружи (возвращаем ссылку, чтобы не копировать вектор)
  std::vector<std::shared_ptr<Entity>> &get_entities(){return _entities.list;}

protected:
  //protected: видно в наследниках (MenuScene, GameScene), но не снаружи.
  //Вместо голого вектора — менеджер сущностей, у каждой сцены свой.
  EntityManager _entities;
};

// GameSystem открывает окно и крутит игровой цикл.
// Он не знает, во что мы играем: каждый кадр он просто обновляет активную сцену,
// просит её положить всё в очередь Renderer-а, а потом Renderer рисует очередь.
// Чтобы сменить экран — меняем активную сцену.
// Всё статическое, а конструктор удалён (= delete), поэтому объект GameSystem создать нельзя.
class GameSystem{
public:
    GameSystem() = delete;

    //открывает окно и запускает игровой цикл
    static void start(unsigned int width, unsigned int height,
                      const std::string& name, const float &time_step = 0.f);
    static void clean();  //выгружает активную сцену и Renderer перед выходом
    static void reset();  //перезагружает активную сцену с нуля
    static void set_active_scene(const std::shared_ptr<Scene>& act_sc); //какую сцену показывать
    static std::shared_ptr<Scene> get_active_scene(); //какая сцена сейчас активна (например, чтобы найти её сущности)

private:
    //private: эти функции вызываются только изнутри GameSystem (из start)
    static void _init();
    static void _update(const float &dt);
    static void _render(); //окно больше не нужно передавать: его знает Renderer
    static std::shared_ptr<Scene> _active_scene; //сцена, которая сейчас обновляется и рисуется
};
