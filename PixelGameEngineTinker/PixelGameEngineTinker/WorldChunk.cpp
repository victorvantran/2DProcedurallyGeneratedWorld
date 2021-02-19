#include "WorldChunk.h"
#include "World.h"
#include "WorldChunkMemory.h"


WorldChunk::WorldChunk()
	: _relativeChunkIndex( 0 ), _chunkIndexX( 0 ), _chunkIndexY( 0 ), _world( nullptr )
{

}


WorldChunk::~WorldChunk()
{

}


WorldChunk::WorldChunk( std::uint16_t relChunkIndex, std::int64_t indexX, std::int64_t indexY, World* world )
	: _relativeChunkIndex( relChunkIndex ), _chunkIndexX( indexX ), _chunkIndexY( indexY ), _world( world )
{

}


void WorldChunk::setWorld( World* world )
{
	this->_world = world;
	return;
}


void WorldChunk::construct()
{
	this->wipeRender();
	//this->wipeLightRender();
	this->wipeLightRenders();
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

void WorldChunk::insertTiles( TileIdentity tileId, TileType tileType, bool consolidatable, bool opaque, bool complementary, std::uint8_t borders, std::uint8_t tileBlobMapIndex, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
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
					selectedTile->setType( tileType );
					selectedTile->setOpaque( opaque );
					selectedTile->setComplementary( complementary );
					selectedTile->setConsolidatable( consolidatable );
					selectedTile->setBorders( 0 ); // [!] dummy border (updated later)
					selectedTile->setTileBlobMapIndex( tileBlobMapIndex );
				}

				this->_world->updateTileBorders( this->_chunkIndexX * this->_size + localCellIndexX, this->_chunkIndexY * this->_size + localCellIndexY );
				this->_world->updateTileBorders( this->_chunkIndexX * this->_size + localCellIndexX - 1, this->_chunkIndexY * this->_size + localCellIndexY - 1 );
				this->_world->updateTileBorders( this->_chunkIndexX * this->_size + localCellIndexX + 0, this->_chunkIndexY * this->_size + localCellIndexY - 1 );
				this->_world->updateTileBorders( this->_chunkIndexX * this->_size + localCellIndexX + 1, this->_chunkIndexY * this->_size + localCellIndexY - 1 );
				this->_world->updateTileBorders( this->_chunkIndexX * this->_size + localCellIndexX + 1, this->_chunkIndexY * this->_size + localCellIndexY + 0 );
				this->_world->updateTileBorders( this->_chunkIndexX * this->_size + localCellIndexX - 1, this->_chunkIndexY * this->_size + localCellIndexY + 1 );
				this->_world->updateTileBorders( this->_chunkIndexX * this->_size + localCellIndexX + 0, this->_chunkIndexY * this->_size + localCellIndexY + 1 );
				this->_world->updateTileBorders( this->_chunkIndexX * this->_size + localCellIndexX + 1, this->_chunkIndexY * this->_size + localCellIndexY + 1 );
				this->_world->updateTileBorders( this->_chunkIndexX * this->_size + localCellIndexX - 1, this->_chunkIndexY * this->_size + localCellIndexY + 0 );

			}
		}
	}

	return;
}




void WorldChunk::insertTileRenders( TileIdentity tileId, bool consolidatable, std::uint8_t borders, std::uint8_t tileBlobMapIndex,
	std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->_tileRenders[0].insert( TileRender( tileId, consolidatable, borders, tileBlobMapIndex, BoundingBox<std::int64_t>( x, y, width, height ) ) );
	return;
}


void WorldChunk::insertTileRenders( const Tile& tile, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{

	this->_tileRenders[0].insert( TileRender( tile.getId(), tile.getConsolidatable(), tile.getBorders(), tile.getTileBlobMapIndex(), BoundingBox<std::int64_t>( x, y, width, height ) ) );
	return;
}


void WorldChunk::insertTile( TileIdentity tileId, TileType tileType, bool consolidatable, bool opaque, bool complementary, std::uint8_t borders, std::uint8_t tileBlobMapIndex, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	// Adds the tile to the world and worldRender
	this->insertTiles( tileId, tileType, consolidatable, opaque, complementary, borders, tileBlobMapIndex, x, y, width, height );
	//this->insertTileRenders( tileId, consolidatable, borders, tileBlobMapIndex, x, y, width, height ); // [!] dummy border
	return;
}


void WorldChunk::insertLightSources( TileIdentity tileId, std::int16_t r, std::int16_t g, std::int16_t b, std::int16_t a, std::int16_t radius,
	std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
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
				std::uint16_t lightSourceIndex = localCellIndexY * this->_size + localCellIndexX;

				if ( this->_lightSources.find( lightSourceIndex ) == this->_lightSources.end() )
				{
					this->_lightSources.emplace( lightSourceIndex, LightSource( tileId, r, g, b, a, radius ) );
				}
			}
		}
	}
	return;
}



void WorldChunk::insertLightSourceTile( TileIdentity tileId, TileType tileType, bool consolidatable, bool opaque, bool complementary, std::uint8_t borders, std::uint8_t tileBlobMapIndex,
	std::int16_t r, std::int16_t g, std::int16_t b, std::int16_t a, std::int16_t radius,
	std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	// Insert tile and  light source pair contingent on both existing
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
				std::uint16_t lightSourceIndex = localCellIndexY * this->_size + localCellIndexX;

				Tile* selectedTile = &this->_tiles[lightSourceIndex];
				if ( selectedTile->isVoid() && this->_lightSources.find( lightSourceIndex ) == this->_lightSources.end() )
				{
					selectedTile->setId( tileId );
					selectedTile->setType( tileType );
					selectedTile->setConsolidatable( consolidatable );
					selectedTile->setOpaque( false );
					selectedTile->setComplementary( complementary );
					selectedTile->setTileBlobMapIndex( tileBlobMapIndex );

					this->_lightSources.emplace( lightSourceIndex, LightSource( tileId, r, g, b, a, radius ) );

				}
			}
		}
	}

	this->insertTileRenders( tileId, consolidatable, borders, 0, x, y, width, height ); // [!] dummy border, consolidatable, and mapblobindex
	return;
}





void WorldChunk::insertVoid( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	return;
}


void WorldChunk::insertWater( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Water, TileType::Empty, true, false, true, borders, 0,
		x, y, width, height );
	return;
}

void WorldChunk::insertStone( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Stone, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}

void WorldChunk::insertDirt( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Dirt, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}





void WorldChunk::insertSand( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Sand, TileType::OneWay, true, false, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertTorch( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertLightSourceTile( TileIdentity::Torch, TileType::Empty, false, false, false, borders, 0,
		255, 255, 255, 255, 15,
		//100, 100, 100, 255, 15,
		x, y, width, height );
	
	/*
	this->insertLightSourceTile( TileIdentity::Torch, TileType::Empty, false, false, false, borders, 0,
		std::rand() % 256, std::rand() % 256, std::rand() % 256, std::rand() % 256, 15,
		x, y, width, height );
	*/
	return;
}


void WorldChunk::insertMossDirt( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::MossDirt, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertMossStone( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::MossStone, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertMull( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Mull, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertMor( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Mor, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertLightClay( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::LightClay, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertDarkClay( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::DarkClay, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertGravel( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Gravel, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertCharcoal( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Charcoal, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertSnow( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Snow, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertPermafrost( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Permafrost, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertPodzol( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Podzol, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertBleachedSand( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::BleachedSand, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertShale( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Shale, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertIronOxide( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::IronOxide, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertAluminiumOxide( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::AluminiumOxide, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertLaterite( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Laterite, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertAridsol( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Aridsol, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertEntisol( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Entisol, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertSaltstone( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Saltstone, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertQuartz( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Quartz, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertAlfisol( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Alfisol, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertYellowClay( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::YellowClay, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertRedClay( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::RedClay, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertCambisol( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Cambisol, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertSilt( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::Silt, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertIronOre( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::IronOre, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertAluminiumOre( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::AluminiumOre, TileType::Block, true, true, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertMapleLog( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::MapleLog, TileType::OneWay, true, false, false, borders, 1,
		x, y, width, height );
	return;
}


void WorldChunk::insertCrimsonMapleLeaves( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::CrimsonMapleLeaves, TileType::OneWay, true, false, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertPumpkinMapleLeaves( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::PumpkinMapleLeaves, TileType::OneWay, true, false, true, borders, 0,
		x, y, width, height );
	return;
}


void WorldChunk::insertGambogeMapleLeaves( std::int64_t x, std::int64_t y, std::uint8_t borders, std::int64_t width, std::int64_t height )
{
	this->insertTile( TileIdentity::GambogeMapleLeaves, TileType::OneWay, true, false, true, borders, 0,
		x, y, width, height );
	return;
}


const WorldChunk::insertFuncType WorldChunk::insertMethods[( std::size_t )TileIdentity::count]{
	&WorldChunk::insertVoid,
	&WorldChunk::insertWater,
	&WorldChunk::insertStone,
	&WorldChunk::insertDirt,
	&WorldChunk::insertSand,
	&WorldChunk::insertTorch,
	&WorldChunk::insertMossDirt,
	&WorldChunk::insertMossStone,
	&WorldChunk::insertMull,
	&WorldChunk::insertMor,
	&WorldChunk::insertLightClay,
	&WorldChunk::insertDarkClay,
	&WorldChunk::insertGravel,
	&WorldChunk::insertCharcoal,
	&WorldChunk::insertSnow,
	&WorldChunk::insertPermafrost,
	&WorldChunk::insertPodzol,
	&WorldChunk::insertBleachedSand,
	&WorldChunk::insertShale,
	&WorldChunk::insertIronOxide,
	&WorldChunk::insertAluminiumOxide,
	&WorldChunk::insertLaterite,
	&WorldChunk::insertAridsol,
	&WorldChunk::insertEntisol,
	&WorldChunk::insertSaltstone,
	&WorldChunk::insertQuartz,
	&WorldChunk::insertAlfisol,
	&WorldChunk::insertYellowClay,
	&WorldChunk::insertRedClay,
	&WorldChunk::insertCambisol,
	&WorldChunk::insertSilt,
	&WorldChunk::insertIronOre,
	&WorldChunk::insertAluminiumOre,
	&WorldChunk::insertMapleLog,
	&WorldChunk::insertCrimsonMapleLeaves,
	&WorldChunk::insertPumpkinMapleLeaves,
	&WorldChunk::insertGambogeMapleLeaves
};


void WorldChunk::insert( TileIdentity tileId, std::uint8_t borders, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	// Pointer to a method is slightly different than a pointer to a function
	//World::funcType method = World::insertMethods[0];
	//( this->*method )( tileId, x, y, width, height );
	//std::cout << ( unsigned long long )tileId << std::endl;
	( this->*WorldChunk::insertMethods[( std::size_t )tileId] )( x, y, borders, width, height ); // [!] error need thread mutex
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
					selectedTile->setType( TileType::Empty );
					selectedTile->setConsolidatable( false );
					selectedTile->setOpaque( false );
					selectedTile->setComplementary( true );
					selectedTile->setBorders( 0 );
					selectedTile->setTileBlobMapIndex( 0 );
				}

				this->_world->updateTileBorders( this->_chunkIndexX * this->_size + localCellIndexX, this->_chunkIndexY * this->_size + localCellIndexY );
				this->_world->updateTileBorders( this->_chunkIndexX * this->_size + localCellIndexX - 1, this->_chunkIndexY * this->_size + localCellIndexY - 1 );
				this->_world->updateTileBorders( this->_chunkIndexX * this->_size + localCellIndexX + 0, this->_chunkIndexY * this->_size + localCellIndexY - 1 );
				this->_world->updateTileBorders( this->_chunkIndexX * this->_size + localCellIndexX + 1, this->_chunkIndexY * this->_size + localCellIndexY - 1 );
				this->_world->updateTileBorders( this->_chunkIndexX * this->_size + localCellIndexX + 1, this->_chunkIndexY * this->_size + localCellIndexY + 0 );
				this->_world->updateTileBorders( this->_chunkIndexX * this->_size + localCellIndexX - 1, this->_chunkIndexY * this->_size + localCellIndexY + 1 );
				this->_world->updateTileBorders( this->_chunkIndexX * this->_size + localCellIndexX + 0, this->_chunkIndexY * this->_size + localCellIndexY + 1 );
				this->_world->updateTileBorders( this->_chunkIndexX * this->_size + localCellIndexX + 1, this->_chunkIndexY * this->_size + localCellIndexY + 1 );
				this->_world->updateTileBorders( this->_chunkIndexX * this->_size + localCellIndexX - 1, this->_chunkIndexY * this->_size + localCellIndexY + 0 );
			}
		}
	}

	return;
}


void WorldChunk::removeTileRenders( TileIdentity id, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->_tileRenders[0].remove( TileRender( id, false, 0, 0, BoundingBox<std::int64_t>( x, y, width, height ) ) );
	return;
}



void WorldChunk::removeTile( TileIdentity id, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTiles( id, x, y, width, height );
	this->removeTileRenders( id, x, y, width, height );
	return;
}


void WorldChunk::removeLightSources( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
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
				std::uint16_t lightSourceIndex = localCellIndexY * this->_size + localCellIndexX;

				if ( this->_lightSources.find( lightSourceIndex ) != this->_lightSources.end() && this->_lightSources.at( lightSourceIndex ).getTileId() == tileId )
				{
					this->_lightSources.erase( lightSourceIndex );
				}
			}
		}
	}

	this->removeTileRenders( tileId, x, y, width, height );

	return;
}


void WorldChunk::removeLightSourceTiles( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	// Remove tile and light source pair contingent on both existing
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
				std::uint16_t lightSourceIndex = localCellIndexY * this->_size + localCellIndexX;


				if ( selectedTile->getId() == tileId &&
					this->_lightSources.find( lightSourceIndex ) != this->_lightSources.end() && this->_lightSources.at( lightSourceIndex ).getTileId() == tileId )
				{
					selectedTile->setId( TileIdentity::Void );
					selectedTile->setOpaque( false );
					this->_lightSources.erase( lightSourceIndex );
				}
			}
		}
	}

	this->removeTileRenders( tileId, x, y, width, height );

	return;
}


void WorldChunk::removeVoid( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	return;
}

void WorldChunk::removeWater( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Water,
		x, y, width, height );
	return;
}


void WorldChunk::removeStone( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Stone,
		x, y, width, height );
	return;
}


void WorldChunk::removeDirt( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Dirt,
		x, y, width, height );
	return;
}


void WorldChunk::removeSand( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Sand,
		x, y, width, height );
	return;
}


void WorldChunk::removeTorch( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	/*
	this->removeTile( TileIdentity::Torch, x, y, width, height );
	this->removeLightSources( TileIdentity::Torch, x, y, width, height );
	*/
	this->removeLightSourceTiles( TileIdentity::Torch, x, y, width, height );
	return;
}


void WorldChunk::removeMossDirt( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::MossDirt,
		x, y, width, height );
	return;
}


void WorldChunk::removeMossStone( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::MossStone,
		x, y, width, height );
	return;
}


void WorldChunk::removeMull( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Mull,
		x, y, width, height );
	return;
}


void WorldChunk::removeMor( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Mor,
		x, y, width, height );
	return;
}


void WorldChunk::removeLightClay( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::LightClay,
		x, y, width, height );
	return;
}


void WorldChunk::removeDarkClay( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::DarkClay,
		x, y, width, height );
	return;
}


void WorldChunk::removeGravel( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Gravel,
		x, y, width, height );
	return;
}


void WorldChunk::removeCharcoal( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Charcoal,
		x, y, width, height );
	return;
}

void WorldChunk::removeSnow( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Snow,
		x, y, width, height );
	return;
}


void WorldChunk::removePermafrost( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Permafrost,
		x, y, width, height );
	return;
}


void WorldChunk::removePodzol( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Podzol,
		x, y, width, height );
	return;
}


void WorldChunk::removeBleachedSand( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::BleachedSand,
		x, y, width, height );
	return;
}


void WorldChunk::removeShale( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Shale,
		x, y, width, height );
	return;
}


void WorldChunk::removeIronOxide( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::IronOxide,
		x, y, width, height );
	return;
}


void WorldChunk::removeAluminiumOxide( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::AluminiumOxide,
		x, y, width, height );
	return;
}


void WorldChunk::removeLaterite( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Laterite,
		x, y, width, height );
	return;
}


void WorldChunk::removeAridsol( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Aridsol,
		x, y, width, height );
	return;
}


void WorldChunk::removeEntisol( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Entisol,
		x, y, width, height );
	return;
}


void WorldChunk::removeSaltstone( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Saltstone,
		x, y, width, height );
	return;
}


void WorldChunk::removeQuartz( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Quartz,
		x, y, width, height );
	return;
}


void WorldChunk::removeAlfisol( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Alfisol,
		x, y, width, height );
	return;
}


void WorldChunk::removeYellowClay( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::YellowClay,
		x, y, width, height );
	return;
}


void WorldChunk::removeRedClay( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::RedClay,
		x, y, width, height );
	return;
}


void WorldChunk::removeCambisol( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Cambisol,
		x, y, width, height );
	return;
}


void WorldChunk::removeSilt( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::Silt,
		x, y, width, height );
	return;
}


void WorldChunk::removeIronOre( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::IronOre,
		x, y, width, height );
	return;
}


void WorldChunk::removeAluminiumOre( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::AluminiumOre,
		x, y, width, height );
	return;
}


void WorldChunk::removeMapleLog( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::MapleLog,
		x, y, width, height );
	return;
}


void WorldChunk::removeCrimsonMapleLeaves( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::CrimsonMapleLeaves,
		x, y, width, height );
	return;
}


void WorldChunk::removePumpkinMapleLeaves( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::PumpkinMapleLeaves,
		x, y, width, height );
	return;
}


void WorldChunk::removeGambogeMapleLeaves( std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	this->removeTile( TileIdentity::GambogeMapleLeaves,
		x, y, width, height );
	return;
}


const WorldChunk::removeFuncType WorldChunk::removeMethods[( std::size_t )TileIdentity::count]{
	&WorldChunk::removeVoid,
	&WorldChunk::removeWater,
	&WorldChunk::removeStone,
	&WorldChunk::removeDirt,
	&WorldChunk::removeSand,
	&WorldChunk::removeTorch,
	&WorldChunk::removeMossDirt,
	&WorldChunk::removeMossStone,
	&WorldChunk::removeMull,
	&WorldChunk::removeMor,
	&WorldChunk::removeLightClay,
	&WorldChunk::removeDarkClay,
	&WorldChunk::removeGravel,
	&WorldChunk::removeCharcoal,
	&WorldChunk::removeSnow,
	&WorldChunk::removePermafrost,
	&WorldChunk::removePodzol,
	&WorldChunk::removeBleachedSand,
	&WorldChunk::removeShale,
	&WorldChunk::removeIronOxide,
	&WorldChunk::removeAluminiumOxide,
	&WorldChunk::removeLaterite,
	&WorldChunk::removeAridsol,
	&WorldChunk::removeEntisol,
	&WorldChunk::removeSaltstone,
	&WorldChunk::removeQuartz,
	&WorldChunk::removeAlfisol,
	&WorldChunk::removeYellowClay,
	&WorldChunk::removeRedClay,
	&WorldChunk::removeCambisol,
	&WorldChunk::removeSilt,
	&WorldChunk::removeIronOre,
	&WorldChunk::removeAluminiumOre,

	&WorldChunk::removeMapleLog,
	&WorldChunk::removeCrimsonMapleLeaves,
	&WorldChunk::removePumpkinMapleLeaves,
	&WorldChunk::removeGambogeMapleLeaves

};



void WorldChunk::remove( TileIdentity tileId, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	( this->*WorldChunk::removeMethods[( std::size_t )tileId] )( x, y, width, height );
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
	// Clears the tile array, lights, and lightSources
	this->_chunkIndexX = 0;
	this->_chunkIndexY = 0;

	std::memset( this->_tiles, 0, sizeof( this->_tiles ) ); // this->_tiles.fill(0)

	this->blackenLights();
	this->clearLightSources();
	return;
}


void WorldChunk::delimit( std::int64_t indexX, std::int64_t indexY )
{
	this->clear();
	this->_chunkIndexX = indexX;
	this->_chunkIndexY = indexY;
	this->wipeRender();
	this->wipeLightRender();
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


BoundingBox<std::int64_t> WorldChunk::getBounds() const
{
	return BoundingBox<std::int64_t>( this->_chunkIndexX * this->_size, this->_chunkIndexY * this->_size, this->_size, this->_size );
}









Tile* WorldChunk::getTiles()
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


std::map<std::uint16_t, LightSource>& WorldChunk::getLightSources()
{
	return this->_lightSources;
}


QuadTree<LightRender>* WorldChunk::getLightRenders()
{
	return ( this->_lightRenders );
}


QuadTree<LightRender>* WorldChunk::getPrevLightRenders()
{
	return ( this->_prevLightRenders );
}


void WorldChunk::resetLights()
{
	this->wipeLightRender();
	this->blackenLights();
	return;
}


/*
void WorldChunk::blackenLights()
{
	//std::cout << "blacken" << std::endl;
	for ( std::uint16_t i = 0; i < this->_size * this->_size; i++ )
	{
		this->_lights[i].blacken();
		//this->_lights[i].whiten();

	}
	return;
}
*/

void WorldChunk::blackenLights()
{
	//std::cout << "blacken" << std::endl;
	static const std::int64_t OVERWORLD_HEIGHT = 1536;
	static const std::int64_t SUNRAY_POWER = 3;

	for ( std::uint16_t x = 0; x < this->_size; x++ )
	{
		std::int64_t worldX = this->_chunkIndexX * this->_size + x;
		long double terraneanHeightPerlinVal = this->_world->getTerraneanHeightMap().getPerlinValue( worldX );
		std::int64_t worldYTerranean = -( std::int64_t )( terraneanHeightPerlinVal * ( long double )OVERWORLD_HEIGHT );

		for ( std::uint16_t y = 0; y < this->_size; y++ )
		{
			std::int64_t worldY = this->_chunkIndexY * this->_size + y;
			if ( worldY < worldYTerranean + SUNRAY_POWER )
			{
				this->_lights[y * this->_size + x].whiten();
			}
			else
			{
				this->_lights[y * this->_size + x].blacken();
			}
		}
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
	( this->_lightRenders )[0].constructQuadTree(
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
	//for ( int i = 0; i < sizeof( this->_lightRenders ) / sizeof( this->_lightRenders[0] ); i++ )
	for ( int i = 0; i < ( WorldChunk::_numTileRenders * sizeof( QuadTree<LightRender> ) ) / sizeof( this->_lightRenders[0] ); i++ )
	{
		( this->_lightRenders )[i].divide();
	}


	//std::cout << WorldChunk::_numTileRenders * sizeof( QuadTree<LightRender> ) << std::endl;
	return;
}


void WorldChunk::wipeLightRenders()
{
	// Need to update chunkIndex of lighting when updating index of worldChunk [!]
	std::int64_t rootQuadTreePositionX = this->_chunkIndexX * this->_size;
	std::int64_t rootQuadTreePositionY = this->_chunkIndexY * this->_size;

	// Intialize quadTrees
	( this->_lightRenders )[0].constructQuadTree(
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
	//for ( int i = 0; i < sizeof( this->_lightRenders ) / sizeof( this->_lightRenders[0] ); i++ )
	for ( int i = 0; i < ( WorldChunk::_numTileRenders * sizeof( QuadTree<LightRender> ) ) / sizeof( this->_lightRenders[0] ); i++ )
	{
		( this->_lightRenders )[i].divide();
	}





	( this->_prevLightRenders )[0].constructQuadTree(
		0,
		-1,
		Settings::WorldChunk::TILE_RENDER_MAX_LEVEL,
		0,
		BoundingBox<std::int64_t>( rootQuadTreePositionX, rootQuadTreePositionY, this->_size, this->_size ),
		this->_prevLightRenders,
		Settings::WorldChunk::TILE_RENDER_MIN_LEVEL,
		Settings::WorldChunk::TILE_RENDER_MAX_LEVEL,
		Settings::WorldChunk::TILE_RENDER_MIN_CELL_SIZE
	);

	// The difference between construct and reconstruct is quick reassignment, unless of course, shallow copy does this for us. Then just use construct
	//for ( int i = 0; i < sizeof( this->_lightRenders ) / sizeof( this->_lightRenders[0] ); i++ )
	for ( int i = 0; i < ( WorldChunk::_numTileRenders * sizeof( QuadTree<LightRender> ) ) / sizeof( this->_prevLightRenders[0] ); i++ )
	{
		( this->_prevLightRenders )[i].divide();
	}


	//std::cout << WorldChunk::_numTileRenders * sizeof( QuadTree<LightRender> ) << std::endl;
	return;
}




void WorldChunk::insertLightRender( std::uint32_t corner0, std::uint32_t corner1, std::uint32_t corner2, std::uint32_t corner3, bool exist, std::int64_t x, std::int64_t y )
{
	( this->_lightRenders )[0].insert( LightRender( corner0, corner1, corner2, corner3, exist,
		BoundingBox<std::int64_t>( x, y, 1, 1 ) ) );
	return;
}


void WorldChunk::insertLightRenders( std::uint32_t corner0, std::uint32_t corner1, std::uint32_t corner2, std::uint32_t corner3, bool exist, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height )
{
	( this->_lightRenders )[0].insert( LightRender( corner0, corner1, corner2, corner3, exist,
		BoundingBox<std::int64_t>( x, y, width, height ) ) );
	return;
}



void  WorldChunk::swapLightRenders()
{
	QuadTree<LightRender>* temp = this->_lightRenders;
	QuadTree<LightRender>* temp2 = this->_prevLightRenders;

	this->_lightRenders = temp2;
	this->_prevLightRenders = temp;
	return;
}
