#pragma once //insure that this header file is included only once and there will no multiple definition of the same thing

struct Parameters{
    //window
    static constexpr int game_width = 800;
    static constexpr int game_height = 600;
    static constexpr int sprite_size = 32;

    //invaders formation
    static constexpr int rows = 5;
    static constexpr int columns = 12;
    static constexpr float invader_spacing = 40.f; //distance between two invaders centres
    static constexpr float formation_top = 60.f; //y of the first row
    static constexpr float invader_speed = 30.f; //starting horizontal speed (px/s)
    static constexpr float invader_acc = 5.f; //speed added each time the formation hits a side
    static constexpr float invader_kill_acc = 2.f; //speed added each time an invader dies
    static constexpr float invader_drop = 24.f; //how far the formation drops at a side
    static constexpr float invader_fire_min = 0.5f; //minimum time between two invader shots (s)
    static constexpr float explosion_time = 0.5f; //how long the explosion sprite stays (s)

    //player
    static constexpr float player_speed = 250.f; //px/s
    static constexpr float player_fire_cooldown = 0.7f; //s
    static constexpr float game_over_delay = 1.5f; //time before the game restarts (s)

    //bullets
    static constexpr float bullet_speed = 300.f; //px/s
    static constexpr int bullet_pool_size = 256; //matches the range of an unsigned char
};
