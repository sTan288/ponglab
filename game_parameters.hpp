#pragma once //файл подключается только один раз, даже если его #include написали в нескольких местах

// Здесь лежат все "магические числа" игры, чтобы они не были разбросаны по коду.
// Все поля "static constexpr":
//  - constexpr : значение известно при компиляции, компилятор просто подставляет число
//  - static    : поле принадлежит самой структуре, объект создавать не нужно -> Parameters::game_width
// В других файлах пишем "using param = Parameters;" и дальше просто "param::game_width".
struct Parameters{
    //---------------------------------------------------------------- уровень
    static constexpr float tile_size = 25.f;                         //размер клетки (px)
    static constexpr const char *level = "res/levels/pacman.txt";   //файл уровня (28 x 29 клеток)

    //---------------------------------------------------------------- окно
    static constexpr unsigned int game_width = 700;  //28 клеток * 25 px = ширина уровня
    static constexpr unsigned int game_height = 760; //29 клеток * 25 px = 725 + полоса снизу для счёта
    static constexpr float time_step = 0.01f;        //пауза в конце каждого кадра (секунды)

    //---------------------------------------------------------------- ресурсы
    static constexpr const char *font = "res/fonts/tuffy.ttf"; //шрифт для текста
    static constexpr const char *highscore_file = "highscore.txt"; //тут хранится рекорд

    //---------------------------------------------------------------- pacman и призраки
    static constexpr float entity_size = 10.f;  //радиус pacman-а и призраков (px), коридор = 25 px
    static constexpr int ghost_count = 4;       //сколько призраков
    static constexpr float player_speed = 150.f; //стартовая скорость pacman-а (px/сек)
    static constexpr float ghost_speed = 100.f;  //стартовая скорость призраков (px/сек)
    static constexpr float max_speed = 300.f;    //быстрее нельзя, сколько ни ешь
    static constexpr float collision_distance = 2.f * entity_size; //центры ближе -> призрак поймал pacman-а

    //---------------------------------------------------------------- nibbles (еда)
    static constexpr float nibble_size = 3.f;        //радиус белой точки (px)
    static constexpr float big_nibble_size = 6.f;    //радиус большой синей точки (px)
    static constexpr float nibble_speed = 1.f;       //насколько белая точка ускоряет того, кто её съел
    static constexpr float big_nibble_speed = 5.f;   //насколько ускоряет синяя
    static constexpr int nibble_points = 10;         //очки за белую точку
    static constexpr int big_nibble_points = 50;     //очки за синюю
    static constexpr float pickup_distance = entity_size; //центр ближе этого -> точку съели
};
