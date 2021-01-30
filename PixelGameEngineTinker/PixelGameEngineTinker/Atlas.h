#pragma once

#include <cstdint>
#include <map>
#include <set>
#include <mutex>

#include "olcPixelGameEngine.h"
#include "Tile.h"
#include "Assets.h"

#include <iostream>

class Atlas
{
private:
	std::mutex _mutexAccessAtlas;
	std::map<TileIdentity, olc::Sprite*> _atlasSprites;
	std::map<TileIdentity, olc::Decal*> _atlasDecals;

public:
	Atlas()
	{

	}


	~Atlas()
	{
		// Deallocate all Sprites and Decals
		std::map<TileIdentity, olc::Sprite*>::iterator it1;
		for ( it1 = this->_atlasSprites.begin(); it1 != this->_atlasSprites.end(); )
		{
			delete it1->second;
			this->_atlasSprites.erase( it1++ );
		}

		std::map<TileIdentity, olc::Decal*>::iterator it2;
		for ( it2 = this->_atlasDecals.begin(); it2 != this->_atlasDecals.end(); )
		{
			delete it2->second;
			this->_atlasDecals.erase( it2++ );
		}
	}


	void insert( TileIdentity tileId )
	{
		// Load a sprite from the SQLite database and insert it into this spriteTileMap.
		// Only create sprite and not decal because this will be called on a separate thread
		// that does not own the OpenGL context

		std::lock_guard lockAcesssAtlas( this->_mutexAccessAtlas );
		if ( this->_atlasSprites.find( tileId ) == this->_atlasSprites.end() )
		{
			olc::Sprite* spriteTile = Assets::loadSpriteTile( ( std::uint64_t )tileId );
			this->_atlasSprites.emplace( tileId, spriteTile );
		}

		return;
	}


	void insert( std::set<TileIdentity> tileIds )
	{
		// Load all sprites from the SQLite database and insert it into this spriteTileMap.
		// Only create sprite and not decal because this will be called on a separate thread
		// that does not own the OpenGL context
		std::lock_guard lockAcesssAtlas( this->_mutexAccessAtlas );

		std::vector<std::uint64_t> newTileIds;
		for ( TileIdentity tileId : tileIds )
		{
			if ( this->_atlasSprites.find( tileId ) == this->_atlasSprites.end() )
			{
				newTileIds.push_back( ( std::uint64_t )tileId );
			}
		}

		std::map<std::uint64_t, olc::Sprite*> spriteTiles = Assets::loadSpriteTiles( newTileIds );
		for ( const std::pair<std::uint64_t, olc::Sprite*>& spriteTile : spriteTiles )
		{
			this->_atlasSprites.emplace( static_cast<TileIdentity>( spriteTile.first ), spriteTile.second );
		}

		return;
	}


	void refresh( std::set<TileIdentity> tileIds )
	{
		// Refresh/clean up any extraneous sprites and decals that are not needed for the current render frame,
		// keeping the necessary sprites/decals based on tileIds
		std::lock_guard<std::mutex> lockAccessAtlas( this->_mutexAccessAtlas );

		std::map<TileIdentity, olc::Sprite*>::iterator it1;
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
		std::map<TileIdentity, olc::Sprite*>::iterator it1;
		// std::cout << this->_atlasSprites.size() << std::endl;
		int maxLoad = 5;
		int currLoad = 0;
		for ( it1 = this->_atlasSprites.begin(); it1 != this->_atlasSprites.end(); it1++ )
		{
			if ( currLoad == maxLoad )
			{
				std::cout << (int)it1->first << std::endl;
				break;
			}


			if ( it1->second != nullptr )
			{
				if ( this->_atlasDecals.find( it1->first ) == this->_atlasDecals.end() )
				{
					this->_atlasDecals.emplace( it1->first, new olc::Decal( it1->second ) );
					currLoad++;
				}
			}
		}

		// Delete extraneous decals
		std::map<TileIdentity, olc::Decal*>::iterator it2;
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


	olc::Sprite* getSprite( TileIdentity tileId ) //const
	{
		// Returns a pointer to a sprite to be used to render a tile based on tileD

		std::lock_guard<std::mutex> lockAccessAtlas( this->_mutexAccessAtlas );

		if ( this->_atlasSprites.find( tileId ) == this->_atlasSprites.end() )
		{
			return nullptr;
		}

		return this->_atlasSprites.at( tileId );
	}


	olc::Decal* getDecal( TileIdentity tileId ) //const
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
		std::map<TileIdentity, olc::Sprite*>::iterator it1;
		for ( it1 = this->_atlasSprites.begin(); it1 != this->_atlasSprites.end(); it1++ )
		{
			std::cout << ( std::uint64_t )it1->first << ": " << it1->second << std::endl;
		}

		std::cout << "Decals" << std::endl;
		std::map<TileIdentity, olc::Decal*>::iterator it2;
		for ( it2 = this->_atlasDecals.begin(); it2 != this->_atlasDecals.end(); it2++ )
		{
			std::cout << ( std::uint64_t )it2->first << ": " << it2->second << std::endl;
		}

		return;
	}
};