#pragma once

#include "Tile.h"
// https://cforest.weebly.com/uploads/1/3/4/9/13495202/7326878.gif?1348042161
// [!] bleached sand , clay and iron oxide

class TemperateSeasonalForest
{
private:

public:
	static TileIdentity getSubstance( long double heightPercentage, long double substanceNormVal )
	{
		if ( 0.999 <= heightPercentage && heightPercentage <= 1.00 )
		{
			return TileIdentity::MossDirt;
		}
		else if ( 0.985 <= heightPercentage && heightPercentage <= 0.999 )
		{
			if ( 0.76 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Mull;
			}
			else if ( 0.75 <= substanceNormVal && substanceNormVal <= 0.76 )
			{
				return TileIdentity::MossStone;
			}
			else if ( 0.20 <= substanceNormVal && substanceNormVal <= 0.75 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.10 <= substanceNormVal && substanceNormVal <= 0.20 )
			{
				return TileIdentity::Stone;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.1 )
			{
				return TileIdentity::LightClay;
			}
		}
		else if ( 0.50 <= heightPercentage && heightPercentage <= 0.985 )
		{
			if ( 0.70 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.68 <= substanceNormVal && substanceNormVal <= 0.70 )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.67 <= substanceNormVal && substanceNormVal <= 0.68 )
			{
				return TileIdentity::BleachedSand;
			}
			else if ( 0.65 <= substanceNormVal && substanceNormVal <= 0.67 )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.64 <= substanceNormVal && substanceNormVal <= 0.65 )
			{
				return TileIdentity::DarkClay;
			}
			else if ( 0.635 <= substanceNormVal && substanceNormVal <= 0.64 )
			{
				return TileIdentity::BleachedSand;
			}
			else if ( 0.55 <= substanceNormVal && substanceNormVal <= 0.635 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.30 <= substanceNormVal && substanceNormVal <= 0.55 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.21 <= substanceNormVal && substanceNormVal <= 0.30 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.2075 <= substanceNormVal && substanceNormVal <= 0.21 )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.10 <= substanceNormVal && substanceNormVal <= 0.2075 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.05 <= substanceNormVal && substanceNormVal <= 0.10 )
			{
				return TileIdentity::Gravel;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.05 )
			{
				return TileIdentity::Stone;
			}
		}
		else if ( 0.35 <= heightPercentage && heightPercentage <= 0.50 )
		{
			if ( 0.85 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.73 <= substanceNormVal && substanceNormVal <= 0.85 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.72 <= substanceNormVal && substanceNormVal <= 0.73 )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.68 <= substanceNormVal && substanceNormVal <= 0.72 )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.40 <= substanceNormVal && substanceNormVal <= 0.68 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.39 <= substanceNormVal && substanceNormVal <= 0.40 )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.02 <= substanceNormVal && substanceNormVal <= 0.39 )
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


