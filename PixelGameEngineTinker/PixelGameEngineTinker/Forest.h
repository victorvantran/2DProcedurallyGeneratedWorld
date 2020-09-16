#pragma once
#include "olcPixelGameEngine.h"
#include "settings.h"
#include "WorldChunk.h"

class Forest : public WorldChunk
{
private:

public:

	Forest();
	~Forest();

	Forest( olc::vi2d chunkPosition, olc::vi2d chunkDimension, int octaves, float scalingBias, Atlas& atlas );

	void create( olc::vi2d chunkPosition, olc::vi2d chunkDimension, int octaves, float scalingBias, Atlas& atlas );
	void constructLayer();
	void constructMesh();

};



Forest::Forest() : WorldChunk() // [!] need to properly call parent class
{
	this->_chunkPosition = settings::WORLD_CHUNK::POSITION;
	this->_chunkDimension = settings::WORLD_CHUNK::DIMENSION;
	this->_octaves = settings::FOREST::OCTAVES;
	this->_scalingBias = settings::FOREST::SCALING_BIAS;
	this->_chunkSeed = nullptr;
	this->_perlinNoise2D = nullptr;
}


Forest::~Forest()
{
	// all deletion done by WorldChunk
}


Forest::Forest( olc::vi2d chunkPosition, olc::vi2d chunkDimension, int octaves, float scalingBias, Atlas& atlas )
{
	this->create( chunkPosition, chunkDimension, octaves, scalingBias, atlas);

}



void Forest::create( olc::vi2d chunkPosition, olc::vi2d chunkDimension, int octaves, float scalingBias, Atlas& atlas )
{
	this->_octaves = std::min<int>(octaves, (int)std::log2( chunkDimension.x ) ); // [!] Limited by x value, so x <= y
	this->_scalingBias = scalingBias;
	this->_chunkPosition = chunkPosition;
	this->_chunkDimension = chunkDimension;
	this->_chunkAtlas = atlas;

	this->generateRandomSeed();
	this->generatePerlinNoise2D();
	this->constructLayer();
	return;
}


void Forest::constructLayer()
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
			forestMapping.push_back( std::tuple{ 3, true, TileType::Block } ); // Stone
		}
		else if ( tileDecider < 64 )
		{
			forestMapping.push_back( std::tuple{ 3, true, TileType::Block } ); // Stone
		}
		else if ( tileDecider < 96 )
		{
			forestMapping.push_back( std::tuple{ 2, true, TileType::Block } ); // Dirt
		}
		else if ( tileDecider < 128 )
		{
			forestMapping.push_back( std::tuple{ 1, true, TileType::Block} ); // Grass
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
	this->constructMesh();

	return;
}



void Forest::constructMesh()
{
	// Constructs a mesh of the map to over lay
	this->_chunkMesh.generateMeshMapping( this->_chunkLayer.getMapping(), this->_chunkLayer.getDimension() );
	
	/*
	int* a =this->_chunkMesh.getMapping();
	for ( int x = 0; x < this->_chunkLayer.getDimension().x - 1; x++ )
	{
		for ( int y = 0; y < this->_chunkLayer.getDimension().y - 1; y++ )
		{
			std::cout << a[y * ( this->_chunkLayer.getDimension().x - 1) + x] << std::endl;
		}
	}
	*/

	this->_chunkMesh.meshify( this->_chunkLayer );
	return;
}

