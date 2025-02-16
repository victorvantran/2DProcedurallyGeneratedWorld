#pragma once

#include <cstdint>
#include <set>
#include <vector>
#include <string>

#include <mutex>
#include <atomic>
#include <condition_variable>

#include <iostream>
#include <fstream>

#include "sqlite/sqlite3.h"

#include "Assets.h"
#include "Tile.h"
#include "WorldChunk.h"
#include "WorldChunkRecall.h"
#include "Atlas.h"
#include "Camera.h"


#include "LightSource.h"
#include "LightCastRow.h"
#include "LightCastQuadrant.h"


#include "ProceduralGenerationThread.h"
#include "TerraneanHeightMap.h"
#include "SubterraneanHeightMap.h"
#include "TemperatureMap.h"
#include "PrecipitationMap.h"
#include "BiomeSubstanceMap.h"
#include "CaveMap.h"

#include "Biome.h"
#include "BorealForest.h"
#include "SubtropicalDesert.h"
#include "TemperateGrassland.h"
#include "TemperateRainforest.h"
#include "TemperateSeasonalForest.h"
#include "TropicalRainforest.h"
#include "TropicalSeasonalForest.h"
#include "Tundra.h"
#include "Woodland.h"

#include "BackgroundRenderData.h"

// Forward Declarations
#include "WorldChunkMemory.h" // class WorldChunkMemory; 
#include "Player.h" // class Player;
#include "SpatialPartition.h" //class SpatialPartition;


class World
{
public:
	std::mutex _modifyWorldChunksMutex; // [!] get rid of saveworldchunksmutex and load, and set it to modify/access
private:

	// WorldChunks
	const static std::uint16_t _chunkRadius = Settings::World::CHUNK_RADIUS;
	const static std::uint16_t _chunkCellSize = Settings::World::CHUNK_CELL_SIZE;

	WorldChunk* _worldChunks = nullptr;
	WorldChunk* _worldChunkPointers[Settings::World::NUM_WORLD_CHUNKS] = { 0 };

	std::uint16_t _numChunkWidth;
	std::uint16_t _numChunkHeight;
	std::uint16_t _numWorldChunks;

	BoundingBox<long double> _focalChunk;
	std::int64_t _focalChunkIndexX;
	std::int64_t _focalChunkIndexY;

	// Lighting
	std::atomic<bool> _runningUpdateLighting;
	std::thread _updateLightingThread;
	std::mutex _mutexUpdateLighting;
	std::condition_variable _condUpdateLighting;

	// Memory
	std::mutex _worldDatabaseMutex;
	//std::mutex _modifyWorldChunksMutex; // [!] get rid of saveworldchunksmutex and load, and set it to modify/access

	std::vector<WorldChunkMemory*> _saveWorldChunks;
	std::atomic<bool> _runningSaveWorldGeography;
	std::thread _saveWorldGeographyThread;
	std::mutex _mutexSaveWorldChunks;

	std::atomic<bool> _runningLoadWorldGeography;
	std::thread _loadWorldGeographyThread;
	std::mutex _mutexLoadWorldChunks;
	std::condition_variable _condLoad;


	Atlas _atlas;

	std::atomic<bool> _runningLoadSpriteTiles;
	std::thread _loadSpriteTilesThread;
	std::mutex _mutexModifyAtlas;
	std::condition_variable _condModifyAtlas;


	// Camera
	Camera* _camera = nullptr;

	//
	SpatialPartition _spatialParition;


	// World Properties
	std::int64_t _seed;
	TerraneanHeightMap _terraneanHeightMap;
	SubterraneanHeightMap _subterraneanHeightMap;

	TemperatureMap _temperatureMap;
	PrecipitationMap _precipitationMap;
	BiomeSubstanceMap _biomeSubstanceMap;

	CaveMap _upperCaveMap;
	CaveMap _lowerCaveMap;

	BorealForest _borealForest;
	SubtropicalDesert _subtropicalDesert;
	TemperateGrassland _temperateGrassland;
	TemperateRainforest _temperateRainforest;
	TemperateSeasonalForest _temperateSeasonalForest;
	TropicalRainforest _tropicalRainforest;
	TropicalSeasonalForest _tropicalSeasonalForest;
	Tundra _tundra;
	Woodland _woodland;


	// Background
	olc::Decal* _dayDecal;
	olc::Pixel _dayTint;
	olc::Decal* _nightDecal;
	olc::Pixel _nightTint;
	olc::Decal* _sunDecal;
	olc::vf2d _sunPosition;
	olc::Decal* _landscapeDecal;
	olc::Pixel _landscapeTint;


	// Time
	std::uint64_t _tick;
	std::uint64_t _solarTimeScale = Settings::World::SOLAR_TIME_SCALE * 25;
	float _tickTimer;

	// Player
	Player* _player = nullptr;

	std::chrono::time_point<std::chrono::system_clock> _t1;
	std::chrono::time_point<std::chrono::system_clock> _t2;
private:
	static unsigned char copyBits( unsigned char& destination, unsigned char copy, unsigned startIndex, unsigned char endIndex );
	static unsigned char copyBits( unsigned char& destination, unsigned dStartIndex, unsigned char dEndIndex, unsigned char copy, unsigned cStartIndex, unsigned char cEndIndex );

public:
	World();
	World( std::int64_t seed,
		const TerraneanHeightMap& terraneanHeightMap,
		const SubterraneanHeightMap& subterraneanHeightMap,

		const TemperatureMap& temperatureMap,
		const PrecipitationMap& precipitationMap,
		const BiomeSubstanceMap& biomeSubstanceMap,

		const CaveMap& upperCaveMap,
		const CaveMap& lowerCaveMap,
		const BorealForest& borealForest,
		const SubtropicalDesert& subtropicalDesert,
		const TemperateGrassland& temperateGrassland,
		const TemperateRainforest& temperateRainforest,
		const TemperateSeasonalForest& temperateSeasonalForest,
		const TropicalRainforest& tropicalRainforest,
		const TropicalSeasonalForest& tropicalSeasonalForest,
		const Tundra& tundra,
		const Woodland& woodland,

		std::uint64_t tick,

		olc::Decal* dayDecal, // Encapsulate in a struct
		olc::Decal* nightDecal,
		olc::Decal* sunDecal,
		olc::Decal* landscapeDecal
	);

	~World();

	// Initialization
	void initializeCamera( Camera* camera );
	void initializeDatabase();
	void initializeDelimits( const BoundingBox<long double>& cameraView );
	void initializeWorldChunks();


	// Modify World
	void insert( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void remove( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height );
	void updateTileBorders( std::int64_t x, std::int64_t y );
	void refreshTileRender( Tile* tile, std::int64_t worldX, std::int64_t worldY );


	// World Chunks
	WorldChunk* getWorldChunks();
	WorldChunk& getWorldChunk( std::int64_t x, std::int64_t y );
	WorldChunk* getRelativeWorldChunks( std::size_t index );
	std::uint16_t getChunkRadius() const;
	std::uint16_t getNumWorldChunks() const;
	std::uint16_t getNumChunkWidth() const;
	std::uint16_t getNumChunkHeight() const;
	std::int16_t getRelativeChunkIndex( std::int64_t x, std::int64_t y ) const;
	static std::uint16_t getRelativeTileIndex( std::int64_t x, std::int64_t y );


	// Save/Load System
	void startWorldMemorySystem();
	void stopWorldMemorySystem();


	// Save
	void saveWorldGeographyTask();
	void saveWorldGeography();
	static unsigned char* createTilesBlob( const Tile* tiles, std::vector<TileIdentity>& palette );
	static std::uint64_t* createPaletteBlob( std::vector<TileIdentity>& palette );
	void addMemory( WorldChunkMemory* worldChunkMemory );


	// Load
	void loadWorldGeographyTask();
	void loadWorldGeography();
	void updateWorldChunkRelativity( const BoundingBox<long double>& focalPoint );
	void updateFocalChunk( const BoundingBox<long double>& focalPoint );
	void updateWorldChunkRelativeIndicies();
	void updateWorldChunkPointers();
	std::vector<std::tuple<std::uint64_t, std::int64_t, std::int64_t>> delimitWorldChunks( const BoundingBox<long double>& cameraView );
	void delimitWorldChunk( WorldChunk& worldChunk, std::int64_t newIndexX, std::int64_t newIndexY );
	void loadTiles( WorldChunk& worldChunk, unsigned char* tilesData, std::uint16_t tilesNumBytes, std::uint64_t* paletteData, std::uint16_t numUniqueKeys );


	// World Generation
	static long double normalizeHistogram( long double value );
	static long double biomeLine1( long double x );
	static long double biomeLine2( long double x );
	static long double biomeLine3( long double x );
	static long double biomeLine4( long double x );
	static long double biomeLine5( long double x );
	static long double biomeLine6( long double x );
	std::int64_t getSeed() const;
	const TerraneanHeightMap& getTerraneanHeightMap() const;
	const SubterraneanHeightMap& getSubterraneanHeightMap() const;
	const TemperatureMap& getTemperatureMap() const;
	const PrecipitationMap& getPrecipitationMap() const;
	const BiomeSubstanceMap& getBiomeSubstanceMap() const;
	const CaveMap& getUpperCaveMap() const;
	const CaveMap& getLowerCaveMap() const;
	const BorealForest& getBorealForest() const;
	const SubtropicalDesert& getSubtropicalDesert() const;
	const TemperateGrassland& getTemperateGrassland() const;
	const TemperateRainforest& getTemperateRainforest() const;
	const TemperateSeasonalForest& getTemperateSeasonalForest() const;
	const TropicalRainforest& getTropicalRainforest() const;
	const TropicalSeasonalForest& getTropicalSeasonalForest() const;
	const Tundra& getTundra() const;
	const Woodland& getWoodland() const;
	BiomeIdentity getBiomeIdentity( std::int64_t tileX, std::int64_t tileY ) const;
	TileIdentity getTerraneanSubstance( std::int64_t tileX, std::int64_t tileY ) const;
	FoliageIdentity getFoliage( std::int64_t tileX, BiomeIdentity biomeId, long double temperatureNormalizedValue, long double precipitationNormalizedValue );
	void addFoliage( TileIdentity* chunk,
		std::int64_t originX, std::int64_t originY, std::int64_t chunkOffsetX, std::int64_t chunkOffsetY, std::int64_t tileX, std::int64_t tileY,
		const TileIdentity* tiles, std::uint_fast8_t upBuffer, std::uint_fast8_t downBuffer, std::uint_fast8_t leftBuffer, std::uint_fast8_t rightBuffer );
	void procedurallyGenerateChunk( WorldChunk& worldChunk );


	// TileDecals
	void initializeSprites();
	void loadSpriteTilesTask();
	void loadSpriteTiles();
	void addSpriteTile( TileIdentity tileId );
	void addSpriteTiles( std::set<TileIdentity> tileIds );
	Atlas& getAtlas();
	void updateDecals();


	// Geography
	Tile* getTile( long double dX, long double dY ) const;
	Tile* getTile( std::int64_t x, std::int64_t y ) const;


	// Lighting
	Light* getLight( std::int64_t x, std::int64_t y ) const;
	const LightSource* getLightSource( std::int64_t x, std::int64_t y ) const;
	//void addLight( std::int64_t x, std::int64_t y, std::int16_t r, std::int16_t g, std::int16_t b, std::int16_t a );
	void addLight( std::int64_t x, std::int64_t y, const LightSource& lightSource, long double intensity );
	void resetLighting();
	bool isOpaque( const olc::v2d_generic<long double>& originPosition, const olc::v2d_generic<long double>& castPosition );
	bool isTransparent( const olc::v2d_generic<long double>& originPosition, const olc::v2d_generic<long double>& castPosition );
	void scanStatic( LightCastQuadrant<long double>& quadrant, LightCastRow& row, const olc::v2d_generic<long double> originPosition, const LightSource& lightSource );
	void revealStatic( LightCastQuadrant<long double>& quadrant, const olc::v2d_generic<long double>& tile, const olc::v2d_generic<long double>& castPosition,
		const olc::v2d_generic<long double>& originPosition, const LightSource& lightSource );
	void emitStaticLightSource( const LightSource& lightSource, std::int64_t x, std::int64_t y );
	void emitStaticLightSources();
	void activateCursorLightSource( long double dX, long double dY, std::int64_t radius );
	void emitDynamicLightSource( const LightSource& lightSource, long double x, long double y );
	void emitPlayerLightSource();
	void updateLightingTask();
	void updateLighting();


	// Background
	static olc::Pixel getDayTint( long double worldSecond );
	static olc::Pixel getNightTint( long double worldSecond );
	static olc::vf2d getSunPosition( float worldSecond );
	static olc::Pixel getLandscapeTint( long double worldSecond );
	const BackgroundRenderData& getRenderData() const;


	// Collision Detection
	SpatialPartition& getSpatialPartition();


	// Player
	void setPlayer( Player* player );


	// Update
	void update( olc::PixelGameEngine& pge );
	bool tick( float deltaTime );
	long double getAbsoluteSecond() const;
	long double getAbsoluteDay() const;
	long double getAbsoluteYear() const;
	std::uint8_t getNormalizedSecond() const;
	std::uint16_t getNormalizedDay() const;
	std::uint64_t getNormalizedYear() const;


	// Render
	Camera* getCamera();
	void calculateTileRenders();
	void calculateLightRenders();
	void renderBackground();
	void renderForeground();


	// Debug
	void DEBUG_PRINT_TILE_SPRITES();
	void printTime() const;
	std::string getTimeString() const;
	std::uint64_t getSolarTimeScale() const;
	void incrementSolarTimeScale();
	void decrementSolarTimeScale();
	void resetSolarTimeScale();
};


