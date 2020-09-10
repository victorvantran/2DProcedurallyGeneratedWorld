#pragma once
#include "olcPixelGameEngine.h"
#include "WorldChunk.h"
#include "settings.h"

class Forest : public WorldChunk
{
private:

public:

	Forest();
	~Forest();

	Forest( olc::vi2d chunkPosition, olc::vi2d chunkDimension, int octaves, float scalingBias, Atlas& atlas );

	void create( olc::vi2d chunkPosition, olc::vi2d chunkDimension, int octaves, float scalingBias, Atlas& atlas );
	void generateLayer();

};



Forest::Forest() : WorldChunk() // [!] need to properly call parent class
{
	this->_chunkPosition = settings::WORLD_CHUNK::POSITION;
	this->_chunkDimension = settings::WORLD_CHUNK::DIMENSION;
	this->_tileDimension = settings::ATLAS::TILE_DIMENSION;
	this->_octaves = settings::FOREST::OCTAVES;
	this->_scalingBias = settings::FOREST::SCALING_BIAS;
	this->_chunkSeed = nullptr;
	this->_perlinNoise2D = nullptr;
}


Forest::~Forest()
{

}


Forest::Forest( olc::vi2d chunkPosition, olc::vi2d chunkDimension, int octaves, float scalingBias, Atlas& atlas )
{
	this->create( chunkPosition, chunkDimension, octaves, scalingBias, atlas );
}



void Forest::create( olc::vi2d chunkPosition, olc::vi2d chunkDimension, int octaves, float scalingBias, Atlas& atlas )
{
	this->_octaves = std::min<int>(octaves, (int)std::log2( chunkDimension.x ) ); // [!] Limited by x value, so x <= y
	this->_scalingBias = scalingBias;
	this->_chunkPosition = chunkPosition;
	this->_chunkDimension = chunkDimension;
	this->_chunkAtlas = atlas;
	this->_tileDimension = atlas.getTileDimension();

	this->generateRandomSeed();
	this->generatePerlinNoise2D();
	this->generateLayer();
	return;
}


void Forest::generateLayer()
{
/// Generates a forest layer
	this->_chunkLayer = Layer<Tile>( this->_chunkDimension );

	int numTiles = this->_chunkDimension.x * this->_chunkDimension.y;

	std::vector<std::tuple<int, bool, TileType>> forestMapping;

	for ( int i = 0; i < numTiles; i++ )
	{
		int tileDecider = (int) (this->_perlinNoise2D[i] * 256.0f);

		if ( tileDecider < 32 )
		{
			forestMapping.push_back( std::tuple{ 3, true, TileType::Block } );
		}
		else if ( tileDecider < 64 )
		{
			forestMapping.push_back( std::tuple{ 3, true, TileType::Block } );
		}
		else if ( tileDecider < 96 )
		{
			forestMapping.push_back( std::tuple{ 2, true, TileType::Block } );
		}
		else if ( tileDecider < 128 )
		{
			forestMapping.push_back( std::tuple{ 1, true, TileType::Block} );
		}
		else if ( tileDecider < 160 )
		{
			forestMapping.push_back( std::tuple{ 0, false, TileType::Empty } );
		}
		else if ( tileDecider < 192 )
		{
			forestMapping.push_back( std::tuple{ 0, false, TileType::Empty } );
		}
		else if ( tileDecider < 224 )
		{
			forestMapping.push_back( std::tuple{ 0, false, TileType::Empty } );
		}
		else if ( tileDecider < 256 )
		{
			forestMapping.push_back( std::tuple{ 0, false, TileType::Empty } );
		}
	}

	this->_chunkLayer.copyMapping( forestMapping );
	

	return;
}


