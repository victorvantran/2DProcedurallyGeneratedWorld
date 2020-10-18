#pragma once

#include <cstdint>
#include <map>
#include <vector>
#include <mutex>
#include <string>
#include <atomic>

#include "sqlite/sqlite3.h"

#include "Tile.h"
#include "WorldChunk.h"
#include "WorldChunkRecall.h"

#include <iostream>
#include <fstream>

class WorldChunkMemory; // Forward Declaration


class World
{
private:

	// World
	const static int _chunkRadius = Settings::World::CHUNK_RADIUS;
	const static int _chunkCellSize = Settings::World::CHUNK_CELL_SIZE;

	WorldChunk* _worldChunks = nullptr;
	std::mutex _modifyWordChunksMutex;

	int _numChunkWidth;
	int _numChunkHeight;
	int _numWorldChunks;

	BoundingBox<float> _focalChunk;
	int _currFocalChunkIndexX;
	int _currFocalChunkIndexY;
	int _prevFocalChunkIndexX;
	int _prevFocalChunkIndexY;

	// Memory
	std::mutex _worldDatabaseMutex;

	std::vector<WorldChunkMemory*> _saveWorldChunks;
	std::atomic<bool> _runningSaveWorldGeography;
	std::thread _saveWorldGeographyThread;
	std::mutex _saveWordChunksMutex;
	std::uint8_t _MAX_SAVED_CHUNKS = 50; // [!] Settings

	std::atomic<bool> _runningLoadWorldGeography;
	std::thread _loadWorldGeographyThread;
	std::mutex _loadWordChunksMutex;
private:
	static unsigned char copyBits( unsigned char& destination, unsigned char copy, unsigned startIndex, unsigned char endIndex );
	static unsigned char copyBits( unsigned char& destination, unsigned dStartIndex, unsigned char dEndIndex, unsigned char copy, unsigned cStartIndex, unsigned char cEndIndex );

public:
	World();
	~World();

	void initializeDatabase();
	void initializeDelimits( const BoundingBox<float>& cameraView );
	void initializeWorldChunks();

	// Modify
	void insert( int x, int y, int width, int height, uint64_t id );
	void remove( int x, int y, int width, int height, uint64_t id );

	// Getters/Setters
	WorldChunk* getWorldChunks();
	WorldChunk& getWorldChunk( int x, int y );
	int getChunkRadius() const;
	int getNumWorldChunks() const;
	int getNumChunkWidth() const;
	int getNumChunkHeight() const;

	// Save/Load System
	void startWorldMemorySystem();
	void stopWorldMemorySystem();

	// Save
	void saveWorldGeographyTask();
	void saveWorldGeography();
	static unsigned char* createTilesBlob( Tile* tiles, std::vector<std::uint64_t>& palette );
	static std::uint64_t* createPaletteBlob( std::vector<std::uint64_t>& palette );
	void addMemory( WorldChunkMemory* worldChunkMemory );

	// Load
	void loadWorldGeographyTask();
	void loadWorldGeography( const BoundingBox<float>& cameraView );
	void updateFocalChunk( BoundingBox<float> focalPoint );
	std::vector<std::tuple<std::uint64_t, int, int>> delimitWorldChunks( const BoundingBox<float>& cameraView );
	void delimitWorldChunk( WorldChunk& worldChunk, int newIndexX, int newIndexY );
	static void loadTiles( WorldChunk& worldChunk, unsigned char* tilesData, std::uint16_t tilesNumBytes, std::uint64_t* paletteData, std::uint16_t numUniqueKeys );
};