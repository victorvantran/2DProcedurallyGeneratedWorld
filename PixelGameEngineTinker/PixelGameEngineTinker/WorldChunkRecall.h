#pragma once

#include <cstdint>

class WorldChunkRecall
{
private:
	std::uint64_t _index;

	unsigned char* _tilesData;
	std::uint16_t _numBytesTiles;

	std::uint64_t* _paletteData;
	std::uint16_t _numBytesPalette;


public:
	WorldChunkRecall( std::uint64_t index, unsigned char* tilesData, std::uint16_t numBytesTiles, std::uint64_t* paletteData, std::uint16_t numBytesPalette )
		: _index( index ),
		_tilesData( tilesData ), _numBytesTiles( numBytesTiles ),
		_paletteData( paletteData ), _numBytesPalette( numBytesPalette )
	{

	}

	~WorldChunkRecall()
	{
		delete[] this->_tilesData;
		delete[] this->_paletteData;
	}


	std::uint64_t getIndex()
	{
		return this->_index;
	}


	unsigned char* getTilesData()
	{
		return this->_tilesData;
	}


	std::uint16_t getNumBytesTiles()
	{
		return this->_numBytesTiles;
	}


	std::uint64_t* getPaletteData()
	{
		return this->_paletteData;
	}


	std::uint16_t getNumBytesPalette()
	{
		return this->_numBytesPalette;
	}
};