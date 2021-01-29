#pragma once

#include "Tile.h"

// Permafrost ( thick )
// Taiga
// Little plant variety, acidic soil
// Snow
// Water

class BorealForest
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
			else if ( 0.65f <= substanceNormVal && substanceNormVal <= 0.75f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.45f <= substanceNormVal && substanceNormVal <= 0.65f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.40f <= substanceNormVal && substanceNormVal <= 0.45f )
			{
				return TileIdentity::Podzol;
			}
			else if ( 0.40f <= substanceNormVal && substanceNormVal <= 0.45f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.35f <= substanceNormVal && substanceNormVal <= 0.40f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.30f <= substanceNormVal && substanceNormVal <= 0.35f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.29f <= substanceNormVal && substanceNormVal <= 0.30f )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.20f <= substanceNormVal && substanceNormVal <= 0.34f )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.10f <= substanceNormVal && substanceNormVal <= 0.20f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.075f <= substanceNormVal && substanceNormVal <= 0.10f )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.05f <= substanceNormVal && substanceNormVal <= 0.075f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.05f <= substanceNormVal && substanceNormVal <= 0.10f )
			{
				return TileIdentity::Snow;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.05 )
			{
				return TileIdentity::Gravel;
			}
		}
		else if ( 0.9f <= heightPercentage && heightPercentage <= 0.98f )
		{
			if ( 0.80f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.65f <= substanceNormVal && substanceNormVal <= 0.80f )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.60f <= substanceNormVal && substanceNormVal <= 0.65f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.50f <= substanceNormVal && substanceNormVal <= 0.60f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.40f <= substanceNormVal && substanceNormVal <= 0.50f )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.30f <= substanceNormVal && substanceNormVal <= 0.40f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.28f <= substanceNormVal && substanceNormVal <= 0.30f )
			{
				return TileIdentity::Podzol;
			}
			else if ( 0.11f <= substanceNormVal && substanceNormVal <= 0.28f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.09f <= substanceNormVal && substanceNormVal <= 0.11f )
			{
				return TileIdentity::Podzol;
			}
			else //  if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.10f )
			{
				return TileIdentity::Snow;
			}
		}
		else if ( 0.6f <= heightPercentage && heightPercentage <= 0.9f )
		{
			if ( 0.80f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.65f <= substanceNormVal && substanceNormVal <= 0.80f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.64f <= substanceNormVal && substanceNormVal <= 0.65f )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.55f <= substanceNormVal && substanceNormVal <= 0.64f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.525f <= substanceNormVal && substanceNormVal <= 0.55f )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.45f <= substanceNormVal && substanceNormVal <= 0.525f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.425f <= substanceNormVal && substanceNormVal <= 0.45f )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.35f <= substanceNormVal && substanceNormVal <= 0.425f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.20f <= substanceNormVal && substanceNormVal <= 0.35f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.10f <= substanceNormVal && substanceNormVal <= 0.20f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.08f <= substanceNormVal && substanceNormVal <= 0.10f )
			{
				return TileIdentity::Gravel;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.10f )
			{
				return TileIdentity::Dirt;
			}
		}
		else if ( 0.5f <= heightPercentage && heightPercentage <= 0.6f )
		{
			if ( 0.70f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.65f <= substanceNormVal && substanceNormVal <= 0.7f )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.64f <= substanceNormVal && substanceNormVal <= 0.65f )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.55f <= substanceNormVal && substanceNormVal <= 0.64f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.30f <= substanceNormVal && substanceNormVal <= 0.55f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.21f <= substanceNormVal && substanceNormVal <= 0.30f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.2075f <= substanceNormVal && substanceNormVal <= 0.21f )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.10f <= substanceNormVal && substanceNormVal <= 0.2075f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.05f <= substanceNormVal && substanceNormVal <= 0.10f )
			{
				return TileIdentity::Gravel;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.05f )
			{
				return TileIdentity::Stone;
			}
		}
		else if ( 0.25f <= heightPercentage && heightPercentage <= 0.5f )
		{
			if ( 0.8f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.65f <= substanceNormVal && substanceNormVal <= 0.8f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.645f <= substanceNormVal && substanceNormVal <= 0.65f )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.34f <= substanceNormVal && substanceNormVal <= 0.645f )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.32f <= substanceNormVal && substanceNormVal <= 0.34f )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.25f <= substanceNormVal && substanceNormVal <= 0.32f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.2475f <= substanceNormVal && substanceNormVal <= 0.25f )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.15f <= substanceNormVal && substanceNormVal <= 0.2475f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.075f <= substanceNormVal && substanceNormVal <= 0.15f )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.04f <= substanceNormVal && substanceNormVal <= 0.075f )
			{
				return TileIdentity::Gravel;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.04f )
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