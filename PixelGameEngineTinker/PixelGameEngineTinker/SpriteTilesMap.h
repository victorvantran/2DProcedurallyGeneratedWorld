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
	//std::map<std::uint64_t, olc::Renderable*> _spriteTilesMap;
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
			delete std::get<0>( it->second ); // Deallocate the sprite
			delete std::get<1>( it->second ); // Deallocate the decal
			this->_spriteTilesMap.erase( it++ );
		}
	}


	void insert( std::uint64_t tileId )
	{
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
		// Refresh/clean up
		std::lock_guard<std::mutex> lockAccessSpriteTilesMap( this->_mutexAccessSpriteTilesMap );

		std::map<std::uint64_t, std::tuple<olc::Sprite*, olc::Decal*>>::iterator it;
		for ( it = this->_spriteTilesMap.begin(); it != this->_spriteTilesMap.end();  )
		{
			if ( tileIds.find( it->first ) == tileIds.end() )
			{
				delete std::get<0>( it->second ); // Deallocate the sprite
				delete std::get<1>( it->second ); // Deallocate the decal
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
		// To be called on the thread that owns the openGL context
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


	olc::Sprite* getSprite( std::uint64_t tileId ) const
	{
		if ( this->_spriteTilesMap.find( tileId ) == this->_spriteTilesMap.end() )
		{
			return nullptr;
		}

		return std::get<0>( this->_spriteTilesMap.at( tileId ) );
	}


	olc::Decal* getDecal( std::uint64_t tileId ) const
	{
		if ( this->_spriteTilesMap.find( tileId ) == this->_spriteTilesMap.end() )
		{
			return nullptr;
		}

		return std::get<1>( this->_spriteTilesMap.at( tileId ) );
	}


	void print()
	{
		
		std::map<std::uint64_t, std::tuple<olc::Sprite*, olc::Decal*>>::iterator it;
		for ( it = this->_spriteTilesMap.begin(); it != this->_spriteTilesMap.end(); it++ )
		{
			std::cout << it->first << ":.. " << "(" << std::get<0>( it->second ) << "," << std::get<1>( it->second ) << ")" << std::endl;
		}
		
		return;
	}
};