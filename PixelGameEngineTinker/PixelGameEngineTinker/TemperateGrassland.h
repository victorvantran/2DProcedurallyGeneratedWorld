#pragma once

#include "Tile.h"

// https://ucmp.berkeley.edu/exhibits/biomes/grasslands.php
// https://ucmp.berkeley.edu/exhibits/biomes/deserts.php


// Deep, nutrient enriched soil
// Mull abundance
// Cottonwood, oak, willow
// Flowers: //
// Grass: Tall, purple needle grass, blue grama, buffalo grass, galleta


// Cold Desert
// Sand and silty soil
// Grass: Short steppes


// 0.975 - 1.00 Big areas of sand/silt/stone/dirt,	 and big areas of dirt, mull, gravel						0.20/0.80 split
// 0.925 - 0.975 sand/silt/stone/dirt continues,	dirt, stone, gravel, sand, clay
// 0.55    - 0.925 dirt, stone, charcoal, clay, gravel 

// [!] salt

class TemperateGrassland
{
private:

public:
	static TileIdentity getSubstance( long double heightPercentage, long double substanceNormVal )
	{
		if ( 0.98 <= heightPercentage && heightPercentage <= 1.00 )
		{
			if ( 0.60 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.55 <= substanceNormVal && substanceNormVal <= 0.60 )
			{
				return TileIdentity::Mull;
			}
			else if ( 0.45 <= substanceNormVal && substanceNormVal <= 0.55 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.40 <= substanceNormVal && substanceNormVal <= 0.45 )
			{
				return TileIdentity::Mull;
			}
			else if ( 0.39 <= substanceNormVal && substanceNormVal <= 0.40 )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.20 <= substanceNormVal && substanceNormVal <= 0.39 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.175 <= substanceNormVal && substanceNormVal <= 0.20 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.10 <= substanceNormVal && substanceNormVal <= 0.175 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.075 <= substanceNormVal && substanceNormVal <= 0.10 )
			{
				return TileIdentity::Silt;
			}
			else if ( 0.025 <= substanceNormVal && substanceNormVal <= 0.075 )
			{
				return TileIdentity::Stone;
			}
			else // if ( 0.00f <= substanceNormVal && substanceNormVal <= 0.025 )
			{
				return TileIdentity::Gravel;
			}
		}
		else if ( 0.925 <= heightPercentage && heightPercentage <= 0.975 )
		{
			// 0.925 - 0.975 sand/silt/stone/dirt continues,	dirt, stone, gravel, sand, clay

			if ( 0.80 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.795 <= substanceNormVal && substanceNormVal <= 0.80 )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.65 <= substanceNormVal && substanceNormVal <= 0.795 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.635 <= substanceNormVal && substanceNormVal <= 0.65 )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.45 <= substanceNormVal && substanceNormVal <= 0.635 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.30 <= substanceNormVal && substanceNormVal <= 0.45 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.25 <= substanceNormVal && substanceNormVal <= 0.30 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.225 <= substanceNormVal && substanceNormVal <= 0.25 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.205 <= substanceNormVal && substanceNormVal <= 0.225 )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.20 <= substanceNormVal && substanceNormVal <= 0.205 )
			{
				return TileIdentity::DarkClay;
			}
			else if ( 0.125 <= substanceNormVal && substanceNormVal <= 0.20 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.075 <= substanceNormVal && substanceNormVal <= 0.125 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.005 <= substanceNormVal && substanceNormVal <= 0.075 )
			{
				return TileIdentity::Silt;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.005 )
			{
				return TileIdentity::Stone;
			}
		}
		else if ( 0.55 <= heightPercentage && heightPercentage <= 0.925 )
		{
			// 0.75 - 0.925 dirt, stone, charcoal, clay
			if ( 0.9 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.80 <= substanceNormVal && substanceNormVal <= 0.90 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.795 <= substanceNormVal && substanceNormVal <= 0.80 )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.75 <= substanceNormVal && substanceNormVal <= 0.795 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.70 <= substanceNormVal && substanceNormVal <= 0.75 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.68 <= substanceNormVal && substanceNormVal <= 0.70 )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.6775 <= substanceNormVal && substanceNormVal <= 0.68 )
			{
				return TileIdentity::DarkClay;
			}
			else if ( 0.65 <= substanceNormVal && substanceNormVal <= 0.6775 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.6475 <= substanceNormVal && substanceNormVal <= 0.65 )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.625 <= substanceNormVal && substanceNormVal <= 0.6475 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.60 <= substanceNormVal && substanceNormVal <= 0.625 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.50 <= substanceNormVal && substanceNormVal <= 0.60 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.40 <= substanceNormVal && substanceNormVal <= 0.50 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.39 <= substanceNormVal && substanceNormVal <= 0.40 )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.3875 <= substanceNormVal && substanceNormVal <= 0.39 )
			{
				return TileIdentity::DarkClay;
			}
			else if ( 0.30 <= substanceNormVal && substanceNormVal <= 0.3875 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.20 <= substanceNormVal && substanceNormVal <= 0.30 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.195 <= substanceNormVal && substanceNormVal <= 0.20 )
			{
				return TileIdentity::Charcoal;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.195 )
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