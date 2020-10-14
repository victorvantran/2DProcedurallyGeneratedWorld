#pragma once

#include "olcPixelGameEngine.h"


/*
Singleton:
	Single location that loads sprites, decals, and variables
	These loaded resources remains persistent
*/

class Assets
{
private:
	// Make the constructor and destructors private, so the user cannot create an instance(s) of the singleton
	static Assets* _instance;
	
	Assets();
	~Assets();

	std::map<std::string, olc::Sprite*> _mapSprites;
	std::map<std::string, olc::Decal*> _mapDecals;
public:
	// Delete the default copy constructor created by the compiler.
	// We do not want to create shallow copies of this and subsequently reload the assets
	Assets( const Assets& ) = delete;

	// Delete the default assignment operator created by the compiler.
	// We do not want to create shallow re-assignments of member variables (subseqently reloading)
	Assets& operator=( const Assets& ) = delete;

	static Assets& get()
	{
		if ( !_instance )
		{
			_instance = new Assets();
		}
		return *_instance;
	}

	// Here we are loading everything at once. As the game grows larger, make it dynamically load objects as needed
	void loadSprites();
	void loadDecals();

	olc::Sprite* getSprite( std::string name );
	olc::Decal* getDecal( std::string name );

};


Assets* Assets::_instance = nullptr;


Assets::Assets()
{

}


Assets::~Assets()
{
	// Delete all loaded assets


	// Deallocate memory of _mapSprites
	for ( std::map<std::string, olc::Sprite*>::iterator it = this->_mapSprites.begin(); it != this->_mapSprites.end(); it++ )
	{
		delete it->second;
	}

	// Deallocate memory of _mapDecals
	for ( std::map<std::string, olc::Decal*>::iterator it = this->_mapDecals.begin(); it != this->_mapDecals.end(); it++ )
	{
		delete it->second;
	}

}


void Assets::loadSprites()
{
	auto load = [&] ( std::string spriteName, std::string filePath )
	{
		olc::Sprite* sprite = new olc::Sprite( filePath );
		//this->_mapSprites.at(spriteName) = sprite;
		this->_mapSprites.emplace( spriteName, sprite );
	};

	load( "ForestTileMap", "C:\\Users\\Victor\\Desktop\\Tinker\\tilemap_16x16_16x16.png" );
	load( "ForestConsolidationTileMap", "C:\\Users\\Victor\\Desktop\\Tinker\\forest_consolidation_2x2_to_32x32_1x2.png" );

	return;
}


void Assets::loadDecals()
{
	auto load = [&] ( std::string spriteName )
	{
		olc::Decal* decal = new olc::Decal( this->_mapSprites.at(spriteName) );
		this->_mapDecals.emplace( spriteName, decal );
	};

	load( "ForestTileMap" );
	load( "ForestConsolidationTileMap" );

	return;
}



olc::Sprite* Assets::getSprite( std::string name )
{
	return this->_mapSprites.at( name );
}


olc::Decal* Assets::getDecal( std::string name )
{
	return this->_mapDecals.at( name );
}

