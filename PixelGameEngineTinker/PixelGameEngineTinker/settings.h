#pragma once

#include "olcPixelGameEngine.h"

namespace settings
{

	struct RESOLUTION
	{
		const static olc::vi2d SCREEN_DIMENSION;
		const static olc::vi2d PIXEL_SCALE;
	};

	struct DYNAMIC_OBJECT
	{
		const static float ONE_WAY_PLATFORM_THRESHOLD;
	};

	struct CHARACTER
	{
		/// Constant static floats need to be initialized in .cpp because compiler does not support in-line initilization of it
		const static float WALK_SPEED;
		const static float JUMP_SPEED;
		const static float GRAVITY;
		const static float TERMINAL_FALLING_SPEED;
		const static float MIN_JUMP_SPEED;

		const static olc::vf2d AABB_HALF_SIZE;
		const static olc::vf2d AABB_OFFSET;

		const static olc::vf2d SCALE;

		const static float ONE_WAY_PLATFORM_THRESHOLD;
	};


	struct ATLAS
	{
		const static olc::vi2d TILE_DIMENSION;
	};



	struct WORLD_CHUNK
	{
		const static olc::vi2d POSITION;
		const static olc::vi2d DIMENSION;
		const static int OCTAVE;
		const static float SCALING_BIAS;
	};



};	


