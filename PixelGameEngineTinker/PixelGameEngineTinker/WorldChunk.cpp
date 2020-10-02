#include "WorldChunk.h"


WorldChunk::WorldChunk()
	: _chunkIndexX( 0 ), _chunkIndexY( 0 ), _size( 32 ) // [!] singleton global
{
}


WorldChunk::~WorldChunk()
{

}


WorldChunk::WorldChunk( int indexX, int indexY, int size )
	: _chunkIndexX( indexX ), _chunkIndexY( indexY ), _size( size )
{

}


void WorldChunk::construct()
{
	int rootQuadTreePositionX = this->_chunkIndexX * this->_size;
	int rootQuadTreePositionY = this->_chunkIndexY * this->_size;

	//std::cout << "[" << rootQuadTreePositionX << "," << rootQuadTreePositionY << "]" << std::endl;
	//std::cout << this->_cellSize << std::endl;


	// Intialize quadTrees
	this->_quadTrees[0].constructQuadTree(
		0,
		-1,
		QuadTree<Tile, TileRender>::_MAX_LEVELS,
		0,
		BoundingBox<int>( rootQuadTreePositionX, rootQuadTreePositionY, this->_size, this->_size ),
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
	int rootQuadTreePositionX = this->_chunkIndexX * this->_size;
	int rootQuadTreePositionY = this->_chunkIndexY * this->_size;

	// Intialize quadTrees
	this->_quadTrees[0].constructQuadTree(
		0,
		-1,
		QuadTree<Tile, TileRender>::_MAX_LEVELS,
		0,
		BoundingBox<int>( rootQuadTreePositionX, rootQuadTreePositionY, this->_size, this->_size ),
		this->_quadTrees,
		this->_tiles
	);

	return;
}


void WorldChunk::insert( int x, int y, int width, int height, uint64_t id )
{
	// insert to Tiles[] (game logic)
	int localCellStartIndexX = x - this->_chunkIndexX * this->_size;
	int localCellStartIndexY = y - this->_chunkIndexY * this->_size;

	int localCellIndexX;
	int localCellIndexY;
	for ( int x = 0; x < width; x++ )
	{
		for ( int y = 0; y < height; y++ )
		{
			localCellIndexX = localCellStartIndexX + x;
			localCellIndexY = localCellStartIndexY + y;

			if ( localCellIndexX >= 0 && localCellIndexY >= 0 && localCellIndexX < this->_size && localCellIndexY < this->_size )
			{
				Tile* selectedTile = &this->_tiles[localCellIndexY * this->_size + localCellIndexX];
				if ( selectedTile->isVoid() )
				{
					selectedTile->setId( id );
				}
			}
		}
	}

	// insert to QuadTree (render)
	this->_quadTrees[0].insert( TileRender( id, BoundingBox<int>( x, y, width, height ), true) );
	return;
}


void WorldChunk::remove( int x, int y, int width, int height, uint64_t id )
{
	// remove from Tiles[] (game logic)
	int localCellStartIndexX = x - this->_chunkIndexX * this->_size;
	int localCellStartIndexY = y - this->_chunkIndexY * this->_size;

	if ( localCellStartIndexX >= 0 && localCellStartIndexY >= 0 && localCellStartIndexX < this->_size && localCellStartIndexY < this->_size )
	{
		Tile* selectedTile = &this->_tiles[localCellStartIndexY * this->_size + localCellStartIndexX];
		if ( selectedTile->getId() == id )
		{
			selectedTile->setId( 0 ); // [!] Void
		}
	}

	// remove from quadTree (render)
	this->_quadTrees[0].remove( TileRender( id, BoundingBox<int>( x, y, width, height ), true ) );
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
	int boxIndexX = ( int )( boundingBox.getCenterX() / this->_size );
	return this->_chunkIndexX - boxIndexX;
}


int WorldChunk::getRelativeChunkIndexY( const BoundingBox<float>& boundingBox ) const
{
	// Returns the chunk index Y relative to a bounding box's position
	int boxIndexY = ( int )( boundingBox.getCenterY() / this->_size );
	return this->_chunkIndexY - boxIndexY;
}


int WorldChunk::getSize() const
{
	return this->_size;
}


int WorldChunk::getPositionX() const
{
	return this->_chunkIndexX * this->_size;
}


int WorldChunk::getPositionY() const
{
	return this->_chunkIndexY * this->_size;
}


QuadTree<Tile, TileRender>& WorldChunk::getQuadTreeRoot()
{
	return this->_quadTrees[0];
}