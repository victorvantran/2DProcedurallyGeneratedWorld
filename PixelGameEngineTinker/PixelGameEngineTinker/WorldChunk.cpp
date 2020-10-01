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


void WorldChunk::construct()
{
	int rootQuadTreePositionX = this->_chunkIndexX * this->_cellSize;
	int rootQuadTreePositionY = this->_chunkIndexY * this->_cellSize;

	//std::cout << "[" << rootQuadTreePositionX << "," << rootQuadTreePositionY << "]" << std::endl;
	//std::cout << this->_cellSize << std::endl;


	// Intialize quadTrees
	this->_quadTrees[0].constructQuadTree(
		0,
		-1,
		QuadTree<Tile, TileConsolidated>::_MAX_LEVELS,
		0,
		BoundingBox<int>( rootQuadTreePositionX, rootQuadTreePositionY, this->_cellSize, this->_cellSize ),
		this->_quadTrees,
		this->_tiles
	);

	// Connect trees
	for ( int i = 0; i < sizeof( this->_quadTrees ) / sizeof( this->_quadTrees[0] ); i++ )
	{
		this->_quadTrees[i].divide();
	}

	return;
}



void WorldChunk::reconstruct()
{
	int rootQuadTreePositionX = this->_chunkIndexX * this->_cellSize;
	int rootQuadTreePositionY = this->_chunkIndexY * this->_cellSize;

	// Intialize quadTrees
	this->_quadTrees[0].constructQuadTree(
		0,
		-1,
		QuadTree<Tile, TileConsolidated>::_MAX_LEVELS,
		0,
		BoundingBox<int>( rootQuadTreePositionX, rootQuadTreePositionY, this->_cellSize, this->_cellSize ),
		this->_quadTrees,
		this->_tiles
	);

	return;
}


void WorldChunk::insert( const TileConsolidated& tileConsolidated )
{
	this->_quadTrees[0].insert( tileConsolidated );
	return;
}


void WorldChunk::remove( const TileConsolidated& tileConsolidated )
{
	this->_quadTrees[0].remove( tileConsolidated );
	return;
}


void WorldChunk::clear()
{
	this->_chunkIndexX = 0;
	this->_chunkIndexY = 0;
	
	// Clears the quad tree's bounds and consolidations
	this->_quadTrees[0].clear();

	// Do not need to waste resources clearing the tiles for now because it will be overwritten
	return;
}


void WorldChunk::delimit( int indexX, int indexY )
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


QuadTree<Tile, TileConsolidated>& WorldChunk::getQuadTreeRoot()
{
	return this->_quadTrees[0];
}