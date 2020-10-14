#pragma once

#include <vector>
#include <set>
#include "Settings.h"
#include "QuadTree.h"
#include "Tile.h"
#include "TileRender.h"

class WorldChunk
{
private:
	// Cell domain
	int _chunkIndexX; 
	int _chunkIndexY; // position = chunkIndex * size
	static const int _size = Settings::WorldChunk::SIZE;
	static const int _numTileRenders = Settings::WorldChunk::NUM_TILE_RENDERS;

	QuadTree<Tile, TileRender> _tileRenders[WorldChunk::_numTileRenders];
	Tile _tiles[WorldChunk::_size * WorldChunk::_size];

public:

	WorldChunk();
	~WorldChunk();

	WorldChunk( int indexX, int indexY );


	void construct();
	void wipeRender();
	void fill( uint64_t id );

	void insert( int x, int y, int width, int height, uint64_t id );
	void remove( int x, int y, int width, int height, uint64_t id );
	void insertTiles( int x, int y, int width, int height, uint64_t id );
	void removeTiles( int x, int y, int width, int height, uint64_t id );
	void insertTileRenders( int x, int y, int width, int height, uint64_t id );
	void removeTileRenders( int x, int y, int width, int height, uint64_t id );
	
	const std::map<uint64_t, unsigned short> createPalette() const;

	void delimit( int indexX, int indexY );
	void clear();


	int getChunkIndexX() const;
	int getChunkIndexY() const;
	int getRelativeChunkIndexX( const BoundingBox<float>& boundingBox ) const;
	int getRelativeChunkIndexY( const BoundingBox<float>& boundingBox ) const;
	int getSize() const;
	int getNumTileRenders() const;
	int getPositionX() const;
	int getPositionY() const;

	Tile* getTiles();
	// Tile getTile( int x, int y )
	QuadTree<Tile, TileRender>& getTileRendersRoot();

	std::vector<std::uint64_t> getPalette();
};

