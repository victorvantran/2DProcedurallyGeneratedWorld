#pragma once
#include "olcPixelGameEngine.h"
#include "Screen.h"
#include "Atlas.h"
#include "Layer.h"
#include "Tile.h"

class WorldChunk
{
private:

protected:
	olc::vi2d _chunkDimension;

	Layer<Tile> _chunkLayer;
	Atlas _chunkAtlas;

	float* _chunkSeed;
	float* _perlinNoise2D;


	int _octaves;
	float _scalingBias;

public:

	WorldChunk();
	~WorldChunk();

	void generateRandomSeed();
	void generatePerlinNoise2D();

	virtual void generateLayer() = 0;

	Layer<Tile> getLayer();
	Atlas getAtlas();

};



WorldChunk::WorldChunk()
{
///
	this->_chunkDimension = olc::vi2d{ 0, 0 };

	this->_chunkSeed = nullptr;
	this->_perlinNoise2D = nullptr;


	this->_octaves = 1;
	this->_scalingBias = 2.0f;
}


WorldChunk::~WorldChunk()
{
///
	delete[] this->_chunkSeed;
	delete[] this->_perlinNoise2D;

}


void WorldChunk::generateRandomSeed()
{
///
	int chunkWidth = (int) this->_chunkLayer.getLayerDimension().x;
	int chunkHeight = (int) this->_chunkLayer.getLayerDimension().y;
	int numTiles = chunkWidth * chunkHeight;

	this->_chunkSeed = new float[numTiles];
	this->_perlinNoise2D = new float[numTiles];


	for ( int i = 0; i < numTiles; i++ )
	{
		this->_chunkSeed[i] = ( float )rand() / ( float )RAND_MAX;
	}

	return;
}


void WorldChunk::generatePerlinNoise2D()
{
///
	int chunkWidth = ( int )this->_chunkLayer.getLayerDimension().x;
	int chunkHeight = ( int )this->_chunkLayer.getLayerDimension().y;

	for ( int x = 0; x < chunkWidth; x++ )
	{
		for ( int y = 0; y < chunkHeight; y++ )
		{
			float noise = 0.0f;
			float scaleAccumulated = 0.0f;
			float scale = 1.0f;

			for ( int o = 0; o < this->_octaves; o++ )
			{
				int pitch = chunkWidth >> o; /// Need to ensure chunkWidth to be a power of 2
				int sampleX1 = ( x / pitch ) * pitch;
				int sampleY1 = ( y / pitch ) * pitch;

				int sampleX2 = ( sampleX1 + pitch ) % chunkWidth;
				int sampleY2 = ( sampleY1 + pitch ) % chunkWidth;

				float blendX = ( float )( x - sampleX1 ) / ( float )pitch;
				float blendY = ( float )( y - sampleY1 ) / ( float )pitch;

				float interpolatedSampleX = ( 1.0f - blendX ) * this->_chunkSeed[sampleY1 * chunkWidth + sampleX1] + ( blendX ) * this->_chunkSeed[sampleY1 * chunkWidth + sampleX2];
				float interpolatedSampleY = ( 1.0f - blendX ) * this->_chunkSeed[sampleY2 * chunkWidth + sampleX1] + ( blendX ) * this->_chunkSeed[sampleY2 * chunkWidth + sampleX2];

				scaleAccumulated += scale;
				noise += ( blendY * ( interpolatedSampleY - interpolatedSampleX ) + interpolatedSampleX) * scale; /// Accumulate noise value with the sampled values

				scale = scale / this->_scalingBias; /// Provides a bias to scaling lower or higher frequency noise
			}

			this->_perlinNoise2D[y * chunkWidth + x] = noise / scaleAccumulated; /// Normalize to [0, 1]
		}
	}

	return;
}



Layer<Tile> WorldChunk::getLayer()
{
///
	return this->_chunkLayer;
}


Atlas WorldChunk::getAtlas()
{
///
	return this->_chunkAtlas;
}
