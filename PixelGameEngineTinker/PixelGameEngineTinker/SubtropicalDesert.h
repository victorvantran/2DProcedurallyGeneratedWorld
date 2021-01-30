#pragma once

#include "Tile.h"

// https://ucmp.berkeley.edu/exhibits/biomes/deserts.php
// https://www.soils4teachers.org/deserts
// https://sciencing.com/types-desert-soil-5548043.html

// Saltstone
// Aridisols ( dry soil )
// sand
// Entisols, yonug soil scattered about
// Sillica / Quartz
// Iron Oxide








class SubtropicalDesert
{
private:

public:
	static TileIdentity getSubstance( long double heightPercentage, long double substanceNormVal )
	{
		if ( 0.9875 <= heightPercentage && heightPercentage <= 1.00 )
		{
			return TileIdentity::Sand;
		}
		else if ( 0.90 <= heightPercentage && heightPercentage <= 0.9875 )
		{
			return ( ( std::int32_t )( heightPercentage * 100 ) % 2 == 0 ) ? TileIdentity::Aridsol : ( ( std::int32_t )( substanceNormVal * 100 ) % 15 ) ? TileIdentity::Entisol : TileIdentity::Sand;
		}
		else if ( 0.675 <= heightPercentage && heightPercentage <= 0.90 )
		{
			// Sand, gravel, aridsol, entisol, iron oxide
			if ( 0.95 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.90 <= substanceNormVal && substanceNormVal <= 0.95 )
			{
				return TileIdentity::Aridsol;
			}
			else if ( 0.875 <= substanceNormVal && substanceNormVal <= 0.90 )
			{
				return ( ( std::int32_t )( substanceNormVal * 100 ) % 17 ) ? TileIdentity::Entisol : TileIdentity::Sand;
			}
			else if ( 0.850 <= substanceNormVal && substanceNormVal <= 0.875 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.825 <= substanceNormVal && substanceNormVal <= 0.850 )
			{
				return TileIdentity::Aridsol;
			}
			else if ( 0.820 <= substanceNormVal && substanceNormVal <= 0.825 )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.80 <= substanceNormVal && substanceNormVal <= 0.820 )
			{
				return ( ( std::int32_t )( substanceNormVal * 100 ) % 17 ) ? TileIdentity::Entisol : TileIdentity::Sand;
			}
			else if ( 0.70 <= substanceNormVal && substanceNormVal <= 0.80 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.69 <= substanceNormVal && substanceNormVal <= 0.70 )
			{
				return TileIdentity::IronOxide;
			}
			else if ( 0.60 <= substanceNormVal && substanceNormVal <= 0.69 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.50 <= substanceNormVal && substanceNormVal <= 0.60 )
			{
				return TileIdentity::Aridsol;
			}
			else if ( 0.40 <= substanceNormVal && substanceNormVal <= 0.50 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.39 <= substanceNormVal && substanceNormVal <= 0.40 )
			{
				return TileIdentity::Aridsol;
			}
			else if ( 0.38 <= substanceNormVal && substanceNormVal <= 0.39 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.375 <= substanceNormVal && substanceNormVal <= 0.38 )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.25 <= substanceNormVal && substanceNormVal <= 0.375 )
			{
				return TileIdentity::Aridsol;
			}
			else if ( 0.10 <= substanceNormVal && substanceNormVal <= 0.25 )
			{
				return TileIdentity::Sand;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.10 )
			{
				return TileIdentity::Saltstone;
			}
		}
		else if ( 0.60 <= heightPercentage && heightPercentage <= 0.675 )
		{
			// Sand, aridsol, saltstone, gravel, quartz
			if ( 0.90 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.875 <= substanceNormVal && substanceNormVal <= 0.90 )
			{
				return TileIdentity::Aridsol;
			}
			else if ( 0.75 <= substanceNormVal && substanceNormVal <= 0.875 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.60 <= substanceNormVal && substanceNormVal <= 0.75 )
			{
				return TileIdentity::Saltstone;
			}
			else if ( 0.50 <= substanceNormVal && substanceNormVal <= 0.60 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.475 <= substanceNormVal && substanceNormVal <= 0.50 )
			{
				return TileIdentity::Aridsol;
			}
			else if ( 0.40 <= substanceNormVal && substanceNormVal <= 0.475 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.25 <= substanceNormVal && substanceNormVal <= 0.40 )
			{
				return TileIdentity::Saltstone;
			}
			else if ( 0.2485 <= substanceNormVal && substanceNormVal <= 0.25 )
			{
				return TileIdentity::Quartz;
			}
			else if ( 0.20 <= substanceNormVal && substanceNormVal <= 0.2485 )
			{
				return TileIdentity::Saltstone;
			}
			else if ( 0.15 <= substanceNormVal && substanceNormVal <= 0.20 )
			{
				return TileIdentity::Sand;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.15 )
			{
				return TileIdentity::Saltstone;
			}
		}
		else if ( 0.475 <= heightPercentage && heightPercentage <= 0.60 )
		{
			// Sand, aridsol, saltstone, stone
			if ( 0.95 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.825 <= substanceNormVal && substanceNormVal <= 0.95 )
			{
				return TileIdentity::Saltstone;
			}
			else if ( 0.820 <= substanceNormVal && substanceNormVal <= 0.825 )
			{
				return TileIdentity::Aridsol;
			}
			else if ( 0.70 <= substanceNormVal && substanceNormVal <= 0.820 )
			{
				return TileIdentity::Saltstone;
			}
			else if ( 0.65 <= substanceNormVal && substanceNormVal <= 0.70 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.475 <= substanceNormVal && substanceNormVal <= 0.65 )
			{
				return TileIdentity::Saltstone;
			}
			else if ( 0.425 <= substanceNormVal && substanceNormVal <= 0.475 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.40 <= substanceNormVal && substanceNormVal <= 0.425 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.375 <= substanceNormVal && substanceNormVal <= 0.40 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.15 <= substanceNormVal && substanceNormVal <= 0.375 )
			{
				return TileIdentity::Saltstone;
			}
			else if ( 0.145 <= substanceNormVal && substanceNormVal <= 0.15 )
			{
				return TileIdentity::Aridsol;
			}
			else if ( 0.10 <= substanceNormVal && substanceNormVal <= 0.145 )
			{
				return TileIdentity::Stone;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.10 )
			{
				return TileIdentity::Sand;
			}
		}
		else if ( 0.40 <= heightPercentage && heightPercentage <= 0.475 )
		{
			// Sand, saltstone, stone
			if ( 0.85 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Saltstone;
			}
			else if ( 0.8475 <= substanceNormVal && substanceNormVal <= 0.85 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.80 <= substanceNormVal && substanceNormVal <= 0.8475 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.50 <= substanceNormVal && substanceNormVal <= 0.80 )
			{
				return TileIdentity::Saltstone;
			}
			else if ( 0.499 <= substanceNormVal && substanceNormVal <= 0.50 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.40 <= substanceNormVal && substanceNormVal <= 0.499 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.25 <= substanceNormVal && substanceNormVal <= 0.40 )
			{
				return TileIdentity::Saltstone;
			}
			else //  if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.25 )
			{
				return TileIdentity::Stone;
			}
		}
		else
		{
			return TileIdentity::Stone;
		}
	}
};