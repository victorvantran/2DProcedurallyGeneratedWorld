#pragma once
#include "olcPixelGameEngine.h"
#include "WorldChunk.h"

class Forest : public WorldChunk
{
private:

public:

	Forest();
	~Forest();

	Forest( olc::vi2d chunkDimension, Atlas& atlas, int octaves = 8, float scalingBias = 0.2f );

	void create( olc::vi2d chunkDimension, Atlas& atlas, int octaves, float scalingBias );
	void generateLayer();

};



Forest::Forest()
{
	this->_chunkDimension = olc::vi2d{ 0, 0 };
	this->_chunkSeed = nullptr;
	this->_perlinNoise2D = nullptr;
}


Forest::~Forest()
{

}


Forest::Forest( olc::vi2d chunkDimension, Atlas& atlas, int octaves, float scalingBias )
{
	this->create( chunkDimension, atlas, octaves, scalingBias );
}



void Forest::create( olc::vi2d chunkDimension, Atlas& atlas, int octaves, float scalingBias )
{
	this->_octaves = octaves;
	this->_scalingBias = scalingBias;
	this->_chunkDimension = chunkDimension;
	this->_chunkAtlas = atlas;

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

	std::vector<std::tuple<int, bool>> forestMapping;

	for ( int i = 0; i < numTiles; i++ )
	{
		int tileDecider = (int) (this->_perlinNoise2D[i] * 256.0f);

		if ( tileDecider < 32 )
		{
			forestMapping.push_back( std::tuple{ 3, true } );
		}
		else if ( tileDecider < 64 )
		{
			forestMapping.push_back( std::tuple{ 3, true } );
		}
		else if ( tileDecider < 96 )
		{
			forestMapping.push_back( std::tuple{ 2, true } );
		}
		else if ( tileDecider < 128 )
		{
			forestMapping.push_back( std::tuple{ 1, true } );
		}
		else if ( tileDecider < 160 )
		{
			forestMapping.push_back( std::tuple{ 0, false } );
		}
		else if ( tileDecider < 192 )
		{
			forestMapping.push_back( std::tuple{ 0, false } );
		}
		else if ( tileDecider < 224 )
		{
			forestMapping.push_back( std::tuple{ 0, false } );
		}
		else if ( tileDecider < 256 )
		{
			forestMapping.push_back( std::tuple{ 0, false } );
		}
	}

	this->_chunkLayer.copyMapping( forestMapping );
	

	return;
}


