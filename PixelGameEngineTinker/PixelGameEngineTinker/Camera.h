#pragma once

#include "Settings.h"
#include "BoundingBox.h"
#include "World.h"
#include "WorldChunk.h"
#include "QuadTree.h"
#include "TileRender.h"
#include "Tile.h"


class Camera : public olc::PGEX
{
private:
	BoundingBox<float> _view; // cell-domain

	int _absolutePixelOffsetX = Settings::Camera::ABSOLUTE_PIXEL_OFFSET_X;  //16 * ( 120 / 2 ) - 16 * ( 32 / 2 );  // tileSize * ( screenCellWidth / 2 ) - tileSize * ( cameraCellWidth / 2 ) [!] need to change based on pixelSize
	int _absolutePixelOffsetY = Settings::Camera::ABSOLUTE_PIXEL_OFFSET_Y;

	float _zoomX;
	float _zoomY;

public:
	Camera();
	~Camera();
	Camera( BoundingBox<float> view, float zoomX, float zoomY );

	void screenToWorld( int pixelX, int pixelY, float& cellX, float& cellY ) const; // int to float ( camera offest determines displacement )
	void worldToScreen( float cellX, float cellY, int& pixelX, int& pixelY ) const; // float to int ( camera offset determines displacement )

	void renderWorld( World& world ) const;
	void renderWorldChunk( WorldChunk& worldChunk ) const;
	void renderTileRenders( QuadTree<Tile, TileRender>& quadTree ) const;
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
	void setView( float width, float height );

	BoundingBox<float> getView() const;
	float getX() const;
	float getY() const;
	float getCenterX() const;
	float getCenterY() const;
	float getWidth() const;
	float getHeight() const;
	float getZoomX() const;
	float getZoomY() const;
};



Camera::Camera( )
	: _view( BoundingBox<float>() ), _zoomX( 1.0f ), _zoomY( 1.0f )
{

}


Camera::Camera( BoundingBox<float> view, float zoomX, float zoomY )
	: _view( view ), _zoomX( zoomX ), _zoomY( zoomY )
{

}


Camera::~Camera()
{

}


void Camera::screenToWorld( int pixelX, int pixelY, float& cellX, float& cellY ) const
{
	// int to float ( camera offest determines displacement )
	int tileSize = 16; // [!] global singleton
	cellX = ( float )( ( ( float )( pixelX - this->_absolutePixelOffsetX ) / ( float )( this->_zoomX * tileSize ) ) + ( this->_view.x ) );
	cellY = ( float )( ( ( float )( pixelY - this->_absolutePixelOffsetY ) / ( float )( this->_zoomY * tileSize ) ) + ( this->_view.y ) );
	return;
}


void Camera::worldToScreen( float cellX, float cellY, int& pixelX, int& pixelY ) const
{
	// float to int ( camera offset determines displacement )
	int tileSize = 16; // [!] global singleton
	pixelX = ( int )( ( cellX - ( this->_view.x ) ) * ( this->_zoomX * tileSize ) ) + this->_absolutePixelOffsetX;
	pixelY = ( int )( ( cellY - ( this->_view.y ) ) * ( this->_zoomY * tileSize ) ) + this->_absolutePixelOffsetY;
	return;
}


void Camera::renderWorld( World& world ) const
{
	WorldChunk* worldChunks = world.getWorldChunks();
	int numWorldChunks = world.getNumWorldChunks();

	for ( int i = 0; i < numWorldChunks; i++ )
	{
		this->renderWorldChunk( worldChunks[i] );
		//this->renderTilesDebug( worldChunks[i] );
	}

	return;
}


void Camera::renderWorldChunk( WorldChunk& worldChunk ) const
{
	int tileSize = 16; // [!] make it from a "global" variable

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

	this->renderTileRenders( worldChunk.getTileRendersRoot() );
	return;
}


void Camera::renderTileRenders( QuadTree<Tile, TileRender>& tileRenders ) const
{
	int tileSize = 16; // [!] make it global variable in singleton
	int chunkSize = 32; // [!] singleton 

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
			int id = currQuadTree.getCells()[0].getId();
			int level = currQuadTree.getLevel();
			int scale = 2 << ( level );

			int decalPositionX = 0;
			for ( int i = 0; i < level; i++ )
			{
				decalPositionX += ( 2 << i );
			}
			int decalPositionY = id * 32;

			float worldPositionX = currQuadTree.getBounds().getX();
			float worldPositionY = currQuadTree.getBounds().getY();
			int pixelX;
			int pixelY;
			worldToScreen( worldPositionX, worldPositionY, pixelX, pixelY );
			olc::vi2d startPos = olc::vi2d{ pixelX, pixelY };
			
			pge->FillRect(
				startPos,
				olc::vf2d{ 1.0f * scale * this->_zoomX * tileSize  , 1.0f * scale * this->_zoomY * tileSize },
				id == 1 ? olc::DARK_CYAN : ( id == 2 ? olc::DARK_GREY : olc::DARK_GREEN )
			);
		}

		return;
	}
	// Fill the small bounding boxes (cells) that are not possibly consolidated
	else
	{
		TileRender* cells = currQuadTree.getCells();
		for ( int i = 0; i < 4; i++ )
		{
			if ( cells[i].getExist() && cells[i].getId() != 0 &&  this->_view.intersects( cells[i].getBounds() ) )
			{
				int id = cells[i].getId();
				olc::vi2d decalSourcePos = olc::vi2d{ cells[i].getId() == 0 ? 7 : 15, 7 }; // dirt, stone

				float worldPositionX = cells[i].getBounds().getX();
				float worldPositionY = cells[i].getBounds().getY();
				int pixelX;
				int pixelY;
				worldToScreen( worldPositionX, worldPositionY, pixelX, pixelY );
				olc::vi2d startPos = olc::vi2d{ pixelX, pixelY };
				olc::vi2d size = olc::vi2d{ tileSize, tileSize }; //olc::vi2d{ ( int )( ( screenEndX - screenStartX ) * tileSize ), ( int )( ( screenEndY - screenStartY ) * tileSize ) };
				olc::vf2d scale = olc::vf2d{ 1.0f * this->_zoomX, 1.0f * this->_zoomY };
			
				pge->FillRect(
					startPos,
					olc::vi2d{ ( int )( tileSize * this->_zoomX ), ( int )( tileSize * this->_zoomY ) }, //-olc::vf2d{ 1.0f / tileSize, 1.0f / tileSize }
					id == 1 ? olc::DARK_CYAN : ( id == 2 ? olc::DARK_GREY : olc::DARK_GREEN )
				);
			}
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
					this->renderTileRenders( tileRenders.getReferenceNodes()[childrenNodeIndicies[i]] );
				}
			}
		}
	}

	return;
}


void Camera::renderCamera() const
{
	int tileSize = 16; // [!]

	pge->DrawRect
	(
		olc::vi2d{ this->_absolutePixelOffsetX, this->_absolutePixelOffsetY }, 
		olc::vi2d{ ( int )( this->_view.width * this->_zoomX * tileSize ), ( int )( this->_view.height * this->_zoomY * tileSize  ) },
		olc::WHITE
	);

	return;
}


void Camera::renderTilesDebug( WorldChunk& worldChunk ) const
{
	// Draw Tiles
	int tileSize = 16;
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
	this->_view.setX( this->_view.getX() + x );
	this->_view.setY( this->_view.getY() + y );
	return;
}


void Camera::panX( float x )
{
	this->_view.setX( this->_view.getX() + x );
	return;
}


void Camera::panY( float y )
{
	this->_view.setY( this->_view.getY() + y );
	return;
}


void Camera::zoom( float s )
{
	int centerCameraPixelXBeforeZoom;
	int centerCameraPixelYBeforeZoom;
	this->worldToScreen( this->_view.getCenterX(), this->_view.getCenterY(), centerCameraPixelXBeforeZoom, centerCameraPixelYBeforeZoom );

	float cameraCenterXBeforeZoom;
	float cameraCenterYBeforeZoom;
	this->screenToWorld( centerCameraPixelXBeforeZoom, centerCameraPixelYBeforeZoom, cameraCenterXBeforeZoom, cameraCenterYBeforeZoom );

	this->_zoomX *= s;
	this->_zoomY *= s;

	int centerCameraPixelXAfterZoom;
	int centerCameraPixelYAfterZoom;
	this->worldToScreen( this->_view.getCenterX(), this->_view.getCenterY(), centerCameraPixelXAfterZoom, centerCameraPixelYAfterZoom );

	float cameraCenterXAfterZoom;
	float cameraCenterYAfterZoom;
	this->screenToWorld( centerCameraPixelXBeforeZoom, centerCameraPixelYBeforeZoom, cameraCenterXAfterZoom, cameraCenterYAfterZoom );

	this->_view.width = this->_view.width * ( 1.0f / s );
	this->_view.height = this->_view.height * ( 1.0f / s );
	this->_view.setX( this->_view.getX() + ( cameraCenterXBeforeZoom - cameraCenterXAfterZoom ) );
	this->_view.setY( this->_view.getY() + ( cameraCenterYBeforeZoom - cameraCenterYAfterZoom ) );

	return;
}


void Camera::zoom( float x, float y )
{
	this->_zoomX *= x;
	this->_zoomY *= y;
	this->_view.width = this->_view.width * ( 1.0f / x );
	this->_view.height = this->_view.height * ( 1.0f / y );
	return;
}


void Camera::zoomX( float x )
{
	this->_zoomX *= x;
	this->_view.width = this->_view.width * ( 1.0f / x );
	return;
}


void Camera::zoomY( float y )
{
	this->_zoomY *= y;
	this->_view.width = this->_view.width * ( 1.0f / y );
	return;
}


void Camera::setPosition( float x, float y )
{
	this->_view.setX( x );
	this->_view.setY( y );
	return;
}


void Camera::setZoom( float s )
{
	this->_zoomX = s;
	this->_zoomY = s;
	this->_view.width = ( 1.0f / s );
	this->_view.height = ( 1.0f / s );
	return;
}

void Camera::setZoom( float x, float y )
{
	this->_zoomX = x;
	this->_zoomY = y;
	this->_view.width = ( 1.0f / x );
	this->_view.height = ( 1.0f / y );
	return;
}


void Camera::setView( float width, float height )
{
	this->_view.setWidth( width );
	this->_view.setHeight( height );
	return;
}


BoundingBox<float> Camera::getView() const
{
	return this->_view;
}


float Camera::getX() const
{
	return this->_view.getX();
}

float Camera::getY() const
{
	return this->_view.getY();
}


float Camera::getCenterX() const
{
	return this->_view.getX() + this->_view.getWidth() / 2;
}


float Camera::getCenterY() const
{
	return this->_view.getY() + this->_view.getHeight() / 2;
}

float Camera::getWidth() const
{
	return this->_view.getWidth();
}

float Camera::getHeight() const
{
	return this->_view.getHeight();
}


float Camera::getZoomX() const
{
	return this->_zoomX;
}


float Camera::getZoomY() const
{
	return this->_zoomY;
}