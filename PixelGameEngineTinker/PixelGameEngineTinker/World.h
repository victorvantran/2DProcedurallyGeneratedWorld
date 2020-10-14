#pragma once

#include "Settings.h"
#include "WorldChunk.h"
#include "MemoryManager.h"
#include <chrono>

class World
{
private:
	const static int _chunkRadius = Settings::World::CHUNK_RADIUS;
	const static int _chunkCellSize = Settings::World::CHUNK_CELL_SIZE;
	//WorldChunk _worldChunks[( 1 + 2 * ( _chunkRadius ) ) * ( 1 + 2 * ( _chunkRadius ) )]; // [!] Assets // Too much to put on stack, must be on heap
	WorldChunk* _worldChunks = nullptr;

	std::map<std::tuple<int, int>, uint64_t> _worldAtlas;

	int _numChunkWidth;
	int _numChunkHeight;
	int _numWorldChunks;

	int _prevCameraIndexX;
	int _prevCameraIndexY;
public:
	World();
	~World();

	void insert( int x, int y, int width, int height, uint64_t id );
	void remove( int x, int y, int width, int height, uint64_t id );

	void saveWorldAtlas();
	void loadWorldAtlas();
	void viewWorldAtlas();
	void updateWorldAtlas( int indexX, int indexY );

	bool findWorldMap( int indexX, int indexY ) const;


	void initializeDelimits( const BoundingBox<float>& cameraView );
	void initializeWorldChunks();

	void delimitWorldChunk( WorldChunk& worldChunk, int newIndexX, int newIndexY );
	void delimitWorldChunks( const BoundingBox<float>& cameraView ); // Camera locality


	void replaceWorldChunk( WorldChunk& worldChunk, int newIndexX, int newIndexY );
	void saveWorldChunk( WorldChunk& worldChunk );
	void loadWorldChunk( WorldChunk& worldChunk );



	bool inBounds( const BoundingBox<float>& cameraView, const WorldChunk& worldChunk );



	WorldChunk* getWorldChunks();
	WorldChunk& getWorldChunk( int x, int y );
	//const std::map<std::tuple<int, int>, uint64_t>& getWorldMap( int x, int y ) const;

	int getChunkRadius() const;
	int getNumWorldChunks() const;
	int getNumChunkWidth() const;
	int getNumChunkHeight() const;
};
