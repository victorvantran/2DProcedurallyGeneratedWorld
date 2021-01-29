#pragma once

#include "Tile.h"

// https://ucmp.berkeley.edu/exhibits/biomes/tundra.php

// Soil, permafrost, talik

class Tundra
{
private:

public:
	static TileIdentity getSubstance( float heightPercentage, float substanceNormVal )
	{
		if ( 0.98f <= heightPercentage && heightPercentage <= 1.00f )
		{
			if ( 0.75f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.50f <= substanceNormVal && substanceNormVal <= 0.75f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.49f <= substanceNormVal && substanceNormVal <= 0.50f )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.30f <= substanceNormVal && substanceNormVal <= 0.49f )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.25f <= substanceNormVal && substanceNormVal <= 0.30f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.245f <= substanceNormVal && substanceNormVal <= 0.25f )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.20f <= substanceNormVal && substanceNormVal <= 0.245f )
			{
				return TileIdentity::Dirt;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.20f )
			{
				return TileIdentity::Snow;
			}
		}
		else if ( 0.90f <= heightPercentage && heightPercentage <= 0.98f )
		{
			if ( 0.80f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.60f <= substanceNormVal && substanceNormVal <= 0.80f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.58f <= substanceNormVal && substanceNormVal <= 0.60f )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.45f <= substanceNormVal && substanceNormVal <= 0.58f )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.40f <= substanceNormVal && substanceNormVal <= 0.45f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.35f <= substanceNormVal && substanceNormVal <= 0.40f )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.30f <= substanceNormVal && substanceNormVal <= 0.35f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.275f <= substanceNormVal && substanceNormVal <= 0.30f )
			{
				return TileIdentity::Permafrost;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.275f )
			{
				return TileIdentity::Snow;
			}
		}
		else if ( 0.60f <= heightPercentage && heightPercentage <= 0.90f )
		{
			// 0.60 -> 0.20 permafrost, talik( dirt, mor, stone, light clay, shale ), snow

			if ( 0.95f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.90f <= substanceNormVal && substanceNormVal <= 0.95f )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.895f <= substanceNormVal && substanceNormVal <= 0.90f )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.875f <= substanceNormVal && substanceNormVal <= 0.895f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.825f <= substanceNormVal && substanceNormVal <= 0.875f )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.775f <= substanceNormVal && substanceNormVal <= 0.825f )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.70f <= substanceNormVal && substanceNormVal <= 0.775f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.675f <= substanceNormVal && substanceNormVal <= 0.70f )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.50f <= substanceNormVal && substanceNormVal <= 0.675f )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.495f <= substanceNormVal && substanceNormVal <= 0.50f )
			{
				return TileIdentity::Shale;
			}
			else if ( 0.475f <= substanceNormVal && substanceNormVal <= 0.495f )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.30f <= substanceNormVal && substanceNormVal <= 0.475f )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.25f <= substanceNormVal && substanceNormVal <= 0.30f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.245f <= substanceNormVal && substanceNormVal <= 0.25f )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.205f <= substanceNormVal && substanceNormVal <= 0.245f )
			{
				return TileIdentity::Snow;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.215f )
			{
				return TileIdentity::Permafrost;
			}
		}
		else // if ( 0.0f <= heightPercentage && heightPercentage <= 0.90f )
		{
			// 0.20 -> 0.60 talik ( dirt, mor, gravel, stone, charcoal, light clay, dark clay, shale -> Uranium Ore ) perma frost, snow, stone

			if ( 0.98f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.95f <= substanceNormVal && substanceNormVal <= 0.98f )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.90f <= substanceNormVal && substanceNormVal <= 0.95f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.89f <= substanceNormVal && substanceNormVal <= 0.90f )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.80f <= substanceNormVal && substanceNormVal <= 0.89f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.775f <= substanceNormVal && substanceNormVal <= 0.80f )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.75f <= substanceNormVal && substanceNormVal <= 0.775f )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.55f <= substanceNormVal && substanceNormVal <= 0.75f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.545f <= substanceNormVal && substanceNormVal <= 0.55f )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.445f <= substanceNormVal && substanceNormVal <= 0.545f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.425f <= substanceNormVal && substanceNormVal <= 0.445f )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.40f <= substanceNormVal && substanceNormVal <= 0.425f )
			{
				return TileIdentity::Shale;
			}
			else if ( 0.39f <= substanceNormVal && substanceNormVal <= 0.40f )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.35f <= substanceNormVal && substanceNormVal <= 0.39f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.345f <= substanceNormVal && substanceNormVal <= 0.35f )
			{
				return TileIdentity::Snow;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.275f )
			{
				return TileIdentity::Stone;
			}
		}
	}
};