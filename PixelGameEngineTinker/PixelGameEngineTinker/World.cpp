#include "World.h"

World::World()
	: _numChunkWidth( 1 + this->_chunkRadius * 2 ), _numChunkHeight( 1 + this->_chunkRadius * 2 ), _numWorldChunks( this->_numChunkWidth* this->_numChunkHeight )
{

}


World::~World()
{

}


void World::updateWorldChunks( const BoundingBox<float>& cameraView )
{
	// Updates the proper world chunks based on the position of the camera ( or a bounding box ) in the world
	// [!] need to fix

	int cameraIndexX = ( int )( cameraView.getCenterX() / this->_chunkCellSize );
	int cameraIndexY = ( int )( cameraView.getCenterY() / this->_chunkCellSize );

	for ( int x = 0; x < this->_numChunkWidth; x++ )
	{
		for ( int y = 0; y < this->_numChunkHeight; y++ )
		{
			int deltaIndexX = x - this->_chunkRadius;
			int deltaIndexY = y - this->_chunkRadius;

			this->_worldChunks[y * this->_numChunkWidth + x].update( cameraIndexX + deltaIndexX, cameraIndexY + deltaIndexY );
		}
	}

	return;
}


WorldChunk& World::getWorldChunk( int x, int y )
{
	return this->_worldChunks[ y * this->_numChunkWidth + x];
}


WorldChunk* World::getWorldChunks()
{
	return this->_worldChunks;
}


int World::getChunkRadius() const
{
	return this->_chunkRadius;
}



int World::getNumWorldChunks() const
{
	return this->_numWorldChunks;
}


int World::getNumChunkWidth() const
{
	return this->_numChunkWidth;
}


int World::getNumChunkHeight() const
{
	return this->_numChunkHeight;
}