#pragma once

#include <cstdint>
#include <map>
#include <set>
#include <vector>
#include <string>

#include <mutex>
#include <atomic>
#include <condition_variable>

#include "sqlite/sqlite3.h"

#include "Assets.h"
#include "Tile.h"
#include "WorldChunk.h"
#include "WorldChunkRecall.h"
#include "Atlas.h"
#include "Camera.h"

#include <iostream>
#include <fstream>

class WorldChunkMemory; // Forward Declaration

class World
{
private:

	// WorldChunks
	const static std::uint16_t _chunkRadius = Settings::World::CHUNK_RADIUS;
	const static std::uint16_t _chunkCellSize = Settings::World::CHUNK_CELL_SIZE;

	WorldChunk* _worldChunks = nullptr;
	WorldChunk* _worldChunkPointers[Settings::World::NUM_WORLD_CHUNKS] = { 0 };

	std::uint16_t _numChunkWidth;
	std::uint16_t _numChunkHeight;
	std::uint16_t _numWorldChunks;

	BoundingBox<long double> _focalChunk;
	std::int64_t _focalChunkIndexX;
	std::int64_t _focalChunkIndexY;

	// Memory
	std::mutex _worldDatabaseMutex;
	std::mutex _modifyWordChunksMutex; // [!] get rid of saveworldchunksmutex and load, and set it to modify/access

	std::vector<WorldChunkMemory*> _saveWorldChunks;
	std::atomic<bool> _runningSaveWorldGeography;
	std::thread _saveWorldGeographyThread;
	std::mutex _saveWorldChunksMutex;

	std::atomic<bool> _runningLoadWorldGeography;
	std::thread _loadWorldGeographyThread;
	std::mutex _loadWorldChunksMutex;

	Atlas _atlas;

	std::atomic<bool> _runningLoadSpriteTiles;
	std::thread _loadSpriteTilesThread;
	std::mutex _mutexModifyAtlas;
	std::condition_variable _condModifyAtlas;

	// Camera
	Camera* _camera = nullptr;


private:
	static unsigned char copyBits( unsigned char& destination, unsigned char copy, unsigned startIndex, unsigned char endIndex );
	static unsigned char copyBits( unsigned char& destination, unsigned dStartIndex, unsigned char dEndIndex, unsigned char copy, unsigned cStartIndex, unsigned char cEndIndex );

public:
	World();
	~World();

	// Initialization
	void initializeCamera( Camera* camera );
	void initializeDatabase();
	void initializeDelimits( const BoundingBox<long double>& cameraView );
	void initializeWorldChunks();

	// Modify
	void insert( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height, uint64_t id );
	void remove( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height, uint64_t id );

	// Getters/Setters
	WorldChunk* getWorldChunks();
	WorldChunk& getWorldChunk( std::int64_t x, std::int64_t y );
	std::uint16_t getChunkRadius() const;
	std::uint16_t getNumWorldChunks() const;
	std::uint16_t getNumChunkWidth() const;
	std::uint16_t getNumChunkHeight() const;
	const Tile* getTile( std::int64_t x, std::int64_t y ) const;
	const Light* getLight( std::int64_t x, std::int64_t y ) const;


	// Save/Load System
	void startWorldMemorySystem();
	void stopWorldMemorySystem();

	// Save
	void saveWorldGeographyTask();
	void saveWorldGeography();
	static unsigned char* createTilesBlob( const Tile* tiles, std::vector<std::uint64_t>& palette );
	static std::uint64_t* createPaletteBlob( std::vector<std::uint64_t>& palette );
	void addMemory( WorldChunkMemory* worldChunkMemory );

	// Load
	void loadWorldGeographyTask();
	void loadWorldGeography();
	void updateWorldChunkRelativity( const BoundingBox<long double>& focalPoint );
	void updateFocalChunk( const BoundingBox<long double>& focalPoint );
	void updateWorldChunkRelativeIndicies();
	void updateWorldChunkPointers();

	std::vector<std::tuple<std::uint64_t, std::int64_t, std::int64_t>> delimitWorldChunks( const BoundingBox<long double>& cameraView );
	void delimitWorldChunk( WorldChunk& worldChunk, std::int64_t newIndexX, std::int64_t newIndexY );
	void loadTiles( WorldChunk& worldChunk, unsigned char* tilesData, std::uint16_t tilesNumBytes, std::uint64_t* paletteData, std::uint16_t numUniqueKeys );

	// TileDecals
	void loadSpriteTilesTask();
	void loadSpriteTiles();
	void addSpriteTile( std::uint64_t tileId );

	Atlas& getAtlas();
	void updateDecals();



	// Debug
	void DEBUG_PRINT_TILE_SPRITES();
	void activateCursorLightSource( long double dX, long double dY, std::int64_t radius );
	void resetLighting();
	void updateLighting();
	


};