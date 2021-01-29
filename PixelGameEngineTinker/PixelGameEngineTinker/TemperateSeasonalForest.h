#pragma once

#include "Tile.h"
// https://cforest.weebly.com/uploads/1/3/4/9/13495202/7326878.gif?1348042161
// bleached sand , clay and iron oxide

class TemperateSeasonalForest
{
private:

public:
	static TileIdentity getSubstance( float heightPercentage, float substanceNormVal )
	{
		if ( 0.999f <= heightPercentage && heightPercentage <= 1.00f )
		{
			return TileIdentity::MossDirt;
		}
		else if ( 0.985f <= heightPercentage && heightPercentage <= 0.999f )
		{
			if ( 0.76 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Mull;
			}
			else if ( 0.75f <= substanceNormVal && substanceNormVal <= 0.76f )
			{
				return TileIdentity::MossStone;
			}
			else if ( 0.20f <= substanceNormVal && substanceNormVal <= 0.75f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.10f <= substanceNormVal && substanceNormVal <= 0.20f )
			{
				return TileIdentity::Stone;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.1 )
			{
				return TileIdentity::LightClay;
			}
		}
		else if ( 0.50f <= heightPercentage && heightPercentage <= 0.985f )
		{
			if ( 0.70f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.68f <= substanceNormVal && substanceNormVal <= 0.7f )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.67f <= substanceNormVal && substanceNormVal <= 0.68f )
			{
				return TileIdentity::BleachedSand;
			}
			else if ( 0.65f <= substanceNormVal && substanceNormVal <= 0.67f )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.64f <= substanceNormVal && substanceNormVal <= 0.65f )
			{
				return TileIdentity::DarkClay;
			}
			else if ( 0.635f <= substanceNormVal && substanceNormVal <= 0.64f )
			{
				return TileIdentity::BleachedSand;
			}
			else if ( 0.55f <= substanceNormVal && substanceNormVal <= 0.635f )
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
		else if ( 0.35f <= heightPercentage && heightPercentage <= 0.50f )
		{
			if ( 0.85f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.73f <= substanceNormVal && substanceNormVal <= 0.85f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.72f <= substanceNormVal && substanceNormVal <= 0.73f )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.68f <= substanceNormVal && substanceNormVal <= 0.72f )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.40f <= substanceNormVal && substanceNormVal <= 0.68f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.39f <= substanceNormVal && substanceNormVal <= 0.40f )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.02f <= substanceNormVal && substanceNormVal <= 0.39f )
			{
				return TileIdentity::Stone;
			}
			else //if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.02 )
			{
				return TileIdentity::Charcoal;
			}
		}
		else
		{
			return TileIdentity::Stone;
		}
	}
};
