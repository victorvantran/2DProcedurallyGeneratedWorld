#pragma once

#include "olcPixelGameEngine.h"
#include "LightCastRow.h"


template <class T>
struct LightCastQuadrant
{
	static constexpr int NORTH = 0;
	static constexpr int EAST = 1;
	static constexpr int SOUTH = 2;
	static constexpr int WEST = 3;

	std::int8_t cardinal;
	olc::v2d_generic<T> origin;

	LightCastQuadrant( std::int8_t cardinal_, T originX_, T originY_ ) : cardinal( cardinal_ ), origin( originX_, originY_ ) {}

	~LightCastQuadrant() {}


	olc::v2d_generic<T> transform( const olc::v2d_generic<T>& tile )
	{
		T col = tile.x;
		T row = tile.y;
		if ( this->cardinal == LightCastQuadrant::NORTH )
		{
			return this->origin + olc::v2d_generic<T>{ col, -row };
		}
		else if ( this->cardinal == LightCastQuadrant::SOUTH )
		{
			return this->origin + olc::v2d_generic<T>{ col, row };
		}
		else if ( this->cardinal == LightCastQuadrant::EAST )
		{
			return this->origin + olc::v2d_generic<T>{ row, col };
		}
		else if ( this->cardinal == LightCastQuadrant::WEST )
		{
			return this->origin + olc::v2d_generic<T>{ -row, col };
		}
		else
		{
			return this->origin;
		}
	}


	static float slopeStatic( const olc::v2d_generic<T>& tile )
	{
		T tileCol = tile.x;
		T rowDepth = tile.y;
		return ( float )( 2 * tileCol - 1 ) / ( float )( 2 * rowDepth );
	}


	static bool isSymmetric( const LightCastRow& row, const olc::v2d_generic<T>& tile )
	{
		T tileCol = tile.x;
		T rowDepth = tile.y;
		return tileCol >= row.depth * row.startSlope && tileCol <= row.depth * row.endSlope;
	}



};

