#include "olcPixelGameEngine.h"
#include "settings.h"


/// Resolution
const olc::vi2d settings::RESOLUTION::SCREEN_DIMENSION = olc::vi2d{ 768, 432 };
const olc::vi2d settings::RESOLUTION::PIXEL_SCALE = olc::vi2d{ 2, 2 };


/// Default character settings
const float settings::CHARACTER::WALK_SPEED = 10.0f;
const float settings::CHARACTER::JUMP_SPEED = 25.0f;
const float settings::CHARACTER::GRAVITY = 98.0f;// 980.0f;
const float settings::CHARACTER::TERMINAL_FALLING_SPEED = 2000.0f;
const float settings::CHARACTER::MIN_JUMP_SPEED = 5.0f;

const olc::vf2d settings::CHARACTER::AABB_HALF_SIZE = olc::vf2d{ 1.0f, 2.0f };
const olc::vf2d settings::CHARACTER::AABB_OFFSET = olc::vf2d{ 0.0f, 0.0f };

const olc::vf2d settings::CHARACTER::SCALE = olc::vf2d{ 1.0f, 1.0f };	



/// Default atlas settings
const olc::vi2d settings::ATLAS::TILE_DIMENSION = olc::vi2d{ 8, 8 };



/// Default WorldChunk settings
const olc::vi2d settings::WORLD_CHUNK::POSITION = olc::vi2d{ 0, 0 };
const olc::vi2d settings::WORLD_CHUNK::DIMENSION = olc::vi2d{ 500, 500 }; // Chunks should ideally be square for optimality (but not necessary)
const int settings::WORLD_CHUNK::OCTAVE = std::max<int>(1, (int)std::log2(DIMENSION.x) - 1);
const float settings::WORLD_CHUNK::SCALING_BIAS = 0.2f;