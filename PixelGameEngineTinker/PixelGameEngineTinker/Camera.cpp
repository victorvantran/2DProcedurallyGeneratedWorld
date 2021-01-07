#include <cstdint>
#include "Settings.h"
#include "BoundingBox.h"
#include "World.h"
#include "Atlas.h"
#include "WorldChunk.h"
#include "QuadTree.h"
#include "TileRender.h"
#include "Tile.h"


Camera::Camera()
	: _focalPoint( BoundingBox<long double>() ), _view( BoundingBox<long double>() ), _zoomX( 1.0f ), _zoomY( 1.0f ), _world( nullptr )
{

}


Camera::Camera( BoundingBox<long double> focalPoint, BoundingBox<long double> view, long double zoomX, long double zoomY, World* world )
	: _focalPoint( focalPoint ), _view( view ), _zoomX( zoomX ), _zoomY( zoomY ), _world( world )
{
	this->_view.setCenterX( this->_focalPoint.getCenterX() );
	this->_view.setCenterY( this->_focalPoint.getCenterY() );


	this->_spriteLight = new olc::Sprite( "./pngs/light.png" );
	this->_decalLight = new olc::Decal( this->_spriteLight );
}


Camera::~Camera()
{
	delete this->_spriteLight;
	this->_spriteLight = nullptr;

	delete this->_decalLight;
	this->_decalLight = nullptr;
}


void Camera::screenToWorld( std::int64_t pixelX, std::int64_t pixelY, long double& cellX, long double& cellY ) const
{
	// int to float ( camera offest determines displacement )

	std::uint16_t tileSize = Settings::Screen::CELL_PIXEL_SIZE;
	cellX = ( long double )( ( ( long double )( pixelX - this->_absolutePixelOffsetX ) / ( long double )( this->_zoomX * tileSize ) ) + ( this->_focalPoint.x ) );
	cellY = ( long double )( ( ( long double )( pixelY - this->_absolutePixelOffsetY ) / ( long double )( this->_zoomY * tileSize ) ) + ( this->_focalPoint.y ) );
	return;
}


void Camera::worldToScreen( long double cellX, long double cellY, std::int64_t& pixelX, std::int64_t& pixelY ) const
{
	// float to int ( camera offset determines displacement )

	std::uint16_t tileSize = Settings::Screen::CELL_PIXEL_SIZE;
	pixelX = ( std::int64_t )( ( cellX - ( this->_focalPoint.x ) ) * ( this->_zoomX * tileSize ) ) + this->_absolutePixelOffsetX;
	pixelY = ( std::int64_t )( ( cellY - ( this->_focalPoint.y ) ) * ( this->_zoomY * tileSize ) ) + this->_absolutePixelOffsetY;
	return;
}


void Camera::renderWorld() const // [!] leave to rendering to world
{
	WorldChunk* worldChunks = this->_world->getWorldChunks();
	int numWorldChunks = this->_world->getNumWorldChunks();

	for ( int i = 0; i < numWorldChunks; i++ )
	{
		this->renderWorldChunk( worldChunks[i], this->_world->getAtlas() );
		//this->renderTilesDebug( worldChunks[i] );
	}

	return;
}


void Camera::renderWorldChunk( WorldChunk& worldChunk, Atlas& atlas ) const
{
	std::uint16_t tileSize = Settings::Screen::CELL_PIXEL_SIZE;; // [!] make it from a "global" variable

	std::uint16_t chunkSize = worldChunk.getSize(); // [@]
	std::int64_t worldPositionX = worldChunk.getChunkIndexX() * chunkSize; // [@]
	std::int64_t worldPositionY = worldChunk.getChunkIndexY() * chunkSize;

	std::int64_t pixelX;
	std::int64_t pixelY;

	worldToScreen( worldPositionX, worldPositionY, pixelX, pixelY );

	olc::v2d_generic<std::int64_t> startPos = olc::v2d_generic<std::int64_t>{ pixelX, pixelY };
	olc::v2d_generic<std::int64_t> size = olc::v2d_generic<std::int64_t>{ ( int )( chunkSize * ( this->_zoomX * tileSize ) ), ( int )( chunkSize * ( this->_zoomY * tileSize ) ) };

	pge->DrawRect(
		startPos,
		size,
		olc::GREEN
	);
	
	this->renderTileRenders( worldChunk.getTileRendersRoot(), atlas );
	this->renderLights( worldChunk );
	return;
}


void Camera::renderTileRenders( QuadTree<TileRender>& tileRenders, Atlas& atlas ) const
{
	// Render every tileRender but only drawing one properlly scaled tile for each consolidated render

	std::uint16_t tileSize = Settings::Screen::CELL_PIXEL_SIZE;
	std::uint16_t chunkSize = Settings::World::CHUNK_CELL_SIZE;

	QuadTree<TileRender> currQuadTree = tileRenders.getReferenceNodes()[tileRenders.getIndex()];
	const BoundingBox<std::int64_t> bounds = currQuadTree.getBounds();

	// No need to render if the camera can not see it
	if ( !this->_view.intersects( bounds ) )
	{
		return;
	}

	// Fill Consolidated
	if ( currQuadTree.isConsolidated() )
	{
		if ( this->_view.intersects( currQuadTree.getBounds() ) && currQuadTree.getCells()[0].exists() )
		{
			std::uint64_t id = currQuadTree.getCells()[0].getId();
			olc::Decal* tileDecal = atlas.getDecal( id );
			if ( tileDecal == nullptr )
			{
				return;
			}

			int level = currQuadTree.getLevel();
			int scale = 2 << ( level );

			std::int64_t worldPositionX = currQuadTree.getBounds().getX(); // [@]
			std::int64_t worldPositionY = currQuadTree.getBounds().getY();
			std::int64_t pixelX;
			std::int64_t pixelY;
			worldToScreen( worldPositionX, worldPositionY, pixelX, pixelY );
			olc::v2d_generic<std::int64_t> startPos = olc::v2d_generic<std::int64_t>{ pixelX, pixelY };

			pge->DrawPartialDecal(
				startPos,
				olc::v2d_generic<long double>{ this->_zoomX * tileSize, this->_zoomY * tileSize } *scale,
				tileDecal,
				olc::vf2d{ 0, Settings::Camera::CONSOLIDATED_TILE_OFFSET },
				olc::vf2d{ 1, 1 } *( Settings::Screen::CELL_PIXEL_SIZE * Settings::Screen::PIXEL_SIZE * scale )
			);
		}

		return;
	}
	// Fill the small bounding boxes (cells) that are not possibly consolidated
	else
	{
		if ( currQuadTree.getLevel() == currQuadTree.getMinLevel() )
		{
			TileRender* cells = currQuadTree.getCells();
			for ( int i = 0; i < 4; i++ )
			{
				if ( cells[i].exists() && this->_view.intersects( cells[i].getBounds() ) )
				{
					std::uint64_t id = cells[i].getId();
					olc::Decal* tileDecal = atlas.getDecal( id );
					if ( tileDecal == nullptr )
					{
						continue;
					}

					std::int64_t worldPositionX = cells[i].getBounds().getX(); // [@]
					std::int64_t worldPositionY = cells[i].getBounds().getY();
					std::int64_t pixelX;
					std::int64_t pixelY;
					worldToScreen( worldPositionX, worldPositionY, pixelX, pixelY );
					olc::v2d_generic<std::int64_t> startPos = olc::v2d_generic<std::int64_t>{ pixelX, pixelY };


					pge->DrawPartialDecal(
						startPos,
						olc::v2d_generic<long double>{ ( tileSize * this->_zoomX ), ( tileSize * this->_zoomY ) },
						tileDecal,
						olc::vf2d{ 0, 128 }, // [!] temp based on configuration
						olc::vf2d{ 1, 1 } * ( Settings::Screen::CELL_PIXEL_SIZE * Settings::Screen::PIXEL_SIZE )
					);

				}
			}

			return;
		}

		// Fill other psosible consolidated (or not-consolidated) boundingboxes
		int* childrenNodeIndicies = currQuadTree.getChildrenNodeIndicies();
		if ( childrenNodeIndicies != nullptr )
		{
			for ( int i = 0; i < 4; i++ )
			{
				if ( childrenNodeIndicies[i] != -1 )
				{
					this->renderTileRenders( tileRenders.getReferenceNodes()[childrenNodeIndicies[i]], atlas );
				}
			}
		}
	}

	return;
}



void Camera::renderLights( WorldChunk& worldChunk ) const
{
	// Render every tileRender but only drawing one properlly scaled tile for each consolidated render

	std::uint16_t tileSize = Settings::Screen::CELL_PIXEL_SIZE;
	std::uint16_t chunkSize = Settings::WorldChunk::SIZE;



	Light* lights = worldChunk.getLights();
	const BoundingBox<std::int64_t> bounds = BoundingBox<std::int64_t>( worldChunk.getChunkIndexX() * chunkSize, worldChunk.getChunkIndexY() * chunkSize, chunkSize, chunkSize );

	
	// No need to render if the camera can not see it
	if ( !this->_view.intersects( bounds ) )
	{
		return;
	}


	for ( std::int16_t i = 0; i < Settings::World::NUM_CELLS_PER_CHUNK; i++ )
	{
		std::uint16_t y = i / chunkSize;
		std::uint16_t x = i % chunkSize;


		std::int64_t absoluteX = x + worldChunk.getChunkIndexX() * chunkSize;
		std::int64_t absoluteY = y + worldChunk.getChunkIndexY() * chunkSize;



		std::int64_t topLeftX = (std::int64_t )std::floor( this->_view.getX() );
		std::int64_t topLeftY = ( std::int64_t )std::floor( this->_view.getY() );


		std::int64_t bottomRightX = ( std::int64_t )std::ceil( this->_view.getX() + this->_view.getWidth()/2 );
		std::int64_t bottomRightY = ( std::int64_t )std::ceil( this->_view.getY() + this->_view.getHeight()/2 );

		// Only render lights within camera frame
		if ( absoluteX >= topLeftX && absoluteX <= bottomRightX && absoluteY >= topLeftY && absoluteY <= bottomRightY )
		{
			//std::cout << "(" << absoluteX << ", " << absoluteY << ")" << std::endl;
			//std::cout << "(" << (std::int64_t)std::ceil( absoluteX - this->_view.getX() ) << ", " << ( std::int64_t )std::ceil( absoluteY - this->_view.getY() ) << ")" << std::endl;

			
			std::int64_t screenTranslatedX = ( std::int64_t )std::ceil( absoluteX - this->_view.getX() );
			std::int64_t screenTranslatedY = ( std::int64_t )std::ceil( absoluteY - this->_view.getY() );


			int ne = ( y - 1 ) * chunkSize + ( x + 1 ); // Northerneastern neighbor
			int n = ( y - 1 ) * chunkSize + x; // Northern neighbor
			int nw = ( y - 1 ) * chunkSize + ( x - 1 ); // // Northwestern

			int e = y * chunkSize + ( x + 1 ); // Eastern neighbor
			int c = y * chunkSize + x; // Current tile
			int w = y * chunkSize + ( x - 1 ); // Western neighbor

			int s = ( y + 1 ) * chunkSize + x; // Southern neighbor
			int sw = ( y + 1 ) * chunkSize + ( x - 1 ); // Southwestern neighbor
			int se = ( y + 1 ) * chunkSize + ( x + 1 ); // Southeastern neighbor



			// Temp in bounds
			if ( ne < 0 && ne >= chunkSize ||
				n < 0 && n >= chunkSize ||
				nw < 0 && nw >= chunkSize ||

				e < 0 && e >= chunkSize ||
				c < 0 && c >= chunkSize ||
				w < 0 && w >= chunkSize ||


				se < 0 && se >= chunkSize ||
				s < 0 && s >= chunkSize ||
				sw < 0 && sw >= chunkSize 
				)
			{
				return;
			}





			std::uint8_t corner0R = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( lights[i].getRed() + lights[nw].getRed() + lights[n].getRed() + lights[w].getRed() ) / 4 ), 255 ) );
			std::uint8_t corner0G = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( lights[i].getGreen() + lights[nw].getGreen() + lights[n].getGreen() + lights[w].getGreen() ) / 4 ), 255 ) );
			std::uint8_t corner0B = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( lights[i].getBlue() + lights[nw].getBlue() + lights[n].getBlue() + lights[w].getBlue() ) / 4 ), 255 ) );
			std::uint8_t corner0A = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( lights[i].getAlpha() + lights[nw].getAlpha() + lights[n].getAlpha() + lights[w].getAlpha() ) / 4 ), 255 ) );


			std::uint8_t corner1R = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( lights[i].getRed() + lights[sw].getRed() + lights[s].getRed() + lights[w].getRed() ) / 4 ), 255 ) );
			std::uint8_t corner1G = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( lights[i].getGreen() + lights[sw].getGreen() + lights[s].getGreen() + lights[w].getGreen() ) / 4 ), 255 ) );
			std::uint8_t corner1B = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( lights[i].getBlue() + lights[sw].getBlue() + lights[s].getBlue() + lights[w].getBlue() ) / 4 ), 255 ) );
			std::uint8_t corner1A = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( lights[i].getAlpha() + lights[sw].getAlpha() + lights[s].getAlpha() + lights[w].getAlpha() ) / 4 ), 255 ) );



			std::uint8_t corner2R = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( lights[i].getRed() + lights[se].getRed() + lights[s].getRed() + lights[e].getRed() ) / 4 ), 255 ) );
			std::uint8_t corner2G = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( lights[i].getGreen() + lights[se].getGreen() + lights[s].getGreen() + lights[e].getGreen() ) / 4 ), 255 ) );
			std::uint8_t corner2B = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( lights[i].getBlue() + lights[se].getBlue() + lights[s].getBlue() + lights[e].getBlue() ) / 4 ), 255 ) );
			std::uint8_t corner2A = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( lights[i].getAlpha() + lights[se].getAlpha() + lights[s].getAlpha() + lights[e].getAlpha() ) / 4 ), 255 ) );


			std::uint8_t corner3R = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( lights[i].getRed() + lights[ne].getRed() + lights[n].getRed() + lights[e].getRed() ) / 4 ), 255 ) );
			std::uint8_t corner3G = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( lights[i].getGreen() + lights[ne].getGreen() + lights[n].getGreen() + lights[e].getGreen() ) / 4 ), 255 ) );
			std::uint8_t corner3B = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( lights[i].getBlue() + lights[ne].getBlue() + lights[n].getBlue() + lights[e].getBlue() ) / 4 ), 255 ) );
			std::uint8_t corner3A = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( lights[i].getAlpha() + lights[ne].getAlpha() + lights[n].getAlpha() + lights[e].getAlpha() ) / 4 ), 255 ) );

			if ( corner0R == 0 && corner0G == 0 && corner0B == 0 &&
				corner1R == 0 && corner1G == 0 && corner1B == 0 &&
				corner2R == 0 && corner2G == 0 && corner2B == 0 &&
				corner3R == 0 && corner3G == 0 && corner3B == 0 )
			{
				//continue;
			}

			olc::Pixel colors[4];
			colors[0] = olc::Pixel{ corner0R, corner0G, corner0B, corner0A };
			colors[1] = olc::Pixel{ corner1R, corner1G, corner1B, corner1A };
			colors[2] = olc::Pixel{ corner2R, corner2G, corner2B, corner2A };
			colors[3] = olc::Pixel{ corner3R, corner3G, corner3B, corner3A };


			std::int64_t pixelX;
			std::int64_t pixelY;
			worldToScreen( absoluteX, absoluteY, pixelX, pixelY );
			olc::v2d_generic<std::int64_t> startPos = olc::v2d_generic<std::int64_t>{ pixelX, pixelY };



			olc::vf2d verticiesB[4];
			verticiesB[0] = olc::vf2d{ ( float )pixelX, ( float )pixelY };
			verticiesB[1] = olc::vf2d{ ( float )pixelX, ( ( float )pixelY + ( float )Settings::Screen::CELL_PIXEL_SIZE ) * ( float )this->_zoomY };
			verticiesB[2] = olc::vf2d{ ( float )pixelX + ( float )Settings::Screen::CELL_PIXEL_SIZE * ( float )this->_zoomX, ( float )pixelY + ( float )Settings::Screen::CELL_PIXEL_SIZE * ( float )this->_zoomY };
			verticiesB[3] = olc::vf2d{ ( float )pixelX + ( float )Settings::Screen::CELL_PIXEL_SIZE * ( float )this->_zoomX, ( float )pixelY };


			olc::vf2d textureCoordinates[4];
			textureCoordinates[0] = olc::vf2d{ 0.0f, 0.0f };
			textureCoordinates[1] = olc::vf2d{ 0.0f, 1.0f };
			textureCoordinates[2] = olc::vf2d{ 1.0f, 1.0f };
			textureCoordinates[3] = olc::vf2d{ 1.0f, 0.0f };


			
			olc::Decal* lightDecal = this->_decalLight;

			
			//pge->SetDrawTarget( this->_spriteLight );

			pge->SetDecalMode( olc::DecalMode::MULTIPLICATIVE );
			pge->DrawExplicitDecal(
				lightDecal,
				verticiesB,
				textureCoordinates,
				colors
			);
			
			
			/*
			std::cout << "_______________________________________________" << std::endl;
			std::cout << "(" << verticiesB[0].x << ", " << verticiesB[0].y << ")" << std::endl;
			std::cout << "(" << verticiesB[1].x << ", " << verticiesB[1].y << ")" << std::endl;
			std::cout << "(" << verticiesB[2].x << ", " << verticiesB[2].y << ")" << std::endl;
			std::cout << "(" << verticiesB[3].x << ", " << verticiesB[3].y << ")" << std::endl;
			*/



			/*
			pge->FillRect(
				olc::v2d_generic<std::int64_t>{ absoluteX, absoluteY },
				olc::v2d_generic<std::int64_t>{16, 16},
				olc::GREEN
			);
			*/

			/*
			pge->DrawDecal(
				olc::v2d_generic<std::int64_t>{ ( std::int64_t )((float)absoluteX/ ( float )chunkSize), ( std::int64_t )(( float )absoluteY/ ( float )chunkSize) },
				lightDecal,
				olc::v2d_generic<std::int64_t>{1, 1},
				olc::RED
			);
			*/





			/*
			std::int64_t pixelX;
			std::int64_t pixelY;
			worldToScreen( absoluteX, absoluteY, pixelX, pixelY );
			olc::v2d_generic<std::int64_t> startPos = olc::v2d_generic<std::int64_t>{ pixelX, pixelY };



			pge->SetDecalMode( olc::DecalMode::MULTIPLICATIVE );

			pge->DrawDecal(
				startPos,
				lightDecal,
				olc::v2d_generic<long double>{1 * this->_zoomX, 1 * this->_zoomY},
				olc::RED
			);

			pge->SetDecalMode( olc::DecalMode::NORMAL );
			*/






			pge->SetDecalMode( olc::DecalMode::MULTIPLICATIVE );

			pge->DrawDecal(
				startPos,
				lightDecal,
				olc::v2d_generic<long double>{1 * this->_zoomX, 1 * this->_zoomY},
				olc::RED
			);

			pge->SetDecalMode( olc::DecalMode::NORMAL );

			//return;
			/*
			startPos,
				olc::v2d_generic<long double>{ this->_zoomX* tileSize, this->_zoomY* tileSize } *scale,
				tileDecal,
				olc::vf2d{ 0, Settings::Camera::CONSOLIDATED_TILE_OFFSET },
				olc::vf2d{ 1, 1 } *( Settings::Screen::CELL_PIXEL_SIZE * Settings::Screen::PIXEL_SIZE * scale )
				*/

		}


		//std::cout << "(" << absoluteX << ", " << absoluteY << ")" << std::endl;
		//std::cout << "(" << localCol << ", " << localRow << ")" << std::endl;
		// std::cout << "(" << worldChunk.getChunkIndexX() * chunkSize << ", " << worldChunk.getChunkIndexY() * chunkSize << ")" << std::endl;
		//std::cout << "(" << chunkSize << ", " <<  chunkSize << ")" << std::endl;

		//std::cout << "(" << worldChunk.getChunkIndexX() *32 << ", " << worldChunk.getChunkIndexY() * 32 << ")" << std::endl;

	}
	//std::cout << "(" << worldChunk.getChunkIndexX() << ", " << worldChunk.getChunkIndexY() << ")" << std::endl;
	//std::cout << "(" << worldChunk.getChunkIndexX() * chunkSize << ", " << worldChunk.getChunkIndexY() * chunkSize << ")" << std::endl;
	//std::cout << "(" << this->_view.getX() << ", " << this->_view.getY() << ")" << std::endl;



	/*
	for ( int i = 0; i < 4; i++ )
	{
		if ( cells[i].getExist() && this->_view.intersects( cells[i].getBounds() ) )
		{

			std::int64_t worldPositionX = cells[i].getBounds().getX(); // [@]
			std::int64_t worldPositionY = cells[i].getBounds().getY();
			std::int64_t pixelX;
			std::int64_t pixelY;
			worldToScreen( worldPositionX, worldPositionY, pixelX, pixelY );
			olc::v2d_generic<std::int64_t> startPos = olc::v2d_generic<std::int64_t>{ pixelX, pixelY };

			tileRenders.get
			std::int64_t localCellStartIndexX = pixelX - this->_chunkIndexX * this->_size;
			std::int64_t localCellStartIndexY = pixelY - this->_chunkIndexY * this->_size;

			pge->DrawPartialDecal(
			startPos,
				olc::v2d_generic<long double>{ ( tileSize* this->_zoomX ), ( tileSize* this->_zoomY ) },
				tileDecal,
				olc::vf2d{ 0, 128 }, // [!] temp based on configuration
				olc::vf2d{ 1, 1 } *( Settings::Screen::CELL_PIXEL_SIZE * Settings::Screen::PIXEL_SIZE )
			);

		}
	}
	*/

	return;
}



void Camera::renderCamera() const
{
	// Render the camera 

	std::uint16_t tileSize = Settings::Screen::CELL_PIXEL_SIZE;

	// Draw Focal Point
	pge->DrawRect
	(
		olc::v2d_generic<std::int64_t>{ this->_absolutePixelOffsetX, this->_absolutePixelOffsetY },
		olc::vi2d{ ( int )( this->_focalPoint.width * this->_zoomX * tileSize ), ( int )( this->_focalPoint.height * this->_zoomY * tileSize ) },
		olc::WHITE
	);

	// Draw View
	pge->DrawRect
	(
		olc::vi2d{ 0, 0 },
		olc::vi2d{ ( int )( this->_view.width * this->_zoomX * tileSize ), ( int )( this->_view.height * this->_zoomY * tileSize ) },
		olc::YELLOW
	);

	return;
}


void Camera::renderTilesDebug( WorldChunk& worldChunk ) const
{
	// Render every single tile directly from the array of tiles for each worldChunk

	std::uint16_t tileSize = Settings::Screen::CELL_PIXEL_SIZE;
	int tileCellSize = 1; // [!] make it from a "global" variable

	int chunkSize = worldChunk.getSize();
	std::int64_t worldPositionX = worldChunk.getChunkIndexX() * chunkSize; // [@]
	std::int64_t worldPositionY = worldChunk.getChunkIndexY() * chunkSize;
	std::int64_t tilePixelX;
	std::int64_t tilePixelY;
	olc::v2d_generic<std::int64_t> tileStartPos;
	olc::vi2d tilePixelSize = olc::vi2d{ ( int )( tileCellSize * ( this->_zoomX * tileSize ) ), ( int )( tileCellSize * ( this->_zoomY * tileSize ) ) };

	Tile* tiles = worldChunk.getTiles();
	for ( int x = 0; x < chunkSize; x++ )
	{
		for ( int y = 0; y < chunkSize; y++ )
		{
			Tile tile = tiles[y * chunkSize + x];
			if ( !tile.isVoid() )
			{
				std::uint64_t id = tile.getId();
				worldToScreen( worldPositionX + x, worldPositionY + y, tilePixelX, tilePixelY );
				tileStartPos = olc::v2d_generic<std::int64_t>{ tilePixelX, tilePixelY };

				pge->FillRect(
					tileStartPos,
					tilePixelSize,
					id == 1 ? olc::DARK_CYAN : ( id == 2 ? olc::DARK_GREY : olc::DARK_GREEN )
				);

			}
		}
	}

	// Draw Quadrant
	std::int64_t chunkPixelX;
	std::int64_t chunkPixelY;
	worldToScreen( worldPositionX, worldPositionY, chunkPixelX, chunkPixelY );
	olc::v2d_generic<std::int64_t> chunkStartPos = olc::v2d_generic<std::int64_t>{ chunkPixelX, chunkPixelY };
	olc::vi2d chunkPixelSize = olc::vi2d{ ( int )( chunkSize * ( this->_zoomX * tileSize ) ), ( int )( chunkSize * ( this->_zoomY * tileSize ) ) };

	pge->DrawRect(
		chunkStartPos,
		chunkPixelSize,
		olc::GREEN
	);

	return;
}


void Camera::pan( long double x, long double y )
{
	this->_focalPoint.setX( this->_focalPoint.getX() + x );
	this->_focalPoint.setY( this->_focalPoint.getY() + y );

	this->_view.setCenterX( this->_focalPoint.getCenterX() );
	this->_view.setCenterY( this->_focalPoint.getCenterY() );
	return;
}


void Camera::panX( long double x )
{
	this->_focalPoint.setX( this->_focalPoint.getX() + x );
	this->_view.setCenterX( this->_focalPoint.getCenterX() );
	return;
}


void Camera::panY( long double y )
{
	this->_focalPoint.setY( this->_focalPoint.getY() + y );
	this->_view.setCenterY( this->_focalPoint.getCenterY() );
	return;
}


void Camera::zoom( long double s )
{
	// [!] sumplify this
	std::int64_t centerCameraPixelXBeforeZoom;
	std::int64_t centerCameraPixelYBeforeZoom;
	this->worldToScreen( this->_focalPoint.getCenterX(), this->_focalPoint.getCenterY(), centerCameraPixelXBeforeZoom, centerCameraPixelYBeforeZoom );

	long double cameraCenterXBeforeZoom;
	long double cameraCenterYBeforeZoom;
	this->screenToWorld( centerCameraPixelXBeforeZoom, centerCameraPixelYBeforeZoom, cameraCenterXBeforeZoom, cameraCenterYBeforeZoom );

	this->_zoomX *= s;
	this->_zoomY *= s;

	std::int64_t centerCameraPixelXAfterZoom;
	std::int64_t centerCameraPixelYAfterZoom;
	this->worldToScreen( this->_focalPoint.getCenterX(), this->_focalPoint.getCenterY(), centerCameraPixelXAfterZoom, centerCameraPixelYAfterZoom );

	long double cameraCenterXAfterZoom;
	long double cameraCenterYAfterZoom;
	this->screenToWorld( centerCameraPixelXBeforeZoom, centerCameraPixelYBeforeZoom, cameraCenterXAfterZoom, cameraCenterYAfterZoom );

	this->_focalPoint.width = this->_focalPoint.width * ( 1.0f / s );
	this->_focalPoint.height = this->_focalPoint.height * ( 1.0f / s );
	this->_focalPoint.setX( this->_focalPoint.getX() + ( cameraCenterXBeforeZoom - cameraCenterXAfterZoom ) );
	this->_focalPoint.setY( this->_focalPoint.getY() + ( cameraCenterYBeforeZoom - cameraCenterYAfterZoom ) );


	this->_view.width = this->_view.width * ( 1.0f / s );
	this->_view.height = this->_view.height * ( 1.0f / s );
	this->_view.setCenterX( this->_focalPoint.getCenterX() );
	this->_view.setCenterY( this->_focalPoint.getCenterY() );
	return;
}


void Camera::zoom( long double x, long double y )
{
	// ...
	this->_zoomX *= x;
	this->_zoomY *= y;
	this->_focalPoint.width = this->_focalPoint.width * ( 1.0f / x );
	this->_focalPoint.height = this->_focalPoint.height * ( 1.0f / y );

	this->_view.width = this->_view.width * ( 1.0f / x );
	this->_view.height = this->_view.height * ( 1.0f / y );
	return;
}


void Camera::zoomX( long double x )
{
	// ...
	this->_zoomX *= x;
	this->_focalPoint.width = this->_focalPoint.width * ( 1.0f / x );
	this->_view.width = this->_view.width * ( 1.0f / x );
	return;
}


void Camera::zoomY( long double y )
{
	// ...
	this->_zoomY *= y;
	this->_focalPoint.height = this->_focalPoint.height * ( 1.0f / y );
	this->_view.height = this->_view.height * ( 1.0f / y );
	return;
}


void Camera::setPosition( long double x, long double y )
{
	this->_focalPoint.setX( x );
	this->_focalPoint.setY( y );

	this->_view.setCenterX( this->_focalPoint.getCenterX() );
	this->_view.setCenterY( this->_focalPoint.getCenterY() );
	return;
}


void Camera::setZoom( long double s )
{
	/*
	this->_zoomX = s;
	this->_zoomY = s;

	this->_focalPoint.width = ( 1.0f / s );
	this->_focalPoint.height = ( 1.0f / s );

	this->_view.width = ( 1.0f / s );
	this->_view.height = ( 1.0f / s );
	*/

	std::int64_t centerCameraPixelXBeforeZoom;
	std::int64_t centerCameraPixelYBeforeZoom;
	this->worldToScreen( this->_focalPoint.getCenterX(), this->_focalPoint.getCenterY(), centerCameraPixelXBeforeZoom, centerCameraPixelYBeforeZoom );

	long double cameraCenterXBeforeZoom;
	long double cameraCenterYBeforeZoom;
	this->screenToWorld( centerCameraPixelXBeforeZoom, centerCameraPixelYBeforeZoom, cameraCenterXBeforeZoom, cameraCenterYBeforeZoom );

	this->_zoomX = s;
	this->_zoomY = s;

	std::int64_t centerCameraPixelXAfterZoom;
	std::int64_t centerCameraPixelYAfterZoom;
	this->worldToScreen( this->_focalPoint.getCenterX(), this->_focalPoint.getCenterY(), centerCameraPixelXAfterZoom, centerCameraPixelYAfterZoom );

	long double cameraCenterXAfterZoom;
	long double cameraCenterYAfterZoom;
	this->screenToWorld( centerCameraPixelXBeforeZoom, centerCameraPixelYBeforeZoom, cameraCenterXAfterZoom, cameraCenterYAfterZoom );

	this->_focalPoint.width = Settings::Camera::FOCAL_POINT_CELL_WIDTH * ( 1.0f / s ); // set
	this->_focalPoint.height = Settings::Camera::FOCAL_POINT_CELL_HEIGHT * ( 1.0f / s ); // set
	this->_focalPoint.setX( this->_focalPoint.getX() + ( cameraCenterXBeforeZoom - cameraCenterXAfterZoom ) );
	this->_focalPoint.setY( this->_focalPoint.getY() + ( cameraCenterYBeforeZoom - cameraCenterYAfterZoom ) );


	this->_view.width = Settings::Camera::VIEW_CELL_WIDTH * ( 1.0f / s ); // set
	this->_view.height = Settings::Camera::VIEW_CELL_HEIGHT * ( 1.0f / s ); // set
	this->_view.setCenterX( this->_focalPoint.getCenterX() );
	this->_view.setCenterY( this->_focalPoint.getCenterY() );

	//BoundingBox<long double>( 0.0f, 0.0f, Settings::Camera::FOCAL_POINT_CELL_WIDTH, Settings::Camera::FOCAL_POINT_CELL_HEIGHT ),
	//BoundingBox<long double>( 0.0f, 0.0f, Settings::Camera::VIEW_CELL_WIDTH, Settings::Camera::VIEW_CELL_HEIGHT ),
	return;
}


void Camera::setZoom( long double x, long double y )
{
	this->_zoomX = x;
	this->_zoomY = y;

	this->_focalPoint.width = ( 1.0f / x );
	this->_focalPoint.height = ( 1.0f / y );

	this->_view.width = ( 1.0f / x );
	this->_view.height = ( 1.0f / y );
	return;
}


void Camera::setFocalPoint( long double width, long double height )
{
	this->_focalPoint.setWidth( width );
	this->_focalPoint.setHeight( height );
	return;
}


BoundingBox<long double> Camera::getFocalPoint() const
{
	return this->_focalPoint;
}


long double Camera::getCenterX() const
{
	return this->_focalPoint.getX() + this->_focalPoint.getWidth() / 2.0f;
}


long double Camera::getCenterY() const
{
	return this->_focalPoint.getY() + this->_focalPoint.getHeight() / 2.0f;
}


long double Camera::getZoomX() const
{
	return this->_zoomX;
}


long double Camera::getZoomY() const
{
	return this->_zoomY;
}