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
	std::uint64_t _width;
	std::uint64_t _height;
	Tile* _tiles;
	Light _lights[Settings::WorldChunk::SIZE * Settings::WorldChunk::SIZE];

	std::map<std::uint16_t, LightSource> _lightSources;


	olc::Sprite* _spriteLight = nullptr;
	olc::Decal* _decalLight = nullptr;


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
	Lighting( std::uint16_t width_, std::uint16_t height_, Tile* tiles_ ) : _width( width_ ), _height( height_ ), _tiles( tiles_ )
	{
		this->_spriteLight = new olc::Sprite( "./tiles/light.png" );
		this->_decalLight = new olc::Decal( this->_spriteLight );

		this->blackenLights();
	}


	~Lighting()
	{
		//delete[] this->_lights;
		//this->_lights = nullptr;

		delete this->_spriteLight;
		this->_spriteLight = nullptr;

		delete this->_decalLight;
		this->_decalLight = nullptr;
	}


	Light* getLights()
	{
		return this->_lights;
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




	void renderLights()
	{
		// 16 bit int may e too small for lights that are close to eahcother exceed 2^15 - 1
		for ( std::uint16_t i = 0; i < this->getNumCells(); i++ )
		{
			std::uint16_t x = i % this->_width;
			std::uint16_t y = i / this->_width;


			/*
			// No interpolation
			// [!] need to fix max glitch
			//std::uint8_t r = std::min<std::uint8_t>( this->_lights[i].getRed(), 255 );
			//std::uint8_t g = std::min<std::uint8_t>( this->_lights[i].getGreen(), 255 );
			//std::uint8_t b = std::min<std::uint8_t>( this->_lights[i].getBlue(), 255 );
			//std::uint8_t a = std::min<std::uint8_t>( this->_lights[i].getAlpha(), 255 );

			std::uint8_t r = ( std::uint8_t )std::min<std::uint16_t>( this->_lights[i].getRed(), 255 );
			std::uint8_t g = ( std::uint8_t )std::min<std::uint16_t>( this->_lights[i].getGreen(), 255 );
			std::uint8_t b = ( std::uint8_t )std::min<std::uint16_t>( this->_lights[i].getBlue(), 255 );
			std::uint8_t a = ( std::uint8_t )std::min<std::uint16_t>( this->_lights[i].getAlpha(), 255 );


			if ( a == 0 ||
				( r == 0 && g == 0 && b == 0 ) )
			{
				//continue;
			}


			olc::Pixel colors[4];
			colors[0] = olc::Pixel{ r, g, b, a };
			colors[1] = olc::Pixel{ r, g, b, a };
			colors[2] = olc::Pixel{ r, g, b, a };
			colors[3] = olc::Pixel{ r, g, b, a };
			*/



			// Bilnear interpolation
			int ne = ( y - 1 ) * this->_width + ( x + 1 ); // Northerneastern neighbor
			int n = ( y - 1 ) * this->_width + x; // Northern neighbor
			int nw = ( y - 1 ) * this->_width + ( x - 1 ); // // Northwestern

			int e = y * this->_width + ( x + 1 ); // Eastern neighbor
			int c = y * this->_width + x; // Current tile
			int w = y * this->_width + ( x - 1 ); // Western neighbor

			int s = ( y + 1 ) * this->_width + x; // Southern neighbor
			int sw = ( y + 1 ) * this->_width + ( x - 1 ); // Southwestern neighbor
			int se = ( y + 1 ) * this->_width + ( x + 1 ); // Southeastern neighbor


			std::uint8_t corner0R = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( this->_lights[i].getRed() + this->_lights[nw].getRed() + this->_lights[n].getRed() + this->_lights[w].getRed() ) / 4 ), 255 ) );
			std::uint8_t corner0G = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( this->_lights[i].getGreen() + this->_lights[nw].getGreen() + this->_lights[n].getGreen() + this->_lights[w].getGreen() ) / 4 ), 255 ) );
			std::uint8_t corner0B = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( this->_lights[i].getBlue() + this->_lights[nw].getBlue() + this->_lights[n].getBlue() + this->_lights[w].getBlue() ) / 4 ), 255 ) );
			std::uint8_t corner0A = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( this->_lights[i].getAlpha() + this->_lights[nw].getAlpha() + this->_lights[n].getAlpha() + this->_lights[w].getAlpha() ) / 4 ), 255 ) );


			std::uint8_t corner1R = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( this->_lights[i].getRed() + this->_lights[sw].getRed() + this->_lights[s].getRed() + this->_lights[w].getRed() ) / 4 ), 255 ) );
			std::uint8_t corner1G = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( this->_lights[i].getGreen() + this->_lights[sw].getGreen() + this->_lights[s].getGreen() + this->_lights[w].getGreen() ) / 4 ), 255 ) );
			std::uint8_t corner1B = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( this->_lights[i].getBlue() + this->_lights[sw].getBlue() + this->_lights[s].getBlue() + this->_lights[w].getBlue() ) / 4 ), 255 ) );
			std::uint8_t corner1A = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( this->_lights[i].getAlpha() + this->_lights[sw].getAlpha() + this->_lights[s].getAlpha() + this->_lights[w].getAlpha() ) / 4 ), 255 ) );



			std::uint8_t corner2R = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( this->_lights[i].getRed() + this->_lights[se].getRed() + this->_lights[s].getRed() + this->_lights[e].getRed() ) / 4 ), 255 ) );
			std::uint8_t corner2G = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( this->_lights[i].getGreen() + this->_lights[se].getGreen() + this->_lights[s].getGreen() + this->_lights[e].getGreen() ) / 4 ), 255 ) );
			std::uint8_t corner2B = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( this->_lights[i].getBlue() + this->_lights[se].getBlue() + this->_lights[s].getBlue() + this->_lights[e].getBlue() ) / 4 ), 255 ) );
			std::uint8_t corner2A = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( this->_lights[i].getAlpha() + this->_lights[se].getAlpha() + this->_lights[s].getAlpha() + this->_lights[e].getAlpha() ) / 4 ), 255 ) );


			std::uint8_t corner3R = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( this->_lights[i].getRed() + this->_lights[ne].getRed() + this->_lights[n].getRed() + this->_lights[e].getRed() ) / 4 ), 255 ) );
			std::uint8_t corner3G = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( this->_lights[i].getGreen() + this->_lights[ne].getGreen() + this->_lights[n].getGreen() + this->_lights[e].getGreen() ) / 4 ), 255 ) );
			std::uint8_t corner3B = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( this->_lights[i].getBlue() + this->_lights[ne].getBlue() + this->_lights[n].getBlue() + this->_lights[e].getBlue() ) / 4 ), 255 ) );
			std::uint8_t corner3A = ( std::uint8_t )std::max<int>( 0, std::min<int>( ( ( this->_lights[i].getAlpha() + this->_lights[ne].getAlpha() + this->_lights[n].getAlpha() + this->_lights[e].getAlpha() ) / 4 ), 255 ) );

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


			olc::vf2d verticiesB[4];
			verticiesB[0] = olc::vf2d{ x * ( float )Settings::Screen::CELL_PIXEL_SIZE, y * ( float )Settings::Screen::CELL_PIXEL_SIZE };
			verticiesB[1] = olc::vf2d{ x * ( float )Settings::Screen::CELL_PIXEL_SIZE, y * ( float )Settings::Screen::CELL_PIXEL_SIZE + ( float )Settings::Screen::CELL_PIXEL_SIZE };
			verticiesB[2] = olc::vf2d{ x * ( float )Settings::Screen::CELL_PIXEL_SIZE + ( float )Settings::Screen::CELL_PIXEL_SIZE, y * ( float )Settings::Screen::CELL_PIXEL_SIZE + ( float )Settings::Screen::CELL_PIXEL_SIZE };
			verticiesB[3] = olc::vf2d{ x * ( float )Settings::Screen::CELL_PIXEL_SIZE + ( float )Settings::Screen::CELL_PIXEL_SIZE, y * ( float )Settings::Screen::CELL_PIXEL_SIZE };

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
			// Also add "additive tick"
			if ( !( corner0R == 0 && corner0G == 0 && corner0B == 0 &&
				corner1R == 0 && corner1G == 0 && corner1B == 0 &&
				corner2R == 0 && corner2G == 0 && corner2B == 0 &&
				corner3R == 0 && corner3G == 0 && corner3B == 0 ) )
			{
				pge->SetDecalMode( olc::DecalMode::ADDITIVE );
				pge->DrawExplicitDecal(
					lightDecal,
					verticiesB,
					textureCoordinates,
					colors
				);
			}
			*/


		}
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