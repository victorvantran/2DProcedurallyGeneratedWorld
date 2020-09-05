#pragma once
#include "olcPixelGameEngine.h"
#include "WorldChunk.h"

class Forest : public WorldChunk
{
private:

public:

	Forest();
	~Forest();

	void create( olc::vi2d chunkDimension, olc::Decal* decalAtlas );
	void generateLayer();
};



Forest::Forest()
{

}


Forest::~Forest()
{

}



void Forest::create( olc::vi2d chunkDimension, olc::Decal* decalAtlas )
{
	this->_chunkDimension = chunkDimension;
	this->_chunkAtlas = Atlas( nullptr, decalAtlas, olc::vi2d{ 2,2 }, olc::vi2d{ 8,8 } );
	this->_chunkAtlas.create( decalAtlas );
	this->generateLayer();

	return;
}


void Forest::generateLayer(  )
{
	this->_chunkLayer.create( this->_chunkDimension );


	this->_octaves = 8; /// The dimension of the chunk must be at least [2^8, 2^8] 
	this->_scalingBias = 0.2f;


	this->generateRandomSeed();
	this->generatePerlinNoise2D();


	int numTiles = this->_chunkDimension.x * this->_chunkDimension.y;

	std::vector<int> forestMapping;

	for ( int i = 0; i < numTiles; i++ )
	{
		int tileDecider = (int) (this->_perlinNoise2D[i] * 256.0f);
		//std::cout << tileDecider << std::endl;
		if ( tileDecider < 32 )
		{
			forestMapping.push_back( 3 );
		}
		else if ( tileDecider < 64 )
		{
			forestMapping.push_back( 3 );
		}
		else if ( tileDecider < 96 )
		{
			forestMapping.push_back( 2 );
		}
		else if ( tileDecider < 128 )
		{
			forestMapping.push_back( 1 );
		}
		else if ( tileDecider < 160 )
		{
			forestMapping.push_back( 0 );
		}
		else if ( tileDecider < 192 )
		{
			forestMapping.push_back( 0 );
		}
		else if ( tileDecider < 224 )
		{
			forestMapping.push_back( 0 );
		}
		else if ( tileDecider < 256 )
		{
			forestMapping.push_back( 0 );
		}
	}

	this->_chunkLayer.copyMapping( forestMapping );
	

	return;
}