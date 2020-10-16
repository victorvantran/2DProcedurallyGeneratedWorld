#pragma once

#include <cstdint>
#include <map>
#include <vector>
#include <mutex>
#include <string>

#include "Tile.h"
#include "sqlite/sqlite3.h"


#include <iostream>
#include <fstream>


class WorldChunkMemory; // Forward Declaration


class WorldMemoryManager
{
private:
	std::vector<WorldChunkMemory*> _saveWorldChunks;
	std::mutex _saveWordChunksMutex;

	std::vector<std::tuple<int, int>> _loadWorldChunks;
	std::mutex _loadWordChunksMutex;


	//std::vector<> _blob;
	std::mutex _worldDatabaseMutex;


	std::uint8_t _MAX_SAVED_CHUNKS = 50;
private:
	
	static unsigned char copyBits( unsigned char& destination, unsigned char copy, unsigned startIndex, unsigned char endIndex );
	static unsigned char copyBits( unsigned char& destination, unsigned dStartIndex, unsigned char dEndIndex, unsigned char copy, unsigned cStartIndex, unsigned char cEndIndex );



public:
	WorldMemoryManager();
	~WorldMemoryManager();


	void add( WorldChunkMemory* worldChunkMemory );
	void saveWorldGeography();
	void loadWorldGeography();



	static unsigned char* createTilesBlob( Tile* tiles, std::vector<std::uint64_t>& palette );
	static std::uint64_t* createPaletteBlob( std::vector<std::uint64_t>& palette );

};