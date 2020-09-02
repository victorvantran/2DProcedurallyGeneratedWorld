#include "olcPixelGameEngine.h"
#include "Atlas.h"


namespace olc
{
	Atlas::Atlas()
	{

	}

	Atlas::~Atlas()
	{

	}

	void Atlas::create( Sprite* tileSheet )
	{
	//
		this->_spriteTileSheet = tileSheet;
		this->_mapping.clear();
	}

	
	rcode Atlas::loadFromFile( std::string filename )
	{
	//
		return FAIL;
	}

	rcode Atlas::saveToFile( std::string filename )
	{
	//
		return FAIL;
	}

	Sprite* Atlas::getTileSheet()
	{
		// returns a tilesheet which depicts an arrangement of unique-looking tiles
		return this->_spriteTileSheet;
	}


	std::vector<std::tuple<int32_t, int32_t, int32_t, int32_t>> Atlas::getMapping()
	{
		// returns a mapping that directs a certain tile to a certain point in the tile sheet
		return this->_mapping;
	}

}