#pragma once

#include "Screen.h"
#include "Atlas.h"
#include "Layer.h"
#include "Tile.h"

class World
{
private:
	std::vector<std::tuple<Layer<Tile>*, Atlas*>> _worldChunks; // WorldChunk Datatype

public:
	World();
	~World();

	void generateTestLayer( olc::vi2d layerDimension, Atlas* atlas );

	std::vector<std::tuple<Layer<Tile>*, Atlas*>> getWorldChunks();
};



World::World()
{

}


World::~World()
{
/// Delete world chunks
}


void World::generateTestLayer( olc::vi2d layerDimension, Atlas* atlas )
{
/// Creates a randomly generated layer of specific size
	Layer<Tile>* layer = new Layer<Tile>();
	layer->create( layerDimension );
	layer->generateRandomness( atlas );
	this->_worldChunks.push_back( std::tuple{ layer, atlas });

	return;
}

std::vector<std::tuple<Layer<Tile>*, Atlas*>> World::getWorldChunks()
{
	return this->_worldChunks;
}