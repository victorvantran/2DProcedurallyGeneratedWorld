#pragma once

#include <cstdint>
#include <map>
#include <set>
#include <mutex>

#include "olcPixelGameEngine.h"
#include "Assets.h"

#include <iostream>

class Atlas
{
private:
	std::mutex _mutexAccessAtlas;
	std::map< std::uint64_t, olc::Sprite*> _atlasSprites;
	std::map< std::uint64_t, olc::Decal*> _atlasDecals;

public:
	Atlas()
	{

	}


	~Atlas()
	{
		// Deallocate all Sprites and Decals
		std::map<std::uint64_t, olc::Sprite*>::iterator it1;
		for ( it1 = this->_atlasSprites.begin(); it1 != this->_atlasSprites.end(); )
		{
			delete it1->second;
			this->_atlasSprites.erase( it1++ );
		}

		std::map<std::uint64_t, olc::Decal*>::iterator it2;
		for ( it2 = this->_atlasDecals.begin(); it2 != this->_atlasDecals.end(); )
		{
			delete it2->second;
			this->_atlasDecals.erase( it2++ );
		}
	}


	void insert( std::uint64_t tileId )
	{
		// Load a sprite from the SQLite database and insert it into this spriteTileMap.
		// Only create sprite and not decal because this will be called on a separate thread
		// that does not own the OpenGL context

		std::lock_guard lockAcesssAtlas( this->_mutexAccessAtlas );
		if ( this->_atlasSprites.find( tileId ) == this->_atlasSprites.end() )
		{
			olc::Sprite* spriteTile = Assets::loadSpriteTile( tileId );
			this->_atlasSprites.emplace( tileId, spriteTile );
		}

		return;
	}


	void refresh( std::set<std::uint64_t> tileIds )
	{
		// Refresh/clean up any extraneous sprites and decals that are not needed for the current render frame,
		// keeping the necessary sprites/decals based on tileIds

		std::lock_guard<std::mutex> lockAccessAtlas( this->_mutexAccessAtlas );

		std::map<std::uint64_t, olc::Sprite*>::iterator it1;
		for ( it1 = this->_atlasSprites.begin(); it1 != this->_atlasSprites.end(); )
		{
			if ( tileIds.find( it1->first ) == tileIds.end() )
			{
				delete it1->second;
				this->_atlasSprites.erase( it1++ );
			}
			else
			{
				it1++;
			}
		}

		return;
	}


	void updateDecals()
	{
		// Create decals within the Atlas, given a sprite has been created from a different thread. 
		// Creating tiles needs to be called on the thread that owns the openGL context.

		std::lock_guard<std::mutex> lockAccessAtlas( this->_mutexAccessAtlas );

		// Create decals
		std::map<std::uint64_t, olc::Sprite*>::iterator it1;
		for ( it1 = this->_atlasSprites.begin(); it1 != this->_atlasSprites.end(); it1++ )
		{
			if ( it1->second != nullptr )
			{
				if ( this->_atlasDecals.find( it1->first ) == this->_atlasDecals.end() )
				{
					this->_atlasDecals.emplace( it1->first, new olc::Decal( it1->second ) );
				}
			}
		}

		// Delete extraneous decals
		std::map<std::uint64_t, olc::Decal*>::iterator it2;
		for ( it2 = this->_atlasDecals.begin(); it2 != this->_atlasDecals.end(); )
		{
			if ( this->_atlasSprites.find( it2->first ) == this->_atlasSprites.end() )
			{
				delete it2->second;
				this->_atlasDecals.erase( it2++ );
			}
			else
			{
				it2++;
			}
		}

		return;
	}


	olc::Sprite* getSprite( std::uint64_t tileId ) //const
	{
		// Returns a pointer to a sprite to be used to render a tile based on tileD

		std::lock_guard<std::mutex> lockAccessAtlas( this->_mutexAccessAtlas );

		if ( this->_atlasSprites.find( tileId ) == this->_atlasSprites.end() )
		{
			return nullptr;
		}

		return this->_atlasSprites.at( tileId );
	}


	olc::Decal* getDecal( std::uint64_t tileId ) //const
	{
		// Returns a pointer to a decal to be used to render a tile based on tileD

		std::lock_guard<std::mutex> lockAccessAtlas( this->_mutexAccessAtlas );

		if ( this->_atlasDecals.find( tileId ) == this->_atlasDecals.end() )
		{
			return nullptr;
		}

		return this->_atlasDecals.at( tileId );
	}


	void print()
	{
		// [DEBUG]

		std::cout << "Sprites" << std::endl;
		std::map<std::uint64_t, olc::Sprite*>::iterator it1;
		for ( it1 = this->_atlasSprites.begin(); it1 != this->_atlasSprites.end(); it1++ )
		{
			std::cout << it1->first << ": " << it1->second << std::endl;
		}

		std::cout << "Decals" << std::endl;
		std::map<std::uint64_t, olc::Decal*>::iterator it2;
		for ( it2 = this->_atlasDecals.begin(); it2 != this->_atlasDecals.end(); it2++ )
		{
			std::cout << it2->first << ": " << it2->second << std::endl;
		}

		return;
	}
};