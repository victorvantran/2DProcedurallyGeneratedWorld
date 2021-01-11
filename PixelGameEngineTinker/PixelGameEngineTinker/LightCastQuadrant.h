#pragma once

#include "olcPixelGameEngine.h"
#include "LightCastRow.h"

struct LightCastQuadrant
{
	static constexpr int NORTH = 0;
	static constexpr int EAST = 1;
	static constexpr int SOUTH = 2;
	static constexpr int WEST = 3;

	std::int8_t cardinal;
	olc::v2d_generic<long double> origin;

	LightCastQuadrant( std::int8_t cardinal_, long double originX_, long double originY_ );
	~LightCastQuadrant();

	olc::v2d_generic<long double> transform( const olc::v2d_generic<long double>& tile );



	static float slopeStatic( const olc::v2d_generic<long double>& tile )
	{
		long double tileCol = tile.x;
		long double rowDepth = tile.y;
		return ( float )( 2 * tileCol - 1 ) / ( float )( 2 * rowDepth );
	}

	
	static float slopeDynamic( const olc::v2d_generic<long double>& tile, const olc::v2d_generic<long double> originPosition, const std::uint8_t cardinality )
	{
		// Tangent Slope for subcell positions
		long double dX = originPosition.x;
		long double dY = originPosition.y;

		long double dOriginX;
		long double fractionX = std::modfl( dX, &dOriginX );
		std::int64_t originX = ( std::int64_t )dOriginX;
		//dOriginX -= fractionX;
		dOriginX -= ( dOriginX >= 0 ) ? fractionX : -fractionX;



		long double dOriginY;
		long double fractionY = std::modfl( dY, &dOriginY );
		std::int64_t originY = ( std::int64_t )dOriginY;
		//dOriginY -= fractionY;
		dOriginY -= ( dOriginY >= 0 ) ? fractionY : -fractionY;



		//if ( fractionY == 0 ) fractionY = 0.99;
		//if ( fractionX == 0 ) fractionX = 0.99;

		//if ( fractionY == 0 ) fractionY = ( originPosition.y >= 0 ) ? 0.99 : -0.99;
		//if ( fractionX == 0 ) fractionX = ( originPosition.x >= 0 ) ? 0.99 : -0.99;

		if ( fractionY == 0 ) fractionY = ( originPosition.y >= 0 ) ? 1 : -1;
		if ( fractionX == 0 ) fractionX = ( originPosition.x >= 0 ) ? 1 : -1;

		long double tileCol;
		long double rowDepth;


		if ( originPosition.x >= 0 && originPosition.y >= 0 )
		{
			//std::cout << "bottomRight" << std::endl;
			if ( cardinality == 0 )
			{
				return ( 2 * tile.x - 1 + fractionX - 0.5 ) / ( 2 * tile.y - fractionY + 0.5 );
			}
			else if ( cardinality == 1 )
			{
				return ( 2 * tile.x - 1 + fractionY - 0.5 ) / ( 2 * tile.y + fractionX - 0.5 );
			}
			else if ( cardinality == 2 )
			{
				return ( 2 * tile.x - 1 + fractionX - 0.5 ) / ( 2 * tile.y + fractionY - 0.5 );
			}
			else if ( cardinality == 3 )
			{
				return ( 2 * tile.x - 1 + fractionY - 0.5 ) / ( 2 * tile.y - fractionX + 0.5 );
			}
			else
			{
				return ( 2 * tile.x - 1 ) / ( 2 * tile.y );
			}
		}
		else if ( originPosition.x < 0 && originPosition.y >= 0 )
		{
			//std::cout << "bottomLeft" << std::endl;
			/*
			if ( cardinality == 0 )
			{
				return ( 2 * tile.x - 1 - fractionX + 0.5 ) / ( 2 * tile.y - fractionY - 0.5 );
			}
			else if ( cardinality == 1 )
			{
				return ( 2 * tile.x - 1 + fractionY + 0.5 ) / ( 2 * tile.y - fractionX + 0.5 );
			}
			else if ( cardinality == 2 )
			{
				return ( 2 * tile.x - 1 - fractionX + 0.5 ) / ( 2 * tile.y + fractionY + 0.5 );
			}
			else if ( cardinality == 3 )
			{
				return ( 2 * tile.x - 1 + fractionY + 0.5 ) / ( 2 * tile.y + fractionX - 0.5 );
			}
			else
			{
				return ( 2 * tile.x - 1 ) / ( 2 * tile.y );
			}
			*/

			/*
			if ( cardinality == 0 )
			{
				return ( 2 * tile.x - 1 - fractionX - 0.5 ) / ( 2 * tile.y - fractionY - 0.5 );
			}
			else if ( cardinality == 1 )
			{
				return ( 2 * tile.x - 1 + fractionY + 0.5 ) / ( 2 * tile.y - fractionX - 0.5 );
			}
			else if ( cardinality == 2 )
			{
				return ( 2 * tile.x - 1 - fractionX - 0.5 ) / ( 2 * tile.y + fractionY + 0.5 );
			}
			else if ( cardinality == 3 )
			{
				return ( 2 * tile.x - 1 + fractionY + 0.5 ) / ( 2 * tile.y + fractionX + 0.5 );
			}
			else
			{
				return ( 2 * tile.x - 1 ) / ( 2 * tile.y );
			}
			*/
			/*
			if ( cardinality == 0 )
			{
				return ( 2 * tile.x - 1 - fractionX - 0.5 ) / ( 2 * tile.y - fractionY - 0.5 );
			}
			else if ( cardinality == 1 )
			{
				return ( 2 * tile.x - 1 + fractionY + 0.5 ) / ( 2 * tile.y - fractionX - 0.5 );
			}
			else if ( cardinality == 2 )
			{
				return ( 2 * tile.x - 1 - fractionX - 0.5 ) / ( 2 * tile.y + fractionY + 0.5 );
			}
			else if ( cardinality == 3 )
			{
				return ( 2 * tile.x - 1 + fractionY + 0.5 ) / ( 2 * tile.y + fractionX + 0.5 );
			}
			else
			{
				return ( 2 * tile.x - 1 ) / ( 2 * tile.y );
			}
			*/



			if ( cardinality == 0 )
			{
				return ( 2 * tile.x - 1 - ( fractionX + 0.5 ) ) / ( 2 * tile.y - fractionY + 0.5 );
			}
			else if ( cardinality == 1 )
			{
				return ( 2 * tile.x - 1 + fractionY - 0.5 ) / ( 2 * tile.y - ( fractionX + 0.5 ) );
			}
			else if ( cardinality == 2 )
			{
				return ( 2 * tile.x - 1 - ( fractionX + 0.5 ) ) / ( 2 * tile.y + fractionY - 0.5 );
			}
			else if ( cardinality == 3 )
			{
				return ( 2 * tile.x - 1 + fractionY - 0.5 ) / ( 2 * tile.y - ( -fractionX - 0.5 ) );
			}
			else
			{
				return ( 2 * tile.x - 1 ) / ( 2 * tile.y );
			}
		}
		else if ( originPosition.x >= 0 && originPosition.y < 0 )
		{
			//std::cout << "topRight" << std::endl;
			/*
			if ( cardinality == 0 )
			{
				return ( 2 * tile.x - 1 + fractionX - 0.5 ) / ( 2 * tile.y + fractionY - 0.5 );
			}
			else if ( cardinality == 1 )
			{
				return ( 2 * tile.x - 1 - fractionY + 0.5 ) / ( 2 * tile.y + fractionX - 0.5 );
			}
			else if ( cardinality == 2 )
			{
				return ( 2 * tile.x - 1 + fractionX - 0.5 ) / ( 2 * tile.y - fractionY + 0.5 );
			}
			else if ( cardinality == 3 )
			{
				return ( 2 * tile.x - 1 - fractionY + 0.5 ) / ( 2 * tile.y - fractionX + 0.5 );
			}
			else
			{
				return ( 2 * tile.x - 1 ) / ( 2 * tile.y );
			}
			*/
			/*
			if ( cardinality == 0 )
			{
				return ( 2 * tile.x - 1 + fractionX - 0.5 ) / ( 2 * tile.y - ( -fractionY + 0.5 ) );
			}
			else if ( cardinality == 1 )
			{
				return ( 2 * tile.x - 1 - ( fractionY - 0.5 ) ) / ( 2 * tile.y + fractionX - 0.5 );
			}
			else if ( cardinality == 2 )
			{
				return ( 2 * tile.x - 1 + fractionX - 0.5 ) / ( 2 * tile.y - ( fractionY - 0.5 ) );
			}
			else if ( cardinality == 3 )
			{
				return ( 2 * tile.x - 1 - ( fractionY - 0.5 ) ) / ( 2 * tile.y - fractionX + 0.5 );
			}
			else
			{
				return ( 2 * tile.x - 1 ) / ( 2 * tile.y );
			}
			*/

			if ( cardinality == 0 )
			{
				return ( 2 * tile.x - 1 + fractionX - 0.5 ) / ( 2 * tile.y - ( -fractionY - 0.5 ) );
			}
			else if ( cardinality == 1 )
			{
				return ( 2 * tile.x - 1 - ( fractionY + 0.5 ) ) / ( 2 * tile.y + fractionX - 0.5 );
			}
			else if ( cardinality == 2 )
			{
				return ( 2 * tile.x - 1 + fractionX - 0.5 ) / ( 2 * tile.y - ( fractionY + 0.5 ) );
			}
			else if ( cardinality == 3 )
			{
				return ( 2 * tile.x - 1 - ( fractionY + 0.5 ) ) / ( 2 * tile.y - fractionX + 0.5 );
			}
			else
			{
				return ( 2 * tile.x - 1 ) / ( 2 * tile.y );
			}
		}
		else // if ( originPosition.x < 0 && originPosition.y < 0 )
		{
			//std::cout << "topLeft" << std::endl;
			/*
			if ( cardinality == 0 )
			{
				return ( 2 * tile.x - 1 - fractionX + 0.5 ) / ( 2 * tile.y - fractionY + 0.5 );
			}
			else if ( cardinality == 1 )
			{
				return ( 2 * tile.x - 1 + fractionY - 0.5 ) / ( 2 * tile.y - fractionX + 0.5 );
			}
			else if ( cardinality == 2 )
			{
				return ( 2 * tile.x - 1 - fractionX + 0.5 ) / ( 2 * tile.y + fractionY - 0.5 );
			}
			else if ( cardinality == 3 )
			{
				return ( 2 * tile.x - 1 + fractionY - 0.5 ) / ( 2 * tile.y + fractionX - 0.5 );
			}
			else
			{
				return ( 2 * tile.x - 1 ) / ( 2 * tile.y );
			}
			*/


			if ( cardinality == 0 )
			{
				return ( 2 * tile.x - 1 - ( fractionX + 0.5 ) ) / ( 2 * tile.y - ( -fractionY - 0.5 ) );
			}
			else if ( cardinality == 1 )
			{
				return ( 2 * tile.x - 1 - ( fractionY + 0.5 ) ) / ( 2 * tile.y - ( fractionX + 0.5 ) );
			}
			else if ( cardinality == 2 )
			{
				return ( 2 * tile.x - 1 - ( fractionX + 0.5 ) ) / ( 2 * tile.y - ( fractionY + 0.5 ) );
			}
			else if ( cardinality == 3 )
			{
				return ( 2 * tile.x - 1 - ( fractionY + 0.5 ) ) / ( 2 * tile.y - ( -fractionX - 0.5 ) );
			}
			else
			{
				return ( 2 * tile.x - 1 ) / ( 2 * tile.y );
			}
			
		}

	}
	
	
	/*
	static float slopeDynamic( const olc::v2d_generic<long double>& tile, const olc::v2d_generic<long double> originPosition, const std::uint8_t cardinality )
	{
		// Tangent Slope for subcell positions
		long double dX = originPosition.x;
		long double dY = originPosition.y;

		long double fOriginX;
		long double fractionX = std::modfl( dX, &fOriginX );
		std::uint16_t originX = ( std::uint16_t )fOriginX;
		fOriginX -= fractionX;


		long double fOriginY;
		long double fractionY = std::modfl( dY, &fOriginY );
		std::uint16_t originY = ( std::uint16_t )fOriginY;
		fOriginY -= fractionY;


		if ( fractionY == 0 ) fractionY = 0.99;
		if ( fractionX == 0 ) fractionX = 0.99;


		long double tileCol;
		long double rowDepth;

		
		if ( cardinality == 0 )
		{
			return ( 2 * tile.x - 1 + fractionX - 0.5 ) / ( 2 * tile.y - fractionY + 0.5 );
		}
		else if ( cardinality == 1 )
		{
			return ( 2 * tile.x - 1 + fractionY - 0.5 ) / ( 2 * tile.y + fractionX - 0.5 );
		}
		else if ( cardinality == 2 )
		{
			return ( 2 * tile.x - 1 + fractionX - 0.5 ) / ( 2 * tile.y + fractionY - 0.5 );
		}
		else if ( cardinality == 3 )
		{
			return ( 2 * tile.x - 1 + fractionY - 0.5 ) / ( 2 * tile.y - fractionX + 0.5 );
		}
		else
		{
			return ( 2 * tile.x - 1 ) / ( 2 * tile.y );
		}
	}
	*/

	static bool isSymmetric( const LightCastRow& row, const olc::v2d_generic<long double>& tile )
	{
		long double tileCol = tile.x;
		long double rowDepth = tile.y;
		return tileCol >= row.depth * row.startSlope && tileCol <= row.depth * row.endSlope;
	}



};

