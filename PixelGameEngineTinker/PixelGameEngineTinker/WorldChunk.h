#pragma once

#include <vector>
#include <set>
#include "Settings.h"
#include "QuadTree.h"
#include "Tile.h"
#include "TileRender.h"
#include "Lighting.h"

class WorldChunkMemory; // Forward Declaration

class WorldChunk
{
private:
	// Cell domain
	std::int64_t _chunkIndexX;
	std::int64_t _chunkIndexY; // position = chunkIndex * size
	static const std::uint16_t _size = Settings::WorldChunk::SIZE;
	static const std::uint16_t _numTileRenders = Settings::WorldChunk::NUM_TILE_RENDERS;

	QuadTree<TileRender> _tileRenders[WorldChunk::_numTileRenders];
	Tile _tiles[WorldChunk::_size * WorldChunk::_size];

	Lighting<long double> _lighting;
public:

	WorldChunk();
	~WorldChunk();

	WorldChunk( std::int64_t indexX, std::int64_t indexY );


	void construct();
	void wipeRender();
	void fill( uint64_t id );

	void insert( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height, uint64_t id );
	void remove( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height, uint64_t id );
	void insertTiles( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height, uint64_t id );
	void removeTiles( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height, uint64_t id );
	void insertTileRenders( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height, uint64_t id );
	void removeTileRenders( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height, uint64_t id );

	const std::map<uint64_t, unsigned short> createPalette() const;

	void delimit( std::int64_t indexX, std::int64_t indexY );
	void clear();


	std::int64_t getChunkIndexX() const;
	std::int64_t getChunkIndexY() const;
	std::int64_t getRelativeChunkIndexX( const BoundingBox<long double>& boundingBox ) const;
	std::int64_t getRelativeChunkIndexY( const BoundingBox<long double>& boundingBox ) const;
	std::int16_t getSize() const;
	std::int16_t getNumTileRenders() const;
	std::int64_t getPositionX() const;
	std::int64_t getPositionY() const;

	Tile* getTiles();
	// Tile getTile( int x, int y )
	QuadTree<TileRender>& getTileRendersRoot();
	std::vector<std::uint64_t> getPalette();
	WorldChunkMemory* createMemory();


	// Lighting
	Lighting<long double>& getLighting();
	Light* getLights();
};

