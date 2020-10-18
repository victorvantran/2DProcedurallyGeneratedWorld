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
	_prevFocalChunkIndexX( 0 ), _prevFocalChunkIndexY( 0 ),
	_currFocalChunkIndexX( 0 ), _currFocalChunkIndexY( 0 ),
	_worldChunks( new WorldChunk[( 1 + 2 * ( _chunkRadius ) ) * ( 1 + 2 * ( _chunkRadius ) )] )
{

}


World::~World()
{
	delete[] this->_worldChunks;

	this->stopWorldMemorySystem();
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


void World::updateFocalChunk( BoundingBox<float> focalPoint )
{
	std::lock_guard<std::mutex> lockLoad( this->_loadWordChunksMutex );

	this->_focalChunk = focalPoint;
	return;
}


void World::insert( int x, int y, int width, int height, uint64_t id )
{
	std::lock_guard<std::mutex> lockModify( this->_modifyWordChunksMutex );

	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		if ( BoundingBox<int>( x, y, width, height ).intersects( this->_worldChunks[i].getTileRendersRoot().getBounds() ) )
		{
			this->_worldChunks[i].insert( x, y, width, height, id );
		}
	}
	return;
}


void World::remove( int x, int y, int width, int height, uint64_t id )
{
	std::lock_guard<std::mutex> lockModify( this->_modifyWordChunksMutex );

	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		if ( BoundingBox<int>( x, y, width, height ).intersects( this->_worldChunks[i].getTileRendersRoot().getBounds() ) )
		{
			this->_worldChunks[i].remove( x, y, width, height, id );
		}
	}
	return;
}


void World::delimitWorldChunk( WorldChunk& worldChunk, int chunkIndexX, int chunkIndexY )
{
	// Clears the worldChunk, updates its proper index, and wipes the render in preparation to load in new data	and new render
	//std::lock_guard<std::mutex> lockModify( this->_modifyWordChunksMutex );

	worldChunk.clear();
	worldChunk.delimit( chunkIndexX, chunkIndexY );
	worldChunk.wipeRender();
	return;
}


void World::initializeDelimits( const BoundingBox<float>& cameraView )
{
	// Based on the initial camera's position, delimit the surrounding worldChunks
	// Load in camera
	int cameraIndexX = cameraView.getCenterX() >= 0 ? ( int )( cameraView.getCenterX() / this->_chunkCellSize ) : ( int )( ( cameraView.getCenterX() - this->_chunkCellSize ) / this->_chunkCellSize );
	int cameraIndexY = cameraView.getCenterY() >= 0 ? ( int )( cameraView.getCenterY() / this->_chunkCellSize ) : ( int )( ( cameraView.getCenterY() - this->_chunkCellSize ) / this->_chunkCellSize );

	for ( int x = 0; x < this->_numChunkWidth; x++ )
	{
		for ( int y = 0; y < this->_numChunkHeight; y++ )
		{
			int deltaIndexX = x - this->_chunkRadius;
			int deltaIndexY = y - this->_chunkRadius;

			int newChunkIndexX = cameraIndexX + deltaIndexX;
			int newChunkIndexY = cameraIndexY + deltaIndexY;

			this->delimitWorldChunk( this->_worldChunks[y * this->_numChunkWidth + x], newChunkIndexX, newChunkIndexY );
		}
	}

	this->_prevFocalChunkIndexX = cameraIndexX;
	this->_prevFocalChunkIndexY = cameraIndexY;

	return;
}


void World::initializeWorldChunks() // [!] fix
{
	for ( int i = 0; i < this->_numWorldChunks; i++ )
	{
		this->_worldChunks[i].construct();
		//if ( !this->loadWorldChunk( &this->_worldChunks[i] ) )
		//{
			// [!] Procedural generation here
		//}
	}

	return;
}


WorldChunk* World::getWorldChunks()
{
	return this->_worldChunks;
}


WorldChunk& World::getWorldChunk( int x, int y )
{
	return this->_worldChunks[y * this->_numChunkWidth + x];
}


int World::getChunkRadius() const
{
	return this->_chunkRadius;
}


int World::getNumWorldChunks() const
{
	return this->_numWorldChunks;
}


int World::getNumChunkWidth() const
{
	return this->_numChunkWidth;
}


int World::getNumChunkHeight() const
{
	return this->_numChunkHeight;
}



// Memory
void World::startWorldMemorySystem()
{
	this->_saveWorldGeographyThread = std::thread( &World::saveWorldGeographyTask, this );
	this->_loadWorldGeographyThread = std::thread( &World::loadWorldGeographyTask, this );

	return;
}


void World::stopWorldMemorySystem()
{
	this->_runningSaveWorldGeography = false;
	this->_saveWorldGeographyThread.join();

	this->_runningLoadWorldGeography = false;
	this->_loadWorldGeographyThread.join();
	return;
}


void World::addMemory( WorldChunkMemory* worldChunkMemory )
{
	std::lock_guard<std::mutex> lockSave( this->_saveWordChunksMutex );

	// Overloaded
	if ( this->_saveWorldChunks.size() >= _MAX_SAVED_CHUNKS )
	{
		delete worldChunkMemory;
		return;
	}


	this->_saveWorldChunks.push_back( worldChunkMemory );
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
	std::lock_guard<std::mutex> lockSave( this->_saveWordChunksMutex );

	if ( this->_saveWorldChunks.empty() )
	{
		return;
	}

	//std::lock_guard<std::mutex> lockDatabase( _worldDatabaseMutex );
	std::unique_lock<std::mutex> lockDatabase( _worldDatabaseMutex );

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

		int chunkIndexX = worldChunkMemory->getChunkIndexX();
		int chunkIndexY = worldChunkMemory->getChunkIndexY();

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


unsigned char* World::createTilesBlob( Tile* tiles, std::vector<std::uint64_t>& palette )
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
		paletteMap.emplace( palette[i], i );
	}

	// Create a condensed array of bytes that holds numTiles of n-bit keys 
	unsigned char* tilesBlob = new unsigned char[tilesNumBytes]();
	uint16_t byteOffset = tilesNumBytes - 1;
	std::uint8_t remainingSegmentBits = numBitsPerSegment;
	for ( int i = numTiles - 1; i >= 0; i-- )
	{
		uint64_t tileId = tiles[i].getId();
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


std::uint64_t* World::createPaletteBlob( std::vector<std::uint64_t>& palette )
{
	// Create a paletteBlob which is an array of bytes.
	std::uint16_t numUniqueKeys = palette.size(); // max is 32*32
	std::uint64_t* paletteBlob = new uint64_t[numUniqueKeys]();

	for ( std::uint16_t i = 0; i < numUniqueKeys; i++ )
	{
		paletteBlob[i] = palette[i];
	}

	return paletteBlob;
}


void World::loadWorldGeographyTask()
{
	this->_runningLoadWorldGeography = true;
	while ( this->_runningLoadWorldGeography )
	{
		this->loadWorldGeography( this->_focalChunk ); // [!] camera
	}

	return;
}


void World::loadWorldGeography( const BoundingBox<float>& cameraFocalPoint )
{
	std::lock_guard<std::mutex> lockLoad( this->_loadWordChunksMutex );

	// Check to see if there is an update
	int cameraIndexX = cameraFocalPoint.getCenterX() >= 0 ? ( int )( cameraFocalPoint.getCenterX() / this->_chunkCellSize ) : ( int )( ( cameraFocalPoint.getCenterX() - this->_chunkCellSize ) / this->_chunkCellSize );
	int cameraIndexY = cameraFocalPoint.getCenterY() >= 0 ? ( int )( cameraFocalPoint.getCenterY() / this->_chunkCellSize ) : ( int )( ( cameraFocalPoint.getCenterY() - this->_chunkCellSize ) / this->_chunkCellSize );
	if ( this->_prevFocalChunkIndexX == cameraIndexX && this->_prevFocalChunkIndexY == cameraIndexY )
	{
		return;
	}

	// Delimit
	std::vector<std::tuple<std::uint64_t, int, int>> chunkRecalls = this->delimitWorldChunks( cameraFocalPoint );
	if ( chunkRecalls.empty() )
	{
		return;
	}

	for ( int i = 0; i < chunkRecalls.size(); i++ )
	{
		std::uint64_t index = std::get<0>( chunkRecalls[i] );
		int chunkIndexX = std::get<1>( chunkRecalls[i] );
		int chunkIndexY = std::get<2>( chunkRecalls[i] );
		WorldChunk* worldChunk = &this->_worldChunks[index];

		this->delimitWorldChunk( *worldChunk, chunkIndexX, chunkIndexY );

	}

	this->_prevFocalChunkIndexX = cameraIndexX;
	this->_prevFocalChunkIndexY = cameraIndexY;

	std::vector<WorldChunkRecall*> worldChunkRecalls;

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


		worldChunkRecalls.push_back( new WorldChunkRecall( index, tilesData, numBytesTiles, paletteData, numBytesPalette ) );
	}

	sqlite3_close( database );
	lockDatabase.unlock();

	for ( int i = 0; i < worldChunkRecalls.size(); i++ )
	{
		std::uint64_t index = worldChunkRecalls[i]->getIndex();
		unsigned char* tilesData = worldChunkRecalls[i]->getTilesData();
		std::uint16_t numBytesTiles = worldChunkRecalls[i]->getNumBytesTiles();
		std::uint64_t* paletteData = worldChunkRecalls[i]->getPaletteData();
		std::uint16_t numBytesPalette = worldChunkRecalls[i]->getNumBytesPalette();

		WorldChunk& worldChunk = this->_worldChunks[index];
		this->loadTiles( worldChunk, tilesData, numBytesTiles, paletteData, numBytesPalette );

		delete worldChunkRecalls[i];
	}
	worldChunkRecalls.clear();

	return;
}


std::vector<std::tuple<std::uint64_t, int, int>> World::delimitWorldChunks( const BoundingBox<float>& cameraView )
{
	// Build recall Vector
	std::vector<std::tuple<std::uint64_t, int, int>> chunkRecalls;

	// Based on the updated camera's position, delimit the surrounding worldChunks
	int cameraIndexX = cameraView.getCenterX() >= 0 ? ( int )( cameraView.getCenterX() / this->_chunkCellSize ) : ( int )( ( cameraView.getCenterX() - this->_chunkCellSize ) / this->_chunkCellSize );
	int cameraIndexY = cameraView.getCenterY() >= 0 ? ( int )( cameraView.getCenterY() / this->_chunkCellSize ) : ( int )( ( cameraView.getCenterY() - this->_chunkCellSize ) / this->_chunkCellSize );

	// No need to delimit if the camera index has not changed
	if ( this->_prevFocalChunkIndexX == cameraIndexX && this->_prevFocalChunkIndexY == cameraIndexY )
	{
		return chunkRecalls;
	}

	for ( std::uint64_t x = 0; x < this->_numChunkWidth; x++ )
	{
		for ( std::uint64_t y = 0; y < this->_numChunkHeight; y++ )
		{
			WorldChunk& worldChunk = this->_worldChunks[y * this->_numChunkWidth + x];
			int worldChunkIndexX = worldChunk.getChunkIndexX();
			int worldChunkIndexY = worldChunk.getChunkIndexY();

			// If the worldChunk is out of range relative to the camera
			if ( std::abs( cameraIndexX - worldChunkIndexX ) > this->_chunkRadius || std::abs( cameraIndexY - worldChunkIndexY ) > this->_chunkRadius )
			{
				// Add to memory
				std::thread addMemoryThread( &World::addMemory, this, worldChunk.createMemory() );
				addMemoryThread.detach();

				// Add to recall
				int newChunkIndexX = cameraIndexX + -( worldChunkIndexX - this->_prevFocalChunkIndexX );
				int newChunkIndexY = cameraIndexY + -( worldChunkIndexY - this->_prevFocalChunkIndexY );
				chunkRecalls.push_back( std::tuple<std::uint64_t, int, int>( y * this->_numChunkWidth + x, newChunkIndexX, newChunkIndexY ) );
			}
		}
	}

	return chunkRecalls;
}


void World::loadTiles( WorldChunk& worldChunk, unsigned char* tilesData, std::uint16_t tilesNumBytes, std::uint64_t* paletteData, std::uint16_t numBytesPalette )
{
	// Load from tilesData which is an array of bytes representing the tiles of the worldChunk given by tilesBlob. We need unpack this condensed "bit-addressable" 
	// array of keys to the 64bit tileId values.

	std::uint16_t numTiles = Settings::World::NUM_CELLS_PER_CHUNK;
	std::uint8_t numBitsPerSegment = Settings::MemoryManager::NUM_BITS_PER_SEGMENT;
	std::uint16_t numUniqueKeys = numBytesPalette / sizeof( std::uint64_t );

	// Uncondense the keys, map it to the correct tileId using paletteData, and set the tile to the respective id.
	uint16_t key = 0;
	std::uint8_t accumulator = 0;
	std::uint8_t numBitsPerKey = ( ceil( log2( numUniqueKeys ) ) ) > 0 ? ( ceil( log2( numUniqueKeys ) ) ) : 1; // [!] // max is 10 bits
	uint16_t byteOffset = tilesNumBytes - 1;
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
			worldChunk.insert( worldChunk.getChunkIndexX() * 32 + ( i % 32 ), worldChunk.getChunkIndexY() * 32 + ( i / 32 ), 1, 1, paletteData[key] );

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
			worldChunk.insert( worldChunk.getChunkIndexX() * 32 + ( i % 32 ), worldChunk.getChunkIndexY() * 32 + ( i / 32 ), 1, 1, paletteData[key] );
		}

		if ( remainingSegmentBits == 0 )
		{
			remainingSegmentBits = numBitsPerSegment;
			byteOffset -= 1;
		}
	}

	return;
}