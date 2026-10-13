cmake_minimum_required(VERSION 3.16)
project(pacman CXX)

# Используем стандарт C++17 (нужен для std::make_unique, constexpr const char* и т.д.)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Компилятор Visual Studio должен читать исходники как UTF-8,
# иначе русские комментарии могут превратиться в "кракозябры" и дать предупреждения
if(MSVC)
  add_compile_options(/utf-8)
endif()

# SFML собираем статически (.lib внутри exe) — тогда не нужно таскать .dll рядом с игрой
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

#### Внешние зависимости ####
# Подключаем SFML из папки lib/SFML (git submodule)
add_subdirectory(lib/SFML)

#### Система уровней (наша библиотека) ####
# add_library вместо add_executable: это не программа, а библиотека без main().
# STATIC = компилируется один раз в tile_level.lib и потом просто вшивается в игру.
add_library(tile_level STATIC tile_level_loader/level_system.cpp)
# INTERFACE: всем, кто подключает tile_level, будет видна папка tile_level_loader
# (поэтому в игре можно писать просто #include "level_system.hpp")
target_include_directories(tile_level INTERFACE tile_level_loader)
# библиотека рисует уровень, значит ей нужна графика SFML
target_link_libraries(tile_level sfml-graphics)

#### Движок (наша библиотека) ####
# Всё "ядро" игры: игровой цикл и сцены (game_system), сущности/компоненты и их менеджер (ecm),
# система отрисовки (renderer). Собирается так же, как tile_level, — в engine.lib.
add_library(engine STATIC
    engine/game_system.cpp
    engine/ecm.cpp
    engine/renderer.cpp
)
# PUBLIC: папка engine видна и самому движку, и всем, кто его подключает
# (#include "game_system.hpp", "renderer.hpp"...)
target_include_directories(engine PUBLIC engine)
# PUBLIC: тот, кто подключил engine, автоматически получает и SFML (окно, графика)
target_link_libraries(engine PUBLIC sfml-graphics sfml-window sfml-system)

#### Игра Pacman ####
# Файлы самой игры: компоненты, сцены и main, всё остальное — в библиотеках
add_executable(pacman
    pacman_main.cpp
    components.cpp
    scenes.cpp
)

# При запуске из Visual Studio рабочая папка = папка с exe (там лежит res/)
set_target_properties(pacman
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY
    "$<TARGET_FILE_DIR:pacman>"
)

# После каждой сборки копируем папку res/ (уровень, шрифт) рядом с exe
add_custom_command(TARGET pacman POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${PROJECT_SOURCE_DIR}/res"
    "$<TARGET_FILE_DIR:pacman>/res"
)

# Линкуем игру с нашим движком и библиотекой уровней (SFML приходит вместе с engine)
target_link_libraries(pacman PRIVATE engine tile_level)
