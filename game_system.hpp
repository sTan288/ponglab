//game_system.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>
#include "entity.hpp"

// Scene (сцена) = один "экран" игры: лабиринт, экран победы, меню...
// По сути это список сущностей (Entity) + общая логика этого экрана.
// Сцена сама загружает то, что ей нужно (load), и освобождает это (unload).
// Класс абстрактный (есть чисто виртуальная load() = 0), поэтому создать
// просто Scene нельзя — только наследников: MazeScene, EndScene.
class Scene {
public:
  Scene() = default;

  //виртуальный деструктор: при удалении через указатель на Scene вызовется деструктор наследника
  virtual ~Scene() = default;
  //virtual = наследник может переопределить (override) эти функции
  virtual void update(const float &dt);          //обновляет все сущности сцены (dt = секунды с прошлого кадра)
  virtual void render(sf::RenderWindow &window); //рисует все сущности сцены
  virtual void load() = 0;                       //= 0 -> каждая сцена ОБЯЗАНА сама написать свою загрузку
  virtual void unload();                         //удаляет все сущности сцены
  //доступ к списку сущностей снаружи (возвращаем ссылку, чтобы не копировать вектор)
  std::vector<std::shared_ptr<Entity>> &get_entities(){return _entities;}

protected:
  //protected: видно в наследниках (MazeScene, EndScene), но не снаружи
  //shared_ptr — умный указатель, сам освобождает память, когда объект больше никому не нужен
  std::vector<std::shared_ptr<Entity>> _entities;
};

// GameSystem открывает окно и крутит игровой цикл.
// Он не знает, во что мы играем: каждый кадр он просто обновляет и рисует
// ту сцену, которая сейчас активна. Чтобы сменить экран — меняем активную сцену.
// Всё статическое, а конструктор удалён (= delete), поэтому объект GameSystem создать нельзя:
// пишем просто GameSystem::start(...).
class GameSystem{
public:
    GameSystem() = delete;

    //открывает окно и запускает игровой цикл (вся та логика, что раньше была в main)
    static void start(unsigned int width, unsigned int height,
                      const std::string& name, const float &time_step = 0.f);
    static void clean();  //выгружает активную сцену перед выходом
    static void reset();  //перезагружает активную сцену с нуля
    static void set_active_scene(const std::shared_ptr<Scene>& act_sc); //какую сцену показывать

private:
    //private: эти функции вызываются только изнутри GameSystem (из start)
    static void _init();
    static void _update(const float &dt);
    static void _render(sf::RenderWindow &window);
    static std::shared_ptr<Scene> _active_scene; //сцена, которая сейчас обновляется и рисуется
};
