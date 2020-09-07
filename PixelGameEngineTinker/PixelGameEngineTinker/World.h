#pragma once

#include "olcPixelGameEngine.h"
#include "settings.h"
#include "Screen.h"
#include "Atlas.h"
#include "Layer.h"
#include "Tile.h"
#include "WorldChunk.h"
#include "Forest.h"

class World
{
private:
	std::vector<WorldChunk*> _worldChunks; // renderable
	
	// workingWorldChunk

	// todo, have all worldchunks follow same tiledimension
	olc::vi2d _tileDimension;
public:
	World();
	~World();

	void constructWorld();
	void destructWorld();


	void generateTestForest( olc::vf2d chunkPosition, olc::vi2d forestDimension, Atlas& atlas );

	std::vector<WorldChunk*> getWorldChunks();


	// add offset position to the calculations
	olc::vi2d getIndexFromPixelPosition( float x, float y );
	olc::vi2d getIndexFromPixelPosition( olc::vf2d pixelPosition );

	WorldChunk* getWorldChunkFromIndex( int x, int y );
	WorldChunk* getWorldChunkFromIndex( olc::vi2d index );

	Tile* getTileFromIndex( int x, int y );
	Tile* getTileFromIndex( olc::vi2d index );

	Tile* getTileFromPixelPosition( float x, float y );
	Tile* getTileFromPixelPosition( olc::vf2d pixelPosition );


	olc::vi2d getTileDimension();
};



World::World()
{
	this->constructWorld();
}


World::~World()
{
/// Delete world chunks
	this->destructWorld();
}


void World::constructWorld()
{	
	this->_tileDimension = settings::ATLAS::TILE_DIMENSION;
	return;
}

void World::destructWorld()
{
	return;
}



void World::generateTestForest( olc::vf2d chunkPosition, olc::vi2d forestDimension, Atlas& atlas )
{
/// Creates a randomly generated Forest layer by Perlin Noise of specific size and atlas
	Forest* forest = new Forest( chunkPosition, forestDimension, atlas );
	this->_worldChunks.push_back( forest );

	return;
}


std::vector<WorldChunk*> World::getWorldChunks()
{
	return this->_worldChunks;
}



WorldChunk* World::getWorldChunkFromIndex( int x, int y )
{
	for ( WorldChunk* worldChunk : this->_worldChunks )
	{
		// If tile is within range of the worldchunk, then render it
		olc::vi2d startWorldChunkPosition = worldChunk->getPosition();
		olc::vi2d endWorldChunkPosition = worldChunk->getPosition() + worldChunk->getDimension();

		if ( x >= startWorldChunkPosition.x && y >= startWorldChunkPosition.y &&
			x < endWorldChunkPosition.x && y < endWorldChunkPosition.y ) {

			return worldChunk;
		}
	}

	return nullptr;
}



WorldChunk* World::getWorldChunkFromIndex( olc::vi2d index )
{
	for ( WorldChunk* worldChunk : this->_worldChunks )
	{
		// If tile is within range of the worldchunk, then render it
		olc::vi2d startWorldChunkPosition = worldChunk->getPosition();
		olc::vi2d endWorldChunkPosition = worldChunk->getPosition() + worldChunk->getDimension();

		if ( index.x >= startWorldChunkPosition.x && index.y >= startWorldChunkPosition.y &&
			index.x < endWorldChunkPosition.x && index.y < endWorldChunkPosition.y ) {

			return worldChunk;
		}
	}

	return nullptr;
}



Tile* World::getTileFromIndex( int x, int y )
{
	for ( WorldChunk* worldChunk : this->_worldChunks )
	{
		// If tile is within range of the worldchunk, then render it
		olc::vi2d startWorldChunkPosition = worldChunk->getPosition();
		olc::vi2d endWorldChunkPosition = worldChunk->getPosition() + worldChunk->getDimension();

		if ( x >= startWorldChunkPosition.x && y >= startWorldChunkPosition.y &&
			x < endWorldChunkPosition.x && y < endWorldChunkPosition.y ) {

			return worldChunk->getTileFromIndex( x, y );
		}
	}

	return nullptr;
}


Tile* World::getTileFromIndex( olc::vi2d index )
{
	for ( WorldChunk* worldChunk : this->_worldChunks )
	{
		// If tile is within range of the worldchunk, then render it
		olc::vi2d startWorldChunkPosition = worldChunk->getPosition();
		olc::vi2d endWorldChunkPosition = worldChunk->getPosition() + worldChunk->getDimension();

		if ( index.x >= startWorldChunkPosition.x && index.y >= startWorldChunkPosition.y &&
			index.x < endWorldChunkPosition.x && index.y < endWorldChunkPosition.y ) {

			return worldChunk->getTileFromIndex( index );
		}
	}

	return nullptr;
}


Tile* World::getTileFromPixelPosition( float x, float y )
{
	return nullptr;
}


Tile* World::getTileFromPixelPosition( olc::vf2d pixelPosition )
{
	return nullptr;
}



olc::vi2d World::getTileDimension()
{
	return this->_tileDimension;
}