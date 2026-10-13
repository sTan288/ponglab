//level_system.cpp
#include "level_system.hpp"
#include <fstream>  //чтение файлов (std::ifstream)
#include <iostream> //вывод в консоль (std::cout)

//Определяем все статические поля, объявленные в .hpp (обязательно один раз в .cpp)
std::unique_ptr<LevelSystem::Tile[]> LevelSystem::_tiles;
int LevelSystem::_width = 0;
int LevelSystem::_height = 0;
sf::Vector2f LevelSystem::_offset(0.0f, 0.0f);

float LevelSystem::_tile_size(100.f);
sf::Vector2f LevelSystem::_start_position(0.0f, 0.0f);
std::vector<std::unique_ptr<sf::RectangleShape>> LevelSystem::_sprites;

//Цвета по умолчанию: стены белые, финиш красный. Остальные типы игра может добавить через set_color.
//ВАЖНО: цвета задаём числами (R, G, B), а НЕ sf::Color::White / sf::Color::Red.
//sf::Color::White — тоже статическая переменная, но из другого файла (библиотеки SFML),
//а порядок создания статических переменных из разных файлов в C++ не определён
//("static initialization order fiasco"). Наш _colors мог создаться раньше, чем SFML заполнит
//sf::Color::White, и скопировать оттуда пустой прозрачный цвет (0,0,0,0) -> стены были невидимыми.
std::map<LevelSystem::Tile, sf::Color> LevelSystem::_colors{ {WALL, sf::Color(255, 255, 255)},
                                                             {END, sf::Color(255, 0, 0)} };

//простые геттеры
int LevelSystem::get_height(){return _height;}
int LevelSystem::get_width(){return _width;}
float LevelSystem::get_tile_size(){return _tile_size;}
sf::Vector2f LevelSystem::get_start_position(){return _start_position;}

sf::Color LevelSystem::get_color(LevelSystem::Tile t) {
  //Ищем цвет через find(), а НЕ через _colors[t]:
  //оператор [] САМ СОЗДАЁТ элемент, если его нет в словаре, а мы хотим просто проверить.
  auto it = _colors.find(t);
  if (it == _colors.end()) {             //end() = "не нашли"
    _colors[t] = sf::Color::Transparent; //нет цвета -> клетка прозрачная (не видна)
  }
  return _colors[t];
}

void LevelSystem::set_color(LevelSystem::Tile t, sf::Color c) {
  //Здесь [] как раз то, что нужно: добавит цвет, если типа нет, или заменит, если есть
  _colors[t] = c;
  //у уже созданных прямоугольников цвет старый -> если уровень загружен, пересоздаём их
  if (_tiles) {
    build_sprites();
  }
}

void LevelSystem::load_level(const std::string& path, float tile_size) {
  _tile_size = tile_size;
  int w = 0, h = 0; //ширина и высота уровня, которые мы посчитаем при чтении
  std::string buffer;

  //1. Читаем ВЕСЬ файл в строку buffer.
  //   binary — читаем байты "как есть", чтобы tellg() дал точный размер файла.
  std::ifstream f(path, std::ios::binary);
  if (f.good()) {
    f.seekg(0, std::ios::end);   //прыгаем в конец файла
    buffer.resize(f.tellg());    //tellg() = позиция = размер файла -> делаем строку такого размера
    f.seekg(0);                  //назад в начало
    f.read(&buffer[0], buffer.size()); //читаем всё в строку
    f.close();
  } else {
    //файл не открылся -> бросаем исключение (строку с ошибкой)
    throw std::string("Couldn't open level file: ") + path;
  }
  int x = 0; //номер текущего столбца в строке

  //2. Проходим по каждому символу и превращаем букву в тип клетки
  std::vector<Tile> temp_tiles; //временный список (размер заранее не знаем)
  for (size_t i = 0; i < buffer.size(); ++i) {
    const char c = buffer[i];
    switch (c) {
    case 'w':
      temp_tiles.push_back(WALL);
      break;
    case 's':
      temp_tiles.push_back(START);
      //запоминаем, где старт: клетка (x, h) -> пиксели на экране
      _start_position = get_tile_position({x,h});
      break;
    case 'e':
      temp_tiles.push_back(END);
      break;
    case ' ':
      temp_tiles.push_back(EMPTY);
      break;
    case '+':
      temp_tiles.push_back(WAYPOINT);
      break;
    case 'n':
      temp_tiles.push_back(ENEMY);
      break;
    case '\r': //в Windows конец строки это "\r\n" — символ '\r' просто пропускаем
      continue;
    case '\n':      //конец строки
      if (w == 0) { //если ширину ещё не записали (это первая строка)
        w = x;      //ширина = сколько клеток было в первой строке
      }
      x = 0; //новая строка начинается с нулевого столбца
      h++;   //строк стало на одну больше
      continue; //continue, а не break: не делаем x++ ниже, иначе следующая строка начнётся с 1
    default:
      std::cout << c << std::endl; //неизвестный символ — выводим его в консоль и пропускаем
      continue;
    }
    x++; //клетку добавили -> следующий столбец
  }
  //3. Проверка: клеток должно быть ровно ширина * высота.
  //   Если нет — файл кривой (например, в конце последней строки нет перевода строки).
  if (temp_tiles.size() != static_cast<size_t>(w * h)) {
    throw std::string("Can't parse level file") + path;
  }
  //4. Копируем клетки из временного vector в массив фиксированного размера
  //   (размер уровня больше не меняется, поэтому массив подходит лучше)
  _tiles = std::make_unique<Tile[]>(w * h);
  _width = w; //сохраняем размеры в статические поля класса
  _height = h;
  std::copy(temp_tiles.begin(), temp_tiles.end(), &_tiles[0]);
  std::cout << "Level " << path << " Loaded. " << w << "x" << h << std::endl;
  //5. Один раз строим прямоугольники для отрисовки
  build_sprites();
}

void LevelSystem::build_sprites() {
  _sprites.clear(); //убираем прямоугольники прошлого уровня
  //для каждой клетки сетки создаём квадрат нужного цвета в нужном месте
  for (int y = 0; y < LevelSystem::get_height(); ++y) {
    for (int x = 0; x < LevelSystem::get_width(); ++x) {
      std::unique_ptr<sf::RectangleShape> s = std::make_unique<sf::RectangleShape>();
      s->setPosition(get_tile_position({x, y}));           //где на экране
      s->setSize(sf::Vector2f(_tile_size, _tile_size));    //размер клетки
      s->setFillColor(get_color(get_tile({x, y})));        //цвет по типу клетки
      _sprites.push_back(std::move(s)); //unique_ptr можно только переместить (move), не скопировать
    }
  }
}

sf::Vector2f LevelSystem::get_tile_position(sf::Vector2i p) {
  //сетка -> экран: номер клетки * размер клетки (+ сдвиг уровня)
  //например клетка (3,1) при размере 100 -> (300, 100) пикселей
  return _offset + (sf::Vector2f(p.x, p.y) * _tile_size);
}

LevelSystem::Tile LevelSystem::get_tile(sf::Vector2i p) {
  //проверяем, что клетка внутри уровня (индексы от 0 до width-1 / height-1)
  if (p.x < 0 || p.y < 0 || p.x >= _width || p.y >= _height) {
    throw std::string("Tile out of range: (") + std::to_string(p.x) + "," + std::to_string(p.y) + ")";
  }
  //2D -> 1D: пропускаем y полных строк по _width клеток, потом сдвигаемся на x по строке.
  //Например уровень шириной 8, клетка (3,1): 1 * 8 + 3 = 11-й элемент массива
  return _tiles[(p.y * _width) + p.x];
}

LevelSystem::Tile LevelSystem::get_tile_at(sf::Vector2f v) {
  auto a = v - _offset; //убираем сдвиг уровня
  //отрицательные пиксели — точно вне уровня
  if (a.x < 0 || a.y < 0) {
    throw std::string("Tile out of range ");
  }
  //экран -> сетка: делим пиксели на размер клетки и отбрасываем дробную часть (Vector2i)
  //например (350, 150) / 100 = (3.5, 1.5) -> клетка (3, 1)
  return get_tile(sf::Vector2i(a / (_tile_size)));
}

std::vector<sf::Vector2i> LevelSystem::find_tiles(LevelSystem::Tile type) {
  std::vector<sf::Vector2i> ret;
  //обходим всю сетку и запоминаем координаты (столбец, строка) всех клеток нужного типа
  for (int y = 0; y < _height; ++y) {
    for (int x = 0; x < _width; ++x) {
      if (_tiles[(y * _width) + x] == type) { //та же формула 2D -> 1D, что и в get_tile
        ret.push_back({x, y});
      }
    }
  }
  return ret;
}

void LevelSystem::render(sf::RenderWindow &window) {
  //просто рисуем все заранее построенные прямоугольники
  for (size_t i = 0; i < _sprites.size(); ++i) {
    window.draw(*_sprites[i]);
  }
}
