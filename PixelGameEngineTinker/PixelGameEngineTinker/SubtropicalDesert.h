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
	static TileIdentity getSubstance( float heightPercentage, float substanceNormVal )
	{
		if ( 0.9875f <= heightPercentage && heightPercentage <= 1.00f )
		{
			return TileIdentity::Sand;
		}
		else if ( 0.90f <= heightPercentage && heightPercentage <= 0.9875f )
		{
			return ( ( int )( heightPercentage * 100 ) % 2 == 0 ) ? TileIdentity::Aridsol : ( ( int )( substanceNormVal * 100 ) % 15 ) ? TileIdentity::Entisol : TileIdentity::Sand;
		}
		else if ( 0.675f <= heightPercentage && heightPercentage <= 0.90f )
		{
			// Sand, gravel, aridsol, entisol, iron oxide
			if ( 0.95f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.90f <= substanceNormVal && substanceNormVal <= 0.95f )
			{
				return TileIdentity::Aridsol;
			}
			else if ( 0.875f <= substanceNormVal && substanceNormVal <= 0.90f )
			{
				return ( ( int )( substanceNormVal * 100 ) % 17 ) ? TileIdentity::Entisol : TileIdentity::Sand;
			}
			else if ( 0.850f <= substanceNormVal && substanceNormVal <= 0.875f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.825f <= substanceNormVal && substanceNormVal <= 0.850f )
			{
				return TileIdentity::Aridsol;
			}
			else if ( 0.820f <= substanceNormVal && substanceNormVal <= 0.825f )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.80f <= substanceNormVal && substanceNormVal <= 0.820f )
			{
				return ( ( int )( substanceNormVal * 100 ) % 17 ) ? TileIdentity::Entisol : TileIdentity::Sand;
			}
			else if ( 0.70f <= substanceNormVal && substanceNormVal <= 0.80f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.69f <= substanceNormVal && substanceNormVal <= 0.70f )
			{
				return TileIdentity::IronOxide;
			}
			else if ( 0.60f <= substanceNormVal && substanceNormVal <= 0.69f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.50f <= substanceNormVal && substanceNormVal <= 0.60f )
			{
				return TileIdentity::Aridsol;
			}
			else if ( 0.40f <= substanceNormVal && substanceNormVal <= 0.50f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.39f <= substanceNormVal && substanceNormVal <= 0.40f )
			{
				return TileIdentity::Aridsol;
			}
			else if ( 0.38f <= substanceNormVal && substanceNormVal <= 0.39f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.375f <= substanceNormVal && substanceNormVal <= 0.38f )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.25f <= substanceNormVal && substanceNormVal <= 0.375f )
			{
				return TileIdentity::Aridsol;
			}
			else if ( 0.10f <= substanceNormVal && substanceNormVal <= 0.25f )
			{
				return TileIdentity::Sand;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.10f )
			{
				return TileIdentity::Saltstone;
			}
		}
		else if ( 0.60f <= heightPercentage && heightPercentage <= 0.675f )
		{
			// Sand, aridsol, saltstone, gravel, quartz
			if ( 0.90f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.875f <= substanceNormVal && substanceNormVal <= 0.90f )
			{
				return TileIdentity::Aridsol;
			}
			else if ( 0.75f <= substanceNormVal && substanceNormVal <= 0.875f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.60f <= substanceNormVal && substanceNormVal <= 0.75f )
			{
				return TileIdentity::Saltstone;
			}
			else if ( 0.50f <= substanceNormVal && substanceNormVal <= 0.60f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.475f <= substanceNormVal && substanceNormVal <= 0.50f )
			{
				return TileIdentity::Aridsol;
			}
			else if ( 0.40f <= substanceNormVal && substanceNormVal <= 0.475f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.25f <= substanceNormVal && substanceNormVal <= 0.40f )
			{
				return TileIdentity::Saltstone;
			}
			else if ( 0.2485f <= substanceNormVal && substanceNormVal <= 0.25f )
			{
				return TileIdentity::Quartz;
			}
			else if ( 0.20f <= substanceNormVal && substanceNormVal <= 0.2485f )
			{
				return TileIdentity::Saltstone;
			}
			else if ( 0.15f <= substanceNormVal && substanceNormVal <= 0.20f )
			{
				return TileIdentity::Sand;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.15f )
			{
				return TileIdentity::Saltstone;
			}
		}
		else if ( 0.475f <= heightPercentage && heightPercentage <= 0.60f )
		{
			// Sand, aridsol, saltstone, stone
			if ( 0.95f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.825f <= substanceNormVal && substanceNormVal <= 0.95f )
			{
				return TileIdentity::Saltstone;
			}
			else if ( 0.820f <= substanceNormVal && substanceNormVal <= 0.825f )
			{
				return TileIdentity::Aridsol;
			}
			else if ( 0.70f <= substanceNormVal && substanceNormVal <= 0.820f )
			{
				return TileIdentity::Saltstone;
			}
			else if ( 0.65f <= substanceNormVal && substanceNormVal <= 0.70f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.475f <= substanceNormVal && substanceNormVal <= 0.65f )
			{
				return TileIdentity::Saltstone;
			}
			else if ( 0.425f <= substanceNormVal && substanceNormVal <= 0.475f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.40f <= substanceNormVal && substanceNormVal <= 0.425f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.375f <= substanceNormVal && substanceNormVal <= 0.40f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.15f <= substanceNormVal && substanceNormVal <= 0.375f )
			{
				return TileIdentity::Saltstone;
			}
			else if ( 0.145f <= substanceNormVal && substanceNormVal <= 0.15f )
			{
				return TileIdentity::Aridsol;
			}
			else if ( 0.10f <= substanceNormVal && substanceNormVal <= 0.145f )
			{
				return TileIdentity::Stone;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.10f )
			{
				return TileIdentity::Sand;
			}
		}
		else if ( 0.40f <= heightPercentage && heightPercentage <= 0.475f )
		{
			// Sand, saltstone, stone
			if ( 0.85f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::Saltstone;
			}
			else if ( 0.8475f <= substanceNormVal && substanceNormVal <= 0.85f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.80f <= substanceNormVal && substanceNormVal <= 0.8475f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.50f <= substanceNormVal && substanceNormVal <= 0.80f )
			{
				return TileIdentity::Saltstone;
			}
			else if ( 0.499f <= substanceNormVal && substanceNormVal <= 0.50f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.40f <= substanceNormVal && substanceNormVal <= 0.499f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.25f <= substanceNormVal && substanceNormVal <= 0.40f )
			{
				return TileIdentity::Saltstone;
			}
			else //  if ( 0.40f <= substanceNormVal && substanceNormVal <= 0.499f )
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