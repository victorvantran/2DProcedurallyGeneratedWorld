#pragma once

#include "BoundingBox.h"
#include "World.h"
#include "WorldChunk.h"
#include "QuadTree.h"
#include "TileConsolidated.h"
#include "Tile.h"


class Camera : public olc::PGEX
{
private:
	BoundingBox<float> _view; // view is cell-domain

	int _absolutePixelOffsetX = 16 * ( 120 / 2 ) - 16 * ( 32 / 2 ); //16 * ( 120 / 2 ) - 16 * ( 32 / 2 );  // tileSize * ( screenCellWidth / 2 ) - tileSize * ( cameraCellWidth / 2 ) [!] need to change based on pixelSize
	int _absolutePixelOffsetY = 16 * ( 75 / 2 ) - 16 * ( 32 / 2 );

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
	void renderQuadTree( QuadTree<Tile, TileConsolidated>& quadTree ) const;
	void renderCamera() const;

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
	/*
	int tileSize = 16; // [!] global singleton
	cellX = ( float )( ( ( float )pixelX / ( float )( this->_zoomX * tileSize ) ) + ( this->_view.x - this->_view.width / 2.0f ) );
	cellY = ( float )( ( ( float )pixelY / ( float )( this->_zoomY * tileSize ) ) + ( this->_view.y - this->_view.height / 2.0f ) );
	*/

	/*
	int tileSize = 16; // [!] global singleton
	cellX = ( float )( ( ( float )( pixelX - this->_absolutePixelOffsetX ) / ( float )( this->_zoomX * tileSize ) ) + ( this->_view.x - this->_view.width / 2.0f ) );
	cellY = ( float )( ( ( float )( pixelY - this->_absolutePixelOffsetY ) / ( float )( this->_zoomY * tileSize ) ) + ( this->_view.y - this->_view.height / 2.0f ) );
	*/


	int tileSize = 16; // [!] global singleton
	cellX = ( float )( ( ( float )( pixelX - this->_absolutePixelOffsetX ) / ( float )( this->_zoomX * tileSize ) ) + ( this->_view.x ) );
	cellY = ( float )( ( ( float )( pixelY - this->_absolutePixelOffsetY ) / ( float )( this->_zoomY * tileSize ) ) + ( this->_view.y ) );
	return;
}


void Camera::worldToScreen( float cellX, float cellY, int& pixelX, int& pixelY ) const
{
	// float to int ( camera offset determines displacement )
	/*
	int tileSize = 16; // [!] global singleton

	pixelX = ( int )( ( cellX - ( this->_view.x - this->_view.width / 2.0f ) ) * ( this->_zoomX * tileSize ) );
	pixelY = ( int )( ( cellY - ( this->_view.y - this->_view.height / 2.0f ) ) * ( this->_zoomY * tileSize ) );
	*/


	/*
	int tileSize = 16; // [!] global singleton

	pixelX = ( int )( ( cellX - ( this->_view.x - this->_view.width / 2.0f ) ) * ( this->_zoomX * tileSize ) ) + this->_absolutePixelOffsetX;
	pixelY = ( int )( ( cellY - ( this->_view.y - this->_view.height / 2.0f ) ) * ( this->_zoomY * tileSize ) ) + this->_absolutePixelOffsetY;
	*/


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

	this->renderQuadTree( worldChunk.getQuadTreeRoot() );


	return;
}



void Camera::renderQuadTree( QuadTree<Tile, TileConsolidated>& quadTree ) const
{
	int tileSize = 16; // [!] make it global variable in singleton
	int chunkSize = 32; // [!] singleton 

	//std::cout << quadTree.getIndex() << std::endl;

	QuadTree<Tile, TileConsolidated> currQuadTree = quadTree.getReferenceNodes()[quadTree.getIndex()];
	const BoundingBox<int> bounds = currQuadTree.getBounds();
	// No need to render if the camera can not see it




	if ( !this->_view.intersects( bounds ) )
	{
		return;
	}

	// Fill Consolidated
	if ( currQuadTree.isConsolidated() )
	{

		if ( this->_view.intersects( currQuadTree.getBounds() ) )
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
				id == 0 ? olc::DARK_GREEN : olc::DARK_GREY //olc::WHITE
			);


		}



		return;
	}
	// Fill the small bounding boxes (cells) that are not possibly consolidated
	else
	{
		TileConsolidated* cells = currQuadTree.getCells();
		for ( int i = 0; i < 4; i++ )
		{


			if ( cells[i].getExist() && this->_view.intersects( cells[i].getBounds() ) )
			{
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
					cells[i].getId() == 0 ? olc::DARK_GREEN : olc::DARK_GREY
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
					this->renderQuadTree( quadTree.getReferenceNodes()[childrenNodeIndicies[i]] );
				}
			}
		}

	}

	return;
}



/*
void Camera::renderQuadTree( QuadTree<Tile, TileConsolidated>& quadTree ) const
{
	int tileSize = 16; // [!] make it global variable in singleton
	int chunkSize = 32; // [!] singleton 

	//std::cout << quadTree.getIndex() << std::endl;

	QuadTree<Tile, TileConsolidated> currQuadTree = quadTree.getReferenceNodes()[quadTree.getIndex()];
	const BoundingBox<int> bounds = currQuadTree.getBounds();
	// No need to render if the camera can not see it




	if ( !this->_view.intersects( bounds ) )
	{
		return;
	}

	// Fill Consolidated
	if ( currQuadTree.isConsolidated() )
	{

		if ( this->_view.intersects( currQuadTree.getBounds() ) )
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

			float cameraOffsetX = this->_view.x;
			float cameraOffsetY = this->_view.y;

			float zoomOffsetX = ( ( cameraOffsetX - this->_view.width ) / 2.0f );
			float zoomOffsetY = ( ( cameraOffsetY - this->_view.height ) / 2.0f );

			float screenStartX = ( ( float )bounds.getX() - cameraOffsetX );
			float screenStartY = ( ( float )bounds.getY() - cameraOffsetY );

			float screenEndX = ( ( float )bounds.getX() + ( float )scale - cameraOffsetX );
			float screenEndY = ( ( float )bounds.getY() + ( float )scale - cameraOffsetY );

			
			//olc::vi2d startPos = olc::vi2d{ ( int )( screenStartX * this->_zoomX * tileSize ), ( int )( screenStartY * this->_zoomY * tileSize ) };
			//		olc::vi2d startPos = olc::vi2d{ ( int )( ( screenStartX - zoomOffsetX ) * this->_zoomX * tileSize ), ( int )( ( screenStartY - zoomOffsetY )* this->_zoomY * tileSize ) };

			//olc::vi2d size = olc::vi2d{ scale * tileSize, scale * tileSize }; //olc::vi2d{ ( int )( (screenEndX - screenStartX) * tileSize ), ( int )( (screenEndY - screenStartY) * tileSize ) };

			//pge->DrawPartialDecal(
			//	startPos,
			//	decalDirtTileConsolidationMap,
			//	olc::vi2d{ olc::vi2d{ decalPositionX, decalPositionY } * tileSize * pixelSize }, // 16x16 decal unit
			//	size,
			//	olc::vf2d{ 1.0f * this->_zoomX , 1.0f * this->_zoomY } //- olc::vf2d{ 1.0f/tileSize, 1.0f/tileSize }
			//);
			

			float worldPositionX = currQuadTree.getBounds().getX();
			float worldPositionY = currQuadTree.getBounds().getY();

			int pixelX;
			int pixelY;

			worldToScreen( worldPositionX, worldPositionY, pixelX, pixelY );
			olc::vi2d startPos = olc::vi2d{ pixelX, pixelY };


			pge->FillRect(
				startPos,
				olc::vf2d{ 1.0f * scale * this->_zoomX * tileSize  , 1.0f * scale * this->_zoomY * tileSize },
				id == 0 ? olc::DARK_GREEN : olc::DARK_GREY //olc::WHITE
			);


		}


		
		return;
	}
	// Fill the small bounding boxes (cells) that are not possibly consolidated
	else
	{
		TileConsolidated* cells = currQuadTree.getCells();
		for ( int i = 0; i < 4; i++ )
		{


			if ( cells[i].getExist() && this->_view.intersects( cells[i].getBounds() ) )
			{
				olc::vi2d decalSourcePos = olc::vi2d{ cells[i].getId() == 0 ? 7 : 15, 7 }; // dirt, stone



				float worldPositionX = cells[i].getBounds().getX();
				float worldPositionY = cells[i].getBounds().getY();

				int pixelX;
				int pixelY;

				worldToScreen( worldPositionX, worldPositionY, pixelX, pixelY );
				olc::vi2d startPos = olc::vi2d{ pixelX, pixelY };


				olc::vi2d size = olc::vi2d{ tileSize, tileSize }; //olc::vi2d{ ( int )( ( screenEndX - screenStartX ) * tileSize ), ( int )( ( screenEndY - screenStartY ) * tileSize ) };
				olc::vf2d scale = olc::vf2d{ 1.0f * this->_zoomX, 1.0f * this->_zoomY };


				// Draw in the camera space domain

				
				// DrawPartialDecal(
				//	startPos,
				//	decalTileMap,
				//	olc::vi2d{ decalSourcePos * tileSize * pixelSize }, // 16x16 decal unit
				//	size,
				//	olc::vf2d{ 1.0f * this->_zoomX, 1.0f * this->_zoomY } //-olc::vf2d{ 1.0f / tileSize, 1.0f / tileSize }
				//);

				
				pge->FillRect(
					startPos,
					olc::vi2d{ (int) (tileSize * this->_zoomX ), (int) (tileSize * this->_zoomY ) }, //-olc::vf2d{ 1.0f / tileSize, 1.0f / tileSize }
					cells[i].getId() == 0 ? olc::DARK_GREEN : olc::DARK_GREY
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
					this->renderQuadTree( quadTree.getReferenceNodes()[childrenNodeIndicies[i]] );
				}
			}
		}

	}

	return;
}
*/


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




	int centerCameraPixelXAfterZoom; // same?
	int centerCameraPixelYAfterZoom;

	this->worldToScreen( this->_view.getCenterX(), this->_view.getCenterY(), centerCameraPixelXAfterZoom, centerCameraPixelYAfterZoom );

	//

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