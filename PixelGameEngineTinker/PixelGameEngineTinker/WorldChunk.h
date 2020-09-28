#pragma once

#include "QuadTree.h"
#include "Tile.h"
#include "TileConsolidated.h"

class WorldChunk
{
private:
	// Cell domain
	int _chunkIndexX; 
	int _chunkIndexY; // position = chunkIndex * size
	int _cellSize;

	QuadTree<Tile, TileConsolidated> _quadTree;

public:


	// Position
	// Size
	// 
	// Tile array

	// Array of pointers to neighboring worldChunks?


	// getPosition
	// getSize
	// Tile* getTiles
	// Tile getTile( int x, int y )

	
	WorldChunk();
	~WorldChunk();

	WorldChunk( int indexX, int indexY, int size );

	void update( int indexX, int indexY );


	int getChunkIndexX() const;
	int getChunkIndexY() const;
	int getRelativeChunkIndexX( const BoundingBox<float>& boundingBox ) const;
	int getRelativeChunkIndexY( const BoundingBox<float>& boundingBox ) const;
	int getSize() const;
	int getPositionX() const;
	int getPositionY() const;


};

