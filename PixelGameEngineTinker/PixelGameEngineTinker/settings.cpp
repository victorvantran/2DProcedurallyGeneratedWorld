#include "olcPixelGameEngine.h"
#include "settings.h"


/// Resolution
const olc::vi2d settings::RESOLUTION::SCREEN_DIMENSION = olc::vi2d{ 768, 432 };
const olc::vi2d settings::RESOLUTION::PIXEL_SCALE = olc::vi2d{ 2, 2 };


/// Default character settings
const float settings::CHARACTER::WALK_SPEED = 100.0f;
const float settings::CHARACTER::JUMP_SPEED = 250.0f;
const float settings::CHARACTER::GRAVITY = 980.0f;
const float settings::CHARACTER::TERMINAL_FALLING_SPEED = 2000.0f;
const float settings::CHARACTER::MIN_JUMP_SPEED = 5.0f;

const olc::vf2d settings::CHARACTER::AABB_HALF_SIZE = olc::vf2d{ 8.0f, 16.0f };
const olc::vf2d settings::CHARACTER::AABB_OFFSET = olc::vf2d{ 0.0f, 0.0f };

const olc::vf2d settings::CHARACTER::SCALE = olc::vf2d{ 1.0f, 1.0f };	