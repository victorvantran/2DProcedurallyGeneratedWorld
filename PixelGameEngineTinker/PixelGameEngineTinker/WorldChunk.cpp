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

void WorldChunk::insertTiles( TileIdentity tileId, bool opaque, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
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
					selectedTile->setId( tileId );
					selectedTile->setOpaque( opaque );
				}
			}
		}
	}

	return;
}


void WorldChunk::insertTileRenders( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->_tileRenders[0].insert( TileRender( tileId, BoundingBox<std::int64_t>( x, y, width, height ) ) );
	return;
}


void WorldChunk::insertTile( TileIdentity tileId, bool opaque, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	//this->insertTiles( tileId, x, y, width, height );
	//this->insertTileRenders( tileId, x, y, width, height );

	this->insertTiles( tileId, opaque, x, y, width, height );
	this->insertTileRenders( tileId, x, y, width, height );
	return;
}


void WorldChunk::insertLightSource( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	return;
}


void WorldChunk::insertVoid( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	//this->insertTile( TileIdentity::Void, x, y, width, height );
	return;
}


void WorldChunk::insertWater( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Water, false,
		x, y, width, height );
	return;
}

void WorldChunk::insertStone( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Stone, true,
		x, y, width, height );
	return;
}

void WorldChunk::insertDirt( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Dirt, true,
		x, y, width, height );
	return;
}

void WorldChunk::insertTorch( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Torch, false,
		x, y, width, height );
	this->insertLightSource( TileIdentity::Torch, x, y, width, height );
	return;
}


const WorldChunk::funcType WorldChunk::insertMethods[( unsigned long long )TileIdentity::count]{ &WorldChunk::insertVoid, &WorldChunk::insertWater, &WorldChunk::insertStone, &WorldChunk::insertDirt, &WorldChunk::insertTorch };


void WorldChunk::insert( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	// Pointer to a method is slightly different than a pointer to a function
	//World::funcType method = World::insertMethods[0];
	//( this->*method )( tileId, x, y, width, height );

	( this->*WorldChunk::insertMethods[( unsigned long long )tileId] )( x, y, width, height );
	return;
}





















void WorldChunk::removeTiles( TileIdentity id, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
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
					selectedTile->setId( TileIdentity::Void );
					selectedTile->setOpaque( false );
				}
			}
		}
	}

	return;
}


void WorldChunk::removeTileRenders( TileIdentity id, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->_tileRenders[0].remove( TileRender( id, BoundingBox<std::int64_t>( x, y, width, height ) ) );
	return;
}



void WorldChunk::removeTile( TileIdentity id, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTiles( id, x, y, width, height );
	this->removeTileRenders( id, x, y, width, height );
	return;
}


void WorldChunk::removeLightSource( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	return;
}


void WorldChunk::removeVoid( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	return;
}

void WorldChunk::removeWater( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Water, x, y, width, height );
	return;
}


void WorldChunk::removeStone( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Stone, x, y, width, height );
	return;
}


void WorldChunk::removeDirt( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Dirt, x, y, width, height );
	return;
}


void WorldChunk::removeTorch( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Torch, x, y, width, height );
	return;
}

const WorldChunk::funcType WorldChunk::removeMethods[( unsigned long long )TileIdentity::count]{ &WorldChunk::removeVoid, &WorldChunk::removeWater, &WorldChunk::removeStone, &WorldChunk::removeDirt, &WorldChunk::removeTorch };



void WorldChunk::remove( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	( this->*WorldChunk::removeMethods[( unsigned long long )tileId] )( x, y, width, height );
	return;
}


















const std::map<TileIdentity, unsigned short> WorldChunk::createPalette() const
{
	std::map<TileIdentity, unsigned short> palette;
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



std::vector<TileIdentity> WorldChunk::getPalette()
{
	// Create a vector of unique tileIds to be used as mappings of smaller-bit keys.
	std::set<TileIdentity> history;
	std::vector<TileIdentity> palette;
	std::uint16_t numTiles = this->_size * this->_size;

	TileIdentity tileId;
	for ( int i = numTiles - 1; i >= 0; i-- )
	{
		tileId = this->_tiles[i].getId();
		if ( history.find( tileId ) == history.end() )
		{
			history.insert( tileId );
		}
	}

	for ( TileIdentity tileId : history )
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