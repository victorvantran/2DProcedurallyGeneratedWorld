#pragma once

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <map>

#include "Settings.h"
#include "Tile.h"
#include "WorldChunk.h"

#include "sqlite/sqlite3.h"



// [!] note endianness

class MemoryManager
{
private:
	MemoryManager();

	static const std::uint32_t WORLD_CHUNK_NUM_BYTES = Settings::MemoryManager::WORLD_CHUNK_NUM_BYTES;

	// Byte Address
	static unsigned char getBit( unsigned char byte, unsigned char index )
	{
		return ( ( byte >> index ) & 0b00000001 );
	}


	unsigned char getBits( unsigned char byte, unsigned startIndex, unsigned char endIndex )
	{
		if ( startIndex < 0 || endIndex < 0 || startIndex > 7 || endIndex > 7 || endIndex < startIndex )
		{
			return 0b00000000;
		}

		unsigned char bits = byte >> startIndex;
		unsigned char mask = ( ( 0b00000001 << ( endIndex - startIndex + 1 ) ) - 1 );

		return bits & mask;
	}


	static unsigned char setBit( unsigned char& byte, unsigned char index )
	{
		return byte = ( byte | ( 0b00000001 << index ) );
	}


	static unsigned char clearBit( unsigned char& byte, unsigned char index )
	{
		return byte = ( byte & ~( 0b00000001 << index ) );
	}

	static unsigned char copyBits( unsigned char& destination, unsigned char copy, unsigned startIndex, unsigned char endIndex )
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


	static unsigned char copyBits( unsigned char& destination, unsigned dStartIndex, unsigned char dEndIndex, unsigned char copy, unsigned cStartIndex, unsigned char cEndIndex )
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


	static void printByte( unsigned char byte )
	{
		std::cout << getStringByte( byte ) << std::endl;
		return;
	}


	static std::string getStringByte( unsigned char byte )
	{
		if ( byte > 0 )
		{
			return getStringByte( byte >> 0b00000001 ) + std::string( std::to_string( byte % 2 ) );
		}
		else
		{
			return std::string( "0b" );
		}
	}


public:

	//[!] care for signed/unsigned


	/*
	static void saveData( const char* filePath, const Data& data )
	{
		std::ofstream out( filePath, std::ios::out | std::ios::binary );

		if ( out )
		{
			for ( int i = 0; i < data.getSize(); i++ )
			{
				// TileContents
				int id = data.getInteger( i );
				out.write( reinterpret_cast< const char* >( &id ), 1 );

			}
		}

		out.close();

		return;
	}



	static void loadData( const char* filePath, Data& data )
	{
		std::ifstream is( filePath );

		char* tileBuffer = new char[1];
		for ( int i = 0; i < data.getSize(); i++ )
		{
			is.read( tileBuffer, 1 );
			int id = tileBuffer[0];
			data.setInteger( i, id );
		}
		delete[] tileBuffer;

		is.close();

		return;
	}
	*/


	// WorldMapping
	//
	// Maps the worldChunk index to the displacement of where its data is
	// 8 bytes for the number of possible items ( uint64_t )
	// For each item:
	//	4 bytes for indexX ( int )
	//	4 bytes for indexY ( int )
	//	8 bytes for the offset ( uint64_t )
	//
	// There are a possible of 2^64 chunks in a world with this current map implementation
	//
	// But the cache memory storage to hold the map, say 1 MB, would only hold ( ( 1000*1000 - 8 )/16 ) = 62499 chunks
	//
	// We can repartition tthe map to point to smaller submaps of a certain size, making a tree of maps to solve memory issue ( Over-engineering this project )
	//
	// This means that we would have to load in worldMappings during gameplay, but that is negligble to the amount of loading from world chunk data
	//


	// Tiles
	// Since each worldChunk usually has a small number of unique blocks, we can palette them
	// Each world chunk will hold a dictionary that stores a small "key" (may be a few bits ( but at max ) #bits = log2(chunkWidth*chunkHeight) ) to the value representing the actual id
	// Pros:
	//	less memory
	//	ability to scale tileID
	// Cons:
	//	more code overhead
	//	more cpu ( bit shifting, etc. )
	//	portability of code


	static void saveWorldChunkCrude( const char* filePath, WorldChunk& worldChunk, uint64_t newChunkOffset )
	{
		uint64_t numChunks = 0;
		Tile* tileIds = worldChunk.getTiles();

		// Open world.dat (should already exist)
		std::fstream worldData( filePath, std::ios::out | std::ios::in | std::ios::binary );
		// If the world is new and the file is blank, fill the first 8 bytes with numChunks as cache
		if ( worldData.peek() == std::fstream::traits_type::eof() )
		{
			worldData.clear();
			worldData.write( reinterpret_cast< const char* >( &numChunks ), sizeof( numChunks ) );
		}
		// Read the num of chunks in the file and assign numChunks to it
		else
		{
			unsigned char numChunkCacheBuffer[sizeof( numChunks )];
			worldData.read( reinterpret_cast< char* >( numChunkCacheBuffer ), sizeof( numChunks ) );
			std::memcpy( &numChunks, numChunkCacheBuffer, sizeof( numChunks ) );
			worldData.clear();
		}

		// If the data for a chunk has already been written
		if ( ( newChunkOffset + 1 ) <= numChunks )
		{
			uint64_t chunkByteOffset = newChunkOffset * MemoryManager::WORLD_CHUNK_NUM_BYTES; // [!] overflow possibility
			worldData.seekp( sizeof( newChunkOffset ) + chunkByteOffset, std::ios::beg );

			// Write the worldChunk tiles
			uint64_t* tileIdsBuffer = new uint64_t[32 * 32]; // [!] chunkSize
			for ( int i = 0; i < 32 * 32; i++ )
			{
				tileIdsBuffer[i] = tileIds[i].getId();
			}
			worldData.write( reinterpret_cast< const char* >( &( *tileIdsBuffer ) ), 32 * 32 * 8 ); // [!] chunkSize
			worldData.flush();
			delete[] tileIdsBuffer;
		}
		// If the data for a chunk is not written and may need to fill in data
		else if ( ( newChunkOffset + 1 ) > numChunks )
		{
			// Go to the numChunks
			worldData.seekp( sizeof( numChunks ) + ( numChunks * MemoryManager::WORLD_CHUNK_NUM_BYTES ), std::ios::beg ); // [!!!]

			// Fill zeros in
			uint64_t numFillChunks = newChunkOffset - numChunks;
			unsigned char zeroBuffer[MemoryManager::WORLD_CHUNK_NUM_BYTES]{};
			for ( uint64_t i = 0; i < numFillChunks; i++ )
			{
				worldData.write( reinterpret_cast< const char* >( &zeroBuffer ), MemoryManager::WORLD_CHUNK_NUM_BYTES );
				worldData.flush();
			}

			// Write the worldChunk tiles data PlaceHolder
			uint64_t* tileIdsBuffer = new uint64_t[32 * 32]; // [!] chunkSize
			for ( int i = 0; i < 32 * 32; i++ )
			{
				tileIdsBuffer[i] = tileIds[i].getId();
			}
			worldData.write( reinterpret_cast< const char* >( &( *tileIdsBuffer ) ), 32 * 32 * 8 ); // [!] chunkSize
			worldData.flush();
			delete[] tileIdsBuffer;

			// Update the cached numChunks
			numChunks = newChunkOffset + 1;
			worldData.seekp( 0, std::ios::beg );
			worldData.write( reinterpret_cast< const char* >( &numChunks ), sizeof( numChunks ) );
			worldData.flush();
		}

		worldData.close();
		return;
	}


	static void loadWorldChunkCrude( const char* filePath, WorldChunk& worldChunk, uint64_t loadChunkOffset )
	{
		uint64_t numChunks = 0;
		Tile* tileIds = worldChunk.getTiles();
		uint64_t loadChunkByteOffset = loadChunkOffset * MemoryManager::WORLD_CHUNK_NUM_BYTES;

		// Open world.dat ( should already exist )
		std::fstream worldData( filePath, std::ios::out | std::ios::in | std::ios::binary );
		// If the world is new and the file is blank, fill the first 8 bytes with numChunks as cache
		if ( worldData.peek() == std::fstream::traits_type::eof() )
		{
			std::cout << "empty file" << std::endl;
			worldData.clear();
			return;
		}
		// Read the num of chunks in the file and assign numChunks to it
		else
		{
			char numChunkCacheBuffer[sizeof( numChunks )];
			worldData.read( reinterpret_cast< char* >( numChunkCacheBuffer ), sizeof( numChunks ) );
			worldData.clear();
			std::memcpy( &numChunks, numChunkCacheBuffer, sizeof( numChunks ) );
		}

		// Cannot load from data that is not written yet
		if ( numChunks <= loadChunkOffset )
		{
			std::cout << "numChunks < loadChunkOffset" << std::endl;
			return;
		}

		// Seek the byteoffset
		worldData.seekg( sizeof( numChunks ) + loadChunkByteOffset, worldData.beg );

		// Load tiles into tilearray
		char* tileIdsBuffer = new char[32 * 32 * 8];
		worldData.read( tileIdsBuffer, 32 * 32 * 8 );
		worldData.clear();
		
		uint64_t tileId = 0;
		uint64_t offset = 0;

		for ( int i = 0; i < 32 * 32; i++ ) // [!] chunkSize
		{
			offset = i * 8;
			std::memcpy( &tileId, &( *tileIdsBuffer ) + offset, 8 ); // [!] is tilesIdBuffer constant?
			if ( tileId != 0 )
			{
				worldChunk.insert(
					worldChunk.getChunkIndexX() * worldChunk.getSize() + ( i % worldChunk.getSize() ),
					worldChunk.getChunkIndexY() * worldChunk.getSize() + ( i / worldChunk.getSize() ),
					1,
					1,
					tileId
				);
			}
		}
		delete[] tileIdsBuffer;

		worldData.close();
		return;
	}


	static void saveWorldAtlas( const char* filePath, std::map<std::tuple<int, int>, uint64_t>& map )
	{
		// Overwties/saves the entire map
		std::ofstream out( filePath, std::ios::out | std::ios::binary );

		if ( out )
		{
			for ( std::map<std::tuple<int, int>, uint64_t>::iterator it = map.begin(); it != map.end(); it++ )
			{
				// Since we are filling int and uint64_t perfectly, there is no need to do checking for signed/unsigned situations
				int indexX = std::get<0>( it->first );
				int indexY = std::get<1>( it->first );
				uint64_t offset = it->second;

				out.write( reinterpret_cast< const char* >( &indexX ), sizeof( indexX ) );  // 4 bytes
				out.write( reinterpret_cast< const char* >( &indexY ), sizeof( indexY ) );  // 4 bytes
				out.write( reinterpret_cast< const char* >( &offset ), sizeof( offset ) );  // 8 bytes
			}
		}
		out.flush();
		out.close();

		return;
	}


	static void updateWorldAtlas( const char* filePath, int newChunkIndexX, int newChunkIndexY, uint64_t mapSize )
	{
		// Appends new data ( index, mapSize to be used as offset ) to the worldMap dictionary file 
		std::ofstream out( filePath, std::ios::app | std::ios::binary );
		if ( out )
		{
			out.write( reinterpret_cast< const char* >( &newChunkIndexX ), 4 );  // 4 bytes
			out.write( reinterpret_cast< const char* >( &newChunkIndexY ), 4 );  // 4 bytes
			out.write( reinterpret_cast< const char* >( &mapSize ), 8 );  // 8 bytes
		}
		out.flush();
		out.close();

		return;
	}


	static void loadWorldAtlas( const char* filePath, std::map<std::tuple<int, int>, uint64_t>& map )
	{
		std::ifstream is( filePath, std::ios::in | std::ios::binary );

		if ( is )
		{
			map.clear();
			char* mapItemBuffer = new char[16];

			while ( is.read( mapItemBuffer, 16 ) )
			{
				int indexX;
				int indexY;
				uint64_t offset;
				std::memcpy( &indexX, mapItemBuffer + 0, 4 );
				std::memcpy( &indexY, mapItemBuffer + 4, 4 );
				std::memcpy( &offset, mapItemBuffer + 8, 8 );

				std::tuple<int, int> index = std::tuple<int, int>{ indexX, indexY };
				map.emplace( index, offset );
			}
			delete[] mapItemBuffer;

		}
		is.clear();
		is.close();

		return;
	}


	static void  viewWorldAtlas( std::map<std::tuple<int, int>, uint64_t>& map )
	{
		for ( std::map<std::tuple<int, int>, uint64_t>::iterator it = map.begin(); it != map.end(); it++ )
		{
			std::cout << "[" << std::get<0>( it->first ) << "," << std::get<1>( it->first ) << "] " << ": " << it->second << std::endl;
		}
		return;
	}


	static void saveTiles( const char* filePath, Tile* tile, int numTiles )
	{
		std::ofstream out( filePath, std::ios::out | std::ios::binary );

		if ( out )
		{
			for ( int i = 0; i < numTiles; i++ )
			{
				// TileContents
				int id = tile[i].getId();
				out.write( reinterpret_cast< const char* >( &id ), 1 );

			}
		}

		out.close();
		return;
	}






	///



	static void initializeWorldDatabase()
	{
		// Create a world file if one does not exist already.
		sqlite3* database = NULL;
		sqlite3_stmt* statement = NULL;
		const char* command = NULL;
		char* errorMessage = NULL;
		int rc;

		// Creates a world.db file if one does not exist
		sqlite3_open( "world.db", &database ); // [!] SETTINGS world.db name

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
			throw std::exception( errorMessage );
		}

		sqlite3_close( database );
		return;
	}


	static unsigned char* createTilesBlob( Tile* tiles, std::vector<std::uint64_t>& palette )
	{
		// Create tilesBlob which is an array of bytes representing the tiles of the worldChunk. However, the tiles are mapped to smaller bit keys in order to save memory.

		std::uint16_t numTiles = Settings::World::NUM_CELLS_PER_CHUNK;
		std::uint8_t numBitsPerSegment = Settings::MemoryManager::NUM_BITS_PER_SEGMENT;

		std::uint16_t numUniqueKeys = palette.size(); // max is 32*32
		std::uint8_t numBitsPerKey = ceil( log2( numUniqueKeys ) ); // max is 10 bits
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


	static std::uint64_t* createPaletteBlob( std::vector<std::uint64_t>& palette )
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


	static void saveWorldChunk( WorldChunk& worldChunk )
	{
		// Saves the worldChunk into the database based on its index. The worldChunk will have tileData and paletteData used to recreate the world when loaded back in.

		std::uint16_t numTiles = Settings::World::NUM_CELLS_PER_CHUNK;

		// Get tiles and palette from the worldChunk
		Tile* tiles = worldChunk.getTiles();
		std::vector<std::uint64_t> palette = worldChunk.getPalette();

		// Get numBytes for worldChunkBlob and paletteBlob
		std::uint16_t numUniqueKeys = palette.size(); // max is 32*32
		std::uint8_t numBitsPerKey = ceil( log2( numUniqueKeys ) ); // max is 10 bits
		std::uint32_t tilesBlobNumBits = ( numBitsPerKey * numTiles ); // max is 32*32*64
		std::uint16_t tilesBlobNumBytes = ( int )ceil( ( float )tilesBlobNumBits / 8.0f );
		std::uint16_t paletteBlobNumBytes = palette.size() * 8;

		// Create worldChunkBlob and paletteBlob
		unsigned char* tilesBlob = createTilesBlob( tiles, palette );
		std::uint64_t* paletteBlob = createPaletteBlob( palette );

		// Get the worldChunk index used as the primary composite key to our table
		int chunkIndexX = worldChunk.getChunkIndexX();
		int chunkIndexY = worldChunk.getChunkIndexY();


		// Mutex_close
		// Insert into database
		sqlite3* database = NULL;
		sqlite3_stmt* statement = NULL;
		const char* command = NULL;
		char* errorMessage = NULL;
		int rc;

		sqlite3_open( "world.db", &database );

		command =
			"INSERT INTO world_geography( chunk_index_x, chunk_index_y, tiles, palette )\n"
			"VALUES ( ?1, ?2, ?3, ?4 )\n"

			"ON CONFLICT( chunk_index_x, chunk_index_y ) DO UPDATE SET tiles = ?3, palette = ?4;";
		rc = sqlite3_prepare_v2( database, command, -1, &statement, NULL );
		rc = sqlite3_bind_int( statement, 1, chunkIndexX );
		rc = sqlite3_bind_int( statement, 2, chunkIndexY );
		rc = sqlite3_bind_blob( statement, 3, tilesBlob, tilesBlobNumBytes, SQLITE_STATIC );
		rc = sqlite3_bind_blob( statement, 4, paletteBlob, paletteBlobNumBytes, SQLITE_STATIC );
		rc = sqlite3_step( statement );

		sqlite3_close( database );
		// Mutex_open

		// Delete allocated memory
		delete[] tilesBlob;
		delete[] paletteBlob;
		return;
	}


	static void loadWorldChunk( WorldChunk& worldChunk )
	{
		// Loads the worldChunk from the database using tileData and paletteData to recreate
		sqlite3* database = NULL;
		sqlite3_stmt* statement = NULL;
		const char* command = NULL;
		char* errorMessage = NULL;
		int rc;

		// Get the worldChunk index used as the primary composite key to our table
		int chunkIndexX = worldChunk.getChunkIndexX();
		int chunkIndexY = worldChunk.getChunkIndexY();

		// Mutex close
		sqlite3_open( "world.db", &database );

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
			// throw
			std::cout << "Row does not exist" << std::endl;
			return;
		}

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

		sqlite3_close( database );
		// Mutex open

		// Load the tiles into the worldChunk
		loadTiles( tilesData, numBytesTiles, paletteData, numUniqueKeys, worldChunk.getTiles() ); // [!]

		delete[] tilesData;
		delete[] paletteData;
		return;
	}


	static void loadTiles( unsigned char* tilesData, std::uint16_t tilesNumBytes, std::uint64_t* paletteData, std::uint16_t numUniqueKeys, Tile* tiles )
	{
		// Load from tilesData which is an array of bytes representing the tiles of the worldChunk given by tilesBlob. We need unpack this condensed "bit-addressable" 
		// array of keys to the 64bit tileId values.

		std::uint16_t numTiles = Settings::World::NUM_CELLS_PER_CHUNK;
		std::uint8_t numBitsPerSegment = Settings::MemoryManager::NUM_BITS_PER_SEGMENT;

		// Uncondense the keys, map it to the correct tileId using paletteData, and set the tile to the respective id.
		uint16_t key = 0;
		std::uint8_t accumulator = 0;
		std::uint8_t numBitsPerKey = ceil( log2( numUniqueKeys ) ); // max is 10 bits
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
				copyBits( accumulator, 0, numBitsPerKey,
					tilesData[byteOffset], ( ( numBitsPerSegment - remainingSegmentBits ) % numBitsPerSegment ), ( ( numBitsPerSegment - remainingSegmentBits ) % numBitsPerSegment ) + numBitsPerKey );

				key = accumulator;
				tiles[i].setId( paletteData[key] );

				remainingSegmentBits -= numBitsPerKey;
			}
			// Otherwise, we need to splice the key into multiple bytes
			else
			{
				// First Splice
				std::uint8_t remainingKeyBits = numBitsPerKey;
				std::uint8_t firstSplice = remainingSegmentBits;

				// Copy bits into the first accumulator and initialize the key to that value
				copyBits( accumulator, 0, firstSplice,
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
				tiles[i].setId( paletteData[key] );
			}

			if ( remainingSegmentBits == 0 )
			{
				remainingSegmentBits = numBitsPerSegment;
				byteOffset -= 1;
			}
		}

		return;
	}

};



