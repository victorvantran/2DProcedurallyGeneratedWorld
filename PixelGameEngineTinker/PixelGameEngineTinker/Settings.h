#pragma once
#include <cstdint>
#include <string>
#include "olcPixelGameEngine.h"


class Settings
{
private:
	Settings()
	{

	}
	~Settings()
	{
	}
public:
	/*
		Implicit Dependencies:
		[ CHUNK_CELL_SIZE, numTileRenders, maxLevel ]
		[ MemoryManager, sizeOf( tileId ) ]
	*/


	// Assets
	struct Assets
	{
		static const std::string SPRITES_FILE_PATH;
	};


	// Game
	struct Game
	{
		static constexpr float TIME_SCALE = 1.0f;
		static constexpr float TICK_RATE = 0.015625f; // ( 1.0f / 64.0f ); == 64Hz
	};

	// World
	struct World
	{
		static const std::uint16_t CELL_SIZE = 16;
		static const std::uint16_t CHUNK_CELL_SIZE = 32; // chunk cell size ( n x n ) // [hardcode]
		static const std::uint16_t NUM_CELLS_PER_CHUNK = CHUNK_CELL_SIZE * CHUNK_CELL_SIZE;
		static const std::uint16_t CHUNK_RADIUS = 3;

		static const std::uint16_t MAX_SAVED_CHUNKS = 50; // Maximum number of chunks in queue to save before discarding
		static const std::uint16_t SPRITE_TILE_REFRESH_RATE = 300; // Every 5 minutes, refresh the tileDecals if it has not yet from condition variable notification

		static const std::uint16_t NUM_WORLD_CHUNKS = ( 1 + CHUNK_RADIUS * 2 ) * ( 1 + CHUNK_RADIUS * 2 );

		static constexpr float GRAVITY = -98.0f;
		static constexpr float TERMINAL_VELOCITY = -2000.0f;
		
		static constexpr std::uint32_t SECONDS_PER_DAY = 86400;
		static constexpr std::uint16_t DAYS_PER_YEAR = 365; // ignore leap years

		static constexpr float SOLAR_TIME_SCALE = 72.0f;
	};


	// Screen
	struct Screen
	{
		static const std::uint16_t SCREEN_RESOLUTION_X = 1920;
		static const std::uint16_t SCREEN_RESOLUTION_Y = 1080;

		static const std::uint16_t PIXEL_SIZE = 1;
		static const std::uint16_t SCREEN_PIXEL_WIDTH = SCREEN_RESOLUTION_X / PIXEL_SIZE;
		static const std::uint16_t SCREEN_PIXEL_HEIGHT = SCREEN_RESOLUTION_Y / PIXEL_SIZE;
		static const std::uint16_t CELL_PIXEL_SIZE = World::CELL_SIZE / PIXEL_SIZE;

		static const std::uint16_t SCREEN_CELL_WIDTH = SCREEN_PIXEL_WIDTH / CELL_PIXEL_SIZE;
		static const std::uint16_t SCREEN_CELL_HEIGHT = SCREEN_PIXEL_HEIGHT / CELL_PIXEL_SIZE;
	};


	// WorldChunk
	struct WorldChunk
	{
		static const std::uint16_t SIZE = World::CHUNK_CELL_SIZE;

		static const std::uint16_t NUM_TILE_RENDERS = 341; // 4^0 + 4^1 + 4^2 + ... + 4^n ( for n = log( chunkSize ) = quadTreeMaxLevel ) // [hardcode]
		static const std::uint16_t TILE_RENDER_MIN_LEVEL = 0; // [ hard code ]
		static const std::uint16_t TILE_RENDER_MAX_LEVEL = 4; // [ hard code ]
		static constexpr long double TILE_RENDER_MIN_CELL_SIZE = 1.0f; // [ hard code ]

	};


	// QuadTree
	struct QuadTree
	{
		static const std::uint16_t MIN_LEVEL = 0;  // [hardcode]
		static const std::uint16_t MAX_LEVEL = 4; // log2( CHUNK_CELL_SIZE ) - 1  // [hardcode] [locked due to tile png not having 256x256]
	};


	// Camera
	struct Camera
	{
		static const std::uint16_t FOCAL_POINT_CELL_WIDTH = World::CHUNK_CELL_SIZE;
		static const std::uint16_t FOCAL_POINT_CELL_HEIGHT = World::CHUNK_CELL_SIZE;

		static const std::uint16_t VIEW_CELL_WIDTH = Screen::SCREEN_CELL_WIDTH;
		static const std::uint16_t VIEW_CELL_HEIGHT = Screen::SCREEN_CELL_HEIGHT + 2;

		static const std::int32_t ABSOLUTE_PIXEL_OFFSET_X = Screen::CELL_PIXEL_SIZE * ( Screen::SCREEN_CELL_WIDTH / 2 ) - Screen::CELL_PIXEL_SIZE * ( FOCAL_POINT_CELL_WIDTH / 2 );
		static const std::int32_t ABSOLUTE_PIXEL_OFFSET_Y = Screen::CELL_PIXEL_SIZE * ( Screen::SCREEN_CELL_HEIGHT / 2 ) - Screen::CELL_PIXEL_SIZE * ( FOCAL_POINT_CELL_HEIGHT / 2 );

		static const std::uint16_t CONSOLIDATED_TILE_OFFSET = 128;
	};


	// MemoryManager
	struct MemoryManager
	{
		static const std::uint32_t WORLD_CHUNK_NUM_BYTES = ( World::CHUNK_CELL_SIZE * World::CHUNK_CELL_SIZE ) * sizeof( std::uint64_t ); // uint64_t is the datatype of tileId
		static const std::uint8_t NUM_BITS_PER_SEGMENT = 8;
	};


	// Render

	// ShadowRender
	struct ShadowRender
	{
		static const std::uint8_t MIN_INTENSITY = 0;
		static const std::uint8_t MAX_INTENSITY = 9;
	};


	// DynamicObject
	struct DynamicObject
	{
		static constexpr float ONE_WAY_PLATFORM_THRESHOLD = ( 2.0f / ( float )World::CELL_SIZE );// 0.125f;
	};


	// Player
	struct Player
	{

		struct Character
		{
			static constexpr long double DEFAULT_CENTER_X = 1040618.0; // 1040003.0;// 0.0;
			static constexpr long double DEFAULT_CENTER_Y = -800.0; // -500.0;
			static constexpr float DEFAULT_HALF_SIZE_X = 0.75f;
			static constexpr float DEFAULT_HALF_SIZE_Y = 1.5f;
			static constexpr float DEFAULT_SCALE_X = 1.0f;
			static constexpr float DEFAULT_SCALE_Y = 1.0f;

			static constexpr float DEFAULT_RUN_SPEED = 20.0f; //15.0f;// 75.0f;//75.0f; //10.0f;
			static constexpr float DEFAULT_JUMP_SPEED = 25.0f;//25.0f; // 100.0f; //50.0f; // 25.0f;
			static constexpr float DEFAULT_MIN_JUMP_RATIO = 0.5f;

			static constexpr float GRAB_LEDGE_START_Y = 0.0f;
			static constexpr float GRAB_LEDGE_END_Y = ( 4.0f / ( float )World::CELL_SIZE );
			static constexpr float GRAB_LEDGE_TILE_OFFSET_Y = -( 4.0f / ( float )World::CELL_SIZE );
			static constexpr std::int32_t GRAB_LEDGE_LET_GO_FRAMES = 3;
			static constexpr std::int32_t JUMP_FRAMES_THREASHOLD = 4;

			static constexpr std::uint8_t ANIMATOR_TICKS_PER_FRAME = 4;
		};
	};



	// QuadTreeCollision
	struct QuadTreeCollision
	{
		static constexpr std::int32_t GRID_WIDTH = 16;
		static constexpr std::int32_t GRID_HEIGHT = 16;
	};


	struct SpatialPartition
	{
		static constexpr std::int32_t WIDTH = 16;
		static constexpr std::int32_t HEIGHT = 16;

		static constexpr std::int64_t NUM_SPACE_COLS = ( Settings::WorldChunk::SIZE * ( 1 + Settings::World::CHUNK_RADIUS * 2 ) ) / ( Settings::SpatialPartition::WIDTH );
		static constexpr std::int64_t NUM_SPACE_ROWS = ( Settings::WorldChunk::SIZE * ( 1 + Settings::World::CHUNK_RADIUS * 2 ) ) / ( Settings::SpatialPartition::HEIGHT );
	};
};


