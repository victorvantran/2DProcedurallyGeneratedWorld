#pragma once

#include "Tile.h"

// https://4temperateshrublandschaparral.weebly.com/soil.html#:~:text=The%20soils'%20composition%20contains%20sand,arid%20regions%20of%20the%20biome.
// https://earthobservatory.nasa.gov/biome/bioshrubland.php
// https://sites.google.com/site/woodlandsandshrublandsbiome/soil

// // Cambisol (reddish soil)
// Gravel and sand, clay, silt
// Fertile soil, Tall grass


class Woodland
{
private:

public:
	static TileIdentity getSubstance( float heightPercentage, float substanceNormVal )
	{
		if ( 0.99f <= heightPercentage && heightPercentage <= 1.00f )
		{
			return TileIdentity::Dirt;
		}
		else if ( 0.90f <= heightPercentage && heightPercentage <= 0.99f )
		{
			if ( 0.70 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.69 <= substanceNormVal && substanceNormVal <= 0.70 )
			{
				return TileIdentity::Cambisol;
			}
			else if ( 0.65f <= substanceNormVal && substanceNormVal <= 0.69f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.45f <= substanceNormVal && substanceNormVal <= 0.65f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.35f <= substanceNormVal && substanceNormVal <= 0.45f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.30f <= substanceNormVal && substanceNormVal <= 0.35f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.275f <= substanceNormVal && substanceNormVal <= 0.30f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.15f <= substanceNormVal && substanceNormVal <= 0.275f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.125f <= substanceNormVal && substanceNormVal <= 0.15f )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.05f <= substanceNormVal && substanceNormVal <= 0.125f )
			{
				return TileIdentity::Cambisol;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.05f )
			{
				return TileIdentity::Stone;
			}
		}
		else if ( 0.65f <= heightPercentage && heightPercentage <= 0.90f )
		{
			// 
			// Gravel, sand, clay, silt
			if ( 0.975f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::Cambisol;
			}
			if ( 0.85f <= substanceNormVal && substanceNormVal <= 0.975f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.69 <= substanceNormVal && substanceNormVal <= 0.70f )
			{
				return TileIdentity::Cambisol;
			}
			else if ( 0.675f <= substanceNormVal && substanceNormVal <= 0.69f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.660f <= substanceNormVal && substanceNormVal <= 0.675f )
			{
				return TileIdentity::Silt;
			}
			else if ( 0.655f <= substanceNormVal && substanceNormVal <= 0.660f )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.65475f <= substanceNormVal && substanceNormVal <= 0.655f )
			{
				return TileIdentity::DarkClay;
			}
			else if ( 0.60f <= substanceNormVal && substanceNormVal <= 0.65475f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.59f <= substanceNormVal && substanceNormVal <= 0.60f )
			{
				return TileIdentity::BleachedSand;
			}
			else if ( 0.50f <= substanceNormVal && substanceNormVal <= 0.59f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.475f <= substanceNormVal && substanceNormVal <= 0.50f )
			{
				return TileIdentity::Cambisol;
			}
			else if ( 0.425f <= substanceNormVal && substanceNormVal <= 0.475f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.375f <= substanceNormVal && substanceNormVal <= 0.425f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.325f <= substanceNormVal && substanceNormVal <= 0.375f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.30f <= substanceNormVal && substanceNormVal <= 0.325f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.28f <= substanceNormVal && substanceNormVal <= 0.30f )
			{
				return TileIdentity::Silt;
			}
			else if ( 0.275f <= substanceNormVal && substanceNormVal <= 0.28f )
			{
				return TileIdentity::BleachedSand;
			}
			else if ( 0.255f <= substanceNormVal && substanceNormVal <= 0.275f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.2525f <= substanceNormVal && substanceNormVal <= 0.255f )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.20f <= substanceNormVal && substanceNormVal <= 0.2525f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.15f <= substanceNormVal && substanceNormVal <= 0.20f )
			{
				return TileIdentity::Cambisol;
			}
			else if ( 0.10f <= substanceNormVal && substanceNormVal <= 0.15f )
			{
				return TileIdentity::Stone;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.10f )
			{
				return TileIdentity::Silt;
			}
		}
		else if ( 0.425f <= heightPercentage && heightPercentage <= 0.65f )
		{
			if ( 0.95f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::Stone;
			}
			if ( 0.90f <= substanceNormVal && substanceNormVal <= 0.95f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.80 <= substanceNormVal && substanceNormVal <= 0.90f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.79f <= substanceNormVal && substanceNormVal <= 0.80f )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.789f <= substanceNormVal && substanceNormVal <= 0.79f )
			{
				return TileIdentity::DarkClay;
			}
			else if ( 0.675f <= substanceNormVal && substanceNormVal <= 0.789f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.670f <= substanceNormVal && substanceNormVal <= 0.675f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.625f <= substanceNormVal && substanceNormVal <= 0.670f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.575f <= substanceNormVal && substanceNormVal <= 0.625f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.425f <= substanceNormVal && substanceNormVal <= 0.575f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.405f <= substanceNormVal && substanceNormVal <= 0.425f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.25f <= substanceNormVal && substanceNormVal <= 0.405f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.225f <= substanceNormVal && substanceNormVal <= 0.25f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.075f <= substanceNormVal && substanceNormVal <= 0.225f )
			{
				return TileIdentity::Stone;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.075f )
			{
				return TileIdentity::Dirt;
			}
		}
		else
		{
			return TileIdentity::Stone;
		}
	}
};