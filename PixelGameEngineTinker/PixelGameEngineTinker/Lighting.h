#pragma once
#include "olcPixelGameEngine.h"

#include <cstdint>
#include <cmath>
#include <vector>
#include <utility>

#include "Settings.h"
#include "Tile.h"
#include "LightSource.h"
#include "Light.h"
#include "LightRender.h"


template <typename T>
struct Quadrant
{
	static constexpr int NORTH = 0;
	static constexpr int EAST = 1;
	static constexpr int SOUTH = 2;
	static constexpr int WEST = 3;

	int cardinal;
	olc::v2d_generic<T> origin;

	Quadrant( int cardinal_, T originX_, T originY_ ) : cardinal( cardinal_ ), origin( originX_, originY_ ) {}

	~Quadrant() {}

	olc::v2d_generic<T> transform( const olc::v2d_generic<T>& tile )
	{
		T col = tile.x;
		T row = tile.y;
		if ( this->cardinal == NORTH )
		{
			return origin + olc::v2d_generic<T>{ col, -row };
		}
		else if ( this->cardinal == SOUTH )
		{
			return origin + olc::v2d_generic<T>{ col, row };
		}
		else if ( this->cardinal == EAST )
		{
			return origin + olc::v2d_generic<T>{ row, col };
		}
		else if ( this->cardinal == WEST )
		{
			return origin + olc::v2d_generic<T>{ -row, col };
		}
		else
		{
			return origin;
		}
	}
};



template <typename T>
struct Row
{
	T depth;
	float startSlope;
	float endSlope;

	Row( T depth_, float startSlope_, float endSlope_ ) : depth( depth_ ), startSlope( startSlope_ ), endSlope( endSlope_ ) {}

	~Row() {}

	std::vector< olc::v2d_generic<T>> getTiles()
	{
		std::vector< olc::v2d_generic<T>> rowTiles;

		float minCol = std::floorf( ( this->depth * this->startSlope ) + 0.5f );
		float maxCol = std::ceilf( ( this->depth * this->endSlope ) - 0.5f );

		for ( float col = minCol; col <= maxCol; col++ )
		{
			rowTiles.push_back( olc::v2d_generic<T>{ ( T )col, this->depth } );
		}

		return rowTiles;
	}

	Row<T> getNext()
	{
		return Row<T>( this->depth + 1.0, this->startSlope, this->endSlope );
	}
};



















template <typename T>
class Lighting : public olc::PGEX
{
private:
	std::int64_t _chunkIndexX;
	std::int64_t _chunkIndexY;
	std::uint16_t _width;
	std::uint16_t _height;
	Tile* _tiles;
	Light _lights[Settings::WorldChunk::SIZE * Settings::WorldChunk::SIZE];
	QuadTree<LightRender> _lightRenders[Settings::WorldChunk::NUM_TILE_RENDERS];

	std::map<std::uint16_t, LightSource> _lightSources;

	void reveal( Quadrant<T>& quadrant, const olc::v2d_generic<T>& tile, const olc::v2d_generic<T>& originPosition, const int maxRadius )
	{
		olc::v2d_generic<T> position = quadrant.transform( tile );
		int tilePosX = ( int )position.x;
		int tilePosY = ( int )position.y;

		int originPosX = ( int )originPosition.x;
		int originPosY = ( int )originPosition.y;

		T rayDistance = std::hypot( tilePosX - originPosX, tilePosY - originPosY );
		T intensity = std::max<T>( 0, ( 1.0 - ( rayDistance / maxRadius ) ) );

		if (
			( quadrant.cardinal == 0 && tile.x == tile.y ) ||
			( quadrant.cardinal == 1 && tile.x == tile.y ) ||
			( quadrant.cardinal == 2 && tile.x == -tile.y ) ||
			( quadrant.cardinal == 3 && tile.x == -tile.y )
			)
		{
			return;
		}

		this->addLight( tilePosX, tilePosY,
			this->_lightSources[originPosY * this->_width + originPosX].getRed() * intensity,
			this->_lightSources[originPosY * this->_width + originPosX].getGreen() * intensity,
			this->_lightSources[originPosY * this->_width + originPosX].getBlue() * intensity,
			this->_lightSources[originPosY * this->_width + originPosX].getAlpha()
		);

		return;
	}


	bool isOpaque( Quadrant<T>& quadrant, const olc::v2d_generic<T>& tile )
	{
		olc::v2d_generic<T> position = quadrant.transform( tile );
		int tileIndex = ( int )( ( ( int )std::ceil( position.y ) * this->_width + ( int )std::ceil( position.x ) ) );
		return this->_tiles[tileIndex].exists();
	}


	bool isTransparent( Quadrant<T>& quadrant, const olc::v2d_generic<T>& tile )
	{
		olc::v2d_generic<T> position = quadrant.transform( tile );
		int tileIndex = ( int )( ( ( int )std::ceil( position.y ) * this->_width + ( int )std::ceil( position.x ) ) );
		return !this->_tiles[tileIndex].exists();
	}


	void scanStatic( Quadrant<T>& quadrant, Row<T>& row, const olc::v2d_generic<T> originPosition, const std::uint16_t maxRadius )
	{
		if ( row.depth >= maxRadius )
		{
			return;
		}

		const olc::v2d_generic<T>* prevTile = nullptr;
		for ( const olc::v2d_generic<T>& tile : row.getTiles() )
		{
			if ( this->isOpaque( quadrant, tile ) || this->isSymmetric( row, tile ) )
			{
				this->reveal( quadrant, tile, originPosition, maxRadius );
			}
			if ( prevTile != nullptr && this->isOpaque( quadrant, *prevTile ) && this->isTransparent( quadrant, tile ) )
			{
				row.startSlope = Lighting::slopeStatic( tile );
			}
			if ( prevTile != nullptr && this->isTransparent( quadrant, *prevTile ) && this->isOpaque( quadrant, tile ) )
			{
				Row<T> nextRow = row.getNext();
				nextRow.endSlope = Lighting::slopeStatic( tile );
				this->scanStatic( quadrant, nextRow, originPosition, maxRadius );
			}
			prevTile = &tile;
		}

		if ( prevTile != nullptr && this->isTransparent( quadrant, *prevTile ) )
		{
			Row<T> nextRow = row.getNext();
			this->scanStatic( quadrant, nextRow, originPosition, maxRadius );
		}

		return;
	}


	static float slopeStatic( const olc::v2d_generic<T>& tile )
	{
		T tileCol = tile.x;
		T rowDepth = tile.y;
		return ( float )( 2 * tileCol - 1 ) / ( float )( 2 * rowDepth );
	}


	void scanDynamic( Quadrant<T>& quadrant, Row<T>& row, const olc::v2d_generic<T> originPosition, const std::uint16_t maxRadius )
	{
		if ( row.depth >= maxRadius )
		{
			return;
		}

		const olc::v2d_generic<T>* prevTile = nullptr;
		for ( const olc::v2d_generic<T>& tile : row.getTiles() )
		{
			if ( this->isOpaque( quadrant, tile ) || this->isSymmetric( row, tile ) )
			{
				olc::v2d_generic<T> position = quadrant.transform( tile );
				T tilePosX = position.x;
				T tilePosY = position.y;

				T originPosX = originPosition.x;
				T originPosY = originPosition.y;

				T rayDistance = std::hypot( tilePosX - ( std::uint16_t )std::ceil( originPosX ), tilePosY - ( std::uint16_t )std::ceil( originPosY ) );
				T intensity = std::max<T>( 0, ( 1.0 - ( rayDistance / maxRadius ) ) );

				if (
					!( quadrant.cardinal == 0 && tile.x == tile.y ) &&
					!( quadrant.cardinal == 1 && tile.x == tile.y ) &&
					!( quadrant.cardinal == 2 && tile.x == -tile.y ) &&
					!( quadrant.cardinal == 3 && tile.x == -tile.y )
					)
				{
					this->addLight( ( std::uint16_t )std::ceil( tilePosX ), ( std::uint16_t )std::ceil( tilePosY ),
						255 * intensity,
						255 * intensity,
						255 * intensity,
						255
					);
				}

			}
			if ( prevTile != nullptr && this->isOpaque( quadrant, *prevTile ) && this->isTransparent( quadrant, tile ) )
			{
				row.startSlope = Lighting::slopeDynamic( tile, originPosition, quadrant.cardinal );
			}
			if ( prevTile != nullptr && this->isTransparent( quadrant, *prevTile ) && this->isOpaque( quadrant, tile ) )
			{
				Row<T> nextRow = row.getNext();
				nextRow.endSlope = Lighting::slopeDynamic( tile, originPosition, quadrant.cardinal );
				this->scanDynamic( quadrant, nextRow, originPosition, maxRadius );
			}
			prevTile = &tile;
		}

		if ( prevTile != nullptr && this->isTransparent( quadrant, *prevTile ) )
		{
			Row<T> nextRow = row.getNext();
			this->scanDynamic( quadrant, nextRow, originPosition, maxRadius );
		}

		return;
	}


	static float slopeDynamic( const olc::v2d_generic<T>& tile, const olc::v2d_generic<T> originPosition, const std::uint8_t cardinality )
	{
		// Tangent Slope for subcell positions
		T dX = originPosition.x;
		T dY = originPosition.y;

		float fOriginX;
		float fractionX = std::modf( dX, &fOriginX );
		std::uint16_t originX = ( std::uint16_t )fOriginX;
		fOriginX -= fractionX;


		float fOriginY;
		float fractionY = std::modf( dY, &fOriginY );
		std::uint16_t originY = ( std::uint16_t )fOriginY;
		fOriginY -= fractionY;


		if ( fractionY == 0 ) fractionY = 0.99f;
		if ( fractionX == 0 ) fractionX = 0.99f;


		T tileCol;
		T rowDepth;

		if ( cardinality == 0 )
		{
			return ( float )( 2 * tile.x - 1 + fractionX - 0.5f ) / ( float )( 2 * tile.y - fractionY + 0.5f );
		}
		else if ( cardinality == 1 )
		{
			return ( float )( 2 * tile.x - 1 + fractionY - 0.5f ) / ( float )( 2 * tile.y + fractionX - 0.5f );
		}
		else if ( cardinality == 2 )
		{
			return ( float )( 2 * tile.x - 1 + fractionX - 0.5f ) / ( float )( 2 * tile.y + fractionY - 0.5f );
		}
		else if ( cardinality == 3 )
		{
			return ( float )( 2 * tile.x - 1 + fractionY - 0.5f ) / ( float )( 2 * tile.y - fractionX + 0.5f );
		}
		else
		{
			return ( float )( 2 * tile.x - 1 ) / ( float )( 2 * tile.y );
		}

	}


	static bool isSymmetric( const Row<T>& row, const olc::v2d_generic<T>& tile )
	{
		T tileCol = tile.x;
		T rowDepth = tile.y;
		return tileCol >= row.depth * row.startSlope && tileCol <= row.depth * row.endSlope;
	}


	std::uint64_t getNumCells() const
	{
		return this->_width * this->_height;
	}


	static bool inBounds( std::uint16_t row, std::uint16_t col, std::uint16_t numRows, std::uint16_t numCols )
	{
		return row >= 0 && std::abs( row ) < numRows && col >= 0 && std::abs( col ) < numCols;
	}


	void addLight( std::uint16_t x, std::uint16_t y, std::int16_t r, std::int16_t g, std::int16_t b, std::int16_t a )
	{
		std::uint64_t index = y * this->_width + x;
		if ( !Lighting::inBounds( x, y, this->_width, this->_height ) )
		{
			return;
		}

		this->_lights[index].addRed( r );
		this->_lights[index].addGreen( g );
		this->_lights[index].addBlue( b );
		this->_lights[index].addAlpha( a );
	}

public:
	Lighting( std::int64_t chunkIndexX_, std::int64_t chunkIndexY_, std::uint16_t width_, std::uint16_t height_, Tile* tiles_ ) : _chunkIndexX( chunkIndexX_ ), _chunkIndexY( chunkIndexY_ ), _width( width_ ), _height( height_ ), _tiles( tiles_ )
	{
		this->wipeRender();
		this->blackenLights();
	}


	~Lighting()
	{
		//delete[] this->_lights;
		//this->_lights = nullptr;
	}


	void setChunkIndexX( std::int64_t chunkIndexX )
	{
		this->_chunkIndexX = chunkIndexX;
		return;
	}

	void setChunkIndexY( std::int64_t chunkIndexY )
	{
		this->_chunkIndexY = chunkIndexY;
		return;
	}



	Light* getLights()
	{
		return this->_lights;
	}



	// QuadTree Render
	QuadTree<LightRender>* lightRenderEncapsulates()
	{
		return this->_lightRenders;
	}

	QuadTree<LightRender>& getLightRendersRoot()
	{
		return this->_lightRenders[0];
	}

	void insertLightRenders( std::uint32_t corner0, std::uint32_t corner1, std::uint32_t corner2, std::uint32_t corner3, bool exist, std::int64_t x, std::int64_t y, std::int64_t width, std::int64_t height)
	{
		this->_lightRenders[0].insert( LightRender( corner0, corner1, corner2, corner3, exist,
			BoundingBox<std::int64_t>( x, y, width, height ) ) );
		return;
	}


	void wipeRender()
	{
		// Need to update chunkIndex of lighting when updating index of worldChunk [!]
		std::int64_t rootQuadTreePositionX = this->_chunkIndexX * this->_width;
		std::int64_t rootQuadTreePositionY = this->_chunkIndexY * this->_height;

		// Intialize quadTrees
		this->_lightRenders[0].constructQuadTree(
			0,
			-1,
			Settings::WorldChunk::TILE_RENDER_MAX_LEVEL,
			0,
			BoundingBox<std::int64_t>( rootQuadTreePositionX, rootQuadTreePositionY, this->_width, this->_height ),
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



	void insertLightSource( std::uint16_t index, std::int16_t r, std::int16_t g, std::int16_t b, std::int16_t a, std::int16_t radius )
	{
		this->_lightSources.emplace( index, LightSource( r, g, b, a, radius ) );
		return;
	}


	void emitDynamicLightSource( float fX, float fY, std::int64_t radius )
	{
		this->emitDynamicLight( fX, fY, radius );
	}


	void emitDynamicLight( float dX, float dY, std::int64_t radius )
	{
		float fOriginX;
		float fractionX = std::modf( dX, &fOriginX );
		std::uint16_t originX = ( std::uint16_t )fOriginX;
		fOriginX -= fractionX;


		float fOriginY;
		float fractionY = std::modf( dY, &fOriginY );
		std::uint16_t originY = ( std::uint16_t )fOriginY;
		fOriginY -= fractionY;

		this->addLight( originX, originY,
			255, 255, 255, 255 );

		olc::v2d_generic<T> originPosition{ ( T )fOriginX, ( T )fOriginY };

		for ( int cardinality = 0; cardinality < 4; cardinality++ )
		{
			Quadrant<T> quadrant( cardinality, ( T )fOriginX, ( T )fOriginY );
			Row<T> initialRow = Row<T>( 1.0f, -1.0f, 1.0f );
			this->scanDynamic( quadrant, initialRow, originPosition, radius );
		}

		return;
	}


	void emitStaticLightSources()
	{
		for ( const std::pair<std::uint16_t, LightSource>& lightSource : this->_lightSources )
		{
			std::uint16_t lightSourceIndex = std::get<0>( lightSource );
			std::uint16_t x = lightSourceIndex % this->_width;
			std::uint16_t y = lightSourceIndex / this->_width;
			std::uint16_t radius = std::get<1>( lightSource ).getRadius();

			this->emitStaticLight( x, y, radius );
		}

		return;
	}


	void emitStaticLight( std::uint16_t originX, std::uint16_t originY, std::uint16_t radius )
	{
		this->addLight( originX, originY,
			this->_lightSources[originY * this->_width + originX].getRed(),
			this->_lightSources[originY * this->_width + originX].getGreen(),
			this->_lightSources[originY * this->_width + originX].getBlue(),
			this->_lightSources[originY * this->_width + originX].getAlpha() );

		olc::v2d_generic<T> originCellPosition{ ( T )originX, ( T )originY };


		for ( int cardinality = 0; cardinality < 4; cardinality++ )
		{
			Quadrant<T> quadrant( cardinality, originX, originY );
			Row<T> initialRow = Row<T>( 1, -1.0f, 1.0f );
			this->scanStatic( quadrant, initialRow, originCellPosition, radius );
		}

		return;
	}


	void reset()
	{
		this->wipeRender();
		this->blackenLights();

		return;
	}


	void resetLights()
	{
		for ( std::uint16_t i = 0; i < this->getNumCells(); i++ )
		{
			this->_lights[i].reset();
		}
		return;
	}

	void blackenLights()
	{
		//std::cout << "blacken" << std::endl;
		for ( std::uint16_t i = 0; i < this->getNumCells(); i++ )
		{
			this->_lights[i].blacken();
		}
		return;
	}

	void whitenLights()
	{
		for ( std::uint16_t i = 0; i < this->getNumCells(); i++ )
		{
			this->_lights[i].whiten();
		}
		return;
	}

};