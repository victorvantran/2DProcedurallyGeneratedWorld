#pragma once

#include "WorldChunk.h"
#include "MemoryManager.h"

class World
{
private:
	const static int _chunkRadius = 1; // [!] Assets
	const static int _chunkCellSize = 32; // Assets (log(2) 32 = 5) == QuadTree // CellDomain // [!] Assets
	WorldChunk _worldChunks[( 1 + 2 * ( _chunkRadius ) ) * ( 1 + 2 * ( _chunkRadius ) )]; // [!] Assets

	std::map<std::tuple<int, int>, uint64_t> _worldMap;

	int _numChunkWidth;
	int _numChunkHeight;
	int _numWorldChunks;
public:
	World();
	~World();

	void saveWorldMap();
	void loadWorldMap();
	void viewWorldMap();

	void updateWorldMap( int indexX, int indexY );
	bool findWorldMap( int indexX, int indexY ) const;

	void saveWorldChunk();
	void loadWorldChunk();

	void updateWorldChunks( const BoundingBox<float>& cameraView ); // Camera locality
	bool inBounds( const BoundingBox<float>& cameraView, const WorldChunk& worldChunk );



	WorldChunk* getWorldChunks();
	WorldChunk& getWorldChunk( int x, int y );
	//const std::map<std::tuple<int, int>, uint64_t>& getWorldMap( int x, int y ) const;

	int getChunkRadius() const;
	int getNumWorldChunks() const;
	int getNumChunkWidth() const;
	int getNumChunkHeight() const;
};
