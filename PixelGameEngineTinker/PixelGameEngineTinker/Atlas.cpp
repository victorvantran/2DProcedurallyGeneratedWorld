#include "olcPixelGameEngine.h"
#include "Atlas.h"



Atlas::Atlas()
{
}

Atlas::~Atlas()
{
	//delete this->_spriteTileSheet;
	//delete this->_decalTileSheet;
}



Atlas::Atlas( olc::Sprite* spriteSheet, olc::Decal* decalSheet, olc::vi2d atlasDimension, olc::vi2d tileDimension, olc::Decal* decalBackground )
{
	// Temporary constructor to create an atlas. In the future, unpack all the information from one datatype
	this->_atlasDimension = atlasDimension;
	this->_tileDimension = tileDimension;
	this->_decalBackground = decalBackground;
	this->create( spriteSheet, decalSheet );
	//this->fillMapping();
}

void Atlas::create( std::string tilePath )
{
	// Creates a sprite and decal tile sheet given a path to a .png file
	// Points to a sprite and decal tile sheet
	this->_spriteTileSheet = new olc::Sprite( tilePath );
	this->_decalTileSheet = new olc::Decal( this->_spriteTileSheet );
	this->mapping.clear();
	this->fillMapping();
}


void Atlas::create( olc::Sprite* spriteSheet )
{
	// Creates a decal tile sheet given a sprite tile sheet
	// Points to a sprite and decal tile sheet
	this->_spriteTileSheet = spriteSheet;
	this->_decalTileSheet = new olc::Decal( spriteSheet );
	this->mapping.clear();
	this->fillMapping();
}


void Atlas::create( olc::Decal* decalSheet )
{
	// Creates a decal tile sheet given a sprite tile sheet
	// Points to a sprite and decal tile sheet
	this->_spriteTileSheet = nullptr;
	this->_decalTileSheet = decalSheet;
	this->mapping.clear();
	this->fillMapping();
}


void Atlas::create( olc::Sprite* spriteSheet, olc::Decal* decalSheet )
{
	// Creates a decal tile sheet given a sprite tile sheet
	// Points to a sprite and decal tile sheet
	this->_spriteTileSheet = spriteSheet;
	this->_decalTileSheet = decalSheet;
	this->mapping.clear();
	this->fillMapping();
}

	
void Atlas::fillMapping()
{
	for ( int row = 0; row < this->_atlasDimension.y; row++ )
	{
		for ( int column = 0; column < this->_atlasDimension.x; column++ )
		{
			this->mapping.emplace_back( ( int )column * this->_tileDimension.x, ( int )row * this->_tileDimension.y, this->_tileDimension.x, this->_tileDimension.y );
		}

	}

	return;
}

olc::rcode Atlas::loadFromFile( std::string filename )
{
	//
	return olc::FAIL;
}

olc::rcode Atlas::saveToFile( std::string filename )
{
	//
	return olc::FAIL;
}

olc::Sprite* Atlas::getSpriteTileSheet()
{
	// Returns a sprite tilesheet which depicts an arrangement of unique-looking tiles
	return this->_spriteTileSheet;
}

olc::Decal* Atlas::getDecalTileSheet()
{
	// Returns a decal tilesheet which depicts an arrangement of unique-looking tiles
	return this->_decalTileSheet;
}


olc::Decal* Atlas::getDecalBackground()
{
	// Returns a decal background of the world chunk
	return this->_decalBackground;
}


olc::vi2d Atlas::getAtlasDimension()
{
	// Returns the dimension of the atlas used to create an emplace array/mapping
	return this->_atlasDimension;
}


olc::vi2d Atlas::getTileDimension()
{
	// Returns the dimension of the tile used to create an emplace array/mapping
	return this->_tileDimension;
}


std::vector<std::tuple<int32_t, int32_t, int32_t, int32_t>> Atlas::getMapping()
{
	// Returns a mapping that directs a certain tile to a certain point in the tile sheet
	return this->mapping;
}
