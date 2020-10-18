#pragma once

#include <cstdint>
#include <map>
#include <vector>
#include <mutex>
#include <string>
#include <atomic>

#include "Tile.h"
#include "sqlite/sqlite3.h"


#include <iostream>
#include <fstream>

#include "WorldChunk.h"
#include "WorldChunkRecall.h"

class WorldChunkMemory; // Forward Declaration


class World
{
public:
	// Temp
	BoundingBox<float> _focalChunk;

private:

	// World
	const static int _chunkRadius = Settings::World::CHUNK_RADIUS;
	const static int _chunkCellSize = Settings::World::CHUNK_CELL_SIZE;

	WorldChunk* _worldChunks = nullptr;
	std::mutex _modifyWordChunksMutex;


	int _numChunkWidth;
	int _numChunkHeight;
	int _numWorldChunks;


	int _currFocalChunkIndexX;
	int _currFocalChunkIndexY;

	int _prevFocalChunkIndexX;
	int _prevFocalChunkIndexY;




	// Memory
	std::atomic<bool> _runningSaveWorldGeography;
	std::thread _saveWorldGeographyThread;

	std::atomic<bool> _runningLoadWorldGeography;
	std::thread _loadWorldGeographyThread;






	std::vector<WorldChunkMemory*> _saveWorldChunks;
	std::mutex _saveWordChunksMutex;

	std::map<std::tuple<int,int>, std::vector<std::tuple<int,int>>> _loadWorldChunks; // camera-index -> vector ( chunkIndex )
	std::mutex _loadWordChunksMutex;


	std::mutex _worldDatabaseMutex;


	std::uint8_t _MAX_SAVED_CHUNKS = 50;
private:
	
	static unsigned char copyBits( unsigned char& destination, unsigned char copy, unsigned startIndex, unsigned char endIndex );
	static unsigned char copyBits( unsigned char& destination, unsigned dStartIndex, unsigned char dEndIndex, unsigned char copy, unsigned cStartIndex, unsigned char cEndIndex );



public:
	World();
	~World();


	void initializeDatabase();

	void insert( int x, int y, int width, int height, uint64_t id );
	void remove( int x, int y, int width, int height, uint64_t id );


	void updateFocalChunk( BoundingBox<float> focalPoint );

	void initializeDelimits( const BoundingBox<float>& cameraView );
	void initializeWorldChunks();

	void delimitWorldChunk( WorldChunk& worldChunk, int newIndexX, int newIndexY );
	std::vector<std::tuple<std::uint64_t, int, int>> delimitWorldChunks( const BoundingBox<float>& cameraView ); // WorldChunkRecall


	WorldChunk* getWorldChunks();
	WorldChunk& getWorldChunk( int x, int y );


	int getChunkRadius() const;
	int getNumWorldChunks() const;
	int getNumChunkWidth() const;
	int getNumChunkHeight() const;




	void startWorldMemorySystem();
	void stopWorldMemorySystem();


	void addMemory( WorldChunkMemory* worldChunkMemory );
	

	void saveWorldGeographyTask();
	void saveWorldGeography();

	void loadWorldGeographyTask();
	void loadWorldGeography( const BoundingBox<float>& cameraView );


	static unsigned char* createTilesBlob( Tile* tiles, std::vector<std::uint64_t>& palette );
	static std::uint64_t* createPaletteBlob( std::vector<std::uint64_t>& palette );

	static void loadTiles( WorldChunk& worldChunk, unsigned char* tilesData, std::uint16_t tilesNumBytes, std::uint64_t* paletteData, std::uint16_t numUniqueKeys );








};