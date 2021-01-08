#include "WorldChunk.h"
#include "WorldChunkMemory.h"


WorldChunk::WorldChunk()
	: _chunkIndexX( 0 ), _chunkIndexY( 0 ), _lighting( 0, 0, Settings::WorldChunk::SIZE, Settings::WorldChunk::SIZE, this->_tiles )
{

}


WorldChunk::~WorldChunk()
{

}


WorldChunk::WorldChunk( std::int64_t indexX, std::int64_t indexY )
	: _chunkIndexX( indexX ), _chunkIndexY( indexY ), _lighting( indexX, indexY, Settings::WorldChunk::SIZE, Settings::WorldChunk::SIZE, this->_tiles )
{

}


void WorldChunk::construct()
{
	// Used solely for initializing the first chunks renders
	std::int64_t rootQuadTreePositionX = this->_chunkIndexX * this->_size;
	std::int64_t rootQuadTreePositionY = this->_chunkIndexY * this->_size;

	// Intialize tileRender QuadTrees
	this->_tileRenders[0].constructQuadTree(
		0,
		-1,
		Settings::WorldChunk::TILE_RENDER_MAX_LEVEL,
		0,
		BoundingBox<std::int64_t>( rootQuadTreePositionX, rootQuadTreePositionY, this->_size, this->_size ),
		this->_tileRenders,
		Settings::WorldChunk::TILE_RENDER_MIN_LEVEL,
		Settings::WorldChunk::TILE_RENDER_MAX_LEVEL,
		Settings::WorldChunk::TILE_RENDER_MIN_CELL_SIZE
	);

	// Connect tileRender nodes
	for ( int i = 0; i < sizeof( this->_tileRenders ) / sizeof( this->_tileRenders[0] ); i++ )
	{
		this->_tileRenders[i].divide();
	}

	return;
}


void WorldChunk::wipeRender()
{
	// [!] don't even need clear becasuse wipe Render calls constructQuadTree which sets root divided = false
	// [!] look to see if we can make it more efficient than construct
	// [!] soluton is to clear, and then jsut set bounding boxes to proper size? Or is no clear more efficient?
	std::int64_t rootQuadTreePositionX = this->_chunkIndexX * this->_size;
	std::int64_t rootQuadTreePositionY = this->_chunkIndexY * this->_size;


	// Intialize quadTrees
	this->_tileRenders[0].constructQuadTree(
		0,
		-1,
		Settings::WorldChunk::TILE_RENDER_MAX_LEVEL,
		0,
		BoundingBox<std::int64_t>( rootQuadTreePositionX, rootQuadTreePositionY, this->_size, this->_size ),
		this->_tileRenders,
		Settings::WorldChunk::TILE_RENDER_MIN_LEVEL,
		Settings::WorldChunk::TILE_RENDER_MAX_LEVEL,
		Settings::WorldChunk::TILE_RENDER_MIN_CELL_SIZE
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


void WorldChunk::insert( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height, uint64_t id )
{
	this->insertTiles( x, y, width, height, id );
	this->insertTileRenders( x, y, width, height, id );
	return;
}


void WorldChunk::insertTiles( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height, uint64_t id )
{
	// insert to Tiles[] (game logic)
	std::int64_t localCellStartIndexX = x - this->_chunkIndexX * this->_size;
	std::int64_t localCellStartIndexY = y - this->_chunkIndexY * this->_size;

	std::int64_t localCellIndexX;
	std::int64_t localCellIndexY;
	for ( std::int64_t x = 0; x < width; x++ )
	{
		for ( std::int64_t y = 0; y < height; y++ )
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


void WorldChunk::insertTileRenders( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height, uint64_t id )
{
	this->_tileRenders[0].insert( TileRender( id, BoundingBox<std::int64_t>( x, y, width, height ) ) );
	return;
}


void WorldChunk::remove( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height, uint64_t id )
{
	this->removeTiles( x, y, width, height, id );
	this->removeTileRenders( x, y, width, height, id );
	return;
}


void WorldChunk::removeTiles( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height, uint64_t id )
{
	// remove from Tiles[] (game logic)
	std::int64_t localCellStartIndexX = x - this->_chunkIndexX * this->_size;
	std::int64_t localCellStartIndexY = y - this->_chunkIndexY * this->_size;

	std::int64_t localCellIndexX;
	std::int64_t localCellIndexY;
	for ( std::int64_t x = 0; x < width; x++ )
	{
		for ( std::int64_t y = 0; y < height; y++ )
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


void WorldChunk::removeTileRenders( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height, uint64_t id )
{
	this->_tileRenders[0].remove( TileRender( id, BoundingBox<std::int64_t>( x, y, width, height ) ) );
	return;
}


const std::map<uint64_t, unsigned short> WorldChunk::createPalette() const
{
	std::map<uint64_t, unsigned short> palette;
	std::int64_t numTiles = this->_size * this->_size;
	for ( std::int64_t i = 0; i < numTiles; i++ )
	{
		palette.emplace( this->_tiles->getId(), palette.size() );
	}

	return palette;
}


void WorldChunk::clear()
{
	// Clears the tile array
	this->_chunkIndexX = 0;
	this->_chunkIndexY = 0;

	std::memset( this->_tiles, 0, sizeof( this->_tiles ) ); // this->_tiles.fill(0)

	return;
}


void WorldChunk::delimit( std::int64_t indexX, std::int64_t indexY )
{
	// pass tile by reference
	// copy contents of tile into this->_tiles
	// reset quadtree
	this->_chunkIndexX = indexX;
	this->_chunkIndexY = indexY;


	this->_lighting.setChunkIndexX( indexX );
	this->_lighting.setChunkIndexY( indexY );

	// [!] Do the same for geography when isolated black boxed
	//this->_geography.setChunkIndexX( indexX );
	//this->_geography.setChunkIndexY( indexY );


	return;
}



std::int64_t WorldChunk::getChunkIndexX() const
{
	return this->_chunkIndexX;
}


std::int64_t WorldChunk::getChunkIndexY() const
{
	return this->_chunkIndexY;
}


std::int64_t WorldChunk::getRelativeChunkIndexX( const BoundingBox<long double>& boundingBox ) const
{
	// Returns the chunk index X relative to a bounding box's position
	std::int64_t boxIndexX = ( std::int64_t )( boundingBox.getCenterX() / this->_size );
	return this->_chunkIndexX - boxIndexX;
}


std::int64_t WorldChunk::getRelativeChunkIndexY( const BoundingBox<long double>& boundingBox ) const
{
	// Returns the chunk index Y relative to a bounding box's position
	std::int64_t boxIndexY = ( std::int64_t )( boundingBox.getCenterY() / this->_size );
	return this->_chunkIndexY - boxIndexY;
}


std::int16_t WorldChunk::getSize() const
{
	return this->_size;
}


std::int16_t WorldChunk::getNumTileRenders() const
{
	return this->_numTileRenders;
}


std::int64_t WorldChunk::getPositionX() const
{
	return this->_chunkIndexX * this->_size;
}


std::int64_t WorldChunk::getPositionY() const
{
	return this->_chunkIndexY * this->_size;
}


Tile* WorldChunk::getTiles()
{
	return this->_tiles;
}


QuadTree<TileRender>& WorldChunk::getTileRendersRoot()
{
	return this->_tileRenders[0];
}



std::vector<std::uint64_t> WorldChunk::getPalette()
{
	// Create a vector of unique tileIds to be used as mappings of smaller-bit keys.
	std::set<std::uint64_t> history;
	std::vector<std::uint64_t> palette;
	std::uint16_t numTiles = this->_size * this->_size;

	std::uint64_t tileId;
	for ( int i = numTiles - 1; i >= 0; i-- )
	{
		tileId = this->_tiles[i].getId();
		if ( history.find( tileId ) == history.end() )
		{
			history.insert( tileId );
		}
	}

	for ( std::uint64_t tileId : history )
	{
		palette.push_back( tileId );
	}

	return palette;
}


WorldChunkMemory* WorldChunk::createMemory()
{
	return new WorldChunkMemory( this );
}





Lighting<long double>& WorldChunk::getLighting()
{
	return this->_lighting;
}


Light* WorldChunk::getLights()
{
	return this->_lighting.getLights();
}


Light* WorldChunk::getLight( std::int64_t x, std::int64_t y )
{
	std::int64_t localCellIndexX = x - this->_chunkIndexX * this->_size;
	std::int64_t localCellIndexY = y - this->_chunkIndexY * this->_size;
	return &this->getLights()[localCellIndexY * this->_size + localCellIndexX];
}
