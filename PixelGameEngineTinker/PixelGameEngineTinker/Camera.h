#pragma once

#include <cstdint>
#include "Settings.h"
#include "BoundingBox.h"
//#include "World.h"
#include "Atlas.h"
#include "WorldChunk.h"
#include "QuadTree.h"
#include "TileRender.h"
#include "Tile.h"

class World; // Forward Declaration

class Camera : public olc::PGEX
{
private:
	BoundingBox<float> _focalPoint; // cell-domain
	BoundingBox<float> _view;

	int _absolutePixelOffsetX = Settings::Camera::ABSOLUTE_PIXEL_OFFSET_X;  //16 * ( 120 / 2 ) - 16 * ( 32 / 2 );  // tileSize * ( screenCellWidth / 2 ) - tileSize * ( cameraCellWidth / 2 ) [!] need to change based on pixelSize
	int _absolutePixelOffsetY = Settings::Camera::ABSOLUTE_PIXEL_OFFSET_Y;

	float _zoomX;
	float _zoomY;


	World* _world = nullptr;

public:
	Camera();
	~Camera();
	Camera( BoundingBox<float> focalPoint, BoundingBox<float> biew, float zoomX, float zoomY, World* _world );

	void screenToWorld( int pixelX, int pixelY, float& cellX, float& cellY ) const; // int to float ( camera offest determines displacement )
	void worldToScreen( float cellX, float cellY, int& pixelX, int& pixelY ) const; // float to int ( camera offset determines displacement )

	void renderWorld() const;
	void renderWorldChunk( WorldChunk& worldChunk, Atlas& atlas ) const;
	void renderTileRenders( QuadTree<TileRender>& tileRenders, Atlas& atlas ) const;
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
