//level_system.hpp
#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>
#include <vector>
#include <map>

// LevelSystem — наша переиспользуемая библиотека (её можно взять в следующие игры).
// Она умеет: загрузить уровень из текстового файла, хранить его и рисовать через SFML.
// Всё статическое, поэтому её можно вызвать откуда угодно: LevelSystem::load_level(...).
// Минус статического класса: от него нельзя наследоваться.
//
// Формат файла (одна буква = одна клетка):
//   'w' стена, 's' старт, 'e' финиш, ' ' пусто, '+' точка пути, 'n' враг.
// Каждая строка (и последняя тоже!) должна заканчиваться переводом строки.
//
// Термины:
//   координата сетки (grid)  — номер клетки: (столбец x, строка y), например (3,1)
//   координата экрана (screen) — пиксели в окне, например (350.f, 150.f)
class LevelSystem {
public:
 //все возможные типы клеток. enum — это просто именованные числа: EMPTY=0, START=1, END=2, WALL=3...
 enum Tile { EMPTY, START, END, WALL, ENEMY, WAYPOINT };

 //загрузить уровень из файла; tile_size — размер клетки в пикселях (по умолчанию 100)
 static void load_level(const std::string&,float tile_size=100.f);
 static void render(sf::RenderWindow &window);   //нарисовать весь уровень
 static sf::Color get_color(Tile t);             //каким цветом рисуется тип клетки
 static void set_color(Tile t, sf::Color c);     //поменять цвет типа клетки
 //Клетка по координате СЕТКИ (столбец, строка)
 static Tile get_tile(sf::Vector2i);
 //Координата на ЭКРАНЕ (левый верхний угол) клетки сетки
 static sf::Vector2f get_tile_position(sf::Vector2i);
 //Клетка, которая лежит под точкой ЭКРАНА
 static Tile get_tile_at(sf::Vector2f);
 static int get_height();       //сколько клеток в высоту
 static int get_width();        //сколько клеток в ширину
 static float get_tile_size();  //размер клетки в пикселях
 //Координата на экране левого верхнего угла клетки старта
 static sf::Vector2f get_start_position();
 //Все клетки сетки заданного типа (например все ENEMY — где появляются призраки)
 static std::vector<sf::Vector2i> find_tiles(Tile);

protected:
 //Уровень двумерный, но хранится в ОДНОМЕРНОМ массиве (строки идут друг за другом).
 //Индекс клетки (x, y) = y * _width + x
 static std::unique_ptr<Tile[]> _tiles; //сам массив клеток
 static int _width;  //сколько клеток в ширину
 static int _height; //сколько клеток в высоту
 static sf::Vector2f _offset; //сдвиг всего уровня на экране (сейчас 0,0 — рисуем от левого верхнего угла)
 static float _tile_size;     //размер одной клетки на экране (px)
 //map = словарь "ключ -> значение": тип клетки -> цвет, которым её рисовать
 static std::map<Tile, sf::Color> _colors;
 static sf::Vector2f _start_position; //где начинает игрок (левый верхний угол клетки 's')

 //прямоугольники SFML для каждой клетки (строим один раз при загрузке, потом только рисуем)
 static std::vector<std::unique_ptr<sf::RectangleShape>> _sprites;
 //создать массив _sprites по массиву _tiles
 static void build_sprites();

private:
 //конструктор и деструктор удалены: объект LevelSystem создать нельзя, только статические вызовы
 LevelSystem() = delete;
 ~LevelSystem() = delete;
};
