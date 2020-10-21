#pragma once

#include <cstdint>
#include "Settings.h"
#include "BoundingBox.h"
#include "World.h"
#include "SpriteTilesMap.h"
#include "WorldChunk.h"
#include "QuadTree.h"
#include "TileRender.h"
#include "Tile.h"



class Camera : public olc::PGEX
{
private:
	BoundingBox<float> _focalPoint; // cell-domain
	BoundingBox<float> _view;


	int _absolutePixelOffsetX = Settings::Camera::ABSOLUTE_PIXEL_OFFSET_X;  //16 * ( 120 / 2 ) - 16 * ( 32 / 2 );  // tileSize * ( screenCellWidth / 2 ) - tileSize * ( cameraCellWidth / 2 ) [!] need to change based on pixelSize
	int _absolutePixelOffsetY = Settings::Camera::ABSOLUTE_PIXEL_OFFSET_Y;

	float _zoomX;
	float _zoomY;

public:
	Camera();
	~Camera();
	Camera( BoundingBox<float> focalPoint, BoundingBox<float> biew, float zoomX, float zoomY );

	void screenToWorld( int pixelX, int pixelY, float& cellX, float& cellY ) const; // int to float ( camera offest determines displacement )
	void worldToScreen( float cellX, float cellY, int& pixelX, int& pixelY ) const; // float to int ( camera offset determines displacement )

	void renderWorld( World* world ) const;
	void renderWorldChunk( WorldChunk& worldChunk, const SpriteTilesMap& spriteTilesMap ) const;
	void renderTileRenders( QuadTree<Tile, TileRender>& quadTree, const SpriteTilesMap& spriteTilesMap ) const;
	void renderCamera() const;

	void renderTilesDebug( WorldChunk& worldChunk ) const;

	void pan( float x, float y );
	void panX( float x );
	void panY( float y );
	void zoom( float s );
	void zoom( float x, float y );
	void zoomX( float x );
	void zoomY( float y );
	void setPosition( float x, float y );
	void setZoom( float s );
	void setZoom( float x, float y );
	void setFocalPoint( float width, float height );

	BoundingBox<float> getFocalPoint() const;

	float getCenterX() const;
	float getCenterY() const;

	float getZoomX() const;
	float getZoomY() const;
};


Camera::Camera( )
	: _focalPoint( BoundingBox<float>() ), _view( BoundingBox<float>() ), _zoomX( 1.0f ), _zoomY( 1.0f )
{

}


Camera::Camera( BoundingBox<float> focalPoint, BoundingBox<float> view, float zoomX, float zoomY )
	: _focalPoint( focalPoint ), _view( view ), _zoomX( zoomX ), _zoomY( zoomY )
{
	this->_view.setCenterX( this->_focalPoint.getCenterX() );
	this->_view.setCenterY( this->_focalPoint.getCenterY() );

}


Camera::~Camera()
{

}


void Camera::screenToWorld( int pixelX, int pixelY, float& cellX, float& cellY ) const
{
	// int to float ( camera offest determines displacement )
	int tileSize = Settings::Screen::CELL_PIXEL_SIZE;; // [!] global singleton
	cellX = ( float )( ( ( float )( pixelX - this->_absolutePixelOffsetX ) / ( float )( this->_zoomX * tileSize ) ) + ( this->_focalPoint.x ) );
	cellY = ( float )( ( ( float )( pixelY - this->_absolutePixelOffsetY ) / ( float )( this->_zoomY * tileSize ) ) + ( this->_focalPoint.y ) );
	return;
}


void Camera::worldToScreen( float cellX, float cellY, int& pixelX, int& pixelY ) const
{
	// float to int ( camera offset determines displacement )
	int tileSize = Settings::Screen::CELL_PIXEL_SIZE;; // [!] global singleton
	pixelX = ( int )( ( cellX - ( this->_focalPoint.x ) ) * ( this->_zoomX * tileSize ) ) + this->_absolutePixelOffsetX;
	pixelY = ( int )( ( cellY - ( this->_focalPoint.y ) ) * ( this->_zoomY * tileSize ) ) + this->_absolutePixelOffsetY;
	return;
}


void Camera::renderWorld( World* world) const
{
	WorldChunk* worldChunks = world->getWorldChunks();
	int numWorldChunks = world->getNumWorldChunks();

	for ( int i = 0; i < numWorldChunks; i++ )
	{
		this->renderWorldChunk( worldChunks[i], world->getSpriteTilesMap() );
		//this->renderTilesDebug( worldChunks[i] );
	}

	return;
}


void Camera::renderWorldChunk( WorldChunk& worldChunk, const SpriteTilesMap& spriteTilesMap ) const
{
	int tileSize = Settings::Screen::CELL_PIXEL_SIZE;; // [!] make it from a "global" variable

	int chunkSize = worldChunk.getSize();
	float worldPositionX = worldChunk.getChunkIndexX() * chunkSize;
	float worldPositionY = worldChunk.getChunkIndexY() * chunkSize;

	int pixelX;
	int pixelY;

	worldToScreen( worldPositionX, worldPositionY, pixelX, pixelY );

	olc::vi2d startPos = olc::vi2d{ pixelX, pixelY };
	olc::vi2d size = olc::vi2d{ ( int )( chunkSize * ( this->_zoomX * tileSize ) ), ( int )( chunkSize * ( this->_zoomY * tileSize ) ) };

	pge->DrawRect(
		startPos,
		size,
		olc::GREEN
	);

	this->renderTileRenders( worldChunk.getTileRendersRoot(), spriteTilesMap );
	return;
}


void Camera::renderTileRenders( QuadTree<Tile, TileRender>& tileRenders, const SpriteTilesMap& spriteTilesMap ) const
{
	int tileSize = Settings::Screen::CELL_PIXEL_SIZE;
	int chunkSize = Settings::World::CHUNK_CELL_SIZE;

	QuadTree<Tile, TileRender> currQuadTree = tileRenders.getReferenceNodes()[tileRenders.getIndex()];
	const BoundingBox<int> bounds = currQuadTree.getBounds();

	// No need to render if the camera can not see it
	if ( !this->_view.intersects( bounds ) )
	{
		return;
	}

	// Fill Consolidated
	if ( currQuadTree.isConsolidated() )
	{
		if ( this->_view.intersects( currQuadTree.getBounds() ) && currQuadTree.getCells()[0].getId() != 0 )
		{
			std::uint64_t id = currQuadTree.getCells()[0].getId();
			if ( spriteTilesMap.getDecal( id ) == nullptr )
			{
				return;
			}

			int level = currQuadTree.getLevel();
			int scale = 2 << ( level );

			float worldPositionX = currQuadTree.getBounds().getX();
			float worldPositionY = currQuadTree.getBounds().getY();
			int pixelX;
			int pixelY;
			worldToScreen( worldPositionX, worldPositionY, pixelX, pixelY );
			olc::vi2d startPos = olc::vi2d{ pixelX, pixelY };

			pge->DrawPartialDecal(
				startPos,
				olc::vf2d{ this->_zoomX * tileSize, this->_zoomY * tileSize } *scale,
				spriteTilesMap.getDecal( id ),
				olc::vf2d{ 0, Settings::Camera::CONSOLIDATED_TILE_OFFSET },
				olc::vf2d{ 1, 1 } *( Settings::Screen::CELL_PIXEL_SIZE * Settings::Screen::PIXEL_SIZE * scale )
			);
		}

		return;
	}
	// Fill the small bounding boxes (cells) that are not possibly consolidated
	else
	{
		if ( currQuadTree.getLevel() == QuadTree<Tile, TileRender>::_MIN_LEVEL )
		{
			TileRender* cells = currQuadTree.getCells();
			for ( int i = 0; i < 4; i++ )
			{
				if ( cells[i].getExist() && cells[i].getId() != 0 && this->_view.intersects( cells[i].getBounds() ) )
				{
					std::uint64_t id = cells[i].getId();
					if ( spriteTilesMap.getDecal( id ) == nullptr )
					{
						continue;
					}

					float worldPositionX = cells[i].getBounds().getX();
					float worldPositionY = cells[i].getBounds().getY();
					int pixelX;
					int pixelY;
					worldToScreen( worldPositionX, worldPositionY, pixelX, pixelY );
					olc::vi2d startPos = olc::vi2d{ pixelX, pixelY };

					pge->DrawPartialDecal(
						startPos,
						olc::vf2d{ ( tileSize * this->_zoomX ), ( tileSize * this->_zoomY ) },
						spriteTilesMap.getDecal( id ),
						olc::vf2d{ 0, 128 }, // [!] temp based on configuration
						olc::vf2d{ 1, 1 } * ( Settings::Screen::CELL_PIXEL_SIZE * Settings::Screen::PIXEL_SIZE )
					);
				
				}
			}

			return;
		}
		

		// Fill other psosible consolidated (or not-consolidated) boundingboxes
		// Get node indicies
		int* childrenNodeIndicies = currQuadTree.getChildrenNodeIndicies();
		if ( childrenNodeIndicies != nullptr )
		{
			for ( int i = 0; i < 4; i++ )
			{
				if ( childrenNodeIndicies[i] != -1 )
				{
					this->renderTileRenders( tileRenders.getReferenceNodes()[childrenNodeIndicies[i]], spriteTilesMap );
				}
			}
		}
	}

	return;
}


void Camera::renderCamera() const
{
	//int tileSize = 16; // [!]
	int tileSize = Settings::Screen::CELL_PIXEL_SIZE;

	// Draw Focal Point
	pge->DrawRect
	(
		olc::vi2d{ this->_absolutePixelOffsetX, this->_absolutePixelOffsetY }, 
		olc::vi2d{ ( int )( this->_focalPoint.width * this->_zoomX * tileSize ), ( int )( this->_focalPoint.height * this->_zoomY * tileSize  ) },
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
	// Draw Tiles
	int tileSize = Settings::Screen::CELL_PIXEL_SIZE;;
	int tileCellSize = 1; // [!] make it from a "global" variable

	int chunkSize = worldChunk.getSize();
	float worldPositionX = worldChunk.getChunkIndexX() * chunkSize;
	float worldPositionY = worldChunk.getChunkIndexY() * chunkSize;
	int tilePixelX;
	int tilePixelY;
	olc::vi2d tileStartPos;
	olc::vi2d tilePixelSize = olc::vi2d{ ( int )( tileCellSize * ( this->_zoomX * tileSize ) ), ( int )( tileCellSize * ( this->_zoomY * tileSize ) ) };

	Tile* tiles = worldChunk.getTiles();
	for ( int x = 0; x < chunkSize; x++ )
	{
		for ( int y = 0; y < chunkSize; y++ )
		{
			Tile tile = tiles[y * chunkSize + x];
			if ( !tile.isVoid() )
			{
				int id = tile.getId();
				worldToScreen( worldPositionX + x, worldPositionY + y , tilePixelX, tilePixelY );
				tileStartPos = olc::vi2d{ tilePixelX, tilePixelY };

				pge->FillRect(
					tileStartPos,
					tilePixelSize,
					id == 1 ? olc::DARK_CYAN : ( id == 2 ? olc::DARK_GREY : olc::DARK_GREEN )
				);

			}
		}
	}

	// Draw Quadrant
	int chunkPixelX;
	int chunkPixelY;
	worldToScreen( worldPositionX, worldPositionY, chunkPixelX, chunkPixelY );
	olc::vi2d chunkStartPos = olc::vi2d{ chunkPixelX, chunkPixelY };
	olc::vi2d chunkPixelSize = olc::vi2d{ ( int )( chunkSize * ( this->_zoomX * tileSize ) ), ( int )( chunkSize * ( this->_zoomY * tileSize ) ) };

	pge->DrawRect(
		chunkStartPos,
		chunkPixelSize,
		olc::GREEN
	);

	return;
}


void Camera::pan( float x, float y )
{
	this->_focalPoint.setX( this->_focalPoint.getX() + x );
	this->_focalPoint.setY( this->_focalPoint.getY() + y );

	this->_view.setCenterX( this->_focalPoint.getCenterX() );
	this->_view.setCenterY( this->_focalPoint.getCenterY() );
	return;
}


void Camera::panX( float x )
{
	this->_focalPoint.setX( this->_focalPoint.getX() + x );
	this->_view.setCenterX( this->_focalPoint.getCenterX() );
	return;
}


void Camera::panY( float y )
{
	this->_focalPoint.setY( this->_focalPoint.getY() + y );
	this->_view.setCenterY( this->_focalPoint.getCenterY() );
	return;
}


void Camera::zoom( float s )
{
	// [!] sumplify this
	int centerCameraPixelXBeforeZoom;
	int centerCameraPixelYBeforeZoom;
	this->worldToScreen( this->_focalPoint.getCenterX(), this->_focalPoint.getCenterY(), centerCameraPixelXBeforeZoom, centerCameraPixelYBeforeZoom );

	float cameraCenterXBeforeZoom;
	float cameraCenterYBeforeZoom;
	this->screenToWorld( centerCameraPixelXBeforeZoom, centerCameraPixelYBeforeZoom, cameraCenterXBeforeZoom, cameraCenterYBeforeZoom );

	this->_zoomX *= s;
	this->_zoomY *= s;

	int centerCameraPixelXAfterZoom;
	int centerCameraPixelYAfterZoom;
	this->worldToScreen( this->_focalPoint.getCenterX(), this->_focalPoint.getCenterY(), centerCameraPixelXAfterZoom, centerCameraPixelYAfterZoom );

	float cameraCenterXAfterZoom;
	float cameraCenterYAfterZoom;
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


void Camera::zoom( float x, float y )
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


void Camera::zoomX( float x )
{
	// ...
	this->_zoomX *= x;
	this->_focalPoint.width = this->_focalPoint.width * ( 1.0f / x );
	this->_view.width = this->_view.width * ( 1.0f / x );
	return;
}


void Camera::zoomY( float y )
{
	// ...
	this->_zoomY *= y;
	this->_focalPoint.height = this->_focalPoint.height * ( 1.0f / y );
	this->_view.height = this->_view.height * ( 1.0f / y );
	return;
}


void Camera::setPosition( float x, float y )
{
	this->_focalPoint.setX( x );
	this->_focalPoint.setY( y );

	this->_view.setCenterX( this->_focalPoint.getCenterX() );
	this->_view.setCenterY( this->_focalPoint.getCenterY() );
	return;
}


void Camera::setZoom( float s )
{
	this->_zoomX = s;
	this->_zoomY = s;

	this->_focalPoint.width = ( 1.0f / s );
	this->_focalPoint.height = ( 1.0f / s );

	this->_view.width = ( 1.0f / s );
	this->_view.height = ( 1.0f / s );
	return;
}


void Camera::setZoom( float x, float y )
{
	this->_zoomX = x;
	this->_zoomY = y;

	this->_focalPoint.width = ( 1.0f / x );
	this->_focalPoint.height = ( 1.0f / y );

	this->_view.width = ( 1.0f / x );
	this->_view.height = ( 1.0f / y );
	return;
}


void Camera::setFocalPoint( float width, float height )
{
	this->_focalPoint.setWidth( width );
	this->_focalPoint.setHeight( height );
	return;
}


BoundingBox<float> Camera::getFocalPoint() const
{
	return this->_focalPoint;
}


float Camera::getCenterX() const
{
	return this->_focalPoint.getX() + this->_focalPoint.getWidth() / 2.0f;
}


float Camera::getCenterY() const
{
	return this->_focalPoint.getY() + this->_focalPoint.getHeight() / 2.0f;
}


float Camera::getZoomX() const
{
	return this->_zoomX;
}


float Camera::getZoomY() const
{
	return this->_zoomY;
}