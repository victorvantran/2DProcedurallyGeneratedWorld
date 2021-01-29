#pragma once

#include "Tile.h"

// https://8kmdbsavannastropicalgrasslands.weebly.com/soil.html
// https://www.britannica.com/science/savanna/Environment
// https://www.blueplanetbiomes.org/savanna.php#:~:text=A%20savanna%20is%20a%20rolling,also%20known%20as%20tropical%20grasslands.&text=Savannas%20have%20warm%20temperature%20year%20round.
// https://ucmp.berkeley.edu/exhibits/biomes/grasslands.php


// TropicalSeasonalForest/Savanna 
// Low soil fertility
// Thin layer of humus ( Mor )
// Red Clay/Laterite
// Stone
// Sand


class TropicalSeasonalForest
{
private:

public:
	static TileIdentity getSubstance( float heightPercentage, float substanceNormVal )
	{

		if ( 0.99f <= heightPercentage && heightPercentage <= 1.00f )
		{
			return TileIdentity::Dirt;
		}
		else if ( 0.989f <= heightPercentage && heightPercentage <= 0.99f )
		{
			if ( 0.92 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.90 <= substanceNormVal && substanceNormVal <= 0.92 )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.875f <= substanceNormVal && substanceNormVal <= 0.90f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.870f <= substanceNormVal && substanceNormVal <= 0.875f )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.869f <= substanceNormVal && substanceNormVal <= 0.870f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.860f <= substanceNormVal && substanceNormVal <= 0.869f )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.775f <= substanceNormVal && substanceNormVal <= 0.860f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.770f <= substanceNormVal && substanceNormVal <= 0.775f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.70f <= substanceNormVal && substanceNormVal <= 0.770f )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.6875f <= substanceNormVal && substanceNormVal <= 0.70f )
			{
				return TileIdentity::RedClay;
			}
			else if ( 0.650f <= substanceNormVal && substanceNormVal <= 0.6875f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.575f <= substanceNormVal && substanceNormVal <= 0.650f )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.45f <= substanceNormVal && substanceNormVal <= 0.575f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.448f <= substanceNormVal && substanceNormVal <= 0.45f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.375f <= substanceNormVal && substanceNormVal <= 0.448f )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.3749f <= substanceNormVal && substanceNormVal <= 0.375f )
			{
				return TileIdentity::RedClay;
			}
			else if ( 0.25f <= substanceNormVal && substanceNormVal <= 0.3749f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.15f <= substanceNormVal && substanceNormVal <= 0.25f )
			{
				return TileIdentity::Mor;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.15f )
			{
				return TileIdentity::Dirt;
			}
		}
		else if ( 0.85f <= heightPercentage && heightPercentage <= 0.989f )
		{
			if ( 0.855 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.840f <= substanceNormVal && substanceNormVal <= 0.855f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.70f <= substanceNormVal && substanceNormVal <= 0.840f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.6875f <= substanceNormVal && substanceNormVal <= 0.70f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.68f <= substanceNormVal && substanceNormVal <= 0.6875f )
			{
				return TileIdentity::RedClay;
			}
			else if ( 0.6795f <= substanceNormVal && substanceNormVal <= 0.68f )
			{
				return TileIdentity::Laterite;
			}
			else if ( 0.645f <= substanceNormVal && substanceNormVal <= 0.6795 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.425f <= substanceNormVal && substanceNormVal <= 0.645f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.375f <= substanceNormVal && substanceNormVal <= 0.425f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.37475f <= substanceNormVal && substanceNormVal <= 0.375f )
			{
				return TileIdentity::RedClay;
			}
			else if ( 0.10f <= substanceNormVal && substanceNormVal <= 0.37475f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.05f <= substanceNormVal && substanceNormVal <= 0.10f )
			{
				return TileIdentity::Stone;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.05f )
			{
				return TileIdentity::Dirt;
			}
		}
		else if ( 0.50f <= heightPercentage && heightPercentage <= 0.85f )
		{
			if ( 0.775 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.68f <= substanceNormVal && substanceNormVal <= 0.775f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.67f <= substanceNormVal && substanceNormVal <= 0.68f )
			{
				return TileIdentity::BleachedSand;
			}
			else if ( 0.65f <= substanceNormVal && substanceNormVal <= 0.67f )
			{
				return TileIdentity::Laterite;
			}
			else if ( 0.60f <= substanceNormVal && substanceNormVal <= 0.65f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.59f <= substanceNormVal && substanceNormVal <= 0.60f )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.45f <= substanceNormVal && substanceNormVal <= 0.59f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.425f <= substanceNormVal && substanceNormVal <= 0.45f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.35f <= substanceNormVal && substanceNormVal <= 0.425f )
			{
				return TileIdentity::Laterite;
			}
			else if ( 0.25f <= substanceNormVal && substanceNormVal <= 0.35f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.10f <= substanceNormVal && substanceNormVal <= 0.25f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.05f <= substanceNormVal && substanceNormVal <= 0.10f )
			{
				return TileIdentity::Dirt;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.05f )
			{
				return TileIdentity::Stone;
			}
		}
		else if ( 0.40f <= heightPercentage && heightPercentage <= 0.50f )
		{
			if ( 0.975f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.90f <= substanceNormVal && substanceNormVal <= 0.975f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.855f <= substanceNormVal && substanceNormVal <= 0.90f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.725f <= substanceNormVal && substanceNormVal <= 0.855f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.70f <= substanceNormVal && substanceNormVal <= 0.725f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.66f <= substanceNormVal && substanceNormVal <= 0.70f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.655f <= substanceNormVal && substanceNormVal <= 0.66f )
			{
				return TileIdentity::Laterite;
			}
			else if ( 0.475f <= substanceNormVal && substanceNormVal <= 0.655f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.445f <= substanceNormVal && substanceNormVal <= 0.475f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.40f <= substanceNormVal && substanceNormVal <= 0.445f )
			{
				return TileIdentity::Laterite;
			}
			else if ( 0.28f <= substanceNormVal && substanceNormVal <= 0.40f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.25f <= substanceNormVal && substanceNormVal <= 0.28f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.06f <= substanceNormVal && substanceNormVal <= 0.25f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.05f <= substanceNormVal && substanceNormVal <= 0.06f )
			{
				return TileIdentity::Dirt;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.05f )
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