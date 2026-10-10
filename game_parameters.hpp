#pragma once //файл подключается только один раз, даже если его #include написали в нескольких местах

// Здесь лежат все "магические числа" игры, чтобы они не были разбросаны по коду.
// Все поля "static constexpr":
//  - constexpr : значение известно при компиляции, компилятор просто подставляет число
//  - static    : поле принадлежит самой структуре, объект создавать не нужно -> Parameters::game_width
// В других файлах пишем "using param = Parameters;" и дальше просто "param::game_width".
struct Parameters{
    //---------------------------------------------------------------- окно
    static constexpr unsigned int game_width = 800;  //ширина окна в пикселях
    static constexpr unsigned int game_height = 600; //высота окна в пикселях
    static constexpr float time_step = 0.01f;        //пауза в конце каждого кадра (секунды)

    //---------------------------------------------------------------- уровни
    static constexpr float tile_size = 100.f;                       //размер одной клетки на экране (px)
    static constexpr const char *maze_1 = "res/levels/maze_1.txt"; //первый лабиринт
    static constexpr const char *maze_2 = "res/levels/maze_2.txt"; //второй (последний) лабиринт
    static constexpr const char *font = "res/fonts/tuffy.ttf";     //шрифт для экрана победы
};
