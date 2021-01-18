#include "Settings.h"
#include "World.h"
#include "WorldChunkMemory.h"






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
	_camera( nullptr )
{

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
	}

	sqlite3_close( database );

	// Allow the tileSprites to refresh
	this->_condModifyAtlas.notify_one();
	return;
}





















// Modification



void World::insert( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	// Pointer to a method is slightly different than a pointer to a function
	//World::funcType method = World::insertMethods[0];
	//( this->*method )( tileId, x, y, width, height );
	std::lock_guard<std::mutex> lockModify( this->_modifyWorldChunksMutex );

	std::thread addSpriteTileThread( &World::addSpriteTile, this, tileId );
	addSpriteTileThread.join(); // [change]

	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		if ( BoundingBox<std::int64_t>( x, y, width, height ).intersects( this->_worldChunks[i].getTileRendersRoot().getBounds() ) )
		{
			this->_worldChunks[i].insert( tileId, x, y, width, height );
		}
	}
	return;
}



void World::remove( TileIdentity id, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	// Remove a tile ( or tiles ) from the proper world chunk.

	std::lock_guard<std::mutex> lockModify( this->_modifyWorldChunksMutex );

	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		if ( BoundingBox<std::int64_t>( x, y, width, height ).intersects( this->_worldChunks[i].getTileRendersRoot().getBounds() ) )
		{
			this->_worldChunks[i].remove( id, x, y, width, height );
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
	this->_loadSpriteTilesThread = std::thread( &World::loadSpriteTilesTask, this );
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
	//this->_condModifyAtlas.notify_one();
	this->_loadSpriteTilesThread.join();



	this->_condRenderWorld.notify_all();
	this->_condUpdateGeography.notify_all();
	this->_condUpdateLighting.notify_all();

	this->_runningUpdateGeography = false;
	//this->_condRenderWorld.notify_one();
	this->_updateGeographyThread.join();

	this->_runningUpdateLighting = false;
	//this->_condRenderWorld.notify_one();
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
		std::cout << "(" << this->_saveWorldChunks[i]->getChunkIndexX() << "," << this->_saveWorldChunks[i]->getChunkIndexY() << ")" << std::endl;
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

	sqlite3_open( "./world.db", &database );

	for ( int i = 0; i < chunkRecalls.size(); i++ )
	{
		int index = std::get<0>( chunkRecalls[i] );
		int chunkIndexX = std::get<1>( chunkRecalls[i] );
		int chunkIndexY = std::get<2>( chunkRecalls[i] );


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

		delete[] tilesData;
		delete[] paletteData;
	}
	worldChunkRecalls.clear();

	// Allow the tileSprites to refresh
	this->_condModifyAtlas.notify_one();

	return;
}


void World::updateWorldChunkRelativity( const BoundingBox<long double>& focalPoint )
{
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
	std::set<uint64_t> historyTileIds;

	// Uncondense the keys, map it to the correct tileId using paletteData, and set the tile to the respective id.
	uint16_t key = 0;
	std::uint8_t accumulator = 0;
	std::uint8_t numBitsPerKey = ( ceil( log2( numUniqueKeys ) ) ) > 0 ? ( ceil( log2( numUniqueKeys ) ) ) : 1; // [!] // max is 10 bits
	uint16_t byteOffset = numBytesTiles - 1;
	std::uint8_t remainingSegmentBits = numBitsPerSegment;
	std::string keyDisplacements;
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
			if ( historyTileIds.find( tileId ) == historyTileIds.end() ) // [!] Make sure that render quad tree jsut continues if it can't find the sprite because it may try to render before the tileId is added
			{
				std::thread addSpriteTileThread( &World::addSpriteTile, this, static_cast< TileIdentity >( tileId ) );
				addSpriteTileThread.join();
				historyTileIds.insert( tileId );
			}
			//worldChunk.insertTile( worldChunk.getChunkIndexX() * 32 + ( i % 32 ), worldChunk.getChunkIndexY() * 32 + ( i / 32 ), 1, 1, tileId );

			if ( static_cast< TileIdentity >( tileId ) != TileIdentity::Void )
			{
				worldChunk.insert( static_cast< TileIdentity >( tileId ), worldChunk.getChunkIndexX() * 32 + ( i % 32 ), worldChunk.getChunkIndexY() * 32 + ( i / 32 ), 1, 1 );
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
			if ( historyTileIds.find( tileId ) == historyTileIds.end() ) // [!] Make sure that render quad tree jsut continues if it can't find the sprite because it may try to render before the tileId is added
			{
				std::thread addSpriteTileThread( &World::addSpriteTile, this, static_cast< TileIdentity >( tileId ) );
				addSpriteTileThread.join();
				historyTileIds.insert( tileId );
			}
			//worldChunk.insertTile( worldChunk.getChunkIndexX() * 32 + ( i % 32 ), worldChunk.getChunkIndexY() * 32 + ( i / 32 ), 1, 1, tileId );

			if ( static_cast< TileIdentity >( tileId ) != TileIdentity::Void )
			{
				worldChunk.insert( static_cast< TileIdentity >( tileId ), worldChunk.getChunkIndexX() * 32 + ( i % 32 ), worldChunk.getChunkIndexY() * 32 + ( i / 32 ), 1, 1 );
			}

		}

		if ( remainingSegmentBits == 0 )
		{
			remainingSegmentBits = numBitsPerSegment;
			byteOffset -= 1;
		}
	}

	return;
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


void World::loadSpriteTiles()
{
	// refresh/clean up
	std::unique_lock<std::mutex> lockModifyTileSprites( this->_mutexModifyAtlas ); // [!] change to mutexmodifyspritetilesmap
	std::chrono::system_clock::time_point secondsPassed = std::chrono::system_clock::now() + std::chrono::seconds( ( long long )Settings::World::SPRITE_TILE_REFRESH_RATE );
	this->_condModifyAtlas.wait_until( lockModifyTileSprites, secondsPassed );
	//this->_condModifyAtlas.wait( lockModifyTileSprites );

	std::set<TileIdentity> tileIds;
	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		const Tile* tiles = this->_worldChunks[i].getTiles();

		for ( std::uint16_t j = 0; j < Settings::World::NUM_CELLS_PER_CHUNK; j++ )
		{
			tileIds.insert( tiles[j].getId() );
		}
	}

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




std::int16_t World::getRelativeChunkIndex( std::int64_t x, std::int64_t y, std::int64_t focalChunkIndexX, std::int64_t focalChunkIndexY )
{
	// Get chunk index relative to the focal chunk
	std::int16_t numChunksWidth = 1 + World::_chunkRadius * 2;

	std::int64_t chunkIndexX = ( x >= 0 ) ? ( std::int64_t )( x / World::_chunkCellSize ) : ( std::int64_t )( ( ( x + 1 ) - World::_chunkCellSize ) / World::_chunkCellSize );
	std::int64_t chunkIndexY = ( y >= 0 ) ? ( std::int64_t )( y / World::_chunkCellSize ) : ( std::int64_t )( ( ( y + 1 ) - World::_chunkCellSize ) / World::_chunkCellSize );

	std::int16_t relativeChunkIndex = ( ( chunkIndexY - focalChunkIndexY ) + World::_chunkRadius ) * numChunksWidth + ( ( chunkIndexX - focalChunkIndexX ) + World::_chunkRadius );

	return relativeChunkIndex;
}



std::uint16_t World::getRelativeTileIndex( std::int64_t x, std::int64_t y )
{
	// Get tile index relative to the chunk
	std::int16_t relativeX = ( x >= 0 ) ? ( x % World::_chunkCellSize ) : ( ( x + 1 ) % World::_chunkCellSize ) + World::_chunkCellSize - 1;
	std::int16_t relativeY = ( y >= 0 ) ? ( y % World::_chunkCellSize ) : ( ( y + 1 ) % World::_chunkCellSize ) + World::_chunkCellSize - 1;
	std::uint16_t relativeTileIndex = relativeY * World::_chunkCellSize + relativeX;

	return relativeTileIndex;
}


// Geography
const Tile* World::getTile( std::int64_t x, std::int64_t y ) const
{
	// Given worldPosition, return the reference of Tile as a pointer
	// The reason for conditionals is to account for negative quadrant offsets
	// Used for edge meshing of tiles
	std::int16_t relativeChunkIndex = World::getRelativeChunkIndex( x, y, this->_focalChunkIndexX, this->_focalChunkIndexY );

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
	std::int16_t relativeChunkIndex = World::getRelativeChunkIndex( x, y, this->_focalChunkIndexX, this->_focalChunkIndexY );

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
	std::int16_t relativeChunkIndex = World::getRelativeChunkIndex( x, y, this->_focalChunkIndexX, this->_focalChunkIndexY );

	// Out of bounds
	if ( relativeChunkIndex < 0 || relativeChunkIndex >= this->_numWorldChunks ) return nullptr;

	std::uint16_t relativeTileIndex = World::getRelativeTileIndex( x, y );

	return &this->_worldChunkPointers[relativeChunkIndex]->getLightSources().at( relativeTileIndex );
}


void World::addLight( std::int64_t x, std::int64_t y, const LightSource& lightSource, long double intensity )
{
	// Given worldPosition, return the reference of Light as a pointer
	// The reason for conditionals is to account for negative quadrant offsets

	std::int16_t relativeChunkIndex = World::getRelativeChunkIndex( x, y, this->_focalChunkIndexX, this->_focalChunkIndexY );

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

	std::int64_t topLeftX = ( std::int64_t )std::floor( cameraView.getX() );
	std::int64_t topLeftY = ( std::int64_t )std::floor( cameraView.getY() );

	std::int64_t bottomRightX = ( std::int64_t )std::ceil( cameraView.getX() + cameraView.getWidth() );
	std::int64_t bottomRightY = ( std::int64_t )std::ceil( cameraView.getY() + cameraView.getHeight() );


	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		const BoundingBox<std::int64_t> chunkBounds = _worldChunks[i].getBounds();

		// No need to render if camera cannot see it
		if ( cameraView.intersects( chunkBounds ) )
		{
			WorldChunk& currWorldChunk = this->_worldChunks[i];
			Tile* tiles = currWorldChunk.getTiles();

			currWorldChunk.wipeRender();

			for ( std::int16_t i = 0; i < numCellsPerChunk; i++ )
			{
				int x = i % chunkSize;
				int y = i / chunkSize;

				std::int64_t worldPosX = x + currWorldChunk.getChunkIndexX() * chunkSize;
				std::int64_t worldPosY = y + currWorldChunk.getChunkIndexY() * chunkSize;

				// Only render lights within camera frame AND within world
				if ( worldPosX >= topLeftX && worldPosX <= bottomRightX && worldPosY >= topLeftY && worldPosY <= bottomRightY )
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
					const Tile* neTile = ( x >= chunkSize - 1 || y <= 0 ) ? this->getTile( worldPosX + 1, worldPosY - 1 ) : &tiles[ne];
					const Tile* nTile = ( y <= 0 ) ? this->getTile( worldPosX, worldPosY - 1 ) : &tiles[n];
					const Tile* nwTile = ( x <= 0 || y <= 0 ) ? this->getTile( worldPosX - 1, worldPosY - 1 ) : &tiles[nw];
					const Tile* eTile = ( x >= chunkSize - 1 ) ? this->getTile( worldPosX + 1, worldPosY ) : &tiles[e];
					Tile* cTile = &tiles[i];
					const Tile* wTile = ( x <= 0 ) ? this->getTile( worldPosX - 1, worldPosY ) : &tiles[w];
					const Tile* seTile = ( x >= chunkSize - 1 || y >= chunkSize - 1 ) ? this->getTile( worldPosX + 1, worldPosY + 1 ) : &tiles[se];
					const Tile* sTile = ( y >= chunkSize - 1 ) ? this->getTile( worldPosX, worldPosY + 1 ) : &tiles[s];
					const Tile* swTile = ( x <= 0 || y >= chunkSize - 1 ) ? this->getTile( worldPosX - 1, worldPosY + 1 ) : &tiles[sw];

					// Out of bounds check
					if ( neTile == nullptr || nTile == nullptr || nwTile == nullptr ||
						eTile == nullptr || wTile == nullptr ||
						seTile == nullptr || sTile == nullptr || swTile == nullptr
						)
					{
						continue;
					}

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


					cTile->setBorders( borders );
					// cTile->setBorders( std::rand() % 47 );
					
					currWorldChunk.insertTileRenders( cTile->getId(), cTile->getBordersDecalIndex(),
						worldPosX, worldPosY, 1, 1 );
				}
			}
		}
	}
	return;
}


void World::updateGeographyTask()
{
	this->_runningUpdateGeography = true;

	while ( this->_runningUpdateGeography )
	{
		this->updateGeography();
	}

	return;
}

void World::updateGeography()
{
	std::unique_lock<std::mutex> lockUpdateGeography( this->_mutexUpdateGeography );
	//this->_condRenderWorld.wait( lockUpdateGeography );
	this->_condUpdateGeography.wait( lockUpdateGeography );

	this->calculateTileRenders();

	return;
}


void World::calculateLightRenders()
{
	const BoundingBox<long double>& cameraView = this->_camera->getView();
	std::uint16_t tileSize = Settings::Screen::CELL_PIXEL_SIZE;
	std::uint16_t chunkSize = Settings::WorldChunk::SIZE;
	std::uint16_t numCellsPerChunk = Settings::World::NUM_CELLS_PER_CHUNK;

	std::int64_t topLeftX = ( std::int64_t )std::floor( cameraView.getX() );
	std::int64_t topLeftY = ( std::int64_t )std::floor( cameraView.getY() );

	std::int64_t bottomRightX = ( std::int64_t )std::ceil( cameraView.getX() + cameraView.getWidth() );
	std::int64_t bottomRightY = ( std::int64_t )std::ceil( cameraView.getY() + cameraView.getHeight() );

	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		const BoundingBox<std::int64_t> chunkBounds = _worldChunks[i].getBounds();

		// No need to render if camera cannot see it
		if ( cameraView.intersects( chunkBounds ) )
		{
			WorldChunk& currWorldChunk = this->_worldChunks[i];
			Light* lights = currWorldChunk.getLights();

			for ( std::int16_t i = 0; i < numCellsPerChunk; i++ )
			{
				int x = i % chunkSize;
				int y = i / chunkSize;

				std::int64_t worldPosX = x + currWorldChunk.getChunkIndexX() * chunkSize;
				std::int64_t worldPosY = y + currWorldChunk.getChunkIndexY() * chunkSize;


				// Only render lights within camera frame AND within world
				if ( worldPosX >= topLeftX && worldPosX <= bottomRightX && worldPosY >= topLeftY && worldPosY <= bottomRightY )
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
					const Light* neLight = ( x >= chunkSize - 1 || y <= 0 ) ? this->getLight( worldPosX + 1, worldPosY - 1 ) : &lights[ne];
					const Light* nLight = ( y <= 0 ) ? this->getLight( worldPosX, worldPosY - 1 ) : &lights[n];
					const Light* nwLight = ( x <= 0 || y <= 0 ) ? this->getLight( worldPosX - 1, worldPosY - 1 ) : &lights[nw];
					const Light* eLight = ( x >= chunkSize - 1 ) ? this->getLight( worldPosX + 1, worldPosY ) : &lights[e];
					const Light* cLight = &lights[i];
					const Light* wLight = ( x <= 0 ) ? this->getLight( worldPosX - 1, worldPosY ) : &lights[w];
					const Light* seLight = ( x >= chunkSize - 1 || y >= chunkSize - 1 ) ? this->getLight( worldPosX + 1, worldPosY + 1 ) : &lights[se];
					const Light* sLight = ( y >= chunkSize - 1 ) ? this->getLight( worldPosX, worldPosY + 1 ) : &lights[s];
					const Light* swLight = ( x <= 0 || y >= chunkSize - 1 ) ? this->getLight( worldPosX - 1, worldPosY + 1 ) : &lights[sw];


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

					currWorldChunk.insertLightRenders( corner0, corner1, corner2, corner3, true, worldPosX, worldPosY, 1, 1 );
				}
			}
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
		originX = ( std::int64_t )std::ceil( originPosition.x - 1 );
		originY = ( std::int64_t )std::floor( originPosition.y );

		rayDistance = std::hypot( ( ( ( long double )originX - x ) - 0.5 ), ( ( ( long double )originY - y ) + 0.5 ) );
		intensity = std::max<long double>( 0, ( 1.0 - ( rayDistance / lightSource.getRadius() ) ) );
	}
	else if ( x >= 0 && y < 0 )
	{
		originX = ( std::int64_t )std::floor( originPosition.x );
		originY = ( std::int64_t )std::ceil( originPosition.y - 1 );

		rayDistance = std::hypot( ( ( ( long double )originX - x ) + 0.5 ), ( ( ( long double )originY - y ) - 0.5 ) );
		intensity = std::max<long double>( 0, ( 1.0 - ( rayDistance / lightSource.getRadius() ) ) );
	}
	else // if ( dX < 0 && dY < 0 )
	{
		originX = ( std::int64_t )std::ceil( originPosition.x - 1 );
		originY = ( std::int64_t )std::ceil( originPosition.y - 1 );

		rayDistance = std::hypot( ( ( ( long double )originX - x ) - 0.5 ), ( ( ( long double )originY - y ) - 0.5 ) );
		intensity = std::max<long double>( 0, ( 1.0 - ( rayDistance / lightSource.getRadius() ) ) );
	}

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
				( lightSourcePosX >= topLeftViewX && lightSourcePosY >= topLeftViewY  &&
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
	std::unique_lock<std::mutex> lockUpdateLighting( this->_mutexUpdateLighting );
	this->_condUpdateLighting.wait( lockUpdateLighting );

	this->resetLighting();
	this->emitStaticLightSources();
	this->calculateLightRenders();

	//this->_condUpdateWorld.notify_one();

	return;
}



void World::render()
{
	std::lock_guard<std::mutex> lockUpdateGeography( this->_mutexUpdateGeography );
	std::lock_guard<std::mutex> lockUpdateLighting( this->_mutexUpdateLighting );

	this->_camera->renderWorld();
	this->updateDecals();

	this->_condUpdateGeography.notify_one();
	this->_condUpdateLighting.notify_one();
	return;
}