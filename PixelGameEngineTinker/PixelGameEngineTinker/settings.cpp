#include "olcPixelGameEngine.h"
#include "settings.h"


/// Resolution
const olc::vi2d settings::RESOLUTION::SCREEN_DIMENSION = olc::vi2d{ 768, 432 };
const olc::vi2d settings::RESOLUTION::PIXEL_SCALE = olc::vi2d{ 2, 2 };
const olc::vi2d settings::RESOLUTION::TILE_DIMENSION = olc::vi2d{ 8, 8 };


/// Default dynamic object settings
const float settings::DYNAMIC_OBJECT::ONE_WAY_PLATFORM_THRESHOLD = 0.125f; // percentage of how much a character must go through a tile before can pass through it


/// Default character settings
const float settings::CHARACTER::WALK_SPEED = 10.0f;
const float settings::CHARACTER::JUMP_SPEED = 25.0f;
const float settings::CHARACTER::GRAVITY = 98.0f;// 980.0f;
const float settings::CHARACTER::TERMINAL_FALLING_SPEED = 2000.0f;
const float settings::CHARACTER::MIN_JUMP_SPEED = 5.0f;

const float settings::CHARACTER::ONE_WAY_PLATFORM_THRESHOLD = 0.125f; // percentage of how much a character must go through a tile before can pass through it

const bool settings::CHARACTER::GRAB_LEDGE_ENABLED = false;
const float settings::CHARACTER::GRAB_LEDGE_START = 0.0f;
const float settings::CHARACTER::GRAB_LEDGE_END = 0.25f; // ( 8 * 0.25 = 2 pixels to grab )
const float settings::CHARACTER::GRAB_LEDGE_OFFSET = 0.5f; // y-offset when hanging on the ledge
const float settings::CHARACTER::GRAB_LEDGE_JUMP_SPEED = 30.0f;
const int settings::CHARACTER::GRAB_LEDGE_HOLD_FRAMES = 4; // [!] should be based on frames per second since if running at 1000fps, barely any threshold time (not useful not, but useful when locking fps)
															// It actually makes things look "jittery" when having a low value for now because not enough time to drop far enough as to not be able to grab ledge again
const int settings::CHARACTER::JUMP_FRAMES_THRESHOLD = 4; // [!] should be based on frames per second since if running at 1000fps, barely any threshold time (not useful not, but useful when locking fps)



const olc::vf2d settings::CHARACTER::AABB_HALF_SIZE = olc::vf2d{ 1.0f, 2.0f };
const olc::vf2d settings::CHARACTER::AABB_OFFSET = olc::vf2d{ 0.0f, 0.0f };

const olc::vf2d settings::CHARACTER::SCALE = olc::vf2d{ 1.0f, 1.0f };	



/// Default atlas settings
const olc::vi2d settings::ATLAS::TILE_DIMENSION = olc::vi2d{ 8, 8 };



/// Default WorldChunk settings
const olc::vi2d settings::WORLD_CHUNK::POSITION = olc::vi2d{ 0, 0 };
const olc::vi2d settings::WORLD_CHUNK::DIMENSION = olc::vi2d{ 50, 50 }; // Chunks should ideally be square for optimality (but not necessary)
const int settings::WORLD_CHUNK::OCTAVES = std::max<int>(1, (int)std::log2(DIMENSION.x) - 1); // [!] Limited by x value, so x <= y
const float settings::WORLD_CHUNK::SCALING_BIAS = 0.2f;


/// Default Forest settings
const float settings::FOREST::SCALING_BIAS = 0.2f;
const int settings::FOREST::OCTAVES = 1; // [!] Limited by x value, so x <= y