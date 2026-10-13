//renderer.cpp
#include "renderer.hpp"
#include <queue>

using namespace std;
using namespace sf;

//static у глобальной переменной = она видна ТОЛЬКО в этом .cpp файле.
//Снаружи к ним не добраться, только через функции Renderer::... — это "внутреннее состояние" системы.
static queue<const Drawable *> sprites; //очередь: первым добавили -> первым нарисовали
static RenderWindow *rw = nullptr;      //указатель на окно (nullptr = окно ещё не задано)

//запоминаем АДРЕС окна (само окно живёт в GameSystem::start)
void Renderer::initialise(sf::RenderWindow &r) { rw = &r; }

sf::RenderWindow &Renderer::get_window() { return *rw; }

void Renderer::shutdown() {
  //выкидываем из очереди всё, что там осталось
  while (!sprites.empty())
    sprites.pop();
}

void Renderer::update(const float &) {} //пока нечего обновлять

void Renderer::render() {
  if (rw == nullptr) {
    throw("No render window set! "); //забыли вызвать initialise()
  }
  //берём объекты из очереди по одному, рисуем и удаляем из очереди
  while (!sprites.empty()) {
    rw->draw(*sprites.front()); //front() — первый в очереди, * — из указателя в сам объект
    sprites.pop();              //убрать его из очереди
  }
}

//В очереди хранятся только УКАЗАТЕЛИ, не копии объектов. Поэтому объект должен
//дожить до конца кадра (обычно это поле сцены или сущности — так и есть).
void Renderer::queue(const sf::Drawable *s) { sprites.push(s); }
