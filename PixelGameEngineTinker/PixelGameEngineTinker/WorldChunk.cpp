#include "WorldChunk.h"


WorldChunk::WorldChunk()
	: _chunkIndexX( 0 ), _chunkIndexY( 0 ), _cellSize( 32 ) // [!] singleton global
{

}


WorldChunk::~WorldChunk()
{

}


WorldChunk::WorldChunk( int indexX, int indexY, int cellSize )
	: _chunkIndexX( indexX ), _chunkIndexY( indexY ), _cellSize( cellSize )
{
}




void WorldChunk::update( int indexX, int indexY )
{
	// pass tile by reference
	// copy contents of tile into this->_tiles
	// reset quadtree

	this->_chunkIndexX = indexX;
	this->_chunkIndexY = indexY;

	return;
}



int WorldChunk::getChunkIndexX() const
{
	return this->_chunkIndexX;
}


int WorldChunk::getChunkIndexY() const
{
	return this->_chunkIndexY;
}


int WorldChunk::getRelativeChunkIndexX( const BoundingBox<float>& boundingBox ) const
{
	// Returns the chunk index X relative to a bounding box's position
	int boxIndexX = ( int )( boundingBox.getCenterX() / this->_cellSize );
	return this->_chunkIndexX - boxIndexX;
}


int WorldChunk::getRelativeChunkIndexY( const BoundingBox<float>& boundingBox ) const
{
	// Returns the chunk index Y relative to a bounding box's position
	int boxIndexY = ( int )( boundingBox.getCenterY() / this->_cellSize );
	return this->_chunkIndexY - boxIndexY;
}


int WorldChunk::getSize() const
{
	return this->_cellSize;
}


int WorldChunk::getPositionX() const
{
	return this->_chunkIndexX * this->_cellSize;
}


int WorldChunk::getPositionY() const
{
	return this->_chunkIndexY * this->_cellSize;
}