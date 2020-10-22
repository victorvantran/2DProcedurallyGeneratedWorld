#pragma once
#include <cstdint>
#include <string>

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

	// Screen
	struct Screen
	{
		static const int SCREEN_RESOLUTION_X = 1920;
		static const int SCREEN_RESOLUTION_Y = 1200;

		static const int PIXEL_SIZE = 1;
		static const int SCREEN_PIXEL_WIDTH = SCREEN_RESOLUTION_X / PIXEL_SIZE;
		static const int SCREEN_PIXEL_HEIGHT = SCREEN_RESOLUTION_Y / PIXEL_SIZE;
		static const int CELL_PIXEL_SIZE = 16 / PIXEL_SIZE;

		static const int SCREEN_CELL_WIDTH = SCREEN_PIXEL_WIDTH / CELL_PIXEL_SIZE;
		static const int SCREEN_CELL_HEIGHT = SCREEN_PIXEL_HEIGHT / CELL_PIXEL_SIZE;
	};


	// World
	struct World
	{
		static const int CHUNK_CELL_SIZE = 32; // chunk cell size ( n x n ) // [hardcode]
		static const int NUM_CELLS_PER_CHUNK = CHUNK_CELL_SIZE * CHUNK_CELL_SIZE;
		static const int CHUNK_RADIUS = 2;

		static const int MAX_SAVED_CHUNKS = 50; // Maximum number of chunks in queue to save before discarding
		static const int SPRITE_TILE_REFRESH_RATE = 300; // Every 5 minutes, refresh the tileDecals if it has not yet from condition variable notification
	};


	// WorldChunk
	struct WorldChunk
	{
		static const int SIZE = World::CHUNK_CELL_SIZE;


		static const int NUM_TILE_RENDERS = 341; // 4^0 + 4^1 + 4^2 + ... + 4^n ( for n = log( chunkSize ) = quadTreeMaxLevel ) // [hardcode]
		static const std::uint32_t TILE_RENDER_MIN_LEVEL = 0; // [ hard code ]
		static const std::uint32_t TILE_RENDER_MAX_LEVEL = 4; // [ hard code ]
		static const float TILE_RENDER_MIN_CELL_SIZE; // [ hard code ]



		static const std::uint32_t NUM_SHADOW_RENDERS = 1365; // // 4^0 + 4^1 + 4^2 + ... + 4^( n + 1 ) ( for n = log( chunkSize ) [ hardcode ] ( 4 shadows per tile )
		static const std::uint32_t SHADOW_RENDER_MIN_LEVEL = 0; // [ hard code ]
		static const std::uint32_t SHADOW_RENDER_MAX_LEVEL = 5; // [ hard code ]
		static const float SHADOW_RENDER_MIN_CELL_SIZE; // [ hard code ]

	};

	
	// QuadTree
	struct QuadTree
	{
		static const int MIN_LEVEL = 0;  // [hardcode]
		static const int MAX_LEVEL = 4; // log2( CHUNK_CELL_SIZE ) - 1  // [hardcode] [locked due to tile png not having 256x256]
	};

	
	// Camera
	struct Camera
	{
		static const int FOCAL_POINT_CELL_WIDTH = World::CHUNK_CELL_SIZE;
		static const int FOCAL_POINT_CELL_HEIGHT = World::CHUNK_CELL_SIZE;

		static const int VIEW_CELL_WIDTH = Screen::SCREEN_CELL_WIDTH;
		static const int VIEW_CELL_HEIGHT = Screen::SCREEN_CELL_HEIGHT;

		static const int ABSOLUTE_PIXEL_OFFSET_X = Screen::CELL_PIXEL_SIZE * ( Screen::SCREEN_CELL_WIDTH / 2 ) - Screen::CELL_PIXEL_SIZE * ( FOCAL_POINT_CELL_WIDTH / 2 );
		static const int ABSOLUTE_PIXEL_OFFSET_Y = Screen::CELL_PIXEL_SIZE * ( Screen::SCREEN_CELL_HEIGHT / 2 ) - Screen::CELL_PIXEL_SIZE * ( FOCAL_POINT_CELL_HEIGHT / 2 );

		static const std::uint32_t CONSOLIDATED_TILE_OFFSET = 128;
	};


	// MemoryManager
	struct MemoryManager
	{
		static const std::uint32_t WORLD_CHUNK_NUM_BYTES = ( World::CHUNK_CELL_SIZE * World::CHUNK_CELL_SIZE ) * sizeof( std::uint64_t ); // uint64_t is the datatype of tileId
		static const std::uint8_t NUM_BITS_PER_SEGMENT = 8;
	};


};


