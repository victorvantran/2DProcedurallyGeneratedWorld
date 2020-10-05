#pragma once

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <map>

#include "Tile.h"
#include "WorldChunk.h"

// [!] note endianness

class MemoryManager
{
private:
	MemoryManager();

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
		const uint32_t worldChunkNumBytes = 32 * 32 * 8; // [!] should be pre calcualted
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
			uint64_t chunkByteOffset = newChunkOffset * worldChunkNumBytes; // [!] overflow possibility
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
			worldData.seekp( sizeof( numChunks ) + ( numChunks * worldChunkNumBytes ), std::ios::beg ); // [!!!]

			// Fill zeros in
			uint64_t numFillChunks = newChunkOffset - numChunks;
			unsigned char zeroBuffer[worldChunkNumBytes]{};
			for ( uint64_t i = 0; i < numFillChunks; i++ )
			{
				worldData.write( reinterpret_cast< const char* >( &zeroBuffer ), worldChunkNumBytes );
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
		uint32_t worldChunkNumBytes = 32 * 32 * 8; // [!] should be pre calcualted
		uint64_t numChunks = 0;
		Tile* tileIds = worldChunk.getTiles();
		uint64_t loadChunkByteOffset = loadChunkOffset * worldChunkNumBytes;

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

	/// offset * ( cellByteSize ) * ( #Cells in a chunk )
};
