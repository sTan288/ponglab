#pragma once //insure that this header file is included only once and there will no multiple definition of the same thing

// All the "magic numbers" of the game live here, so no hard coded values are spread in the code.
// Every member is "static constexpr":
//  - constexpr : the value is known at compile time, the compiler just pastes the number where it is used
//  - static    : it belongs to the struct itself, so we don't need an object -> Parameters::game_width
// Other files usually write "using param = Parameters;" and then just "param::game_width".
struct Parameters{
    //---------------------------------------------------------------- window
    static constexpr int game_width = 800;  //window width in pixels
    static constexpr int game_height = 600; //window height in pixels
    static constexpr int sprite_size = 32;  //every sprite in invaders_sheet.png is a 32x32 square

    //---------------------------------------------------------------- invaders formation
    static constexpr int rows = 5;                   //number of invader rows
    static constexpr int columns = 12;               //number of invaders in each row
    static constexpr float invader_spacing = 40.f;   //distance between two invaders centres (px)
    static constexpr float formation_top = 60.f;     //y of the first (top) row
    static constexpr float invader_speed = 30.f;     //starting horizontal speed (px/s)
    static constexpr float invader_acc = 5.f;        //speed added each time the formation hits a side
    static constexpr float invader_kill_acc = 2.f;   //speed added each time an invader dies
    static constexpr float invader_drop = 24.f;      //how far the formation drops down at a side (px)
    static constexpr float invader_fire_min = 0.5f;  //minimum time between two invader shots (s)
    static constexpr float explosion_time = 0.5f;    //how long the explosion sprite stays visible (s)

    //---------------------------------------------------------------- player
    static constexpr float player_speed = 250.f;        //horizontal speed (px/s)
    static constexpr float player_fire_cooldown = 0.7f; //time between two player shots (s)
    static constexpr float game_over_delay = 1.5f;      //pause after win/lose before the game restarts (s)

    //---------------------------------------------------------------- bullets
    static constexpr float bullet_speed = 300.f;  //vertical speed (px/s)
    static constexpr int bullet_pool_size = 256;  //matches the range of an unsigned char (0..255)
};
