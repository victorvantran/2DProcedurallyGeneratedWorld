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
	pixelX = ( std::int64_t )std::ceil( ( ( cellX - ( this->_focalPoint.x ) ) * ( this->_zoomX * tileSize ) ) + this->_absolutePixelOffsetX );
	pixelY = ( std::int64_t )std::ceil( ( ( cellY - ( this->_focalPoint.y ) ) * ( this->_zoomY * tileSize ) ) + this->_absolutePixelOffsetY );
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
	
	pge->DrawStringDecal(
		startPos,
		std::to_string( worldChunk.getRelativeChunkIndex() ),
		olc::GREEN
	);

	this->renderTileRenders( worldChunk.getTileRendersRoot(), atlas, worldChunk.getLightRenders() );
	this->renderLightRenders( worldChunk.getLightRenders()[0] );

	return;
}


void Camera::renderTileRenders( QuadTree<TileRender>& tileRenders, Atlas& atlas, QuadTree<LightRender>* lightRenderEncapsulates ) const
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


	// If this bounding box happens to be encapsulated in a solid, black shadow, just render the black. No need to render the tile
	QuadTree<LightRender> currLightEncapsulate = lightRenderEncapsulates[tileRenders.getIndex()];
	LightRender& referenceLight = currLightEncapsulate.getCells()[0];
	bool isShadowEncapsulated = currLightEncapsulate.isConsolidated() &&
		referenceLight.corner0 == 0x000000ff;

	// Fill Consolidated
	if ( currQuadTree.isConsolidated() )
	{
		if ( this->_view.intersects( currQuadTree.getBounds() ) && currQuadTree.getCells()[0].exists() && !isShadowEncapsulated )
		{
			TileIdentity id = currQuadTree.getCells()[0].getId();
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
				if ( cells[i].exists() && this->_view.intersects( cells[i].getBounds() ) && !isShadowEncapsulated )
				{
					TileIdentity id = cells[i].getId();
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
					this->renderTileRenders( tileRenders.getReferenceNodes()[childrenNodeIndicies[i]], atlas, lightRenderEncapsulates );
				}
			}
		}
	}

	return;
}



void Camera::renderLightRenders( QuadTree<LightRender>& lightRenders ) const
{
	// Render every lightRender but only drawing one properlly scaled tile for each consolidated render
	std::uint16_t tileSize = Settings::Screen::CELL_PIXEL_SIZE;
	std::uint16_t chunkSize = Settings::World::CHUNK_CELL_SIZE;
	QuadTree<LightRender> currQuadTree = lightRenders.getReferenceNodes()[lightRenders.getIndex()];
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
			int level = currQuadTree.getLevel();
			int scale = 2 << ( level );

			std::int64_t worldPositionX = currQuadTree.getBounds().getX();
			std::int64_t worldPositionY = currQuadTree.getBounds().getY();
			std::int64_t pixelX;
			std::int64_t pixelY;
			worldToScreen( worldPositionX, worldPositionY, pixelX, pixelY );
			olc::v2d_generic<std::int64_t> startPos = olc::v2d_generic<std::int64_t>{ pixelX, pixelY };

			std::uint32_t corner0 = currQuadTree.getCells()[0].corner0;
			std::uint8_t r = ( std::uint8_t )( ( corner0 & 0xff000000 ) >> 24 );
			std::uint8_t g = ( std::uint8_t )( ( corner0 & 0x00ff0000 ) >> 16 );
			std::uint8_t b = ( std::uint8_t )( ( corner0 & 0x0000ff00 ) >> 8 );
			std::uint8_t alpha = ( std::uint8_t )( ( corner0 & 0x000000ff ) );
			olc::Pixel color = olc::Pixel{ r, g, b, alpha };
			
			pge->SetDecalMode( olc::DecalMode::MULTIPLICATIVE );
			//pge->SetDecalMode( olc::DecalMode::ADDITIVE );
			pge->FillRectDecal(
				startPos,
				olc::vf2d{ (float)this->_zoomX * ( float )tileSize, ( float )this->_zoomY * ( float )tileSize } *( float )scale,
				color
			);

			pge->SetDecalMode( olc::DecalMode::NORMAL );
		}

		return;
	}
	// Fill the small bounding boxes (cells) that are not possibly consolidated
	else
	{
		if ( currQuadTree.getLevel() == currQuadTree.getMinLevel() )
		{
			LightRender* cells = currQuadTree.getCells();
			for ( int i = 0; i < 4; i++ )
			{
				if ( cells[i].exists() && this->_view.intersects( cells[i].getBounds() ) )
				{
					
					std::uint32_t corner0 = cells[i].corner0;
					std::uint32_t corner1 = cells[i].corner1;
					std::uint32_t corner2 = cells[i].corner2;
					std::uint32_t corner3 = cells[i].corner3;

					
					olc::Pixel colors[4];
					colors[0] = olc::Pixel{ ( std::uint8_t )( ( corner0 & 0xff000000 ) >> 24 ), ( std::uint8_t )( ( corner0 & 0x00ff0000 ) >> 16 ), ( std::uint8_t )( ( corner0 & 0x0000ff00 ) >> 8 ), ( std::uint8_t )( ( corner0 & 0x000000ff ) ) };
					colors[1] = olc::Pixel{ ( std::uint8_t )( ( corner1 & 0xff000000 ) >> 24 ), ( std::uint8_t )( ( corner1 & 0x00ff0000 ) >> 16 ), ( std::uint8_t )( ( corner1 & 0x0000ff00 ) >> 8 ), ( std::uint8_t )( ( corner1 & 0x000000ff ) ) };
					colors[2] = olc::Pixel{ ( std::uint8_t )( ( corner2 & 0xff000000 ) >> 24 ), ( std::uint8_t )( ( corner2 & 0x00ff0000 ) >> 16 ), ( std::uint8_t )( ( corner2 & 0x0000ff00 ) >> 8 ), ( std::uint8_t )( ( corner2 & 0x000000ff ) ) };
					colors[3] = olc::Pixel{ ( std::uint8_t )( ( corner3 & 0xff000000 ) >> 24 ), ( std::uint8_t )( ( corner3 & 0x00ff0000 ) >> 16 ), ( std::uint8_t )( ( corner3 & 0x0000ff00 ) >> 8 ), ( std::uint8_t )( ( corner3 & 0x000000ff ) ) };
					

					std::int64_t worldPositionX = cells[i].getBounds().getX();
					std::int64_t worldPositionY = cells[i].getBounds().getY();
					std::int64_t topLeftPixelX;
					std::int64_t topLeftPixelY;
					std::int64_t bottomRightPixelX;
					std::int64_t bottomRightPixelY;
					worldToScreen( worldPositionX, worldPositionY, topLeftPixelX, topLeftPixelY );
					worldToScreen( worldPositionX + 1, worldPositionY + 1, bottomRightPixelX, bottomRightPixelY );

					olc::vf2d verticiesB[4];
					verticiesB[0] = olc::vf2d{ ( float )topLeftPixelX, ( float )topLeftPixelY };
					verticiesB[1] = olc::vf2d{ ( float )topLeftPixelX, ( float )bottomRightPixelY };
					verticiesB[2] = olc::vf2d{ ( float )bottomRightPixelX, ( float )bottomRightPixelY };
					verticiesB[3] = olc::vf2d{ ( float )bottomRightPixelX , ( float )topLeftPixelY };
					

					/*
					verticiesB[0] = olc::vf2d{ ( float )topLeftPixelX, ( float )topLeftPixelY };
					verticiesB[1] = olc::vf2d{ ( float )topLeftPixelX, ( float )bottomRightPixelY };
					verticiesB[2] = olc::vf2d{ ( float )topLeftPixelX + ( float )17, ( float )topLeftPixelY + ( float )17 };
					verticiesB[3] = olc::vf2d{ ( float )topLeftPixelX + ( float )17 , ( float )topLeftPixelY };
					*/

					olc::vf2d textureCoordinates[4];
					textureCoordinates[0] = olc::vf2d{ 0.0f, 0.0f };
					textureCoordinates[1] = olc::vf2d{ 0.0f, 1.0f };
					textureCoordinates[2] = olc::vf2d{ 1.0f, 1.0f };
					textureCoordinates[3] = olc::vf2d{ 1.0f, 0.0f };
						
					olc::Decal* lightDecal = this->_decalLight;
					pge->SetDecalMode( olc::DecalMode::MULTIPLICATIVE );
					//pge->SetDecalMode( olc::DecalMode::ADDITIVE );
					pge->DrawExplicitDecal(
						lightDecal,
						verticiesB,
						textureCoordinates,
						colors
					);
					pge->SetDecalMode( olc::DecalMode::NORMAL );					
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
					this->renderLightRenders( lightRenders.getReferenceNodes()[childrenNodeIndicies[i]] );
				}
			}
		}
	}

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

	const Tile* tiles = worldChunk.getTiles();
	for ( int x = 0; x < chunkSize; x++ )
	{
		for ( int y = 0; y < chunkSize; y++ )
		{
			Tile tile = tiles[y * chunkSize + x];
			if ( !tile.isVoid() )
			{
				TileIdentity id = tile.getId();
				worldToScreen( worldPositionX + x, worldPositionY + y, tilePixelX, tilePixelY );
				tileStartPos = olc::v2d_generic<std::int64_t>{ tilePixelX, tilePixelY };

				pge->FillRect(
					tileStartPos,
					tilePixelSize,
					( std::uint64_t )id == 1 ? olc::DARK_CYAN : ( ( std::uint64_t )id == 2 ? olc::DARK_GREY : olc::DARK_GREEN )
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


const BoundingBox<long double>& Camera::getFocalPoint() const
{
	return this->_focalPoint;
}


const BoundingBox<long double>& Camera::getView() const
{
	return this->_view;
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