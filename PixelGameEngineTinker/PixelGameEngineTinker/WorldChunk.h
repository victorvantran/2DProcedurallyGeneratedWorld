#pragma once

#include <vector>
#include <set>
#include <map>
#include "Settings.h"
#include "QuadTree.h"
#include "Tile.h"
#include "TileRender.h"
#include "Light.h"
#include "LightRender.h"
#include "LightSource.h"


// Forward Declaration
class World;
class WorldChunkMemory;


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

	//BackgroundTile _tiles[WorldChunk::_size * WorldChunk::_size];
	//QuadTree<TileRender> _tileRenders[WorldChunk::_numTileRenders];

	Light _lights[WorldChunk::_size * WorldChunk::_size];

	QuadTree<LightRender> _lightRendersA[WorldChunk::_numTileRenders];
	QuadTree<LightRender> _lightRendersB[WorldChunk::_numTileRenders];
	QuadTree<LightRender>* _lightRenders = this->_lightRendersA;
	QuadTree<LightRender>* _prevLightRenders = this->_lightRendersB;

	std::map<std::uint16_t, LightSource> _lightSources;

	World* _world;

public:
	// Constructors
	WorldChunk();
	WorldChunk( std::uint16_t relChunkIndex, std::int64_t indexX, std::int64_t indexY, World* world );
	~WorldChunk();

	void setWorld( World* world );
	void construct();
	void delimit( std::int64_t indexX, std::int64_t indexY );
	void clear();


	// Getters
	std::uint16_t getRelativeChunkIndex() const;
	void setRelativeChunkIndex( std::uint16_t relChunkIndex );
	std::int64_t getChunkIndexX() const;
	std::int64_t getChunkIndexY() const;
	std::int64_t getRelativeChunkIndexX( const BoundingBox<long double>& boundingBox ) const;
	std::int64_t getRelativeChunkIndexY( const BoundingBox<long double>& boundingBox ) const;
	std::int16_t getSize() const;
	std::int16_t getNumTileRenders() const;
	std::int64_t getPositionX() const;
	std::int64_t getPositionY() const;
	BoundingBox<std::int64_t> getBounds() const;


	// Insertion
	typedef void ( WorldChunk::* insertFuncType )( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	typedef void ( WorldChunk::* removeFuncType )( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void insertTiles( TileIdentity tileId, TileType tileType, bool consolidatable, bool opaque, bool complementary, std::uint8_t borders, std::uint8_t tileBlobMapIndex,
		std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void insertTileRenders( const Tile& tile, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void insertTileRenders( TileIdentity tileId, bool consolidatable, std::uint8_t bordersDecalIndex, std::uint8_t tileBlobMapIndex,
		std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void insertTile( TileIdentity tileId, TileType tileType, bool consolidatable, bool opaque, bool complementary, std::uint8_t borders, std::uint8_t tileBlobMapIndex,
		std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void insertLightSourceTile( TileIdentity tileId, TileType tileType, bool consolidatable, bool opaque, bool complementary, std::uint8_t borders, std::uint8_t tileBlobMapIndex,
		std::int16_t r, std::int16_t g, std::int16_t b, std::int16_t a, std::int16_t radius,
		std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void insertLightSources( TileIdentity tileId, std::int16_t r, std::int16_t g, std::int16_t b, std::int16_t a, std::int16_t radius,
		std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );

	void insertVoid( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertWater( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertStone( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertDirt( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertSand( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertTorch( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertMossDirt( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertMossStone( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertMull( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertMor( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertLightClay( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertDarkClay( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertGravel( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertCharcoal( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertSnow( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertPermafrost( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertPodzol( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertBleachedSand( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertShale( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertIronOxide( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertAluminiumOxide( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertLaterite( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertAridsol( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertEntisol( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertSaltstone( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertQuartz( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertAlfisol( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertYellowClay( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertRedClay( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertCambisol( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertSilt( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertIronOre( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertAluminiumOre( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertMapleLog( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertCrimsonMapleLeaves( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertPumpkinMapleLeaves( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );
	void insertGambogeMapleLeaves( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height );

	static const insertFuncType insertMethods[( std::size_t )TileIdentity::count];
	void insert( TileIdentity tileId, std::uint8_t borders, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );


	// Removal
	void removeTiles( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeTileRenders( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeTile( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeLightSources( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeLightSourceTiles( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeVoid( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeWater( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeStone( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeDirt( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeSand( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeTorch( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeMossDirt( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeMossStone( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeMull( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeMor( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeLightClay( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeDarkClay( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeGravel( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeCharcoal( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeSnow( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removePermafrost( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removePodzol( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeBleachedSand( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeShale( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeIronOxide( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeAluminiumOxide( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeLaterite( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeAridsol( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeEntisol( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeSaltstone( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeQuartz( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeAlfisol( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeYellowClay( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeRedClay( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeCambisol( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeSilt( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeIronOre( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeAluminiumOre( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeMapleLog( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeCrimsonMapleLeaves( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removePumpkinMapleLeaves( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void removeGambogeMapleLeaves( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );

	static const removeFuncType removeMethods[( unsigned long long )TileIdentity::count];
	void remove( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );


	// Geography
	Tile* getTiles();
	void wipeTileRenders();
	QuadTree<TileRender>& getTileRendersRoot();


	// Lighting
	Light* getLights();
	Light* getLight( std::int64_t x, std::int64_t y );
	void resetLights();
	void wipeLightRenders();
	void blackenLights();
	void sunlitLights();

	std::map<std::uint16_t, LightSource>& getLightSources();
	void clearLightSources();

	void initializeLightRenders();
	QuadTree<LightRender>* getLightRenders();
	QuadTree<LightRender>* getPrevLightRenders();
	void insertLightRender( std::uint32_t corner0, std::uint32_t corner1, std::uint32_t corner2, std::uint32_t corner3, bool exist, std::int64_t x, std::int64_t y ); // [!] overhaul and model after insert
	void insertLightRenders( std::uint32_t corner0, std::uint32_t corner1, std::uint32_t corner2, std::uint32_t corner3, bool exist, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void swapLightRenders();


	// Memory Management
	const std::map<TileIdentity, unsigned short> createPalette() const;
	std::vector<TileIdentity> getPalette();
	WorldChunkMemory* createMemory();
};

