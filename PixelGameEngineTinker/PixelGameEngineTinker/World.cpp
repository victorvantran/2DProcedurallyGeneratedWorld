#include "Settings.h"
#include "World.h"
#include "WorldChunkMemory.h"
#include "Player.h"






unsigned char World::copyBits( unsigned char& destination, unsigned char copy, unsigned startIndex, unsigned char endIndex )
{
	// Copy a subset of bits from one byte to another using the same positions within each byte

	endIndex = endIndex - 1;

	if ( startIndex < 0 || endIndex < 0 || startIndex > 7 || endIndex > 7 || endIndex < startIndex )
	{
		// throw
		return 0b00000000;
	}

	unsigned char mask = ( ( 0b00000001 << ( endIndex - startIndex + 1 ) ) - 1 );
	mask = ( mask << startIndex );
	unsigned char negateMask = ~mask;

	copy = copy & mask;

	destination = ( destination & negateMask );
	destination = ( destination | copy );

	return copy;
}


unsigned char World::copyBits( unsigned char& destination, unsigned dStartIndex, unsigned char dEndIndex, unsigned char copy, unsigned cStartIndex, unsigned char cEndIndex )
{
	// Copy a subset of bits from one byte to another using the differing positions within each byte

	dEndIndex = dEndIndex - 1;
	cEndIndex = cEndIndex - 1;

	if ( dStartIndex < 0 || dEndIndex < 0 || dStartIndex > 7 || dEndIndex > 7 || dEndIndex < dStartIndex || \
		cStartIndex < 0 || cEndIndex < 0 || cStartIndex > 7 || cEndIndex > 7 || cEndIndex < cStartIndex )
	{
		return 0b00000000; // throw
	}

	int range = dEndIndex - dStartIndex;
	if ( range != ( cEndIndex - cStartIndex ) )
	{
		return 0b00000000; // throw
	}

	unsigned char cMask = ( ( 0b00000001 << ( cEndIndex - cStartIndex + 1 ) ) - 1 );
	cMask = ( cMask << cStartIndex );

	unsigned char negateMask = ( ( 0b00000001 << ( dEndIndex - dStartIndex + 1 ) ) - 1 );
	negateMask = ( negateMask << dStartIndex );
	negateMask = ~negateMask;

	int offset = dStartIndex - cStartIndex;
	copy = ( offset > 0 ) ? ( ( copy & cMask ) << offset ) : ( ( copy & cMask ) >> -offset );

	destination = ( destination & negateMask );
	destination = ( destination | copy );

	return copy;
}



World::World()
	: _numChunkWidth( 1 + this->_chunkRadius * 2 ), _numChunkHeight( 1 + this->_chunkRadius * 2 ), _numWorldChunks( this->_numChunkWidth* this->_numChunkHeight ),
	_focalChunkIndexX( 0 ), _focalChunkIndexY( 0 ),
	_worldChunks( new WorldChunk[Settings::World::NUM_WORLD_CHUNKS] ),
	_atlas(),
	_camera( nullptr ),
	_spatialParition( this ),
	_seed( 0 )
{

}


World::World( std::int64_t seed,
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
	const Woodland& woodland )
	: _numChunkWidth( 1 + this->_chunkRadius * 2 ), _numChunkHeight( 1 + this->_chunkRadius * 2 ), _numWorldChunks( this->_numChunkWidth* this->_numChunkHeight ),
	_focalChunkIndexX( 0 ), _focalChunkIndexY( 0 ),
	_worldChunks( new WorldChunk[Settings::World::NUM_WORLD_CHUNKS] ),
	_atlas(),
	_camera( nullptr ),
	_spatialParition( this ),
	_seed( seed ),
	_terraneanHeightMap( terraneanHeightMap ),
	_subterraneanHeightMap( subterraneanHeightMap ),

	_temperatureMap( temperatureMap ),
	_precipitationMap( precipitationMap ),
	_biomeSubstanceMap( biomeSubstanceMap ),

	_upperCaveMap( upperCaveMap ),
	_lowerCaveMap( lowerCaveMap ),

	_borealForest( borealForest ),
	_subtropicalDesert( subtropicalDesert ),
	_temperateGrassland( temperateGrassland ),
	_temperateRainforest( temperateRainforest ),
	_temperateSeasonalForest( temperateSeasonalForest ),
	_tropicalRainforest( tropicalRainforest ),
	_tropicalSeasonalForest( tropicalSeasonalForest ),
	_tundra( tundra ),
	_woodland( woodland )
{
	for ( std::size_t i = 0; i < Settings::World::NUM_WORLD_CHUNKS; i++ )
	{
		this->_worldChunks[i].setWorld( this );
	}
}


World::~World()
{
	// Stop the daemond threads
	this->stopWorldMemorySystem();

	// Manual Save ( stop threads first )
	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		WorldChunkMemory* worldChunkMemory = this->_worldChunks[i].createMemory();
		if ( this->_saveWorldChunks.size() >= Settings::World::MAX_SAVED_CHUNKS )
		{
			delete worldChunkMemory;
			return;
		}

		this->_saveWorldChunks.push_back( worldChunkMemory );
	}
	this->saveWorldGeography();

	// Deallocate
	delete[] this->_worldChunks;
	this->_worldChunks = nullptr;

	// Clean array to pointers
	for ( int i = 0; i < Settings::World::NUM_WORLD_CHUNKS; i++ )
	{
		this->_worldChunkPointers[i] = nullptr;
	}
}


void World::initializeCamera( Camera* camera )
{
	this->_camera = camera;
}


void World::initializeDatabase()
{
	// Create a world file if one does not exist already.

	sqlite3* database = NULL;
	sqlite3_stmt* statement = NULL;
	const char* command = NULL;
	char* errorMessage = NULL;
	int rc;

	sqlite3_open( "./world.db", &database );

	// Create Table [ chunk_x, chunk_y ] -> [ tiles, palette ]
	command =
		"CREATE TABLE IF NOT EXISTS world_geography\n"\
		"(\n"\
		"	chunk_index_x INT,\n"\
		"	chunk_index_y INT,\n"\
		"	tiles BLOB,\n"\
		"	palette BLOB,\n"\
		"	PRIMARY KEY ( chunk_index_x, chunk_index_y )\n"\
		");";
	rc = sqlite3_exec(
		database,
		command,
		NULL,
		NULL,
		&errorMessage
	);
	if ( rc != SQLITE_OK )
	{
		sqlite3_close( database );
	}

	sqlite3_close( database );
	return;
}


void World::initializeDelimits( const BoundingBox<long double>& focalPoint )
{
	// Based on the initial camera's position, delimit the surrounding worldChunks

	// Load in camera
	int focalIndexX = focalPoint.getCenterX() >= 0 ? ( int )( focalPoint.getCenterX() / this->_chunkCellSize ) : ( int )( ( focalPoint.getCenterX() - this->_chunkCellSize ) / this->_chunkCellSize );
	int focalIndexY = focalPoint.getCenterY() >= 0 ? ( int )( focalPoint.getCenterY() / this->_chunkCellSize ) : ( int )( ( focalPoint.getCenterY() - this->_chunkCellSize ) / this->_chunkCellSize );

	for ( int x = 0; x < this->_numChunkWidth; x++ )
	{
		for ( int y = 0; y < this->_numChunkHeight; y++ )
		{
			int deltaIndexX = x - this->_chunkRadius;
			int deltaIndexY = y - this->_chunkRadius;

			int newChunkIndexX = focalIndexX + deltaIndexX;
			int newChunkIndexY = focalIndexY + deltaIndexY;

			this->delimitWorldChunk( this->_worldChunks[y * this->_numChunkWidth + x], newChunkIndexX, newChunkIndexY );
		}
	}

	this->_focalChunkIndexX = focalIndexX;
	this->_focalChunkIndexY = focalIndexY;

	this->updateWorldChunkRelativeIndicies();
	this->updateWorldChunkPointers();
	return;
}


void World::initializeWorldChunks()
{
	// Load in the tiles of the initial starting chunks
	std::lock_guard<std::mutex> lockLoad( this->_mutexLoadWorldChunks );
	std::lock_guard<std::mutex> lockDatabase( this->_worldDatabaseMutex );

	sqlite3* database = NULL;
	sqlite3_stmt* statement = NULL;
	const char* command = NULL;
	char* errorMessage = NULL;
	int rc;

	sqlite3_open( "./world.db", &database );

	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		this->_worldChunks[i].construct();

		int index = i;
		int chunkIndexX = this->_worldChunks[i].getChunkIndexX();
		int chunkIndexY = this->_worldChunks[i].getChunkIndexY();

		// Check if world chunk ( chunkIndexX, chunkIndexY ) exists
		command =
			"SELECT EXISTS( SELECT 1 FROM world_geography WHERE chunk_index_x = ?1 AND chunk_index_y = ?2 );";
		rc = sqlite3_prepare_v2( database, command, -1, &statement, NULL );
		rc = sqlite3_bind_int( statement, 1, chunkIndexX );
		rc = sqlite3_bind_int( statement, 2, chunkIndexY );
		rc = sqlite3_step( statement );


		// If it does not exists, return or throw error
		bool exists = sqlite3_column_int( statement, 0 );
		if ( !exists )
		{
			rc = sqlite3_step( statement );
			rc = sqlite3_finalize( statement );

			std::cout << "Chunk does has not existed beforehand. Procedural generation" << std::endl;
			this->procedurallyGenerate( this->_worldChunks[index] );
			//this->_worldChunks[index].resetLights();
			// [!] procedural generation here

			continue;
		}
		rc = sqlite3_step( statement ); // Extra step for null
		rc = sqlite3_finalize( statement );

		// Select tile_data and palette based on primary key ( chunkIndexX, chunkIndexY )
		command =
			"SELECT tiles, palette\n"
			"FROM world_geography\n"
			"WHERE chunk_index_x = ? AND chunk_index_y = ?;";
		rc = sqlite3_prepare_v2( database, command, -1, &statement, NULL );
		rc = sqlite3_bind_int( statement, 1, chunkIndexX );
		rc = sqlite3_bind_int( statement, 2, chunkIndexY );
		rc = sqlite3_step( statement );

		// Create tilesData used as reference when reconstructing the worldChunk's tiles
		std::uint16_t numBytesTiles = sqlite3_column_bytes( statement, 0 );
		unsigned char* tilesData = new unsigned char[numBytesTiles];
		std::memcpy( tilesData, sqlite3_column_blob( statement, 0 ), numBytesTiles );

		// Create paletteData used as reference when reconstructing the worldChunk's tiles
		std::uint16_t numBytesPalette = sqlite3_column_bytes( statement, 1 );
		std::uint16_t numUniqueKeys = numBytesPalette / sizeof( std::uint64_t );
		std::uint64_t* paletteData = new std::uint64_t[numUniqueKeys];
		std::memcpy( paletteData, sqlite3_column_blob( statement, 1 ), numBytesPalette );

		rc = sqlite3_step( statement ); // Extra step for null
		rc = sqlite3_finalize( statement );

		loadTiles( this->_worldChunks[i], tilesData, numBytesTiles, paletteData, numBytesPalette );

		delete[] tilesData;
		delete[] paletteData;

		//this->_worldChunks[i].resetLights();
	}

	sqlite3_close( database );

	// Allow the tileSprites to refresh
	this->_condModifyAtlas.notify_one();
	return;
}





















// Modification



void World::insert( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	// Insert a tile ( or tiles ) to proper worldChunk
	std::lock_guard<std::mutex> lockModify( this->_modifyWorldChunksMutex );
	///std::unique_lock<std::mutex> lockUpdateLighting( this->_mutexUpdateLighting );


	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		if ( BoundingBox<std::int64_t>( x, y, width, height ).intersects( this->_worldChunks[i].getTileRendersRoot().getBounds() ) )
		{
			this->_worldChunks[i].insert( tileId, 0, x, y, width, height ); // [~!]
		}
	}
	return;
}



void World::remove( TileIdentity id, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	// Remove a tile ( or tiles ) from the proper world chunk.
	std::lock_guard<std::mutex> lockModify( this->_modifyWorldChunksMutex );
	///std::unique_lock<std::mutex> lockUpdateLighting( this->_mutexUpdateLighting );


	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		if ( BoundingBox<std::int64_t>( x, y, width, height ).intersects( this->_worldChunks[i].getTileRendersRoot().getBounds() ) )
		{
			this->_worldChunks[i].remove( id, x, y, width, height );
		}
	}
	return;
}



void World::updateTileBorders( std::int64_t worldX, std::int64_t worldY )
{
	// Out of bounds check
	if ( !( worldX >= this->_worldChunkPointers[0]->getChunkIndexX() * Settings::WorldChunk::SIZE &&
		worldY >= this->_worldChunkPointers[0]->getChunkIndexY() * Settings::WorldChunk::SIZE &&
		worldX < this->_worldChunkPointers[0]->getChunkIndexX() * Settings::WorldChunk::SIZE + ( Settings::World::CHUNK_RADIUS * 2 + 1 ) * Settings::WorldChunk::SIZE &&
		worldY < this->_worldChunkPointers[0]->getChunkIndexY() * Settings::WorldChunk::SIZE + ( Settings::World::CHUNK_RADIUS * 2 + 1 ) * Settings::WorldChunk::SIZE )
		)
	{
		return;
	}

	// Quadrant edge merge
	Tile* cTile = this->getTile( worldX, worldY );
	if ( cTile == nullptr ) return;

	const Tile* neTile = this->getTile( worldX + 1, worldY - 1 );
	const Tile* nTile = this->getTile( worldX, worldY - 1 );
	const Tile* nwTile = this->getTile( worldX - 1, worldY - 1 );
	const Tile* eTile = this->getTile( worldX + 1, worldY );

	const Tile* wTile = this->getTile( worldX - 1, worldY );
	const Tile* seTile = this->getTile( worldX + 1, worldY + 1 );
	const Tile* sTile = this->getTile( worldX, worldY + 1 );
	const Tile* swTile = this->getTile( worldX - 1, worldY + 1 );

	std::uint8_t borders = 0;

	if ( cTile->getComplementary() )
	{
		borders = (
			( ( nwTile == nullptr || nwTile->getId() == TileIdentity::Void || !nwTile->getComplementary() ) ? 0 : 1 ) |
			( ( nTile == nullptr || nTile->getId() == TileIdentity::Void || !nTile->getComplementary() ) ? 0 : 1 ) << 1 |
			( ( neTile == nullptr || neTile->getId() == TileIdentity::Void || !neTile->getComplementary() ) ? 0 : 1 ) << 2 |
			( ( eTile == nullptr || eTile->getId() == TileIdentity::Void || !eTile->getComplementary() ) ? 0 : 1 ) << 3 |
			( ( seTile == nullptr || seTile->getId() == TileIdentity::Void || !seTile->getComplementary() ) ? 0 : 1 ) << 4 |
			( ( sTile == nullptr || sTile->getId() == TileIdentity::Void || !sTile->getComplementary() ) ? 0 : 1 ) << 5 |
			( ( swTile == nullptr || swTile->getId() == TileIdentity::Void || !swTile->getComplementary() ) ? 0 : 1 ) << 6 |
			( ( wTile == nullptr || wTile->getId() == TileIdentity::Void || !wTile->getComplementary() ) ? 0 : 1 << 7 )
			);
	}
	else
	{
		TileIdentity complementaryId = cTile->getId();
		borders = (
			( ( nwTile == nullptr || nwTile->getId() != complementaryId ) ? 0 : 1 ) |
			( ( nTile == nullptr || nTile->getId() != complementaryId ) ? 0 : 1 ) << 1 |
			( ( neTile == nullptr || neTile->getId() != complementaryId ) ? 0 : 1 ) << 2 |
			( ( eTile == nullptr || eTile->getId() != complementaryId ) ? 0 : 1 ) << 3 |
			( ( seTile == nullptr || seTile->getId() != complementaryId ) ? 0 : 1 ) << 4 |
			( ( sTile == nullptr || sTile->getId() != complementaryId ) ? 0 : 1 ) << 5 |
			( ( swTile == nullptr || swTile->getId() != complementaryId ) ? 0 : 1 ) << 6 |
			( ( wTile == nullptr || wTile->getId() != complementaryId ) ? 0 : 1 << 7 )
			);
	}


	/*
	if ( neTile == nullptr || nTile == nullptr || nwTile == nullptr || eTile == nullptr || wTile == nullptr || seTile == nullptr || sTile == nullptr || swTile == nullptr )
	{
		std::cout << "NULL FOUND" << std::endl;
	}
	*/
	/*
	if ( wTile == nullptr )
	{
		std::cout << "NULL: " << worldX - 1 << std::endl;
	}
	*/

	if ( worldX == 1040448 && worldY == -559 )
	{
		std::int64_t wWorldX = worldX - 1;


		std::int16_t numChunksWidth = 1 + World::_chunkRadius * 2;

		std::int64_t chunkIndexX = ( wWorldX >= 0 ) ? ( std::int64_t )( wWorldX / World::_chunkCellSize ) : ( std::int64_t )( ( ( wWorldX + 1 ) - World::_chunkCellSize ) / World::_chunkCellSize );
		std::int64_t chunkIndexY = ( worldY >= 0 ) ? ( std::int64_t )( worldY / World::_chunkCellSize ) : ( std::int64_t )( ( ( worldY + 1 ) - World::_chunkCellSize ) / World::_chunkCellSize );
		//std::int16_t relativeChunkIndex = ( ( chunkIndexY - this->_focalChunkIndexY ) + World::_chunkRadius ) * numChunksWidth + ( ( chunkIndexX - this->_focalChunkIndexX ) + World::_chunkRadius );

		//std::cout << chunkIndexX << std::endl;

		std::cout << ( ( chunkIndexX - this->_focalChunkIndexX ) + World::_chunkRadius ) << std::endl;

		//std::cout << int(this->getTile( worldX - 1, worldY ) == nullptr) << std::endl;
	}



	cTile->setBorders( borders );
	this->refreshTileRender( cTile, worldX, worldY );

	return;
}



void World::refreshTileRender( Tile* tile, std::int64_t worldX, std::int64_t worldY )
{
	// 

	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		if ( BoundingBox<std::int64_t>( worldX, worldY, 1, 1 ).intersects( this->_worldChunks[i].getTileRendersRoot().getBounds() ) )
		{
			this->_worldChunks[i].removeTileRenders( tile->getId(), worldX, worldY, 1, 1 );
			this->_worldChunks[i].insertTileRenders( *tile, worldX, worldY, 1, 1 );
			return;
		}
	}
	return;
}



WorldChunk* World::getWorldChunks()
{
	return this->_worldChunks;
}


WorldChunk& World::getWorldChunk( std::int64_t x, std::int64_t y )
{
	return this->_worldChunks[y * this->_numChunkWidth + x];
}


WorldChunk* World::getRelativeWorldChunks( std::size_t index )
{
	return this->_worldChunkPointers[index];
}


std::uint16_t World::getChunkRadius() const
{
	return this->_chunkRadius;
}


std::uint16_t World::getNumWorldChunks() const
{
	return this->_numWorldChunks;
}


std::uint16_t World::getNumChunkWidth() const
{
	return this->_numChunkWidth;
}


std::uint16_t World::getNumChunkHeight() const
{
	return this->_numChunkHeight;
}





// Memory System
void World::startWorldMemorySystem()
{
	// Daemon threads
	this->_saveWorldGeographyThread = std::thread( &World::saveWorldGeographyTask, this );
	this->_loadWorldGeographyThread = std::thread( &World::loadWorldGeographyTask, this );
	// this->_loadSpriteTilesThread = std::thread( &World::loadSpriteTilesTask, this );
	this->_updateGeographyThread = std::thread( &World::updateGeographyTask, this );
	this->_updateLightingThread = std::thread( &World::updateLightingTask, this );
	return;
}


void World::stopWorldMemorySystem()
{
	this->_runningSaveWorldGeography = false;
	this->_saveWorldGeographyThread.join();

	this->_runningLoadWorldGeography = false;
	this->_loadWorldGeographyThread.join();


	this->_condModifyAtlas.notify_all();
	this->_runningLoadSpriteTiles = false;
	// this->_loadSpriteTilesThread.join();



	this->_condRenderWorld.notify_all();
	this->_condUpdateGeography.notify_all();
	this->_condUpdateLighting.notify_all();

	this->_runningUpdateGeography = false;
	//this->_condRenderWorld.notify_one();
	this->_updateGeographyThread.join();

	this->_runningUpdateLighting = false;
	///this->_condRenderWorld.notify_one();
	this->_updateLightingThread.join();
	return;
}


void World::saveWorldGeographyTask()
{
	this->_runningSaveWorldGeography = true;
	while ( this->_runningSaveWorldGeography )
	{
		this->saveWorldGeography();
	}

	return;
}


void World::saveWorldGeography()
{
	// Saves all the worldChunks that have been built up in the queue into the SQLite database
	std::lock_guard<std::mutex> lockSave( this->_mutexSaveWorldChunks );

	if ( this->_saveWorldChunks.empty() )
	{
		return;
	}

	/*
	long accumulator = 0;
	long double* a = new long double[2048 * 2048];
	for ( int i = 0; i < 2048 * 2048; i++)
	{
		a[i] = std::rand() / RAND_MAX;
		if ( (int)(a[i] * 256) % 2 == 0 )
		{
			accumulator += a[i];
		}
	}


	delete[] a;
	std::cout << accumulator << std::endl;
	*/

	//std::lock_guard<std::mutex> lockDatabase( _worldDatabaseMutex );
	std::unique_lock<std::mutex> lockDatabase( this->_worldDatabaseMutex );

	sqlite3* database = NULL;
	sqlite3_stmt* statement = NULL;
	const char* command = NULL;
	char* errorMessage = NULL;
	int rc;
	command =
		"INSERT INTO world_geography( chunk_index_x, chunk_index_y, tiles, palette )\n"
		"VALUES ( ?1, ?2, ?3, ?4 )\n"
		"ON CONFLICT( chunk_index_x, chunk_index_y ) DO UPDATE SET tiles = ?3, palette = ?4;";

	sqlite3_open( "./world.db", &database );

	// Save all blobs/palettes in one go
	for ( int i = 0; i < this->_saveWorldChunks.size(); i++ )
	{
		// std::cout << "(" << this->_saveWorldChunks[i]->getChunkIndexX() << "," << this->_saveWorldChunks[i]->getChunkIndexY() << ")" << std::endl; // [~!]
		// save

		WorldChunkMemory* worldChunkMemory = this->_saveWorldChunks[i];

		std::int64_t chunkIndexX = worldChunkMemory->getChunkIndexX();
		std::int64_t chunkIndexY = worldChunkMemory->getChunkIndexY();

		unsigned char* tilesBlob = worldChunkMemory->getTilesBlob();
		std::uint64_t* paletteBlob = worldChunkMemory->getPaletteBlob();

		std::uint16_t tilesBlobNumBytes = worldChunkMemory->getTilesBlobNumBytes();
		std::uint16_t paletteBlobNumBytes = worldChunkMemory->getPaletteBlobNumBytes();

		rc = sqlite3_prepare_v2( database, command, -1, &statement, NULL );
		rc = sqlite3_bind_int( statement, 1, chunkIndexX );
		rc = sqlite3_bind_int( statement, 2, chunkIndexY );
		rc = sqlite3_bind_blob( statement, 3, tilesBlob, tilesBlobNumBytes, SQLITE_STATIC );
		rc = sqlite3_bind_blob( statement, 4, paletteBlob, paletteBlobNumBytes, SQLITE_STATIC );
		rc = sqlite3_step( statement );
		rc = sqlite3_finalize( statement );

	}

	sqlite3_close( database );
	lockDatabase.unlock();

	for ( int i = 0; i < _saveWorldChunks.size(); i++ )
	{
		delete _saveWorldChunks[i];
	}
	this->_saveWorldChunks.clear();

	return;
}


unsigned char* World::createTilesBlob( const Tile* tiles, std::vector<TileIdentity>& palette )
{
	// Create tilesBlob which is an array of bytes representing the tiles of the worldChunk. However, the tiles are mapped to smaller bit keys in order to save memory.

	std::uint16_t numTiles = Settings::World::NUM_CELLS_PER_CHUNK;
	std::uint8_t numBitsPerSegment = Settings::MemoryManager::NUM_BITS_PER_SEGMENT;

	std::uint16_t numUniqueKeys = palette.size(); // max is 32*32
	std::uint8_t numBitsPerKey = ( ceil( log2( numUniqueKeys ) ) ) > 0 ? ( ceil( log2( numUniqueKeys ) ) ) : 1; // [!] // max is 10 bits
	std::uint32_t tilesNumBits = ( numBitsPerKey * numTiles ); // max is 32*32*64
	std::uint16_t tilesNumBytes = ( int )ceil( ( float )tilesNumBits / 8.0f ); // max is 32*32*64/8

	// Create PaletteMap
	std::map<std::uint64_t, std::uint16_t> paletteMap;
	for ( int i = 0; i < numUniqueKeys; i++ )
	{
		paletteMap.emplace( ( std::uint64_t )palette[i], i );
	}

	// Create a condensed array of bytes that holds numTiles of n-bit keys 
	unsigned char* tilesBlob = new unsigned char[tilesNumBytes]();
	uint16_t byteOffset = tilesNumBytes - 1;
	std::uint8_t remainingSegmentBits = numBitsPerSegment;
	for ( int i = numTiles - 1; i >= 0; i-- )
	{
		uint64_t tileId = ( uint64_t )tiles[i].getId();
		uint16_t key = paletteMap.at( tileId );

		// If there is enough bits left on the byte to fill the entire key, then fill
		if ( remainingSegmentBits >= numBitsPerKey )
		{
			// Copy the bits of the key to the proper byte of the tilesBlob in the proper position within the byte
			copyBits( tilesBlob[byteOffset], ( ( numBitsPerSegment - remainingSegmentBits ) % numBitsPerSegment ), ( ( numBitsPerSegment - remainingSegmentBits ) % numBitsPerSegment ) + numBitsPerKey,
				key, 0, numBitsPerKey );

			remainingSegmentBits -= numBitsPerKey;
		}
		// Otherwise, we need to splice the key into multiple bytes
		else
		{
			// First Splice
			std::uint8_t remainingKeyBits = numBitsPerKey;
			std::uint8_t firstSplice = remainingSegmentBits;

			// Copy the bits of the key to the proper byte of the tilesBlob in the proper position within the byte
			copyBits( tilesBlob[byteOffset], ( ( numBitsPerSegment - remainingSegmentBits ) % numBitsPerSegment ), ( ( numBitsPerSegment - remainingSegmentBits ) % numBitsPerSegment ) + firstSplice,
				key, 0, firstSplice );
			remainingKeyBits -= firstSplice;
			key = key >> firstSplice;

			// Second Splice ( can be comprised of multiple subsplices )
			std::uint8_t secondSplice = remainingKeyBits;
			std::uint8_t numBytesNeeded = ( int )ceil( ( float )secondSplice / ( float )numBitsPerSegment );
			std::uint8_t prevSubSplice = firstSplice;
			std::uint8_t subSplice = 0;
			for ( std::uint8_t j = 0; j < numBytesNeeded; j++ )
			{
				byteOffset -= 1;
				// Our bottlenecks to determine the next avaiable number of bits we should splice
				subSplice = numBitsPerKey < secondSplice ? ( numBitsPerKey < numBitsPerSegment ? numBitsPerKey : numBitsPerSegment ) : ( secondSplice < numBitsPerSegment ? secondSplice : numBitsPerSegment );

				// For each byte needed, copy the proper amount of bits from the key to the respective byte in the proper position within the byte
				copyBits( tilesBlob[byteOffset], 0, subSplice,
					key, 0, subSplice );

				// Reduce the remaining key bits in the secondSplice by the subsplice
				secondSplice -= subSplice;
				key = key >> subSplice;
				prevSubSplice = prevSubSplice + subSplice;
			}

			remainingSegmentBits = numBitsPerSegment - subSplice;
		}

		if ( remainingSegmentBits == 0 )
		{
			remainingSegmentBits = numBitsPerSegment;
			byteOffset -= 1;
		}
	}

	return tilesBlob;
}


std::uint64_t* World::createPaletteBlob( std::vector<TileIdentity>& palette )
{
	// Create a paletteBlob which is an array of bytes.
	std::uint16_t numUniqueKeys = palette.size(); // max is 32*32
	std::uint64_t* paletteBlob = new uint64_t[numUniqueKeys]();

	for ( std::uint16_t i = 0; i < numUniqueKeys; i++ )
	{
		paletteBlob[i] = ( std::uint64_t )palette[i];
	}

	return paletteBlob;
}


void World::addMemory( WorldChunkMemory* worldChunkMemory )
{
	std::lock_guard<std::mutex> lockSave( this->_mutexSaveWorldChunks );

	// Overloaded
	if ( this->_saveWorldChunks.size() >= Settings::World::MAX_SAVED_CHUNKS )
	{
		delete worldChunkMemory;
		return;
	}

	this->_saveWorldChunks.push_back( worldChunkMemory );
	return;
}


void World::loadWorldGeographyTask()
{
	this->_runningLoadWorldGeography = true;
	while ( this->_runningLoadWorldGeography )
	{
		this->updateWorldChunkRelativity( this->_camera->getFocalPoint() );
	}

	return;
}


void World::loadWorldGeography()
{
	// Load the proper worldChunks from the SQLite database 

	// Check to see if there is an update
	std::int64_t focalIndexX = this->_focalChunk.getCenterX() >= 0 ? ( std::int64_t )( this->_focalChunk.getCenterX() / this->_chunkCellSize ) : ( std::int64_t )( ( this->_focalChunk.getCenterX() - this->_chunkCellSize ) / this->_chunkCellSize );
	std::int64_t focalIndexY = this->_focalChunk.getCenterY() >= 0 ? ( std::int64_t )( this->_focalChunk.getCenterY() / this->_chunkCellSize ) : ( std::int64_t )( ( this->_focalChunk.getCenterY() - this->_chunkCellSize ) / this->_chunkCellSize );
	if ( this->_focalChunkIndexX == focalIndexX && this->_focalChunkIndexY == focalIndexY )
	{
		return;
	}

	// Delimit
	std::vector<std::tuple<std::uint64_t, std::int64_t, std::int64_t>> chunkRecalls = this->delimitWorldChunks( this->_focalChunk );
	if ( chunkRecalls.empty() )
	{
		return;
	}


	std::unique_lock<std::mutex> lockUpdateLighting( this->_mutexUpdateLighting ); // [~!]

	for ( int i = 0; i < chunkRecalls.size(); i++ )
	{
		std::uint64_t index = std::get<0>( chunkRecalls[i] );
		std::int64_t chunkIndexX = std::get<1>( chunkRecalls[i] );
		std::int64_t chunkIndexY = std::get<2>( chunkRecalls[i] );
		WorldChunk* worldChunk = &this->_worldChunks[index];

		this->delimitWorldChunk( *worldChunk, chunkIndexX, chunkIndexY );
	}

	this->_focalChunkIndexX = focalIndexX;
	this->_focalChunkIndexY = focalIndexY;

	lockUpdateLighting.unlock(); // [~!]

	this->updateWorldChunkRelativeIndicies();
	this->updateWorldChunkPointers();

	std::vector<WorldChunkRecall> worldChunkRecalls;

	// Load
	//std::lock_guard<std::mutex> lockDatabase( _worldDatabaseMutex ); // [!] unique lock just for reading
	std::unique_lock<std::mutex> lockDatabase( _worldDatabaseMutex );


	sqlite3* database = NULL;
	sqlite3_stmt* statement = NULL;
	const char* command = NULL;
	char* errorMessage = NULL;
	int rc;

	//std::vector<std::int64_t> proceduralGenerateCache;

	sqlite3_open( "./world.db", &database );

	for ( int i = 0; i < chunkRecalls.size(); i++ )
	{
		std::int64_t index = std::get<0>( chunkRecalls[i] );
		std::int64_t chunkIndexX = std::get<1>( chunkRecalls[i] );
		std::int64_t chunkIndexY = std::get<2>( chunkRecalls[i] );


		// Check if world chunk ( chunkIndexX, chunkIndexY ) exists
		command =
			"SELECT EXISTS( SELECT 1 FROM world_geography WHERE chunk_index_x = ?1 AND chunk_index_y = ?2 );";
		rc = sqlite3_prepare_v2( database, command, -1, &statement, NULL );
		rc = sqlite3_bind_int( statement, 1, chunkIndexX );
		rc = sqlite3_bind_int( statement, 2, chunkIndexY );
		rc = sqlite3_step( statement );


		// If it does not exists, return or throw error
		bool exists = sqlite3_column_int( statement, 0 );
		if ( !exists )
		{
			rc = sqlite3_step( statement );
			rc = sqlite3_finalize( statement );

			// std::cout << "Chunk does has not existed beforehand. Procedural generation" << std::endl; [~!]
			// std::cout << this->_worldChunks[index].getRelativeChunkIndex() << std::endl;
			//proceduralGenerateCache.push_back( index );
			this->procedurallyGenerate( this->_worldChunks[index] );
			//this->_worldChunks[index].resetLights();

			//if ( this->_worldChunks[index].getRelativeChunkIndex() == 21 ) proceduralGenerateCache.push_back( index );
			continue;
		}
		rc = sqlite3_step( statement ); // Extra step for null
		rc = sqlite3_finalize( statement );

		// Select tile_data and palette based on primary key ( chunkIndexX, chunkIndexY )
		command =
			"SELECT tiles, palette\n"
			"FROM world_geography\n"
			"WHERE chunk_index_x = ? AND chunk_index_y = ?;";
		rc = sqlite3_prepare_v2( database, command, -1, &statement, NULL );
		rc = sqlite3_bind_int( statement, 1, chunkIndexX );
		rc = sqlite3_bind_int( statement, 2, chunkIndexY );
		rc = sqlite3_step( statement );

		// Create tilesData used as reference when reconstructing the worldChunk's tiles
		std::uint16_t numBytesTiles = sqlite3_column_bytes( statement, 0 );
		unsigned char* tilesData = new unsigned char[numBytesTiles];
		std::memcpy( tilesData, sqlite3_column_blob( statement, 0 ), numBytesTiles );

		// Create paletteData used as reference when reconstructing the worldChunk's tiles
		std::uint16_t numBytesPalette = sqlite3_column_bytes( statement, 1 );
		std::uint16_t numUniqueKeys = numBytesPalette / sizeof( std::uint64_t );
		std::uint64_t* paletteData = new std::uint64_t[numUniqueKeys];
		std::memcpy( paletteData, sqlite3_column_blob( statement, 1 ), numBytesPalette );

		rc = sqlite3_step( statement ); // Extra step for null
		rc = sqlite3_finalize( statement );

		worldChunkRecalls.push_back( WorldChunkRecall( index, tilesData, numBytesTiles, paletteData, numBytesPalette ) );
	}

	sqlite3_close( database );
	lockDatabase.unlock();

	for ( int i = 0; i < worldChunkRecalls.size(); i++ )
	{
		std::uint64_t index = worldChunkRecalls[i].getIndex();
		unsigned char* tilesData = worldChunkRecalls[i].getTilesData();
		std::uint16_t numBytesTiles = worldChunkRecalls[i].getNumBytesTiles();
		std::uint64_t* paletteData = worldChunkRecalls[i].getPaletteData();
		std::uint16_t numBytesPalette = worldChunkRecalls[i].getNumBytesPalette();

		WorldChunk& worldChunk = this->_worldChunks[index];
		this->loadTiles( worldChunk, tilesData, numBytesTiles, paletteData, numBytesPalette );

		//worldChunk.resetLights();


		delete[] tilesData;
		delete[] paletteData;
	}
	worldChunkRecalls.clear();


	//for ( std::int64_t worldChunkIndex : proceduralGenerateCache )
	//{
	//	this->procedurallyGenerate( this->_worldChunks[worldChunkIndex] );
	//}


	// Allow the tileSprites to refresh
	//this->_condUpdateLighting.notify_one();
	this->_condModifyAtlas.notify_one();
	//////

	/*
	long double accumulate = 0;
	std::int64_t numcalculations = 0;
	for ( int i = 0; i < 10; i++ )
	{
		long double acc = 0;
		for ( int x = 0; x < 32; x++)
		{
			for ( int y = 0; y < 32; y++ )
			{
				for ( int i = 0; i < 20; i++ )
				{
					numcalculations++;

					std::int64_t hash = 657576373737 ^ 24243263266877 ^ 6452774574444;
					hash *= 0x27d4eb2f;
					hash *= hash;
					hash ^= hash << 19;
					hash = hash* ( 1.0 / ( long double )std::numeric_limits<std::int64_t>::max() );

					acc = std::rand();

					float blend = std::rand() / RAND_MAX;
					long double hashld = ( long double )hash;
					hashld = ( hashld * blend ) + ( 1 - blend ) * acc;
					accumulate += hashld;
				}
				for ( int i = 0; i < 20; i++ )
				{
					numcalculations++;

					std::int64_t hash = 62456247277 ^ 345324672472 ^ 452345262;
					hash *= 0x27d4eb2f;
					hash *= hash;
					hash ^= hash << 19;
					hash = hash * ( 1.0 / ( long double )std::numeric_limits<std::int64_t>::max() );

					acc = std::rand();

					float blend = std::rand() / RAND_MAX;
					long double hashld = ( long double )hash;
					hashld = ( hashld * blend ) + ( 1 - blend ) * acc;
					accumulate += hashld;
				}
				for ( int i = 0; i < 20; i++ )
				{
					numcalculations++;

					std::int64_t hash = 64373457 ^ 46532466 ^ 34532377;
					hash *= 0x27d4eb2f;
					hash *= hash;
					hash ^= hash << 19;
					hash = hash * ( 1.0 / ( long double )std::numeric_limits<std::int64_t>::max() );

					acc = std::rand();

					float blend = std::rand() / RAND_MAX;
					long double hashld = ( long double )hash;
					hashld = ( hashld * blend ) + ( 1 - blend ) * acc;
					accumulate += hashld;
				}
			}
		}
	}

	std::cout << accumulate << std::endl;
	std::cout << numcalculations << std::endl;
	*/

	///////
	return;
}


void World::updateWorldChunkRelativity( const BoundingBox<long double>& focalPoint )
{
	std::lock_guard<std::mutex> lockModify( this->_modifyWorldChunksMutex );
	std::lock_guard<std::mutex> lockLoad( this->_mutexLoadWorldChunks );

	//std::lock_guard<std::mutex> lockUpdateGeography( this->_mutexUpdateGeography );
	//std::lock_guard<std::mutex> lockUpdateLighting( this->_mutexUpdateLighting );

	//std::unique_lock<std::mutex> lockUpdateGeography( this->_mutexUpdateGeography );
	//this->_condRenderWorld.wait( lockUpdateGeography );
	//std::lock_guard<std::mutex> lockUpdateLighting( this->_mutexUpdateLighting );


	this->updateFocalChunk( focalPoint );
	this->loadWorldGeography();

	//this->_condUpdateWorld.notify_one();
	return;
}


void World::updateFocalChunk( const BoundingBox<long double>& focalPoint )
{
	// Update the focalChunk of the world relative to a camera's focal point.
	// Inhibit changing the focalChunk while loading for any nasty race conditions.
	// std::lock_guard<std::mutex> lockUpdateLighting( this->_mutexUpdateLighting ); //[!~]

	this->_focalChunk = focalPoint;
	return;
}


void World::updateWorldChunkRelativeIndicies()
{
	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		std::uint16_t relativeChunkIndex = ( this->_worldChunks[i].getChunkIndexY() - ( this->_focalChunkIndexY - Settings::World::CHUNK_RADIUS ) ) * this->_numChunkWidth
			+ ( this->_worldChunks[i].getChunkIndexX() - ( this->_focalChunkIndexX - Settings::World::CHUNK_RADIUS ) );
		this->_worldChunks[i].setRelativeChunkIndex( relativeChunkIndex );
	}
	return;
}


void World::updateWorldChunkPointers()
{
	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		std::uint16_t relativeChunkIndex = this->_worldChunks[i].getRelativeChunkIndex();
		this->_worldChunkPointers[relativeChunkIndex] = &this->_worldChunks[i];
	}

	return;
}


std::vector<std::tuple<std::uint64_t, std::int64_t, std::int64_t>> World::delimitWorldChunks( const BoundingBox<long double>& focalPoint )
{
	// Based on the focalPoint, prepare all the world chunks that need to be delimited and updated 

	// Build recall Vector
	std::vector<std::tuple<std::uint64_t, std::int64_t, std::int64_t>> chunkRecalls;

	// Based on the updated camera's position, delimit the surrounding worldChunks
	std::int64_t focalIndexX = focalPoint.getCenterX() >= 0 ? ( std::int64_t )( focalPoint.getCenterX() / this->_chunkCellSize ) : ( std::int64_t )( ( focalPoint.getCenterX() - this->_chunkCellSize ) / this->_chunkCellSize );
	std::int64_t focalIndexY = focalPoint.getCenterY() >= 0 ? ( std::int64_t )( focalPoint.getCenterY() / this->_chunkCellSize ) : ( std::int64_t )( ( focalPoint.getCenterY() - this->_chunkCellSize ) / this->_chunkCellSize );

	// No need to delimit if the camera index has not changed
	if ( this->_focalChunkIndexX == focalIndexX && this->_focalChunkIndexY == focalIndexY )
	{
		return chunkRecalls;
	}

	for ( std::uint64_t x = 0; x < this->_numChunkWidth; x++ )
	{
		for ( std::uint64_t y = 0; y < this->_numChunkHeight; y++ )
		{
			WorldChunk& worldChunk = this->_worldChunks[y * this->_numChunkWidth + x];
			std::int64_t worldChunkIndexX = worldChunk.getChunkIndexX();
			std::int64_t worldChunkIndexY = worldChunk.getChunkIndexY();

			// If the worldChunk is out of range relative to the camera
			if ( std::abs( focalIndexX - worldChunkIndexX ) > this->_chunkRadius || std::abs( focalIndexY - worldChunkIndexY ) > this->_chunkRadius )
			{
				// Add to memory
				std::thread addMemoryThread( &World::addMemory, this, worldChunk.createMemory() );
				addMemoryThread.detach();

				// Add to recall
				std::int64_t newChunkIndexX = focalIndexX + -( worldChunkIndexX - this->_focalChunkIndexX );
				std::int64_t newChunkIndexY = focalIndexY + -( worldChunkIndexY - this->_focalChunkIndexY );
				chunkRecalls.push_back( std::tuple<std::uint64_t, std::int64_t, std::int64_t>( y * this->_numChunkWidth + x, newChunkIndexX, newChunkIndexY ) );
			}
		}
	}

	return chunkRecalls;
}


void World::delimitWorldChunk( WorldChunk& worldChunk, std::int64_t chunkIndexX, std::int64_t chunkIndexY )
{
	// Clears the worldChunk, updates its proper index, and wipes the render in preparation to load in new data	and new render
	worldChunk.delimit( chunkIndexX, chunkIndexY );
	return;
}


void World::loadTiles( WorldChunk& worldChunk, unsigned char* tilesData, std::uint16_t numBytesTiles, std::uint64_t* paletteData, std::uint16_t numBytesPalette )
{
	// Load from tilesData which is an array of bytes representing the tiles of the worldChunk given by tilesBlob. We need unpack this condensed "bit-addressable" 
	// array of keys to the 64bit tileId values.

	std::uint16_t numTiles = Settings::World::NUM_CELLS_PER_CHUNK;
	std::uint8_t numBitsPerSegment = Settings::MemoryManager::NUM_BITS_PER_SEGMENT;
	std::uint16_t numUniqueKeys = numBytesPalette / sizeof( std::uint64_t );

	// Seen tileId to limit the amount of threads for addSpriteTiles
	std::set<TileIdentity> historyTileIds;

	// Uncondense the keys, map it to the correct tileId using paletteData, and set the tile to the respective id.
	uint16_t key = 0;
	std::uint8_t accumulator = 0;
	std::uint8_t numBitsPerKey = ( ceil( log2( numUniqueKeys ) ) ) > 0 ? ( ceil( log2( numUniqueKeys ) ) ) : 1; // [!] // max is 10 bits
	uint16_t byteOffset = numBytesTiles - 1;
	std::uint8_t remainingSegmentBits = numBitsPerSegment;
	std::string keyDisplacements;


	std::int16_t chunkSize = worldChunk.getSize();

	//worldChunk.wipeRender();


	for ( int i = numTiles - 1; i >= 0; i-- )
	{
		key = 0;
		accumulator = 0;

		// If there is enough bits left on the byte to find the value of the entire key, then fill
		if ( remainingSegmentBits >= numBitsPerKey )
		{
			// Copy bits into the first accumulator and initialize the key to that value
			World::copyBits( accumulator, 0, numBitsPerKey,
				tilesData[byteOffset], ( ( numBitsPerSegment - remainingSegmentBits ) % numBitsPerSegment ), ( ( numBitsPerSegment - remainingSegmentBits ) % numBitsPerSegment ) + numBitsPerKey );

			key = accumulator;

			// Insert tiles to the Tile[] and the tileSprites map
			std::uint64_t tileId = paletteData[key];
			if ( historyTileIds.find( static_cast< TileIdentity >( tileId ) ) == historyTileIds.end() ) // [!] Make sure that render quad tree jsut continues if it can't find the sprite because it may try to render before the tileId is added
			{
				//std::thread addSpriteTileThread( &World::addSpriteTile, this, static_cast< TileIdentity >( tileId ) );
				//addSpriteTileThread.join();
				historyTileIds.insert( static_cast< TileIdentity >( tileId ) );
			}
			//worldChunk.insertTile( worldChunk.getChunkIndexX() * 32 + ( i % 32 ), worldChunk.getChunkIndexY() * 32 + ( i / 32 ), 1, 1, tileId );

			//std::cout << "1: " << ( unsigned long long )tileId << std::endl; 
			//if ( ( unsigned long long )tileId > 100 ) tileId = 0; // [!] crude corruption detection debug

			if ( static_cast< TileIdentity >( tileId ) != TileIdentity::Void )
			{


				worldChunk.insert( static_cast< TileIdentity >( tileId ), 0, worldChunk.getChunkIndexX() * 32 + ( i % 32 ), worldChunk.getChunkIndexY() * 32 + ( i / 32 ), 1, 1 );




			}

			remainingSegmentBits -= numBitsPerKey;
		}
		// Otherwise, we need to splice the key into multiple bytes
		else
		{
			// First Splice
			std::uint8_t remainingKeyBits = numBitsPerKey;
			std::uint8_t firstSplice = remainingSegmentBits;

			// Copy bits into the first accumulator and initialize the key to that value
			World::copyBits( accumulator, 0, firstSplice,
				tilesData[byteOffset], ( ( numBitsPerSegment - remainingSegmentBits ) % numBitsPerSegment ), ( ( numBitsPerSegment - remainingSegmentBits ) % numBitsPerSegment ) + firstSplice );
			key += accumulator;
			accumulator = 0;

			remainingKeyBits -= firstSplice;

			// Second Splice ( can be comprised of multiple subsplices )
			std::uint8_t secondSplice = remainingKeyBits; //remainingSegmentBits; // remainingKeyBits;
			std::uint8_t numBytesNeeded = ( int )ceil( ( float )secondSplice / ( float )numBitsPerSegment );
			std::uint8_t prevSubSplice = firstSplice;
			std::uint8_t subSplice = 0;
			for ( std::uint8_t j = 0; j < numBytesNeeded; j++ )
			{
				byteOffset -= 1;
				// Our bottlenecks to determine the next avaiable number of bits we should splice
				subSplice = numBitsPerKey < secondSplice ? ( numBitsPerKey < numBitsPerSegment ? numBitsPerKey : numBitsPerSegment ) : ( secondSplice < numBitsPerSegment ? secondSplice : numBitsPerSegment );

				// For each byte needed, we set the accumulator to the proper value and add it to they key, remembering to offset by the number of bits of the keys left
				copyBits( accumulator, 0, subSplice,
					tilesData[byteOffset], 0, subSplice );
				key += ( accumulator << ( numBitsPerKey - subSplice ) );
				accumulator = 0;

				// Reduce the remaining key bits in the secondSplice by the subsplice
				secondSplice -= subSplice;
				prevSubSplice = prevSubSplice + subSplice;
			}
			remainingSegmentBits = numBitsPerSegment - subSplice;

			// Insert tiles to the Tile[] and the tileSprites map
			std::uint64_t tileId = paletteData[key];
			if ( historyTileIds.find( static_cast< TileIdentity >( tileId ) ) == historyTileIds.end() ) // [!] Make sure that render quad tree jsut continues if it can't find the sprite because it may try to render before the tileId is added
			{
				//std::thread addSpriteTileThread( &World::addSpriteTile, this, static_cast< TileIdentity >( tileId ) );
				//addSpriteTileThread.join();
				historyTileIds.insert( static_cast< TileIdentity >( tileId ) );
			}
			//worldChunk.insertTile( worldChunk.getChunkIndexX() * 32 + ( i % 32 ), worldChunk.getChunkIndexY() * 32 + ( i / 32 ), 1, 1, tileId );
			//std::cout << "2: " << ( unsigned long long )tileId << std::endl;
			//if ( ( unsigned long long )tileId > 100 ) tileId = 0; // [!] crude corruption detection debug

			if ( static_cast< TileIdentity >( tileId ) != TileIdentity::Void )
			{
				//worldChunk.insert( static_cast< TileIdentity >( tileId ), worldChunk.getChunkIndexX() * 32 + ( i % 32 ), worldChunk.getChunkIndexY() * 32 + ( i / 32 ), 1, 1 );

				worldChunk.insert( static_cast< TileIdentity >( tileId ), 0, worldChunk.getChunkIndexX() * 32 + ( i % 32 ), worldChunk.getChunkIndexY() * 32 + ( i / 32 ), 1, 1 );

			}

		}

		if ( remainingSegmentBits == 0 )
		{
			remainingSegmentBits = numBitsPerSegment;
			byteOffset -= 1;
		}
	}


	// Load in the necessary new sprites
	std::thread addSpriteTileThread( &World::addSpriteTiles, this, historyTileIds );
	addSpriteTileThread.join();

	return;
}


long double World::normalizeHistogram( long double value )
{
	if ( value <= 0.324 )
	{
		return 6.15421 * ( value * value * value ) - 1.70354 * ( value * value ) + 0.215019f * ( value );
	}
	else if ( value >= 0.324 && value <= 0.676 )
	{
		return -16.6757 * ( value * value * value ) + 25.0136 * ( value * value ) - 9.71832 * ( value )+1.19033;
	}
	else // if ( value >= 0.676 )
	{
		return 6.15421 * ( value * value * value ) - 16.7591 * ( value * value ) + 15.2706 * ( value )-3.66557;
	}
}


long double World::biomeLine1( long double x ) { return 10.64 * x - 0.468; }
long double World::biomeLine2( long double x ) { return 3.55 * x - 0.489; }
long double World::biomeLine3( long double x ) { return 3.05 * x - 1.591; }
long double World::biomeLine4( long double x ) { return 0.063 * x + 0.125; }
long double World::biomeLine5( long double x ) { return 0.094 * x + 0.271; }
long double World::biomeLine6( long double x ) { return -0.223 * x + 0.882; }


std::int64_t World::getSeed() const { return this->_seed; }


const TerraneanHeightMap& World::getTerraneanHeightMap() const { return this->_terraneanHeightMap; }
const SubterraneanHeightMap& World::getSubterraneanHeightMap() const { return this->_subterraneanHeightMap; }
const TemperatureMap& World::getTemperatureMap() const { return this->_temperatureMap; }
const PrecipitationMap& World::getPrecipitationMap() const { return this->_precipitationMap; }
const BiomeSubstanceMap& World::getBiomeSubstanceMap() const { return this->_biomeSubstanceMap; }
const CaveMap& World::getUpperCaveMap() const { return this->_upperCaveMap; }
const CaveMap& World::getLowerCaveMap() const { return this->_lowerCaveMap; }

const BorealForest& World::getBorealForest() const { return this->_borealForest; }
const SubtropicalDesert& World::getSubtropicalDesert() const { return this->_subtropicalDesert; }
const TemperateGrassland& World::getTemperateGrassland() const { return this->_temperateGrassland; }
const TemperateRainforest& World::getTemperateRainforest() const { return this->_temperateRainforest; }
const TemperateSeasonalForest& World::getTemperateSeasonalForest() const { return this->_temperateSeasonalForest; }
const TropicalRainforest& World::getTropicalRainforest() const { return this->_tropicalRainforest; }
const TropicalSeasonalForest& World::getTropicalSeasonalForest() const { return this->_tropicalSeasonalForest; }
const Tundra& World::getTundra() const { return this->_tundra; }
const Woodland& World::getWoodland() const { return this->_woodland; }


BiomeIdentity World::getBiomeIdentity( std::int64_t tileX, std::int64_t tileY ) const
{
	long double temperaturePerlinValue = this->getTemperatureMap().getPerlinValue( tileX, tileY );
	long double normTemperaturePerlinValue = this->normalizeHistogram( temperaturePerlinValue );

	long double precipitationPerlinValue = this->getPrecipitationMap().getPerlinValue( tileX, tileY );
	long double normPrecipitationPerlinValue = normalizeHistogram( precipitationPerlinValue );

	long double line1Val = this->biomeLine1( normTemperaturePerlinValue );
	long double line2Val = this->biomeLine2( normTemperaturePerlinValue );
	long double line3Val = this->biomeLine3( normTemperaturePerlinValue );
	long double line4Val = this->biomeLine4( normTemperaturePerlinValue );
	long double line5Val = this->biomeLine5( normTemperaturePerlinValue );
	long double line6Val = this->biomeLine6( normTemperaturePerlinValue );

	if ( normPrecipitationPerlinValue >= line1Val ) {
		// Tundra
		return BiomeIdentity::Tundra;
	}
	else if (
		normPrecipitationPerlinValue <= line1Val &&
		normPrecipitationPerlinValue <= line4Val &&
		normPrecipitationPerlinValue >= line3Val )
	{
		// Temperate GrassLand / Cold Desert
		return BiomeIdentity::TemperateGrassland;
	}
	else if (
		normPrecipitationPerlinValue <= line1Val &&
		normPrecipitationPerlinValue >= line4Val &&
		normPrecipitationPerlinValue <= line5Val &&
		normPrecipitationPerlinValue >= line3Val )
	{
		// Woodland / Shrubland
		return BiomeIdentity::Woodland;
	}
	else if (
		normPrecipitationPerlinValue <= line1Val &&
		normPrecipitationPerlinValue >= line2Val &&
		normPrecipitationPerlinValue >= line5Val )
	{
		// Boreal Forest
		return BiomeIdentity::BorealForest;
	}
	else if (
		normPrecipitationPerlinValue <= line2Val &&
		normPrecipitationPerlinValue >= line3Val &&
		normPrecipitationPerlinValue >= line5Val &&
		normPrecipitationPerlinValue <= line6Val )
	{
		// Temperate Seasonal Forest
		return BiomeIdentity::TemperateSeasonalForest;
	}
	else if (
		normPrecipitationPerlinValue <= line2Val &&
		normPrecipitationPerlinValue >= line3Val &&
		normPrecipitationPerlinValue >= line6Val )
	{
		// Temperate Rain Forest
		return BiomeIdentity::TemperateRainforest;
	}
	else if (
		normPrecipitationPerlinValue <= line3Val &&
		normPrecipitationPerlinValue <= line4Val )
	{
		// Subtropical Desert
		return BiomeIdentity::SubtropicalDesert;
	}
	else if (
		normPrecipitationPerlinValue <= line3Val &&
		normPrecipitationPerlinValue >= line4Val &&
		normPrecipitationPerlinValue <= line6Val )
	{
		// Savanna
		return BiomeIdentity::TropicalSeasonalForest;
	}
	else if (
		normPrecipitationPerlinValue <= line3Val &&
		normPrecipitationPerlinValue >= line6Val )
	{
		return BiomeIdentity::TropicalRainforest;
	}
	else
	{
		return BiomeIdentity::Unknown;
	}
}



TileIdentity World::getTerraneanSubstance( std::int64_t tileX, std::int64_t tileY ) const
{
	static const std::int64_t OVERWORLD_HEIGHT = 1536;

	BiomeIdentity biome = this->getBiomeIdentity( tileX, tileY );

	long double terraneanHeightPerlinVal = this->getTerraneanHeightMap().getPerlinValue( tileX );
	std::int64_t yTerranean = -( std::int64_t )( terraneanHeightPerlinVal * ( long double )OVERWORLD_HEIGHT ) + ( std::int64_t )OVERWORLD_HEIGHT;
	long double subterraneanHeightPerlinVal = this->getSubterraneanHeightMap().getPerlinValue( tileX );
	std::int64_t ySubterranean = ( std::int64_t )( subterraneanHeightPerlinVal * ( OVERWORLD_HEIGHT - yTerranean ) * 0.5f ) + yTerranean;

	long double biomeSubstancePerlinVal = this->getBiomeSubstanceMap().getPerlinValue( tileX, tileY );
	long double biomeSubstanceNormVal = this->normalizeHistogram( biomeSubstancePerlinVal );
	long double biomeHeightPercentage = ( ySubterranean != yTerranean ) ? ( long double )( ySubterranean - tileY ) / ( long double )( ySubterranean - yTerranean ) : 1.0;

	if ( biome == BiomeIdentity::TemperateSeasonalForest )
	{
		return TemperateSeasonalForest::getSubstance( biomeHeightPercentage, biomeSubstanceNormVal );
	}
	else if ( biome == BiomeIdentity::TropicalSeasonalForest )
	{
		return TropicalSeasonalForest::getSubstance( biomeHeightPercentage, biomeSubstanceNormVal );
	}
	else if ( biome == BiomeIdentity::TropicalRainforest )
	{
		return TropicalRainforest::getSubstance( biomeHeightPercentage, biomeSubstanceNormVal );
	}
	else if ( biome == BiomeIdentity::Woodland )
	{
		return Woodland::getSubstance( biomeHeightPercentage, biomeSubstanceNormVal );
	}
	else if ( biome == BiomeIdentity::TemperateGrassland )
	{
		return TemperateGrassland::getSubstance( biomeHeightPercentage, biomeSubstanceNormVal );
	}
	else if ( biome == BiomeIdentity::SubtropicalDesert )
	{
		return SubtropicalDesert::getSubstance( biomeHeightPercentage, biomeSubstanceNormVal );
	}
	else if ( biome == BiomeIdentity::BorealForest )
	{
		return BorealForest::getSubstance( biomeHeightPercentage, biomeSubstanceNormVal );
	}
	else if ( biome == BiomeIdentity::TemperateRainforest )
	{
		return TemperateRainforest::getSubstance( biomeHeightPercentage, biomeSubstanceNormVal );
	}
	else if ( biome == BiomeIdentity::Tundra )
	{
		return Tundra::getSubstance( biomeHeightPercentage, biomeSubstanceNormVal );
	}
	else
	{
		return TileIdentity::Stone;
	}
}


FoliageIdentity World::getFoliage( std::int64_t tileX, BiomeIdentity biomeId, long double temperatureNormalizedValue, long double precipitationNormalizedValue )
{
	//long double terraneanHeightPerlinVal = this->getTerraneanHeightMap().getPerlinValue( tileX );
	//std::int64_t yTerranean = -( std::int64_t )( terraneanHeightPerlinVal * ( long double )this->_height ) + ( std::int64_t )this->_height;

	switch ( biomeId )
	{
	case BiomeIdentity::BorealForest:
		return this->_borealForest.getFoliage( tileX );
	case BiomeIdentity::SubtropicalDesert:
		return this->_subtropicalDesert.getFoliage( tileX );
	case BiomeIdentity::TemperateGrassland:
		return this->_temperateGrassland.getFoliage( tileX );
	case BiomeIdentity::TemperateRainforest:
		return this->_temperateRainforest.getFoliage( tileX );
	case BiomeIdentity::TemperateSeasonalForest:
		return this->_temperateSeasonalForest.getFoliage( tileX, temperatureNormalizedValue, precipitationNormalizedValue );
	case BiomeIdentity::TropicalRainforest:
		return this->_tropicalRainforest.getFoliage( tileX );
	case BiomeIdentity::TropicalSeasonalForest:
		return this->_tropicalSeasonalForest.getFoliage( tileX );
	case BiomeIdentity::Tundra:
		return this->_tundra.getFoliage( tileX );
	case BiomeIdentity::Woodland:
		return this->_woodland.getFoliage( tileX );
	default:
		return FoliageIdentity::Void;
	}
}




/*
TileIdentity* World::getProceduralChunk( std::int64_t chunkIndexX, std::int64_t chunkIndexY )
{
	// Get topleft coordinates of a chunk and procedurally generate
	static const std::int64_t OVERWORLD_HEIGHT = 1536;

	std::int64_t originX = chunkIndexX * 32;
	std::int64_t originY = chunkIndexY * 32;

	TileIdentity* chunk = new TileIdentity[32 * 32]{ TileIdentity::Stone };

	for ( std::int64_t tileY = originY + OVERWORLD_HEIGHT; tileY < originY + OVERWORLD_HEIGHT + 32; tileY++ )
	{
		for ( std::int64_t tileX = originX; tileX < originX + 32; tileX++ )
		{
			// std::cout << tileX << ", " << tileY << std::endl;
			long double terraneanHeightPerlinVal = this->getTerraneanHeightMap().getPerlinValue( tileX );
			std::int64_t yTerranean = -( std::int64_t )( terraneanHeightPerlinVal * ( long double )OVERWORLD_HEIGHT ) + OVERWORLD_HEIGHT;

			long double subterraneanHeightPerlinVal = this->getSubterraneanHeightMap().getPerlinValue( tileX );
			std::int64_t ySubterranean = ( std::int64_t )( subterraneanHeightPerlinVal * ( OVERWORLD_HEIGHT - yTerranean ) * 0.5 ) + yTerranean;

			BiomeIdentity biome = this->getBiomeIdentity( tileX, yTerranean );
			// std::cout << yTerranean << ", " << ySubterranean << std::endl;
			// std::cout << tileX << ", " << tileY << std::endl;


			// Over Terranean Generation
			if ( tileY < 0 )
			{
				chunk[( tileY - ( originY + OVERWORLD_HEIGHT ) ) * 32 + ( tileX - originX )] = TileIdentity::Void; // Clouds
			}
			// On Top Of Terranean Generation
			else if ( tileY < yTerranean )
			{
				chunk[( tileY - ( originY + OVERWORLD_HEIGHT ) ) * 32 + ( tileX - originX )] = TileIdentity::Void; // Features ( Trees, Bushes, Flowers )
			}
			// Terranean Generation
			else if ( tileY >= yTerranean && tileY < ySubterranean )
			{
				// Height Map
				long double temperaturePerlinValue = this->getTemperatureMap().getPerlinValue( tileX, tileY );


				// Draw BioSubstance
				TileIdentity bioSubstance = this->getTerraneanSubstance( tileX, tileY );
				chunk[( tileY - ( originY + OVERWORLD_HEIGHT ) ) * 32 + ( tileX - originX )] = bioSubstance;


				// Bleed out tunnel voids
				long double upperCavePerlinValue = this->getUpperCaveMap().getPerlinValue( tileX, tileY ); // Same for all 2D
				int64_t tunnel = ( int64_t )( upperCavePerlinValue * 256 );

				if ( ( terraneanHeightPerlinVal < subterraneanHeightPerlinVal * subterraneanHeightPerlinVal ) ||
					( tunnel >= 183 && tunnel < 186 ) ||
					( tunnel >= 62 && tunnel < 64 ) ||
					( tunnel >= 125 && tunnel < 128 )

					)
				{
					// Draw Bleeding Tunnel Map
					if ( ( tunnel >= 183 && tunnel < 186 ) ||
						( tunnel >= 62 && tunnel < 64 ) ||
						( tunnel >= 125 && tunnel < 128 )
						)
					{
						chunk[( tileY - ( originY + OVERWORLD_HEIGHT ) ) * 32 + ( tileX - originX )] = TileIdentity::Void;
					}
				}
			}
			// Subterranean Generation
			else if ( tileY >= ySubterranean && tileY <= OVERWORLD_HEIGHT )
			{
				chunk[( tileY - ( originY + OVERWORLD_HEIGHT ) ) * 32 + ( tileX - originX )] = TileIdentity::Stone;

				// Draw Connecting Bleeding Tunnel Map
				long double upperCavePerlinValue = this->getUpperCaveMap().getPerlinValue( tileX, tileY ); // Same for all 2D
				int64_t tunnel = ( int64_t )( upperCavePerlinValue * 256 );
				if (
					(
						( tunnel >= 137 && tunnel < 141 ) ||
						( tunnel >= 183 && tunnel < 186 ) ||

						( tunnel >= 14 && tunnel < 16 ) ||
						( tunnel >= 62 && tunnel < 64 ) ||

						( tunnel >= 125 && tunnel < 128 )

						)
					)
				{
					chunk[( tileY - ( originY + OVERWORLD_HEIGHT ) ) * 32 + ( tileX - originX )] = TileIdentity::Void;
				}

				// Draw Subterranean Tunnel Map
				long double lowerCavePerlinValue = this->getLowerCaveMap().getPerlinValue( tileX, tileY ); // Same for all 2D // [!] Need another tunnel map instance
				tunnel = ( int64_t )( lowerCavePerlinValue * 256 );
				if (
					(
						( tunnel >= 41 && tunnel < 46 ) ||
						( tunnel >= 97 && tunnel < 103 ) ||

						( tunnel >= 157 && tunnel < 160 ) ||

						( tunnel >= 234 && tunnel < 240 )
						)
					)
				{
					chunk[( tileY - ( originY + OVERWORLD_HEIGHT ) ) * 32 + ( tileX - originX )] = TileIdentity::Void;
				}
			}
			else
			{
				// Generate Underworld
				if ( tileY > OVERWORLD_HEIGHT )
				{
					chunk[( tileY - ( originY + OVERWORLD_HEIGHT ) ) * 32 + ( tileX - originX )] = TileIdentity::Stone;
				}
			}

		}
	}

	return chunk;
}
*/



void World::addFoliage( TileIdentity* chunk,
	std::int64_t originX, std::int64_t originY, std::int64_t chunkOffsetX, std::int64_t chunkOffsetY, std::int64_t tileX, std::int64_t tileY,
	const TileIdentity* tiles, std::uint_fast8_t upBuffer, std::uint_fast8_t downBuffer, std::uint_fast8_t leftBuffer, std::uint_fast8_t rightBuffer )
{
	static const std::int64_t OVERWORLD_HEIGHT = 1536;
	std::int64_t seedY = tileY - ( originY + OVERWORLD_HEIGHT );
	std::int64_t seedX = tileX - originX;


	std::int8_t left = std::max( ( std::int8_t )( seedX - leftBuffer ), ( std::int8_t )0 );
	std::int8_t right = std::min( ( std::int8_t )( seedX + rightBuffer ), ( std::int8_t )31 );
	std::int8_t up = std::max( ( std::int8_t )( seedY - upBuffer ), ( std::int8_t )0 );
	std::int8_t down = std::min( ( std::int8_t )( seedY + downBuffer ), ( std::int8_t )31 );

	std::int8_t foliageOffsetX = std::max( leftBuffer - seedX, ( std::int64_t )0 );
	std::int8_t foliageOffsetY = std::max( upBuffer - seedY, ( std::int64_t )0 );

	for ( std::int8_t x = left; x <= right; x++ )
	{
		for ( std::int8_t y = up; y <= down; y++ )
		{
			if ( chunk[y * 32 + x] == TileIdentity::Void )
			{
				TileIdentity tileId = tiles[( foliageOffsetY + ( y - up ) ) * ( leftBuffer + 1 + rightBuffer ) + ( foliageOffsetX + ( x - left ) )];
				if ( tileId != TileIdentity::Void )
				{
					chunk[y * 32 + x] = tileId;
				}
			}
		}
	}

	return;
}


TileIdentity* World::getProceduralChunk( std::int64_t chunkIndexX, std::int64_t chunkIndexY )
{
	// Get topleft coordinates of a chunk and procedurally generate
	static const std::int64_t OVERWORLD_HEIGHT = 1536;

	std::int64_t originX = chunkIndexX * 32;
	std::int64_t originY = chunkIndexY * 32;

	TileIdentity* chunk = new TileIdentity[32 * 32];


	/*
	for ( int row = 0; row < 32; row++ )
	{
		for ( int col = 0; col < 32; col++ )
		{
			if ( row == 0 )
			{
				chunk[row * 32 + col] = TileIdentity::Stone;

			}
			else if ( col != 5 )
			{
				chunk[row * 32 + col] = TileIdentity::Water;

			}
			else
			{
				chunk[row * 32 + col] = TileIdentity::Void;

			}
		}
	}

	return chunk;
	*/

	for ( std::int64_t tileY = originY + OVERWORLD_HEIGHT; tileY < originY + OVERWORLD_HEIGHT + 32; tileY++ )
	{
		for ( std::int64_t tileX = originX; tileX < originX + 32; tileX++ )
		{
			// std::cout << tileX << ", " << tileY << std::endl;
			long double terraneanHeightPerlinVal = this->getTerraneanHeightMap().getPerlinValue( tileX );
			std::int64_t yTerranean = -( std::int64_t )( terraneanHeightPerlinVal * ( long double )OVERWORLD_HEIGHT ) + OVERWORLD_HEIGHT;

			long double subterraneanHeightPerlinVal = this->getSubterraneanHeightMap().getPerlinValue( tileX );
			std::int64_t ySubterranean = ( std::int64_t )( subterraneanHeightPerlinVal * ( OVERWORLD_HEIGHT - yTerranean ) * 0.5 ) + yTerranean;

			BiomeIdentity biome = this->getBiomeIdentity( tileX, yTerranean );
			// std::cout << yTerranean << ", " << ySubterranean << std::endl;
			// std::cout << tileX << ", " << tileY << std::endl;


			// Over Terranean Generation
			if ( tileY < 0 )
			{
				chunk[( tileY - ( originY + OVERWORLD_HEIGHT ) ) * 32 + ( tileX - originX )] = TileIdentity::Void; // Clouds
			}
			// On Top Of Terranean Generation
			else if ( tileY < yTerranean )
			{
				chunk[( tileY - ( originY + OVERWORLD_HEIGHT ) ) * 32 + ( tileX - originX )] = TileIdentity::Void; // Features ( Trees, Bushes, Flowers )
			}
			// Terranean Generation
			else if ( tileY >= yTerranean && tileY < ySubterranean )
			{
				// Height Map
				long double temperaturePerlinValue = this->getTemperatureMap().getPerlinValue( tileX, tileY );


				// Draw BioSubstance
				TileIdentity bioSubstance = this->getTerraneanSubstance( tileX, tileY );
				chunk[( tileY - ( originY + OVERWORLD_HEIGHT ) ) * 32 + ( tileX - originX )] = bioSubstance;


				// Bleed out tunnel voids
				long double upperCavePerlinValue = this->getUpperCaveMap().getPerlinValue( tileX, tileY ); // Same for all 2D
				int64_t tunnel = ( int64_t )( upperCavePerlinValue * 256 );

				if ( ( terraneanHeightPerlinVal < subterraneanHeightPerlinVal * subterraneanHeightPerlinVal ) ||
					( tunnel >= 183 && tunnel < 186 ) ||
					( tunnel >= 62 && tunnel < 64 ) ||
					( tunnel >= 125 && tunnel < 128 )

					)
				{
					// Draw Bleeding Tunnel Map
					if ( ( tunnel >= 183 && tunnel < 186 ) ||
						( tunnel >= 62 && tunnel < 64 ) ||
						( tunnel >= 125 && tunnel < 128 )
						)
					{
						chunk[( tileY - ( originY + OVERWORLD_HEIGHT ) ) * 32 + ( tileX - originX )] = TileIdentity::Void;
					}
				}
			}
			// Subterranean Generation
			else if ( tileY >= ySubterranean && tileY <= OVERWORLD_HEIGHT )
			{
				chunk[( tileY - ( originY + OVERWORLD_HEIGHT ) ) * 32 + ( tileX - originX )] = TileIdentity::Stone;

				// Draw Connecting Bleeding Tunnel Map
				long double upperCavePerlinValue = this->getUpperCaveMap().getPerlinValue( tileX, tileY ); // Same for all 2D
				int64_t tunnel = ( int64_t )( upperCavePerlinValue * 256 );
				if (
					(
						( tunnel >= 137 && tunnel < 141 ) ||
						( tunnel >= 183 && tunnel < 186 ) ||

						( tunnel >= 14 && tunnel < 16 ) ||
						( tunnel >= 62 && tunnel < 64 ) ||

						( tunnel >= 125 && tunnel < 128 )

						)
					)
				{
					chunk[( tileY - ( originY + OVERWORLD_HEIGHT ) ) * 32 + ( tileX - originX )] = TileIdentity::Void;
				}

				// Draw Subterranean Tunnel Map
				long double lowerCavePerlinValue = this->getLowerCaveMap().getPerlinValue( tileX, tileY ); // Same for all 2D // [!] Need another tunnel map instance
				tunnel = ( int64_t )( lowerCavePerlinValue * 256 );
				if (
					(
						( tunnel >= 41 && tunnel < 46 ) ||
						( tunnel >= 97 && tunnel < 103 ) ||

						( tunnel >= 157 && tunnel < 160 ) ||

						( tunnel >= 234 && tunnel < 240 )
						)
					)
				{
					chunk[( tileY - ( originY + OVERWORLD_HEIGHT ) ) * 32 + ( tileX - originX )] = TileIdentity::Void;
				}
			}
			else
			{
				// Generate Underworld
				if ( tileY > OVERWORLD_HEIGHT )
				{
					chunk[( tileY - ( originY + OVERWORLD_HEIGHT ) ) * 32 + ( tileX - originX )] = TileIdentity::Stone;
				}
			}

		}









		// Foliage
		std::int64_t chunkOffsetsX[] = { -2, -1, 0, 1, 2 };
		std::int64_t chunkOffsetsY[] = { -2, -1, 0, 1, 2 };

		for ( std::int64_t chunkOffsetX : chunkOffsetsX )
		{
			for ( std::int64_t tileX = ( originX + chunkOffsetX * 32 ); tileX < ( originX + chunkOffsetX * 32 ) + 32; tileX++ )
			{
				long double terraneanHeightPerlinVal = this->getTerraneanHeightMap().getPerlinValue( tileX );
				std::int64_t yTerranean = -( std::int64_t )( terraneanHeightPerlinVal * ( long double )OVERWORLD_HEIGHT ) + OVERWORLD_HEIGHT;
				std::int64_t tileY = yTerranean - 1;


				long double fNormalizedTunnelNoise = this->getUpperCaveMap().getPerlinValue( tileX, tileY ); // Same for all 2D
				int64_t tunnel = ( int64_t )( fNormalizedTunnelNoise * 256 );
				bool foliage = !( ( tunnel >= 183 && tunnel < 186 ) || ( tunnel >= 62 && tunnel < 64 ) || ( tunnel >= 125 && tunnel < 128 ) );
				if ( !foliage ) continue;


				for ( std::int64_t chunkOffsetY : chunkOffsetsY )
				{
					if ( ( ( originY + chunkOffsetY * 32 ) + OVERWORLD_HEIGHT ) <= yTerranean && yTerranean < ( ( originY + chunkOffsetY * 32 ) + OVERWORLD_HEIGHT ) + 32 )
					{
						//std::cout << ( tileX - originX ) << ", " << ( tileY - ( originY + OVERWORLD_HEIGHT ) ) << std::endl;
						//std::cout << "Foliage" << std::endl;

						BiomeIdentity biomeId = this->getBiomeIdentity( tileX, tileY );

						long double temperaturePerlinValue = this->getTemperatureMap().getPerlinValue( tileX, tileY );
						long double normTemperaturePerlinValue = this->normalizeHistogram( temperaturePerlinValue );

						long double precipitationPerlinValue = this->getPrecipitationMap().getPerlinValue( tileX, tileY ); // Can calculate this prior
						long double normPrecipitationValue = this->normalizeHistogram( precipitationPerlinValue );

						FoliageIdentity foliageId = this->getFoliage( tileX, biomeId, normTemperaturePerlinValue, normPrecipitationValue );



						switch ( foliageId )
						{
						case FoliageIdentity::MapleTree_0_0_0:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_0_0_0::tiles, Foliage::MapleTree_0_0_0::upBuffer, Foliage::MapleTree_0_0_0::downBuffer, Foliage::MapleTree_0_0_0::leftBuffer, Foliage::MapleTree_0_0_0::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_0_0_1:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_0_0_1::tiles, Foliage::MapleTree_0_0_1::upBuffer, Foliage::MapleTree_0_0_1::downBuffer, Foliage::MapleTree_0_0_1::leftBuffer, Foliage::MapleTree_0_0_1::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_0_0_2:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_0_0_2::tiles, Foliage::MapleTree_0_0_2::upBuffer, Foliage::MapleTree_0_0_2::downBuffer, Foliage::MapleTree_0_0_2::leftBuffer, Foliage::MapleTree_0_0_2::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_0_0_3:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_0_0_3::tiles, Foliage::MapleTree_0_0_3::upBuffer, Foliage::MapleTree_0_0_3::downBuffer, Foliage::MapleTree_0_0_3::leftBuffer, Foliage::MapleTree_0_0_3::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_0_0_4:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_0_0_4::tiles, Foliage::MapleTree_0_0_4::upBuffer, Foliage::MapleTree_0_0_4::downBuffer, Foliage::MapleTree_0_0_4::leftBuffer, Foliage::MapleTree_0_0_4::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_1_0_0:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_1_0_0::tiles, Foliage::MapleTree_1_0_0::upBuffer, Foliage::MapleTree_1_0_0::downBuffer, Foliage::MapleTree_1_0_0::leftBuffer, Foliage::MapleTree_1_0_0::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_1_0_1:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_1_0_1::tiles, Foliage::MapleTree_1_0_1::upBuffer, Foliage::MapleTree_1_0_1::downBuffer, Foliage::MapleTree_1_0_1::leftBuffer, Foliage::MapleTree_1_0_1::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_1_0_2:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_1_0_2::tiles, Foliage::MapleTree_1_0_2::upBuffer, Foliage::MapleTree_1_0_2::downBuffer, Foliage::MapleTree_1_0_2::leftBuffer, Foliage::MapleTree_1_0_2::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_1_0_3:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_1_0_3::tiles, Foliage::MapleTree_1_0_3::upBuffer, Foliage::MapleTree_1_0_3::downBuffer, Foliage::MapleTree_1_0_3::leftBuffer, Foliage::MapleTree_1_0_3::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_2_0_0:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_2_0_0::tiles, Foliage::MapleTree_2_0_0::upBuffer, Foliage::MapleTree_2_0_0::downBuffer, Foliage::MapleTree_2_0_0::leftBuffer, Foliage::MapleTree_2_0_0::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_2_0_1:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_2_0_1::tiles, Foliage::MapleTree_2_0_1::upBuffer, Foliage::MapleTree_2_0_1::downBuffer, Foliage::MapleTree_2_0_1::leftBuffer, Foliage::MapleTree_2_0_1::rightBuffer
							);
							break;


						case FoliageIdentity::MapleTree_0_1_0:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_0_1_0::tiles, Foliage::MapleTree_0_1_0::upBuffer, Foliage::MapleTree_0_1_0::downBuffer, Foliage::MapleTree_0_1_0::leftBuffer, Foliage::MapleTree_0_1_0::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_0_1_1:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_0_1_1::tiles, Foliage::MapleTree_0_1_1::upBuffer, Foliage::MapleTree_0_1_1::downBuffer, Foliage::MapleTree_0_1_1::leftBuffer, Foliage::MapleTree_0_1_1::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_0_1_2:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_0_1_2::tiles, Foliage::MapleTree_0_1_2::upBuffer, Foliage::MapleTree_0_1_2::downBuffer, Foliage::MapleTree_0_1_2::leftBuffer, Foliage::MapleTree_0_1_2::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_0_1_3:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_0_1_3::tiles, Foliage::MapleTree_0_1_3::upBuffer, Foliage::MapleTree_0_1_3::downBuffer, Foliage::MapleTree_0_1_3::leftBuffer, Foliage::MapleTree_0_1_3::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_0_1_4:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_0_1_4::tiles, Foliage::MapleTree_0_1_4::upBuffer, Foliage::MapleTree_0_1_4::downBuffer, Foliage::MapleTree_0_1_4::leftBuffer, Foliage::MapleTree_0_1_4::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_1_1_0:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_1_1_0::tiles, Foliage::MapleTree_1_1_0::upBuffer, Foliage::MapleTree_1_1_0::downBuffer, Foliage::MapleTree_1_1_0::leftBuffer, Foliage::MapleTree_1_1_0::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_1_1_1:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_1_1_1::tiles, Foliage::MapleTree_1_1_1::upBuffer, Foliage::MapleTree_1_1_1::downBuffer, Foliage::MapleTree_1_1_1::leftBuffer, Foliage::MapleTree_1_1_1::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_1_1_2:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_1_1_2::tiles, Foliage::MapleTree_1_1_2::upBuffer, Foliage::MapleTree_1_1_2::downBuffer, Foliage::MapleTree_1_1_2::leftBuffer, Foliage::MapleTree_1_1_2::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_1_1_3:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_1_1_3::tiles, Foliage::MapleTree_1_1_3::upBuffer, Foliage::MapleTree_1_1_3::downBuffer, Foliage::MapleTree_1_1_3::leftBuffer, Foliage::MapleTree_1_1_3::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_2_1_0:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_2_1_0::tiles, Foliage::MapleTree_2_1_0::upBuffer, Foliage::MapleTree_2_1_0::downBuffer, Foliage::MapleTree_2_1_0::leftBuffer, Foliage::MapleTree_2_1_0::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_2_1_1:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_2_1_1::tiles, Foliage::MapleTree_2_1_1::upBuffer, Foliage::MapleTree_2_1_1::downBuffer, Foliage::MapleTree_2_1_1::leftBuffer, Foliage::MapleTree_2_1_1::rightBuffer
							);
							break;



						case FoliageIdentity::MapleTree_0_2_0:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_0_2_0::tiles, Foliage::MapleTree_0_2_0::upBuffer, Foliage::MapleTree_0_2_0::downBuffer, Foliage::MapleTree_0_2_0::leftBuffer, Foliage::MapleTree_0_2_0::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_0_2_1:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_0_2_1::tiles, Foliage::MapleTree_0_2_1::upBuffer, Foliage::MapleTree_0_2_1::downBuffer, Foliage::MapleTree_0_2_1::leftBuffer, Foliage::MapleTree_0_2_1::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_0_2_2:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_0_2_2::tiles, Foliage::MapleTree_0_2_2::upBuffer, Foliage::MapleTree_0_2_2::downBuffer, Foliage::MapleTree_0_2_2::leftBuffer, Foliage::MapleTree_0_2_2::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_0_2_3:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_0_2_3::tiles, Foliage::MapleTree_0_2_3::upBuffer, Foliage::MapleTree_0_2_3::downBuffer, Foliage::MapleTree_0_2_3::leftBuffer, Foliage::MapleTree_0_2_3::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_0_2_4:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_0_2_4::tiles, Foliage::MapleTree_0_2_4::upBuffer, Foliage::MapleTree_0_2_4::downBuffer, Foliage::MapleTree_0_2_4::leftBuffer, Foliage::MapleTree_0_2_4::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_1_2_0:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_1_2_0::tiles, Foliage::MapleTree_1_2_0::upBuffer, Foliage::MapleTree_1_2_0::downBuffer, Foliage::MapleTree_1_2_0::leftBuffer, Foliage::MapleTree_1_2_0::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_1_2_1:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_1_2_1::tiles, Foliage::MapleTree_1_2_1::upBuffer, Foliage::MapleTree_1_2_1::downBuffer, Foliage::MapleTree_1_2_1::leftBuffer, Foliage::MapleTree_1_2_1::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_1_2_2:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_1_2_2::tiles, Foliage::MapleTree_1_2_2::upBuffer, Foliage::MapleTree_1_2_2::downBuffer, Foliage::MapleTree_1_2_2::leftBuffer, Foliage::MapleTree_1_2_2::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_1_2_3:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_1_2_3::tiles, Foliage::MapleTree_1_2_3::upBuffer, Foliage::MapleTree_1_2_3::downBuffer, Foliage::MapleTree_1_2_3::leftBuffer, Foliage::MapleTree_1_2_3::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_2_2_0:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_2_2_0::tiles, Foliage::MapleTree_2_2_0::upBuffer, Foliage::MapleTree_2_2_0::downBuffer, Foliage::MapleTree_2_2_0::leftBuffer, Foliage::MapleTree_2_2_0::rightBuffer
							);
							break;
						case FoliageIdentity::MapleTree_2_2_1:
							this->addFoliage( chunk, originX, originY, chunkOffsetX, chunkOffsetY, tileX, tileY,
								Foliage::MapleTree_2_2_1::tiles, Foliage::MapleTree_2_2_1::upBuffer, Foliage::MapleTree_2_2_1::downBuffer, Foliage::MapleTree_2_2_1::leftBuffer, Foliage::MapleTree_2_2_1::rightBuffer
							);
							break;


						default:
							break;
						}

					}
				}
			}
		}

	}


	return chunk;
}


void World::procedurallyGenerate( WorldChunk& worldChunk )
{

	static const std::int64_t OVERWORLD_HEIGHT = 1536; // [!] put in settings
	TileIdentity* proceduralChunk = this->getProceduralChunk( worldChunk.getChunkIndexX(), worldChunk.getChunkIndexY() );
	for ( int row = 0; row < 32; row++ )
	{
		for ( int col = 0; col < 32; col++ )
		{
			//  std::cout << ( int )proceduralChunk[row * 32 + col] << std::endl;
			worldChunk.insert( proceduralChunk[row * 32 + col], 0, worldChunk.getChunkIndexX() * 32 + col, worldChunk.getChunkIndexY() * 32 + row, 1, 1 );
		}
	}

	delete[] proceduralChunk;

	return;



	/*
	static const std::int64_t OVERWORLD_HEIGHT = 1536;

	std::int64_t originX = worldChunk.getChunkIndexX() * 32;
	std::int64_t originY = worldChunk.getChunkIndexY() * 32;

	TileIdentity* chunk = new TileIdentity[32 * 32];



	for ( int row = 0; row < 32; row++ )
	{
		for ( int col = 0; col < 32; col++ )
		{

			if ( row == 0 )
			{
				worldChunk.insert( TileIdentity::Stone, originX + col, originY + row, 1, 1 );
				//chunk[row * 32 + col] = TileIdentity::Stone;

			}
			else if ( col != 5 )
			{
				worldChunk.insert( TileIdentity::Water, originX + col, originY + row, 1, 1 );

				//chunk[row * 32 + col] = TileIdentity::Water;

			}
			else
			{
				worldChunk.insert( TileIdentity::Void, originX + col, originY + row, 1, 1 );

				//chunk[row * 32 + col] = TileIdentity::Void;

			}

			worldChunk.insert( TileIdentity::Torch, originX + col, originY + row, 1, 1 );

		}
	}


	return;
	*/

}



// Collision Detection

SpatialPartition& World::getSpatialPartition()
{
	return this->_spatialParition;
}




void World::loadSpriteTilesTask()
{
	this->_runningLoadSpriteTiles = true;
	while ( this->_runningLoadSpriteTiles )
	{
		this->loadSpriteTiles();
	}
	return;
}



void World::initializeSprites()
{
	std::set<TileIdentity> tileIds;
	for ( std::uint64_t tileId = 0; tileId < ( std::uint64_t )TileIdentity::count; tileId++ )
	{
		for ( std::uint16_t j = 0; j < Settings::World::NUM_CELLS_PER_CHUNK; j++ )
		{
			tileIds.insert( static_cast< TileIdentity >( tileId ) );
		}
	}

	this->addSpriteTiles( tileIds );
	this->_atlas.refresh( tileIds );
	return;
}



void World::loadSpriteTiles()
{
	// refresh/clean up
	std::unique_lock<std::mutex> lockModifyTileSprites( this->_mutexModifyAtlas ); // [!] change to mutexmodifyspritetilesmap
	std::chrono::system_clock::time_point secondsPassed = std::chrono::system_clock::now() + std::chrono::seconds( ( long long )Settings::World::SPRITE_TILE_REFRESH_RATE );
	this->_condModifyAtlas.wait_until( lockModifyTileSprites, secondsPassed );

	std::set<TileIdentity> tileIds;
	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		const Tile* tiles = this->_worldChunks[i].getTiles();

		for ( std::uint16_t j = 0; j < Settings::World::NUM_CELLS_PER_CHUNK; j++ )
		{
			tileIds.insert( tiles[j].getId() );
		}
	}

	this->addSpriteTiles( tileIds );
	this->_atlas.refresh( tileIds );
	return;
}


void World::addSpriteTile( TileIdentity tileId )
{
	// directly adds
	std::lock_guard<std::mutex> lockModifyTileSprites( this->_mutexModifyAtlas );
	this->_atlas.insert( tileId );
	return;
}


void World::addSpriteTiles( std::set<TileIdentity> tileIds )
{
	// directly adds
	this->_atlas.insert( tileIds );
	return;
}


Atlas& World::getAtlas()
{
	return this->_atlas;
}


void World::updateDecals()
{
	this->_atlas.updateDecals();
}


void World::DEBUG_PRINT_TILE_SPRITES()
{
	this->_atlas.print();
	return;
}



std::int16_t World::getRelativeChunkIndex( std::int64_t worldX, std::int64_t worldY ) const
{
	// Get chunk index relative to the focal chunk
	std::int16_t numChunksWidth = 1 + World::_chunkRadius * 2;

	std::int64_t chunkX = ( worldX >= 0 ) ? ( std::int64_t )( worldX / World::_chunkCellSize ) : ( std::int64_t )( ( ( worldX + 1 ) - World::_chunkCellSize ) / World::_chunkCellSize );
	std::int64_t chunkY = ( worldY >= 0 ) ? ( std::int64_t )( worldY / World::_chunkCellSize ) : ( std::int64_t )( ( ( worldY + 1 ) - World::_chunkCellSize ) / World::_chunkCellSize );

	std::int64_t relativeChunkX = chunkX - this->_focalChunkIndexX + World::_chunkRadius;
	std::int64_t relativeChunkY = chunkY - this->_focalChunkIndexY + World::_chunkRadius;

	std::int16_t relativeChunkIndex = ( relativeChunkX >= 0 && relativeChunkX < numChunksWidth&& relativeChunkY >= 0 && relativeChunkY < numChunksWidth ) ? ( relativeChunkY * numChunksWidth + relativeChunkX ) : -1;

	return relativeChunkIndex;
}


std::uint16_t World::getRelativeTileIndex( std::int64_t x, std::int64_t y )
{
	// Get tile index relative to the chunk
	std::int16_t relativeX = ( x >= 0 ) ? ( x % Settings::WorldChunk::SIZE ) : ( ( x + 1 ) % Settings::WorldChunk::SIZE ) + Settings::WorldChunk::SIZE - 1;
	std::int16_t relativeY = ( y >= 0 ) ? ( y % Settings::WorldChunk::SIZE ) : ( ( y + 1 ) % Settings::WorldChunk::SIZE ) + Settings::WorldChunk::SIZE - 1;

	//std::uint16_t relativeTileIndex = relativeY * World::_chunkCellSize + relativeX;
	std::uint16_t relativeTileIndex = ( relativeX >= 0 && relativeX < Settings::WorldChunk::SIZE&& relativeY >= 0 && relativeY < Settings::WorldChunk::SIZE ) ? relativeY * World::_chunkCellSize + relativeX : -1;

	return relativeTileIndex;
}


// Geography
Tile* World::getTile( long double dX, long double dY ) const
{
	// Given worldPosition, return the reference of Tile as a pointer
	// The reason for conditionals is to account for negative quadrant offsets
	// Used for edge meshing of tiles
	std::int64_t x = ( std::int64_t )std::floor( dX );
	std::int64_t y = ( std::int64_t )std::floor( dY );

	std::int16_t relativeChunkIndex = this->getRelativeChunkIndex( x, y );

	// Out of bounds
	if ( relativeChunkIndex < 0 || relativeChunkIndex >= this->_numWorldChunks ) return nullptr;

	std::uint16_t relativeTileIndex = World::getRelativeTileIndex( x, y );

	return &this->_worldChunkPointers[relativeChunkIndex]->getTiles()[relativeTileIndex];
}



Tile* World::getTile( std::int64_t x, std::int64_t y ) const
{
	// Given worldPosition, return the reference of Tile as a pointer
	// The reason for conditionals is to account for negative quadrant offsets
	// Used for edge meshing of tiles
	std::int16_t relativeChunkIndex = this->getRelativeChunkIndex( x, y );

	// Out of bounds
	if ( relativeChunkIndex < 0 || relativeChunkIndex >= this->_numWorldChunks ) return nullptr;

	std::uint16_t relativeTileIndex = World::getRelativeTileIndex( x, y );

	return &this->_worldChunkPointers[relativeChunkIndex]->getTiles()[relativeTileIndex];
}


// Lighting
const Light* World::getLight( std::int64_t x, std::int64_t y ) const
{
	// Given worldPosition, return the reference of Light as a pointer
	// The reason for conditionals is to account for negative quadrant offsets
	// Used for edge meshing of lights
	std::int16_t relativeChunkIndex = this->getRelativeChunkIndex( x, y );

	// Out of bounds
	if ( relativeChunkIndex < 0 || relativeChunkIndex >= this->_numWorldChunks ) return nullptr;

	std::uint16_t relativeTileIndex = World::getRelativeTileIndex( x, y );

	return &this->_worldChunkPointers[relativeChunkIndex]->getLights()[relativeTileIndex];
}


const LightSource* World::getLightSource( std::int64_t x, std::int64_t y ) const
{
	// Given worldPosition, return the reference of Light as a pointer
	// The reason for conditionals is to account for negative quadrant offsets
	// Used for edge meshing of lights
	std::int16_t relativeChunkIndex = this->getRelativeChunkIndex( x, y );

	// Out of bounds
	if ( relativeChunkIndex < 0 || relativeChunkIndex >= this->_numWorldChunks ) return nullptr;

	std::uint16_t relativeTileIndex = World::getRelativeTileIndex( x, y );

	return &this->_worldChunkPointers[relativeChunkIndex]->getLightSources().at( relativeTileIndex );
}


void World::addLight( std::int64_t x, std::int64_t y, const LightSource& lightSource, long double intensity )
{
	// Given worldPosition, return the reference of Light as a pointer
	// The reason for conditionals is to account for negative quadrant offsets

	std::int16_t relativeChunkIndex = this->getRelativeChunkIndex( x, y );

	// Out of bounds
	if ( relativeChunkIndex < 0 || relativeChunkIndex >= this->_numWorldChunks ) return;

	std::uint16_t relativeTileIndex = World::getRelativeTileIndex( x, y );
	Light& light = this->_worldChunkPointers[relativeChunkIndex]->getLights()[relativeTileIndex];
	light.addRed( lightSource.getRed() * intensity );
	light.addGreen( lightSource.getGreen() * intensity );
	light.addBlue( lightSource.getBlue() * intensity );
	light.addAlpha( lightSource.getAlpha() );

	return;
}



void World::resetLighting()
{
	const BoundingBox<long double>& cameraView = this->_camera->getView();
	std::uint16_t tileSize = Settings::Screen::CELL_PIXEL_SIZE;
	std::uint16_t chunkSize = Settings::WorldChunk::SIZE;
	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		const BoundingBox<std::int64_t> chunkBounds = BoundingBox<std::int64_t>( _worldChunks[i].getChunkIndexX() * chunkSize, _worldChunks[i].getChunkIndexY() * chunkSize, chunkSize, chunkSize );
		// [!] add a method to retreive chunkBounds
		// No need to render if camera cannot see it
		if ( cameraView.intersects( chunkBounds ) )
		{
			this->_worldChunks[i].resetLights();
		}
	}

	return;
}





void World::calculateTileRenders()
{
	const BoundingBox<long double>& cameraView = this->_camera->getView();
	std::uint16_t tileSize = Settings::Screen::CELL_PIXEL_SIZE;
	std::uint16_t chunkSize = Settings::WorldChunk::SIZE;
	std::uint16_t numCellsPerChunk = Settings::World::NUM_CELLS_PER_CHUNK;


	std::int8_t buffer = 32; 
	std::int64_t topLeftX = ( std::int64_t )std::floor( cameraView.getX() ) - buffer;
	std::int64_t topLeftY = ( std::int64_t )std::floor( cameraView.getY() ) - buffer;

	std::int64_t bottomRightX = ( std::int64_t )std::ceil( cameraView.getX() + cameraView.getWidth() ) + buffer;
	std::int64_t bottomRightY = ( std::int64_t )std::ceil( cameraView.getY() + cameraView.getHeight() ) + buffer;


	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		const BoundingBox<std::int64_t> chunkBounds = _worldChunks[i].getBounds();

		// No need to render if camera cannot see it
		if ( cameraView.intersects( chunkBounds ) )
		{
			WorldChunk& currWorldChunk = this->_worldChunks[i];
			Tile* tiles = currWorldChunk.getTiles();

			//currWorldChunk.wipeRender();

			for ( std::int16_t i = 0; i < numCellsPerChunk; i++ )
			{
				int x = i % chunkSize;
				int y = i / chunkSize;

				std::int64_t worldX = x + currWorldChunk.getChunkIndexX() * chunkSize;
				std::int64_t worldY = y + currWorldChunk.getChunkIndexY() * chunkSize;

				// Only render lights within camera frame AND within world
				if ( worldX >= topLeftX && worldX <= bottomRightX && worldY >= topLeftY && worldY <= bottomRightY )
				{
					int ne = ( y - 1 ) * chunkSize + ( x + 1 ); // Northerneastern neighbor
					int n = ( y - 1 ) * chunkSize + x; // Northern neighbor
					int nw = ( y - 1 ) * chunkSize + ( x - 1 ); // // Northwestern

					int e = y * chunkSize + ( x + 1 ); // Eastern neighbor
					int c = y * chunkSize + x; // Current tile
					int w = y * chunkSize + ( x - 1 ); // Western neighbor

					int se = ( y + 1 ) * chunkSize + ( x + 1 ); // Southeastern neighbor
					int s = ( y + 1 ) * chunkSize + x; // Southern neighbor
					int sw = ( y + 1 ) * chunkSize + ( x - 1 ); // Southwestern neighbor

					// Quadrant edge merge
					const Tile* neTile = ( x >= chunkSize - 1 || y <= 0 ) ? this->getTile( worldX + 1, worldY - 1 ) : &tiles[ne];
					const Tile* nTile = ( y <= 0 ) ? this->getTile( worldX, worldY - 1 ) : &tiles[n];
					const Tile* nwTile = ( x <= 0 || y <= 0 ) ? this->getTile( worldX - 1, worldY - 1 ) : &tiles[nw];
					const Tile* eTile = ( x >= chunkSize - 1 ) ? this->getTile( worldX + 1, worldY ) : &tiles[e];
					Tile* cTile = &tiles[i];
					const Tile* wTile = ( x <= 0 ) ? this->getTile( worldX - 1, worldY ) : &tiles[w];
					const Tile* seTile = ( x >= chunkSize - 1 || y >= chunkSize - 1 ) ? this->getTile( worldX + 1, worldY + 1 ) : &tiles[se];
					const Tile* sTile = ( y >= chunkSize - 1 ) ? this->getTile( worldX, worldY + 1 ) : &tiles[s];
					const Tile* swTile = ( x <= 0 || y >= chunkSize - 1 ) ? this->getTile( worldX - 1, worldY + 1 ) : &tiles[sw];

					// Out of bounds check
					if ( neTile == nullptr || nTile == nullptr || nwTile == nullptr ||
						eTile == nullptr || wTile == nullptr ||
						seTile == nullptr || sTile == nullptr || swTile == nullptr
						)
					{
						continue;
					}


					/*
					std::uint8_t borders = (
						( ( nwTile == nullptr || nwTile->getId() == TileIdentity::Void ) ? 0 : 1 ) |
						( ( nTile == nullptr || nTile->getId() == TileIdentity::Void ) ? 0 : 1 ) << 1 |
						( ( neTile == nullptr || neTile->getId() == TileIdentity::Void ) ? 0 : 1 ) << 2 |
						( ( eTile == nullptr || eTile->getId() == TileIdentity::Void ) ? 0 : 1 ) << 3 |
						( ( seTile == nullptr || seTile->getId() == TileIdentity::Void ) ? 0 : 1 ) << 4 |
						( ( sTile == nullptr || sTile->getId() == TileIdentity::Void ) ? 0 : 1 ) << 5 |
						( ( swTile == nullptr || swTile->getId() == TileIdentity::Void ) ? 0 : 1 ) << 6 |
						( ( wTile == nullptr || wTile->getId() == TileIdentity::Void ) ? 0 : 1 << 7 )
						);
					*/

					std::uint8_t borders;

					if ( cTile->getComplementary() )
					{

						borders = (
							( ( nwTile == nullptr || nwTile->getId() == TileIdentity::Void || !nwTile->getComplementary() ) ? 0 : 1 ) |
							( ( nTile == nullptr || nTile->getId() == TileIdentity::Void || !nTile->getComplementary() ) ? 0 : 1 ) << 1 |
							( ( neTile == nullptr || neTile->getId() == TileIdentity::Void || !neTile->getComplementary() ) ? 0 : 1 ) << 2 |
							( ( eTile == nullptr || eTile->getId() == TileIdentity::Void || !eTile->getComplementary() ) ? 0 : 1 ) << 3 |
							( ( seTile == nullptr || seTile->getId() == TileIdentity::Void || !seTile->getComplementary() ) ? 0 : 1 ) << 4 |
							( ( sTile == nullptr || sTile->getId() == TileIdentity::Void || !sTile->getComplementary() ) ? 0 : 1 ) << 5 |
							( ( swTile == nullptr || swTile->getId() == TileIdentity::Void || !swTile->getComplementary() ) ? 0 : 1 ) << 6 |
							( ( wTile == nullptr || wTile->getId() == TileIdentity::Void || !wTile->getComplementary() ) ? 0 : 1 << 7 )
							);

						/*
						borders = (
							( ( nwTile == nullptr || nwTile->getId() == TileIdentity::Void ) ? 0 : 1 ) |
							( ( nTile == nullptr || nTile->getId() == TileIdentity::Void ) ? 0 : 1 ) << 1 |
							( ( neTile == nullptr || neTile->getId() == TileIdentity::Void ) ? 0 : 1 ) << 2 |
							( ( eTile == nullptr || eTile->getId() == TileIdentity::Void ) ? 0 : 1 ) << 3 |
							( ( seTile == nullptr || seTile->getId() == TileIdentity::Void ) ? 0 : 1 ) << 4 |
							( ( sTile == nullptr || sTile->getId() == TileIdentity::Void ) ? 0 : 1 ) << 5 |
							( ( swTile == nullptr || swTile->getId() == TileIdentity::Void ) ? 0 : 1 ) << 6 |
							( ( wTile == nullptr || wTile->getId() == TileIdentity::Void ) ? 0 : 1 << 7 )
							);
						*/
					}
					else
					{
						TileIdentity complementaryId = cTile->getId();
						borders = (
							( ( nwTile == nullptr || nwTile->getId() != complementaryId ) ? 0 : 1 ) |
							( ( nTile == nullptr || nTile->getId() != complementaryId ) ? 0 : 1 ) << 1 |
							( ( neTile == nullptr || neTile->getId() != complementaryId ) ? 0 : 1 ) << 2 |
							( ( eTile == nullptr || eTile->getId() != complementaryId ) ? 0 : 1 ) << 3 |
							( ( seTile == nullptr || seTile->getId() != complementaryId ) ? 0 : 1 ) << 4 |
							( ( sTile == nullptr || sTile->getId() != complementaryId ) ? 0 : 1 ) << 5 |
							( ( swTile == nullptr || swTile->getId() != complementaryId ) ? 0 : 1 ) << 6 |
							( ( wTile == nullptr || wTile->getId() != complementaryId ) ? 0 : 1 << 7 )
							);
					}

					cTile->setBorders( borders );

					currWorldChunk.insertTileRenders( *cTile,
						worldX, worldY, 1, 1 );


				}
			}
		}
	}
	return;
}


void World::updateGeographyTask()
{
	/*
	this->_runningUpdateGeography = true;

	while ( this->_runningUpdateGeography )
	{
		this->updateGeography();
	}
	*/
	return;
}

void World::updateGeography()
{
	//std::unique_lock<std::mutex> lockUpdateGeography( this->_mutexUpdateGeography );
	//this->_condRenderWorld.wait( lockUpdateGeography );
	//this->_condUpdateGeography.wait( lockUpdateGeography ); [~!]

	// this->calculateTileRenders(); [~!]

	//std::thread calculateTileRenders0 = std::thread( &World::calculateTileRenders0, this );
	//calculateTileRenders0.join();
	return;
}



void World::calculateLightRenders()
{
	const BoundingBox<long double>& cameraView = this->_camera->getView();
	std::uint16_t tileSize = Settings::Screen::CELL_PIXEL_SIZE;
	std::uint16_t chunkSize = Settings::WorldChunk::SIZE;
	std::uint16_t numCellsPerChunk = Settings::World::NUM_CELLS_PER_CHUNK;


	std::int8_t buffer = 32;

	std::int64_t topLeftX = ( std::int64_t )std::floor( cameraView.getX() ) - buffer;
	std::int64_t topLeftY = ( std::int64_t )std::floor( cameraView.getY() ) - buffer;

	std::int64_t bottomRightX = ( std::int64_t )std::ceil( cameraView.getX() + cameraView.getWidth() ) + buffer;
	std::int64_t bottomRightY = ( std::int64_t )std::ceil( cameraView.getY() + cameraView.getHeight() ) + buffer;


	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		const BoundingBox<std::int64_t> chunkBounds = _worldChunks[i].getBounds();
		WorldChunk& currWorldChunk = this->_worldChunks[i];
		currWorldChunk.wipeLightRender();
		// No need to render if camera cannot see it
		//if ( cameraView.intersects( chunkBounds ) )
		if ( true )
		{
			WorldChunk& currWorldChunk = this->_worldChunks[i];
			Light* lights = currWorldChunk.getLights();

			for ( std::int16_t i = 0; i < numCellsPerChunk; i++ )
			{
				int x = i % chunkSize;
				int y = i / chunkSize;

				std::int64_t worldX = x + currWorldChunk.getChunkIndexX() * chunkSize;
				std::int64_t worldY = y + currWorldChunk.getChunkIndexY() * chunkSize;


				// Only render lights within camera frame AND within world
				//if ( worldX >= topLeftX && worldX <= bottomRightX && worldY >= topLeftY && worldY <= bottomRightY )
				if ( true )
				{
					int ne = ( y - 1 ) * chunkSize + ( x + 1 ); // Northerneastern neighbor
					int n = ( y - 1 ) * chunkSize + x; // Northern neighbor
					int nw = ( y - 1 ) * chunkSize + ( x - 1 ); // // Northwestern

					int e = y * chunkSize + ( x + 1 ); // Eastern neighbor
					int c = y * chunkSize + x; // Current tile
					int w = y * chunkSize + ( x - 1 ); // Western neighbor

					int se = ( y + 1 ) * chunkSize + ( x + 1 ); // Southeastern neighbor
					int s = ( y + 1 ) * chunkSize + x; // Southern neighbor
					int sw = ( y + 1 ) * chunkSize + ( x - 1 ); // Southwestern neighbor


					// Quadrant edge merge
					const Light* neLight = ( x >= chunkSize - 1 || y <= 0 ) ? this->getLight( worldX + 1, worldY - 1 ) : &lights[ne];
					const Light* nLight = ( y <= 0 ) ? this->getLight( worldX, worldY - 1 ) : &lights[n];
					const Light* nwLight = ( x <= 0 || y <= 0 ) ? this->getLight( worldX - 1, worldY - 1 ) : &lights[nw];
					const Light* eLight = ( x >= chunkSize - 1 ) ? this->getLight( worldX + 1, worldY ) : &lights[e];
					const Light* cLight = &lights[i];
					const Light* wLight = ( x <= 0 ) ? this->getLight( worldX - 1, worldY ) : &lights[w];
					const Light* seLight = ( x >= chunkSize - 1 || y >= chunkSize - 1 ) ? this->getLight( worldX + 1, worldY + 1 ) : &lights[se];
					const Light* sLight = ( y >= chunkSize - 1 ) ? this->getLight( worldX, worldY + 1 ) : &lights[s];
					const Light* swLight = ( x <= 0 || y >= chunkSize - 1 ) ? this->getLight( worldX - 1, worldY + 1 ) : &lights[sw];


					// Out of bounds check
					if ( neLight == nullptr || nLight == nullptr || nwLight == nullptr ||
						eLight == nullptr || wLight == nullptr ||
						seLight == nullptr || sLight == nullptr || swLight == nullptr
						)
					{
						continue;
						//return;
					}

					std::uint8_t corner0R = ( std::uint8_t )std::max<std::int32_t>( 0, std::min<std::int32_t>( ( ( cLight->getRed() + nwLight->getRed() + nLight->getRed() + wLight->getRed() ) / 4 ), 255 ) );
					std::uint8_t corner0G = ( std::uint8_t )std::max<std::int32_t>( 0, std::min<std::int32_t>( ( ( cLight->getGreen() + nwLight->getGreen() + nLight->getGreen() + wLight->getGreen() ) / 4 ), 255 ) );
					std::uint8_t corner0B = ( std::uint8_t )std::max<std::int32_t>( 0, std::min<std::int32_t>( ( ( cLight->getBlue() + nwLight->getBlue() + nLight->getBlue() + wLight->getBlue() ) / 4 ), 255 ) );
					std::uint8_t corner0A = ( std::uint8_t )std::max<std::int32_t>( 0, std::min<std::int32_t>( ( ( cLight->getAlpha() + nwLight->getAlpha() + nLight->getAlpha() + wLight->getAlpha() ) / 4 ), 255 ) );

					std::uint8_t corner1R = ( std::uint8_t )std::max<std::int32_t>( 0, std::min<std::int32_t>( ( ( cLight->getRed() + swLight->getRed() + sLight->getRed() + wLight->getRed() ) / 4 ), 255 ) );
					std::uint8_t corner1G = ( std::uint8_t )std::max<std::int32_t>( 0, std::min<std::int32_t>( ( ( cLight->getGreen() + swLight->getGreen() + sLight->getGreen() + wLight->getGreen() ) / 4 ), 255 ) );
					std::uint8_t corner1B = ( std::uint8_t )std::max<std::int32_t>( 0, std::min<std::int32_t>( ( ( cLight->getBlue() + swLight->getBlue() + sLight->getBlue() + wLight->getBlue() ) / 4 ), 255 ) );
					std::uint8_t corner1A = ( std::uint8_t )std::max<std::int32_t>( 0, std::min<std::int32_t>( ( ( cLight->getAlpha() + swLight->getAlpha() + sLight->getAlpha() + wLight->getAlpha() ) / 4 ), 255 ) );

					std::uint8_t corner2R = ( std::uint8_t )std::max<std::int32_t>( 0, std::min<std::int32_t>( ( ( cLight->getRed() + seLight->getRed() + sLight->getRed() + eLight->getRed() ) / 4 ), 255 ) );
					std::uint8_t corner2G = ( std::uint8_t )std::max<std::int32_t>( 0, std::min<std::int32_t>( ( ( cLight->getGreen() + seLight->getGreen() + sLight->getGreen() + eLight->getGreen() ) / 4 ), 255 ) );
					std::uint8_t corner2B = ( std::uint8_t )std::max<std::int32_t>( 0, std::min<std::int32_t>( ( ( cLight->getBlue() + seLight->getBlue() + sLight->getBlue() + eLight->getBlue() ) / 4 ), 255 ) );
					std::uint8_t corner2A = ( std::uint8_t )std::max<std::int32_t>( 0, std::min<std::int32_t>( ( ( cLight->getAlpha() + seLight->getAlpha() + sLight->getAlpha() + eLight->getAlpha() ) / 4 ), 255 ) );

					std::uint8_t corner3R = ( std::uint8_t )std::max<std::int32_t>( 0, std::min<std::int32_t>( ( ( cLight->getRed() + neLight->getRed() + nLight->getRed() + eLight->getRed() ) / 4 ), 255 ) );
					std::uint8_t corner3G = ( std::uint8_t )std::max<std::int32_t>( 0, std::min<std::int32_t>( ( ( cLight->getGreen() + neLight->getGreen() + nLight->getGreen() + eLight->getGreen() ) / 4 ), 255 ) );
					std::uint8_t corner3B = ( std::uint8_t )std::max<std::int32_t>( 0, std::min<std::int32_t>( ( ( cLight->getBlue() + neLight->getBlue() + nLight->getBlue() + eLight->getBlue() ) / 4 ), 255 ) );
					std::uint8_t corner3A = ( std::uint8_t )std::max<std::int32_t>( 0, std::min<std::int32_t>( ( ( cLight->getAlpha() + neLight->getAlpha() + nLight->getAlpha() + eLight->getAlpha() ) / 4 ), 255 ) );

					std::uint32_t corner0 = ( corner0R << 24 ) + ( corner0G << 16 ) + ( corner0B << 8 ) + ( corner0A );
					std::uint32_t corner1 = ( corner1R << 24 ) + ( corner1G << 16 ) + ( corner1B << 8 ) + ( corner1A );
					std::uint32_t corner2 = ( corner2R << 24 ) + ( corner2G << 16 ) + ( corner2B << 8 ) + ( corner2A );
					std::uint32_t corner3 = ( corner3R << 24 ) + ( corner3G << 16 ) + ( corner3B << 8 ) + ( corner3A );

					currWorldChunk.insertLightRenders( corner0, corner1, corner2, corner3, true, worldX, worldY, 1, 1 );

				}
			}

		}
	}



	std::lock_guard<std::mutex> lockUpdateGeography( this->_mutexUpdateGeography );

	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		const BoundingBox<long double>& cameraView = this->_camera->getView();

		const BoundingBox<std::int64_t> chunkBounds = _worldChunks[i].getBounds();

		// No need to render if camera cannot see it
		//if ( cameraView.intersects( chunkBounds ) )
		if ( true )
		{
			WorldChunk& currWorldChunk = this->_worldChunks[i];
			currWorldChunk.swapLightRenders();

		}
	}


	return;
}




void World::revealStatic( LightCastQuadrant<long double>& quadrant, const olc::v2d_generic<long double>& tile, const olc::v2d_generic<long double>& castPosition,
	const olc::v2d_generic<long double>& originPosition, const LightSource& lightSource )
{
	if (
		( quadrant.cardinal == 0 && tile.x == tile.y ) ||
		( quadrant.cardinal == 1 && tile.x == tile.y ) ||
		( quadrant.cardinal == 2 && tile.x == -tile.y ) ||
		( quadrant.cardinal == 3 && tile.x == -tile.y )
		)
	{
		return;
	}

	long double originPosX = originPosition.x;
	long double originPosY = originPosition.y;


	long double rayDistance = std::hypot( castPosition.x - originPosition.x, castPosition.y - originPosition.y );
	long double intensity = std::max<long double>( 0, ( 1.0 - ( rayDistance / ( long double )lightSource.getRadius() ) ) );
	intensity = intensity * 2;

	if ( originPosition.x >= 0 && originPosition.y >= 0 )
	{
		this->addLight( ( std::int64_t )std::ceil( castPosition.x ), ( std::int64_t )std::ceil( castPosition.y ), lightSource, intensity ); // bottomRight [!]
	}
	else if ( originPosition.x < 0 && originPosition.y >= 0 )
	{
		this->addLight( ( std::int64_t )std::floor( castPosition.x ), ( std::int64_t )std::ceil( castPosition.y ), lightSource, intensity );
	}
	else if ( originPosition.x >= 0 && originPosition.y < 0 )
	{
		this->addLight( ( std::int64_t )std::ceil( castPosition.x ), ( std::int64_t )std::floor( castPosition.y ), lightSource, intensity );
	}
	else // if ( originPosition.x < 0 && originPosition.y < 0 )
	{
		this->addLight( ( std::int64_t )std::floor( castPosition.x ), ( std::int64_t )std::floor( castPosition.y ), lightSource, intensity );
	}

	return;
}



bool World::isOpaque( const olc::v2d_generic<long double>& originPosition, const olc::v2d_generic<long double>& castPosition )
{
	std::int64_t tilePosX = ( originPosition.x >= 0 ) ? ( std::int64_t )std::floor( castPosition.x ) : ( std::int64_t )std::ceil( castPosition.x );
	std::int64_t tilePosY = ( originPosition.y >= 0 ) ? ( std::int64_t )std::floor( castPosition.y ) : ( std::int64_t )std::ceil( castPosition.y );

	const Tile* tile = this->getTile( tilePosX, tilePosY );
	if ( tile == nullptr )
	{
		return true;
	}
	else
	{
		return tile->getOpaque();
	}
}


bool World::isTransparent( const olc::v2d_generic<long double>& originPosition, const olc::v2d_generic<long double>& castPosition )
{
	std::int64_t tilePosX = ( originPosition.x >= 0 ) ? ( std::int64_t )std::floor( castPosition.x ) : ( std::int64_t )std::ceil( castPosition.x );
	std::int64_t tilePosY = ( originPosition.y >= 0 ) ? ( std::int64_t )std::floor( castPosition.y ) : ( std::int64_t )std::ceil( castPosition.y );

	const Tile* tile = this->getTile( tilePosX, tilePosY );
	if ( tile == nullptr )
	{
		return false;
	}
	else
	{
		return !tile->getOpaque();
	}
}



void World::scanStatic( LightCastQuadrant<long double>& quadrant, LightCastRow& row, const olc::v2d_generic<long double> originPosition, const LightSource& lightSource )
{
	if ( row.depth >= lightSource.getRadius() )
	{
		return;
	}

	const olc::v2d_generic<long double>* prevTile = nullptr;
	for ( const olc::v2d_generic<long double>& tile : row.getTiles() )
	{
		olc::v2d_generic<long double> castPosition = quadrant.transform( tile );

		if ( this->isOpaque( originPosition, castPosition ) || LightCastQuadrant<long double>::isSymmetric( row, tile ) )
		{
			this->revealStatic( quadrant, tile, castPosition, originPosition, lightSource );
		}
		if ( prevTile != nullptr && this->isOpaque( originPosition, quadrant.transform( *prevTile ) ) && this->isTransparent( originPosition, castPosition ) )
		{
			row.startSlope = LightCastQuadrant<long double>::slopeStatic( tile );
		}
		if ( prevTile != nullptr && this->isTransparent( originPosition, quadrant.transform( *prevTile ) ) && this->isOpaque( originPosition, castPosition ) )
		{
			LightCastRow nextRow = row.getNext();
			nextRow.endSlope = LightCastQuadrant<long double>::slopeStatic( tile );
			this->scanStatic( quadrant, nextRow, originPosition, lightSource );
		}

		prevTile = &tile;
	}


	if ( prevTile != nullptr && this->isTransparent( originPosition, quadrant.transform( *prevTile ) ) ) // [!] [!] [!] access violation!
	{
		LightCastRow nextRow = row.getNext();
		this->scanStatic( quadrant, nextRow, originPosition, lightSource );
	}

	return;
}



void World::emitStaticLightSource( const LightSource& lightSource, std::int64_t x, std::int64_t y )
{
	olc::v2d_generic<long double> originPosition = olc::v2d_generic<long double>{ ( long double )x, ( long double )y };
	std::int64_t originX;
	std::int64_t originY;

	long double rayDistance;
	long double intensity;

	if ( x >= 0 && y >= 0 )
	{
		originX = ( std::int64_t )std::floor( originPosition.x );
		originY = ( std::int64_t )std::floor( originPosition.y );

		rayDistance = std::hypot( ( ( ( long double )originX - x ) + 0.5 ), ( ( ( long double )originY - y ) + 0.5 ) );
		intensity = std::max<long double>( 0, ( 1.0 - ( rayDistance / lightSource.getRadius() ) ) );
	}
	else if ( x < 0 && y >= 0 )
	{
		originX = ( std::int64_t )std::ceil( originPosition.x );
		originY = ( std::int64_t )std::floor( originPosition.y );

		rayDistance = std::hypot( ( ( ( long double )originX - x ) - 0.5 ), ( ( ( long double )originY - y ) + 0.5 ) );
		intensity = std::max<long double>( 0, ( 1.0 - ( rayDistance / lightSource.getRadius() ) ) );
	}
	else if ( x >= 0 && y < 0 )
	{
		originX = ( std::int64_t )std::floor( originPosition.x );
		originY = ( std::int64_t )std::ceil( originPosition.y );

		rayDistance = std::hypot( ( ( ( long double )originX - x ) + 0.5 ), ( ( ( long double )originY - y ) - 0.5 ) );
		intensity = std::max<long double>( 0, ( 1.0 - ( rayDistance / lightSource.getRadius() ) ) );
	}
	else // if ( dX < 0 && dY < 0 )
	{
		originX = ( std::int64_t )std::ceil( originPosition.x );
		originY = ( std::int64_t )std::ceil( originPosition.y );

		rayDistance = std::hypot( ( ( ( long double )originX - x ) - 0.5 ), ( ( ( long double )originY - y ) - 0.5 ) );
		intensity = std::max<long double>( 0, ( 1.0 - ( rayDistance / lightSource.getRadius() ) ) );
	}

	intensity = intensity * 2;

	this->addLight( originX, originY, lightSource, intensity );

	for ( int cardinality = 0; cardinality < 4; cardinality++ )
	{
		LightCastQuadrant<long double> quadrant( cardinality, originX, originY );
		LightCastRow initialRow = LightCastRow( 1.0f, -1.0f, 1.0f );
		this->scanStatic( quadrant, initialRow, originPosition, lightSource );
	}

	return;
}




void World::emitStaticLightSources()
{
	const BoundingBox<long double>& cameraView = this->_camera->getView();

	long double topLeftViewX = cameraView.getX();
	long double topLeftViewY = cameraView.getY();

	long double bottomRightViewX = cameraView.getX() + cameraView.getWidth();
	long double bottomRightViewY = cameraView.getY() + cameraView.getHeight();

	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		WorldChunk& currWorldChunk = this->_worldChunks[i];
		const BoundingBox<std::int64_t> chunkBounds = currWorldChunk.getBounds();
		//if ( cameraView.intersects( chunkBounds ) )
		//{
		std::map<std::uint16_t, LightSource>& currLightSources = currWorldChunk.getLightSources();
		for ( std::map<std::uint16_t, LightSource>::iterator it = currLightSources.begin(); it != currLightSources.end(); it++ )
		{
			std::uint16_t localX = it->first % currWorldChunk.getSize();
			std::uint16_t localY = it->first / currWorldChunk.getSize();

			const LightSource& lightSource = it->second;

			std::int64_t lightSourcePosX = chunkBounds.getX() + localX;
			std::int64_t lightSourcePosY = chunkBounds.getY() + localY;
			std::int16_t lightSourceRadius = lightSource.getRadius();

			// If light is in view of the camera, light it up
			if (
				( lightSourcePosX >= topLeftViewX && lightSourcePosY >= topLeftViewY &&
					lightSourcePosX <= bottomRightViewX && lightSourcePosY <= bottomRightViewY
					) ||
				(
					( ( lightSourcePosX )+lightSourceRadius >= topLeftViewX &&
						( lightSourcePosY )+lightSourceRadius >= topLeftViewY ) &&
					( ( lightSourcePosX )+lightSourceRadius <= bottomRightViewX &&
						( lightSourcePosY )+lightSourceRadius <= bottomRightViewY )
					) ||
				(
					( ( lightSourcePosX )-lightSourceRadius >= topLeftViewX &&
						( lightSourcePosY )+lightSourceRadius >= topLeftViewY ) &&
					( ( lightSourcePosX )-lightSourceRadius <= bottomRightViewX &&
						( lightSourcePosY )+lightSourceRadius <= bottomRightViewY )
					) ||
				(
					( ( lightSourcePosX )+lightSourceRadius >= topLeftViewX &&
						( lightSourcePosY )-lightSourceRadius >= topLeftViewY ) &&
					( ( lightSourcePosX )+lightSourceRadius <= bottomRightViewX &&
						( lightSourcePosY )-lightSourceRadius <= bottomRightViewY )
					) ||
				(
					( ( lightSourcePosX )-lightSourceRadius >= topLeftViewX &&
						( lightSourcePosY )-lightSourceRadius >= topLeftViewY ) &&
					( ( lightSourcePosX )-lightSourceRadius <= bottomRightViewX &&
						( lightSourcePosY )-lightSourceRadius <= bottomRightViewY )
					)
				)
			{
				this->emitStaticLightSource( lightSource, lightSourcePosX, lightSourcePosY );
			}
		}
		//}
	}

	return;
}




void World::emitDynamicLightSource( const LightSource& lightSource, long double x, long double y )
{
	olc::v2d_generic<long double> originPosition = olc::v2d_generic<long double>{ ( long double )x, ( long double )y };
	std::int64_t originX;
	std::int64_t originY;

	long double rayDistance;
	long double intensity;

	if ( x >= 0 && y >= 0 )
	{
		originX = ( std::int64_t )std::floor( originPosition.x );
		originY = ( std::int64_t )std::floor( originPosition.y );

		rayDistance = std::hypot( ( ( ( long double )originX - x ) + 0.5 ), ( ( ( long double )originY - y ) + 0.5 ) );
		intensity = std::max<long double>( 0, ( 1.0 - ( rayDistance / lightSource.getRadius() ) ) );
	}
	else if ( x < 0 && y >= 0 )
	{
		originX = ( std::int64_t )std::ceil( originPosition.x );
		originY = ( std::int64_t )std::floor( originPosition.y );

		rayDistance = std::hypot( ( ( ( long double )originX - x ) - 0.5 ), ( ( ( long double )originY - y ) + 0.5 ) );
		intensity = std::max<long double>( 0, ( 1.0 - ( rayDistance / lightSource.getRadius() ) ) );
	}
	else if ( x >= 0 && y < 0 )
	{
		originX = ( std::int64_t )std::floor( originPosition.x );
		originY = ( std::int64_t )std::ceil( originPosition.y );

		rayDistance = std::hypot( ( ( ( long double )originX - x ) + 0.5 ), ( ( ( long double )originY - y ) - 0.5 ) );
		intensity = std::max<long double>( 0, ( 1.0 - ( rayDistance / lightSource.getRadius() ) ) );
	}
	else // if ( dX < 0 && dY < 0 )
	{
		originX = ( std::int64_t )std::ceil( originPosition.x );
		originY = ( std::int64_t )std::ceil( originPosition.y );

		rayDistance = std::hypot( ( ( ( long double )originX - x ) - 0.5 ), ( ( ( long double )originY - y ) - 0.5 ) );
		intensity = std::max<long double>( 0, ( 1.0 - ( rayDistance / lightSource.getRadius() ) ) );
	}

	intensity = intensity * 2;

	this->addLight( originX, originY, lightSource, intensity );

	for ( int cardinality = 0; cardinality < 4; cardinality++ )
	{
		LightCastQuadrant<long double> quadrant( cardinality, originX, originY );
		LightCastRow initialRow = LightCastRow( 1.0f, -1.0f, 1.0f );
		this->scanStatic( quadrant, initialRow, originPosition, lightSource );
	}

	return;
}


void World::emitPlayerLightSource()
{
	const LightSource& playerLight = LightSource( TileIdentity::Void, 255, 255, 255, 255, 15 );
	this->emitDynamicLightSource( playerLight, this->_player->getCharacter().getCurrPosition().x, this->_player->getCharacter().getCurrPosition().y );
	return;
}




void World::activateCursorLightSource( long double dX, long double dY, std::int64_t radius )
{
	const LightSource& lightSource = LightSource( TileIdentity::Void, 255, 255, 255, 255, ( std::int16_t )radius );

	long double dOriginX;
	long double fractionX = std::modfl( dX, &dOriginX );

	long double dOriginY;
	long double fractionY = std::modfl( dY, &dOriginY );

	olc::v2d_generic<long double> originPosition;
	std::int64_t originX;
	std::int64_t originY;

	long double rayDistance;
	long double intensity;

	if ( dX >= 0 && dY >= 0 )
	{
		originPosition = olc::v2d_generic<long double>{ dX, dY };
		originX = ( std::int64_t )std::floor( originPosition.x );
		originY = ( std::int64_t )std::floor( originPosition.y );

		rayDistance = std::hypot( ( ( ( long double )originX - dX ) + 0.5 ), ( ( ( long double )originY - dY ) + 0.5 ) );
		intensity = std::max<long double>( 0, ( 1.0 - ( rayDistance / radius ) ) );
	}
	else if ( dX < 0 && dY >= 0 )
	{
		originPosition = olc::v2d_generic<long double>{ dOriginX + fractionX, dY };
		originX = ( std::int64_t )std::ceil( originPosition.x - 1 );
		originY = ( std::int64_t )std::floor( originPosition.y );

		rayDistance = std::hypot( ( ( ( long double )originX - dX ) - 0.5 ), ( ( ( long double )originY - dY ) + 0.5 ) );
		intensity = std::max<long double>( 0, ( 1.0 - ( rayDistance / radius ) ) );
	}
	else if ( dX >= 0 && dY < 0 )
	{
		originPosition = olc::v2d_generic<long double>{ dX, dOriginY + fractionY };

		originX = ( std::int64_t )std::floor( originPosition.x );
		originY = ( std::int64_t )std::ceil( originPosition.y - 1 );

		rayDistance = std::hypot( ( ( ( long double )originX - dX ) + 0.5 ), ( ( ( long double )originY - dY ) - 0.5 ) );
		intensity = std::max<long double>( 0, ( 1.0 - ( rayDistance / radius ) ) );
	}
	else // if ( dX < 0 && dY < 0 )
	{
		originPosition = olc::v2d_generic<long double>{ dX, dY };
		originX = ( std::int64_t )std::ceil( originPosition.x - 1 );
		originY = ( std::int64_t )std::ceil( originPosition.y - 1 );

		rayDistance = std::hypot( ( ( ( long double )originX - dX ) - 0.5 ), ( ( ( long double )originY - dY ) - 0.5 ) );
		intensity = std::max<long double>( 0, ( 1.0 - ( rayDistance / radius ) ) );
	}

	intensity = intensity * 2;


	this->addLight( originX, originY, lightSource, intensity );

	for ( int cardinality = 0; cardinality < 4; cardinality++ )
	{
		LightCastQuadrant<long double> quadrant( cardinality, originX, originY );
		LightCastRow initialRow = LightCastRow( 1.0f, -1.0f, 1.0f );
		this->scanStatic( quadrant, initialRow, originPosition, lightSource );
	}
	return;
}



void World::updateLightingTask()
{
	this->_runningUpdateLighting = true;


	while ( this->_runningUpdateLighting )
	{
		this->updateLighting();
	}


	return;
}



void World::updateLighting()
{
	///std::unique_lock<std::mutex> lockUpdateLighting( this->_mutexUpdateLighting );
	///this->_condUpdateLighting.wait( lockUpdateLighting );


	this->resetLighting();
	this->emitStaticLightSources();
	this->emitPlayerLightSource(); // [~!]
	this->calculateLightRenders();

	// swap

	return;
}





void World::setPlayer( Player* player )
{
	this->_player = player;
	return;
}




void World::render()
{
	std::lock_guard<std::mutex> lockUpdateGeography( this->_mutexUpdateGeography );


	this->_camera->renderWorld();





	this->updateDecals(); // [!] only need to be called once after in render thread after sprites have been added in another thread ( or make sprite add thread in render thread and call update decals right after, guaranteed synchronous )

	return;
}