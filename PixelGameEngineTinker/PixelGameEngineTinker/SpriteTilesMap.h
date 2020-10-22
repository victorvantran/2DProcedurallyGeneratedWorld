#pragma once

#include <cstdint>
#include <map>
#include <set>
#include <mutex>

#include "olcPixelGameEngine.h"
#include "Assets.h"

#include <iostream>

class SpriteTilesMap
{
private:
	std::mutex _mutexAccessSpriteTilesMap;
	std::map < std::uint64_t, std::tuple<olc::Sprite*, olc::Decal*>> _spriteTilesMap;

public:
	SpriteTilesMap()
	{

	}


	~SpriteTilesMap()
	{
		// Deallocate all Sprites and Decals
		std::map<std::uint64_t, std::tuple<olc::Sprite*, olc::Decal*>>::iterator it;
		for ( it = this->_spriteTilesMap.begin(); it != this->_spriteTilesMap.end(); )
		{
			delete std::get<0>( it->second );
			delete std::get<1>( it->second );
			this->_spriteTilesMap.erase( it++ );
		}
	}


	void insert( std::uint64_t tileId )
	{
		// Load a sprite from the SQLite database and insert it into this spriteTileMap.
		// Only create sprite and not decal because this will be called on a separate thread
		// that does not own the OpenGL context

		std::lock_guard lockAcesssSpriteTilesMap( this->_mutexAccessSpriteTilesMap );
		if ( this->_spriteTilesMap.find( tileId ) == this->_spriteTilesMap.end() )
		{
			olc::Sprite* spriteTile = Assets::loadSpriteTile( tileId );
			this->_spriteTilesMap.insert( std::pair<std::uint64_t, std::tuple<olc::Sprite*, olc::Decal*>>
				(
					tileId,
					std::tuple<olc::Sprite*, olc::Decal*>( spriteTile, nullptr )
				)
			);
		}

		return;
	}


	void refresh( std::set<std::uint64_t> tileIds )
	{
		// Refresh/clean up any extraneous sprites and decals that are not needed for the current render frame,
		// keeping the necessary sprites/decals based on tileIds

		std::lock_guard<std::mutex> lockAccessSpriteTilesMap( this->_mutexAccessSpriteTilesMap );

		std::map<std::uint64_t, std::tuple<olc::Sprite*, olc::Decal*>>::iterator it;
		for ( it = this->_spriteTilesMap.begin(); it != this->_spriteTilesMap.end();  )
		{
			if ( tileIds.find( it->first ) == tileIds.end() )
			{
				delete std::get<0>( it->second );
				delete std::get<1>( it->second );
				this->_spriteTilesMap.erase( it++ );
			}
			else
			{
				it++;
			}
		}

		return;
	}

	
	void updateDecals()
	{
		// Create decals within the spriteTilesMap, given a sprite has been created from a different thread. 
		// Creating tiles needs to be called on the thread that owns the openGL context.

		std::lock_guard<std::mutex> lockAccessSpriteTilesMap( this->_mutexAccessSpriteTilesMap );

		std::map<std::uint64_t, std::tuple<olc::Sprite*, olc::Decal*>>::iterator it;
		for ( it = this->_spriteTilesMap.begin(); it != this->_spriteTilesMap.end(); it++ )
		{
			if ( std::get<1>( it->second ) == nullptr )
			{
				std::get<1>( it->second ) = new olc::Decal( std::get<0>( it->second ) );
			}
		}

		return;
	}


	olc::Sprite* getSprite( std::uint64_t tileId ) //const
	{
		// Returns a pointer to a sprite to be used to render a tile based on tileD

		std::lock_guard<std::mutex> lockAccessSpriteTilesMap( this->_mutexAccessSpriteTilesMap );

		if ( this->_spriteTilesMap.find( tileId ) == this->_spriteTilesMap.end() )
		{
			return nullptr;
		}

		return std::get<0>( this->_spriteTilesMap.at( tileId ) );
	}


	olc::Decal* getDecal( std::uint64_t tileId ) //const
	{
		// Returns a pointer to a decal to be used to render a tile based on tileD

		std::lock_guard<std::mutex> lockAccessSpriteTilesMap( this->_mutexAccessSpriteTilesMap );

		if ( this->_spriteTilesMap.find( tileId ) == this->_spriteTilesMap.end() )
		{
			return nullptr;
		}

		return std::get<1>( this->_spriteTilesMap.at( tileId ) );
	}


	void print()
	{
		// [DEBUG]

		std::map<std::uint64_t, std::tuple<olc::Sprite*, olc::Decal*>>::iterator it;
		for ( it = this->_spriteTilesMap.begin(); it != this->_spriteTilesMap.end(); it++ )
		{
			std::cout << it->first << ":.. " << "(" << std::get<0>( it->second ) << "," << std::get<1>( it->second ) << ")" << std::endl;
		}
		
		return;
	}
};