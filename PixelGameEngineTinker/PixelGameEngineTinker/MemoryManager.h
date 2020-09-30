#pragma once

#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
//#include "Data.h"

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



	static void saveMap( const char* filePath, std::map<std::tuple<int, int>, uint64_t>& map )
	{
		std::ofstream out( filePath, std::ios::out | std::ios::binary );

		if ( out )
		{
			uint64_t mapSize = map.size();
			out.write( reinterpret_cast< const char* >( &mapSize ), 8 );

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

		out.close();

		return;
	}


	static void loadMap( const char* filePath, std::map<std::tuple<int, int>, uint64_t>& map )
	{
		std::ifstream is( filePath );

		if ( is )
		{
			map.clear();

			char* mapSizeBuffer = new char[8];
			is.read( mapSizeBuffer, 8 );
			uint64_t mapSize;
			std::memcpy( &mapSize, mapSizeBuffer + 0, 8 );
			delete[] mapSizeBuffer;

			char* mapItemBuffer = new char[16];
			for ( int i = 0; i < mapSize; i++ )
			{
				is.read( mapItemBuffer, 16 );
				// Since we are filling int and uint64_t perfectly, there is no need to do checking for signed/unsigned situations
				int indexX;
				int indexY;
				uint64_t offset;
				std::memcpy( &indexX, mapItemBuffer + 0, 4 );
				std::memcpy( &indexY, mapItemBuffer + ( sizeof( indexX ) ), sizeof( indexY ) );
				std::memcpy( &offset, mapItemBuffer + ( sizeof( indexX ) + sizeof( indexY ) ), sizeof( offset ) );

				std::tuple<int, int> index = std::tuple<int, int>{ indexX, indexY };
				map.emplace( index, offset );
			}
			delete[] mapItemBuffer;
		}

		is.close();

		return;
	}


	static void  viewMap( std::map<std::tuple<int, int>, uint64_t>& map )
	{
		for ( std::map<std::tuple<int, int>, uint64_t>::iterator it = map.begin(); it != map.end(); it++ )
		{
			std::cout << "[" << std::get<0>( it->first ) << "," << std::get<1>( it->first ) << "] " << ": " << it->second << std::endl;
		}
		return;
	}

	/// offset * ( cellByteSize ) * ( #Cells in a chunk )
};
