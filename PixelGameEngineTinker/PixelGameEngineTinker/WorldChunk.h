#pragma once

#include "QuadTree.h"
#include "Tile.h"
#include "TileRender.h"

class WorldChunk
{
private:
	// Cell domain
	int _chunkIndexX; 
	int _chunkIndexY; // position = chunkIndex * size
	int _size;

	QuadTree<Tile, TileRender> _quadTrees[341]; // [!] singleton
	Tile _tiles[32*32];
	//Tile _tiles[(2 << QuadTree<Tile,TileRender>::_MAX_LEVELS) * ( 2 << QuadTree<Tile, TileRender>::_MAX_LEVELS )]; // [!] singleton

	//QuadTree<Tile, TileRender> _quadTree;
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


	void construct();
	void reconstruct();

	void insert( int x, int y, int width, int height, uint64_t id );
	void remove( int x, int y, int width, int height, uint64_t id );

	void delimit( int indexX, int indexY );
	void clear();


	int getChunkIndexX() const;
	int getChunkIndexY() const;
	int getRelativeChunkIndexX( const BoundingBox<float>& boundingBox ) const;
	int getRelativeChunkIndexY( const BoundingBox<float>& boundingBox ) const;
	int getSize() const;
	int getPositionX() const;
	int getPositionY() const;

	QuadTree<Tile, TileRender>& getQuadTreeRoot();
};

