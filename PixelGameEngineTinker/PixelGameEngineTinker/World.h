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

	void generateTestLayer( olc::vi2d layerDimension, Atlas* atlas );
	void generateTestForest( olc::vi2d forestDimension, olc::Decal* decalAtlas );

	std::vector<WorldChunk*> getWorldChunks();
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
	//this->_worldChunks.push_back( std::tuple{ layer, atlas });

	return;
}


void World::generateTestForest( olc::vi2d forestDimension, olc::Decal* decalAtlas )
{
	/// Creates a randomly generated layer of specific size

	Forest* forest = new Forest();
	forest->create( forestDimension, decalAtlas );
	this->_worldChunks.push_back( forest );



	return;
}


std::vector<WorldChunk*> World::getWorldChunks()
{
	return this->_worldChunks;
}



/*
Perlin Noise

Developed by Ken Perlin
Procedural randomness: randomness with a perceived coherence, features that look natural in clusters
Can be used to create artifical terrain simply by adding a color map to the Perlin Noise

Perlin Noise can be 1D, 2D, and abstracted to 3D

Scaling factor and pitch on the sample seed and cummulatively addd, linear interpolation

P = 1.0*S(x,16) + 0.5*S(x,8) + 0.25*S(x, 4) + 0.125*S(x,2) + 0.0625*S(x,1)

Tesolation from beginning and end

*/