#pragma once
#include <cstdint>


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
	};


	// WorldChunk
	struct WorldChunk
	{
		static const int SIZE = World::CHUNK_CELL_SIZE;
		static const int NUM_TILE_RENDERS = 341; // 4^0 + 4^1 + 4^2 + ... + 4^n ( for n = log( chunkSize ) = quadTreeMaxLevel ) // [hardcode]
	};

	
	// QuadTree
	struct QuadTree
	{
		static const int MIN_LEVEL = 0;  // [hardcode]
		static const int MAX_LEVEL = 4; // log2( CHUNK_CELL_SIZE ) - 1  // [hardcode]
	};

	
	// Camera
	struct Camera
	{
		static const int ABSOLUTE_PIXEL_OFFSET_X = Screen::CELL_PIXEL_SIZE * ( Screen::SCREEN_CELL_WIDTH / 2 ) - Screen::CELL_PIXEL_SIZE * ( WorldChunk::SIZE / 2 );
		static const int ABSOLUTE_PIXEL_OFFSET_Y = Screen::CELL_PIXEL_SIZE * ( Screen::SCREEN_CELL_HEIGHT / 2 ) - Screen::CELL_PIXEL_SIZE * ( WorldChunk::SIZE / 2 );
	};


	// MemoryManager
	struct MemoryManager
	{
		static const std::uint32_t WORLD_CHUNK_NUM_BYTES = ( World::CHUNK_CELL_SIZE * World::CHUNK_CELL_SIZE ) * sizeof( std::uint64_t ); // uint64_t is the datatype of tileId
		static const std::uint8_t NUM_BITS_PER_SEGMENT = 8;
	};





};

