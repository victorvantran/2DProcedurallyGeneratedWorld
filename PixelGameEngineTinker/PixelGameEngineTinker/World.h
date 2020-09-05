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
/// Frees the memory reserved for:
///		1) pointers to atlases
///		2) pointers to layers
/// Be sure that the memory of each atlas and layer is freed too
	//delete[] this->_atlases; 
	//delete[] this->layers;
}


void World::generateTestLayer( olc::vi2d layerDimension, Atlas* atlas )
{
/// Creates a randomly generated layer of specific size
	Layer<Tile>* layer = new Layer<Tile>();
	layer->create( olc::vi2d{ 480, 270 }, olc::vi2d{ 8, 8 } );
	//layer->generateRandomness( atlas );
	this->_worldChunks.push_back( std::tuple{ layer, atlas });

	return;
}

std::vector<std::tuple<Layer<Tile>*, Atlas*>> World::getWorldChunks()
{
	return this->_worldChunks;
}