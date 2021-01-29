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


class TemperateGrassland
{
private:

public:
	static TileIdentity getSubstance( float heightPercentage, float substanceNormVal )
	{
		if ( 0.98f <= heightPercentage && heightPercentage <= 1.00f )
		{
			if ( 0.60f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.55f <= substanceNormVal && substanceNormVal <= 0.60f )
			{
				return TileIdentity::Mull;
			}
			else if ( 0.45f <= substanceNormVal && substanceNormVal <= 0.55f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.40f <= substanceNormVal && substanceNormVal <= 0.45f )
			{
				return TileIdentity::Mull;
			}
			else if ( 0.39f <= substanceNormVal && substanceNormVal <= 0.40f )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.20f <= substanceNormVal && substanceNormVal <= 0.39f )
			{
				return TileIdentity::Dirt;
			}


			else if ( 0.175f <= substanceNormVal && substanceNormVal <= 0.20f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.10f <= substanceNormVal && substanceNormVal <= 0.175f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.075f <= substanceNormVal && substanceNormVal <= 0.10f )
			{
				return TileIdentity::Silt;
			}
			else if ( 0.025f <= substanceNormVal && substanceNormVal <= 0.075f )
			{
				return TileIdentity::Stone;
			}
			else // if ( 0.00f <= substanceNormVal && substanceNormVal <= 0.025f )
			{
				return TileIdentity::Gravel;
			}
		}
		else if ( 0.925f <= heightPercentage && heightPercentage <= 0.975f )
		{
			// 0.925 - 0.975 sand/silt/stone/dirt continues,	dirt, stone, gravel, sand, clay

			if ( 0.80f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.795f <= substanceNormVal && substanceNormVal <= 0.80f )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.65f <= substanceNormVal && substanceNormVal <= 0.80f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.635f <= substanceNormVal && substanceNormVal <= 0.65f )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.45f <= substanceNormVal && substanceNormVal <= 0.635f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.30f <= substanceNormVal && substanceNormVal <= 0.45f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.25f <= substanceNormVal && substanceNormVal <= 0.30f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.225f <= substanceNormVal && substanceNormVal <= 0.25f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.205f <= substanceNormVal && substanceNormVal <= 0.225f )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.20f <= substanceNormVal && substanceNormVal <= 0.205f )
			{
				return TileIdentity::DarkClay;
			}


			else if ( 0.125f <= substanceNormVal && substanceNormVal <= 0.20f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.075f <= substanceNormVal && substanceNormVal <= 0.125f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.005f <= substanceNormVal && substanceNormVal <= 0.075f )
			{
				return TileIdentity::Silt;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.005f )
			{
				return TileIdentity::Stone;
			}
		}
		else if ( 0.55f <= heightPercentage && heightPercentage <= 0.925f )
		{
			// 0.75 - 0.925 dirt, stone, charcoal, clay
			if ( 0.9f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.80f <= substanceNormVal && substanceNormVal <= 0.90f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.795f <= substanceNormVal && substanceNormVal <= 0.80f )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.75f <= substanceNormVal && substanceNormVal <= 0.795f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.70f <= substanceNormVal && substanceNormVal <= 0.75f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.68f <= substanceNormVal && substanceNormVal <= 0.70f )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.6775f <= substanceNormVal && substanceNormVal <= 0.68f )
			{
				return TileIdentity::DarkClay;
			}
			else if ( 0.65f <= substanceNormVal && substanceNormVal <= 0.6775f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.6475f <= substanceNormVal && substanceNormVal <= 0.65f )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.625f <= substanceNormVal && substanceNormVal <= 0.6475f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.60f <= substanceNormVal && substanceNormVal <= 0.625f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.50f <= substanceNormVal && substanceNormVal <= 0.60f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.40f <= substanceNormVal && substanceNormVal <= 0.50f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.39f <= substanceNormVal && substanceNormVal <= 0.40f )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.3875f <= substanceNormVal && substanceNormVal <= 0.39f )
			{
				return TileIdentity::DarkClay;
			}
			else if ( 0.30f <= substanceNormVal && substanceNormVal <= 0.3875f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.20f <= substanceNormVal && substanceNormVal <= 0.30f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.195f <= substanceNormVal && substanceNormVal <= 0.20f )
			{
				return TileIdentity::Charcoal;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.195f )
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