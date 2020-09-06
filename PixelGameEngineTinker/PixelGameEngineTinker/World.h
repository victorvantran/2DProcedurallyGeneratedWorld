#pragma once

#include "Screen.h"
#include "Atlas.h"
#include "Layer.h"
#include "Tile.h"
#include "WorldChunk.h"
#include "Forest.h"

class World
{
private:
	std::vector<WorldChunk*> _worldChunks;

public:
	World();
	~World();

	void generateTestForest( olc::vi2d forestDimension, Atlas& atlas );

	std::vector<WorldChunk*> getWorldChunks();
};



World::World()
{

}


World::~World()
{
/// Delete world chunks
}



void World::generateTestForest( olc::vi2d forestDimension, Atlas& atlas )
{
/// Creates a randomly generated Forest layer by Perlin Noise of specific size and atlas
	Forest* forest = new Forest( forestDimension, atlas );
	this->_worldChunks.push_back( forest );

	return;
}


std::vector<WorldChunk*> World::getWorldChunks()
{
	return this->_worldChunks;
}

