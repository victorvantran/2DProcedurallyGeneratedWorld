#include "WorldChunk.h"
#include "WorldChunkMemory.h"


WorldChunk::WorldChunk()
	: _relativeChunkIndex( 0 ), _chunkIndexX( 0 ), _chunkIndexY( 0 )
{

}


WorldChunk::~WorldChunk()
{

}


WorldChunk::WorldChunk( std::uint16_t relChunkIndex, std::int64_t indexX, std::int64_t indexY )
	: _relativeChunkIndex( relChunkIndex ), _chunkIndexX( indexX ), _chunkIndexY( indexY )
{

}


void WorldChunk::construct()
{
	this->wipeRender();
	this->wipeLightRender();
	this->blackenLights();
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
	this->insertTile( this->getChunkIndexX() * this->_size, this->getChunkIndexY() * this->_size, this->_size, this->_size, id );
	return;
}


std::uint16_t WorldChunk::getRelativeChunkIndex() const
{
	return this->_relativeChunkIndex;
}


void WorldChunk::setRelativeChunkIndex( std::uint16_t relChunkIndex )
{
	this->_relativeChunkIndex = relChunkIndex;
	return;
}








// Modification

void WorldChunk::insertTile( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	//this->insertTiles( tileId, x, y, width, height );
	//this->insertTileRenders( tileId, x, y, width, height );

	this->insertTiles( x, y, width, height, ( std::uint64_t )( tileId ) );
	this->insertTileRenders( x, y, width, height, ( std::uint64_t )( tileId ) );
	return;
}


void WorldChunk::insertLightSource( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	return;
}


const WorldChunk::funcType WorldChunk::insertMethods[( unsigned long long )TileIdentity::count]{ &WorldChunk::insertWater, &WorldChunk::insertStone, &WorldChunk::insertDirt, &WorldChunk::insertTorch };


void WorldChunk::insertWater( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->insertTile( tileId, x, y, width, height );
	return;
}

void WorldChunk::insertStone( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->insertTile( tileId, x, y, width, height );
	return;
}

void WorldChunk::insertDirt( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->insertTile( tileId, x, y, width, height );
	return;
}

void WorldChunk::insertTorch( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->insertTile( tileId, x, y, width, height );
	this->insertLightSource( tileId, x, y, width, height );
	return;
}


void WorldChunk::insert( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	// Pointer to a method is slightly different than a pointer to a function
	//World::funcType method = World::insertMethods[0];
	//( this->*method )( tileId, x, y, width, height );

	( this->*WorldChunk::insertMethods[( unsigned long long )tileId] )( tileId, x, y, width, height );
	return;
}

























void WorldChunk::insertTile( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height, uint64_t id )
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

	this->blackenLights();
	this->clearLightSources();
	return;
}


void WorldChunk::delimit( std::int64_t indexX, std::int64_t indexY )
{
	// pass tile by reference
	// copy contents of tile into this->_tiles
	// reset quadtree
	this->_chunkIndexX = indexX;
	this->_chunkIndexY = indexY;


	//this->_lighting.setChunkIndexX( indexX );
	//this->_lighting.setChunkIndexY( indexY );

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


const Tile* WorldChunk::getTiles()
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



// Lighting
Light* WorldChunk::getLights()
{
	return this->_lights;
}


Light* WorldChunk::getLight( std::int64_t x, std::int64_t y )
{
	std::int64_t localCellIndexX = x - this->_chunkIndexX * this->_size;
	std::int64_t localCellIndexY = y - this->_chunkIndexY * this->_size;
	return &this->_lights[localCellIndexY * this->_size + localCellIndexX];
}


const std::map<std::uint16_t, LightSource>& WorldChunk::getLightSources()
{
	return this->_lightSources;
}


QuadTree<LightRender>* WorldChunk::getLightRenders()
{
	return this->_lightRenders;
}


void WorldChunk::resetLights()
{
	this->wipeLightRender();
	this->blackenLights();
	return;
}


void WorldChunk::blackenLights()
{
	//std::cout << "blacken" << std::endl;
	for ( std::uint16_t i = 0; i < this->_size * this->_size; i++ )
	{
		this->_lights[i].blacken();
	}
	return;
}


void WorldChunk::clearLightSources()
{
	this->_lightSources.clear();
	return;
}


void WorldChunk::wipeLightRender()
{
	// Need to update chunkIndex of lighting when updating index of worldChunk [!]
	std::int64_t rootQuadTreePositionX = this->_chunkIndexX * this->_size;
	std::int64_t rootQuadTreePositionY = this->_chunkIndexY * this->_size;

	// Intialize quadTrees
	this->_lightRenders[0].constructQuadTree(
		0,
		-1,
		Settings::WorldChunk::TILE_RENDER_MAX_LEVEL,
		0,
		BoundingBox<std::int64_t>( rootQuadTreePositionX, rootQuadTreePositionY, this->_size, this->_size ),
		this->_lightRenders,
		Settings::WorldChunk::TILE_RENDER_MIN_LEVEL,
		Settings::WorldChunk::TILE_RENDER_MAX_LEVEL,
		Settings::WorldChunk::TILE_RENDER_MIN_CELL_SIZE
	);

	// The difference between construct and reconstruct is quick reassignment, unless of course, shallow copy does this for us. Then just use construct
	for ( int i = 0; i < sizeof( this->_lightRenders ) / sizeof( this->_lightRenders[0] ); i++ )
	{
		this->_lightRenders[i].divide();
	}

	return;
}





void WorldChunk::insertLightRender( std::uint32_t corner0, std::uint32_t corner1, std::uint32_t corner2, std::uint32_t corner3, bool exist, std::int64_t x, std::int64_t y )
{
	// model after insert

	return;
}


void WorldChunk::insertLightRenders( std::uint32_t corner0, std::uint32_t corner1, std::uint32_t corner2, std::uint32_t corner3, bool exist, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->_lightRenders[0].insert( LightRender( corner0, corner1, corner2, corner3, exist,
		BoundingBox<std::int64_t>( x, y, width, height ) ) );
	return;
}