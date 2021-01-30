#pragma once

#include "Tile.h"

// https://4temperateshrublandschaparral.weebly.com/soil.html#:~:text=The%20soils'%20composition%20contains%20sand,arid%20regions%20of%20the%20biome.
// https://earthobservatory.nasa.gov/biome/bioshrubland.php
// https://sites.google.com/site/woodlandsandshrublandsbiome/soil

// // Cambisol (reddish soil)
// Gravel and sand, clay, silt


// Fertile soil, Tall grass
// 0.425

class Woodland
{
private:

public:
	static TileIdentity getSubstance( long double heightPercentage, long double substanceNormVal )
	{
		if ( 0.99 <= heightPercentage && heightPercentage <= 1.00 )
		{
			return TileIdentity::Dirt;
		}
		else if ( 0.90 <= heightPercentage && heightPercentage <= 0.99 )
		{
			if ( 0.70 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.69 <= substanceNormVal && substanceNormVal <= 0.70 )
			{
				return TileIdentity::Cambisol;
			}
			else if ( 0.65 <= substanceNormVal && substanceNormVal <= 0.69 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.45 <= substanceNormVal && substanceNormVal <= 0.65 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.35 <= substanceNormVal && substanceNormVal <= 0.45 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.30 <= substanceNormVal && substanceNormVal <= 0.35 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.275 <= substanceNormVal && substanceNormVal <= 0.30 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.15 <= substanceNormVal && substanceNormVal <= 0.275 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.125 <= substanceNormVal && substanceNormVal <= 0.15 )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.05 <= substanceNormVal && substanceNormVal <= 0.125 )
			{
				return TileIdentity::Cambisol;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.05 )
			{
				return TileIdentity::Stone;
			}
		}
		else if ( 0.65 <= heightPercentage && heightPercentage <= 0.90 )
		{
			// 
			// Gravel, sand, clay, silt
			if ( 0.975 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Cambisol;
			}
			if ( 0.85 <= substanceNormVal && substanceNormVal <= 0.975 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.84 <= substanceNormVal && substanceNormVal <= 0.85 )
			{
				return TileIdentity::BleachedSand;
			}
			else if ( 0.825 <= substanceNormVal && substanceNormVal <= 0.84 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.80 <= substanceNormVal && substanceNormVal <= 0.825 )
			{
				return TileIdentity::Cambisol;
			}
			if ( 0.70 <= substanceNormVal && substanceNormVal <= 0.80 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.69 <= substanceNormVal && substanceNormVal <= 0.70 )
			{
				return TileIdentity::Cambisol;
			}
			else if ( 0.675 <= substanceNormVal && substanceNormVal <= 0.69 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.660 <= substanceNormVal && substanceNormVal <= 0.675 )
			{
				return TileIdentity::Silt;
			}
			else if ( 0.655 <= substanceNormVal && substanceNormVal <= 0.660 )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.65475 <= substanceNormVal && substanceNormVal <= 0.655 )
			{
				return TileIdentity::DarkClay;
			}
			else if ( 0.60 <= substanceNormVal && substanceNormVal <= 0.65475 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.59 <= substanceNormVal && substanceNormVal <= 0.60 )
			{
				return TileIdentity::BleachedSand;
			}
			else if ( 0.50 <= substanceNormVal && substanceNormVal <= 0.59 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.475 <= substanceNormVal && substanceNormVal <= 0.50 )
			{
				return TileIdentity::Cambisol;
			}
			else if ( 0.425 <= substanceNormVal && substanceNormVal <= 0.475 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.375 <= substanceNormVal && substanceNormVal <= 0.425 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.325 <= substanceNormVal && substanceNormVal <= 0.375 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.30 <= substanceNormVal && substanceNormVal <= 0.325 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.28 <= substanceNormVal && substanceNormVal <= 0.30 )
			{
				return TileIdentity::Silt;
			}
			else if ( 0.275 <= substanceNormVal && substanceNormVal <= 0.28 )
			{
				return TileIdentity::BleachedSand;
			}
			else if ( 0.255 <= substanceNormVal && substanceNormVal <= 0.275 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.2525 <= substanceNormVal && substanceNormVal <= 0.255 )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.20 <= substanceNormVal && substanceNormVal <= 0.2525 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.15 <= substanceNormVal && substanceNormVal <= 0.20 )
			{
				return TileIdentity::Cambisol;
			}
			else if ( 0.10 <= substanceNormVal && substanceNormVal <= 0.15 )
			{
				return TileIdentity::Stone;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.10 )
			{
				return TileIdentity::Silt;
			}
		}
		else if ( 0.425 <= heightPercentage && heightPercentage <= 0.65 )
		{
			if ( 0.95 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Stone;
			}
			if ( 0.90 <= substanceNormVal && substanceNormVal <= 0.95 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.80 <= substanceNormVal && substanceNormVal <= 0.90 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.79 <= substanceNormVal && substanceNormVal <= 0.80 )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.789 <= substanceNormVal && substanceNormVal <= 0.79 )
			{
				return TileIdentity::DarkClay;
			}
			else if ( 0.675 <= substanceNormVal && substanceNormVal <= 0.789 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.670 <= substanceNormVal && substanceNormVal <= 0.675 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.625 <= substanceNormVal && substanceNormVal <= 0.670 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.575 <= substanceNormVal && substanceNormVal <= 0.625 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.425 <= substanceNormVal && substanceNormVal <= 0.575 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.405 <= substanceNormVal && substanceNormVal <= 0.425 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.25 <= substanceNormVal && substanceNormVal <= 0.405 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.225 <= substanceNormVal && substanceNormVal <= 0.25 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.075 <= substanceNormVal && substanceNormVal <= 0.225 )
			{
				return TileIdentity::Stone;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.075 )
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