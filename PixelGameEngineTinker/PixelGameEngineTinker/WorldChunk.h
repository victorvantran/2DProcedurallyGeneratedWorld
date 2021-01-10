#pragma once

#include <vector>
#include <set>
#include "Settings.h"
#include "QuadTree.h"
#include "Tile.h"
#include "TileRender.h"
#include "Light.h"
#include "LightRender.h"
#include "LightSource.h"

class WorldChunkMemory; // Forward Declaration

class WorldChunk
{
private:
	// Cell domain
	std::uint16_t _relativeChunkIndex;
	std::int64_t _chunkIndexX;
	std::int64_t _chunkIndexY; // position = chunkIndex * size
	static const std::uint16_t _size = Settings::WorldChunk::SIZE;
	static const std::uint16_t _numTileRenders = Settings::WorldChunk::NUM_TILE_RENDERS;

	Tile _tiles[WorldChunk::_size * WorldChunk::_size];
	QuadTree<TileRender> _tileRenders[WorldChunk::_numTileRenders];

	Light _lights[WorldChunk::_size * WorldChunk::_size];
	QuadTree<LightRender> _lightRenders[WorldChunk::_numTileRenders];


	//LightSource _lightSources[WorldChunk::_size * WorldChunk::_size];
	std::map<std::uint16_t, LightSource> _lightSources;


public:

	WorldChunk();
	~WorldChunk();

	WorldChunk( std::uint16_t relChunkIndex, std::int64_t indexX, std::int64_t indexY );


	void construct();
	void fill( uint64_t id );

	std::uint16_t getRelativeChunkIndex() const;
	void setRelativeChunkIndex( std::uint16_t relChunkIndex );

	void insert( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height, uint64_t id );
	void remove( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height, uint64_t id );
	void insertTiles( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height, uint64_t id );
	void removeTiles( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height, uint64_t id );
	void insertTileRenders( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height, uint64_t id );
	void removeTileRenders( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height, uint64_t id );
	void wipeRender();

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

	const Tile* getTiles();
	// Tile getTile( int x, int y )
	QuadTree<TileRender>& getTileRendersRoot();
	std::vector<std::uint64_t> getPalette();
	WorldChunkMemory* createMemory();


	// Lighting
	Light* getLights();
	Light* getLight( std::int64_t x, std::int64_t y );
	const std::map<std::uint16_t, LightSource>& getLightSources();
	QuadTree<LightRender>* getLightRenders();

	void resetLights();
	void wipeLightRender();
	void blackenLights();
	void clearLightSources();

	void insertLightRender( std::uint32_t corner0, std::uint32_t corner1, std::uint32_t corner2, std::uint32_t corner3, bool exist, std::int64_t x, std::int64_t y ); // [!] overhaul and model after insert
	void insertLightRenders( std::uint32_t corner0, std::uint32_t corner1, std::uint32_t corner2, std::uint32_t corner3, bool exist, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
};

