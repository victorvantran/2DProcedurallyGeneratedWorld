#include "olcPixelGameEngine.h"
#include "Atlas.h"


namespace olc
{
	Atlas::Atlas()
	{

	}

	Atlas::~Atlas()
	{
		delete this->_spriteTileSheet;
		delete this->_decalTileSheet;
	}

	void Atlas::create( std::string tilePath )
	{
		/// Creates a sprite and decal tile sheet given a path to a .png file
		/// Points to a sprite and decal tile sheet
		this->_spriteTileSheet = new Sprite( tilePath );
		this->_decalTileSheet = new Decal( this->_spriteTileSheet );
		this->mapping.clear();
	}


	void Atlas::create( Sprite* spriteSheet )
	{
	/// Creates a decal tile sheet given a sprite tile sheet
	/// Points to a sprite and decal tile sheet
		this->_spriteTileSheet = spriteSheet;
		this->_decalTileSheet = new Decal( spriteSheet );
		this->mapping.clear();
	}


	void Atlas::create( Decal* decalSheet )
	{
	/// Creates a decal tile sheet given a sprite tile sheet
	/// Points to a sprite and decal tile sheet
		this->_spriteTileSheet = nullptr;
		this->_decalTileSheet = decalSheet;
		this->mapping.clear();
	}


	void Atlas::create( Sprite* spriteSheet, Decal* decalSheet )
	{
		/// Creates a decal tile sheet given a sprite tile sheet
		/// Points to a sprite and decal tile sheet
		this->_spriteTileSheet = spriteSheet;
		this->_decalTileSheet = decalSheet;
		this->mapping.clear();
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

	Sprite* Atlas::getSpriteTileSheet()
	{
	/// Returns a sprite tilesheet which depicts an arrangement of unique-looking tiles
		return this->_spriteTileSheet;
	}

	Decal* Atlas::getDecalTileSheet()
	{
		// Returns a sprite tilesheet which depicts an arrangement of unique-looking tiles
		return this->_decalTileSheet;
	}


	std::vector<std::tuple<int32_t, int32_t, int32_t, int32_t>> Atlas::getMapping()
	{
	// Returns a mapping that directs a certain tile to a certain point in the tile sheet
		return this->mapping;
	}

}