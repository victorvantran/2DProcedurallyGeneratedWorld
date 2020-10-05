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
	// Used solely for initializing the first chunks renders
	int rootQuadTreePositionX = this->_chunkIndexX * this->_size;
	int rootQuadTreePositionY = this->_chunkIndexY * this->_size;

	// Intialize quadTrees
	this->_tileRenders[0].constructQuadTree(
		0,
		-1,
		QuadTree<Tile, TileRender>::_MAX_LEVELS,
		0,
		BoundingBox<int>( rootQuadTreePositionX, rootQuadTreePositionY, this->_size, this->_size ),
		this->_tileRenders,
		this->_tiles
	);

	// Connect trees
	for ( int i = 0; i < sizeof( this->_tileRenders ) / sizeof( this->_tileRenders[0] ); i++ )
	{
		this->_tileRenders[i].divide();
	}

	return;
}


void WorldChunk::wipeRender()
{
	// [!] look to see if we can make it more efficient that construct
	int rootQuadTreePositionX = this->_chunkIndexX * this->_size;
	int rootQuadTreePositionY = this->_chunkIndexY * this->_size;
	
	// Intialize quadTrees
	this->_tileRenders[0].constructQuadTree(
		0,
		-1,
		QuadTree<Tile, TileRender>::_MAX_LEVELS,
		0,
		BoundingBox<int>( rootQuadTreePositionX, rootQuadTreePositionY, this->_size, this->_size ),
		this->_tileRenders,
		this->_tiles
	);
	
	// The difference between construct and reconstruct is quick reassignment, unless of course, shallow copy does this for us. Then just use construct
	for ( int i = 0; i < sizeof( this->_tileRenders ) / sizeof( this->_tileRenders[0] ); i++ )
	{
		this->_tileRenders[i].divide();
	}

	return;
}


void WorldChunk::fill( uint64_t id )
{
	this->insert( this->getChunkIndexX() * this->_size, this->getChunkIndexY() * this->_size, this->_size, this->_size, id );
	return;
}


void WorldChunk::insert( int x, int y, int width, int height, uint64_t id )
{
	this->insertTiles( x, y, width, height, id );
	this->insertTileRenders( x, y, width, height, id );
	return;
}


void WorldChunk::insertTiles( int x, int y, int width, int height, uint64_t id )
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

	return;
}


void WorldChunk::insertTileRenders( int x, int y, int width, int height, uint64_t id )
{
	this->_tileRenders[0].insert( TileRender( id, BoundingBox<int>( x, y, width, height ), true ) );
	return;
}


void WorldChunk::remove( int x, int y, int width, int height, uint64_t id )
{
	this->removeTiles( x, y, width, height, id );
	this->removeTileRenders( x, y, width, height, id );
	return;
}


void WorldChunk::removeTiles( int x, int y, int width, int height, uint64_t id )
{
	// remove from Tiles[] (game logic)
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
				if ( selectedTile->getId() == id )
				{
					selectedTile->setId( 0 ); // [!] void
				}
			}
		}
	}

	return;
}


void WorldChunk::removeTileRenders( int x, int y, int width, int height, uint64_t id )
{
	this->_tileRenders[0].remove( TileRender( id, BoundingBox<int>( x, y, width, height ), true ) );
	return;
}


const std::map<uint64_t, unsigned short> WorldChunk::createPalette() const
{
	std::map<uint64_t, unsigned short> palette;
	unsigned short numTiles = this->_size * this->_size;
	for ( unsigned short i = 0; i < numTiles; i++ )
	{
		palette.emplace( this->_tiles->getId(), palette.size() );
	}

	return palette;
}


void WorldChunk::clear()
{
	this->_chunkIndexX = 0;
	this->_chunkIndexY = 0;

	// Clears the tile array
	std::memset( this->_tiles, 0, sizeof( this->_tiles ) ); // this->_tiles.fill(0)

	// Clears the quad tree's bounds and consolidations
	this->_tileRenders[0].clear();
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


Tile* WorldChunk::getTiles()
{
	return this->_tiles;
}


QuadTree<Tile, TileRender>& WorldChunk::getTileRendersRoot()
{
	return this->_tileRenders[0];
}

