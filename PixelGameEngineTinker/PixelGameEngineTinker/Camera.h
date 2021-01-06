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
	BoundingBox<long double> _focalPoint; // cell-domain
	BoundingBox<long double> _view;

	std::int64_t _absolutePixelOffsetX = Settings::Camera::ABSOLUTE_PIXEL_OFFSET_X;  //16 * ( 120 / 2 ) - 16 * ( 32 / 2 );  // tileSize * ( screenCellWidth / 2 ) - tileSize * ( cameraCellWidth / 2 ) [!] need to change based on pixelSize
	std::int64_t _absolutePixelOffsetY = Settings::Camera::ABSOLUTE_PIXEL_OFFSET_Y;

	long double _zoomX;
	long double _zoomY;


	World* _world = nullptr;

public:
	Camera();
	~Camera();
	Camera( BoundingBox<long double> focalPoint, BoundingBox<long double> biew, long double zoomX, long double zoomY, World* _world );

	void screenToWorld( std::int64_t pixelX, std::int64_t pixelY, long double& cellX, long double& cellY ) const; // int to float ( camera offest determines displacement )
	void worldToScreen( long double cellX, long double cellY, std::int64_t& pixelX, std::int64_t& pixelY ) const; // float to int ( camera offset determines displacement )

	void renderWorld() const;
	void renderWorldChunk( WorldChunk& worldChunk, Atlas& atlas ) const;
	void renderTileRenders( QuadTree<TileRender>& tileRenders, Atlas& atlas ) const;
	void renderCamera() const;

	void renderTilesDebug( WorldChunk& worldChunk ) const;

	void pan( long double x, long double y );
	void panX( long double x );
	void panY( long double y );
	void zoom( long double s );
	void zoom( long double x, long double y );
	void zoomX( long double x );
	void zoomY( long double y );
	void setPosition( long double x, long double y );
	void setZoom( long double s );
	void setZoom( long double x, long double y );
	void setFocalPoint( long double width, long double height );

	BoundingBox<long double> getFocalPoint() const;
	long double getCenterX() const;
	long double getCenterY() const;
	long double getZoomX() const;
	long double getZoomY() const;
};
