#pragma once

//#include "olcPixelGameEngine.h"
#include "WorldChunk.h"
//#include "Camera.h"

class World
{
private:
	const static int _chunkRadius = 1; // [!] Assets
	const static int _chunkCellSize = 32; // Assets (log(2) 32 = 5) == QuadTree // CellDomain // [!] Assets
	WorldChunk _worldChunks[( 1 + 2 * ( _chunkRadius ) ) * ( 1 + 2 * ( _chunkRadius ) )]; // [!] Assets

	int _numChunkWidth;
	int _numChunkHeight;
	int _numWorldChunks;
public:
	World();
	~World();

	void updateWorldChunks( const BoundingBox<float>& cameraView ); // Camera locality

	WorldChunk* getWorldChunks();
	WorldChunk& getWorldChunk( int x, int y );

	int getChunkRadius() const;
	int getNumWorldChunks() const;
	int getNumChunkWidth() const;
	int getNumChunkHeight() const;
};
