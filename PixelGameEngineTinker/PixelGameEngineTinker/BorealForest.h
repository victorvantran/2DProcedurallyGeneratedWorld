#pragma once

#include "Biome.h"
#include "Tile.h"

// Permafrost ( thick )



// Taiga
// Little plant variety, acidic soil
// Snow
// Water


class BorealForest : public Biome
{
private:

public:
	BorealForest() {}
	BorealForest( const FoliageMap& foliageMap, const std::pair<long double, long double>& temperatureRange, const std::pair<long double, long double>& precipitationRange ) :
		Biome( foliageMap, temperatureRange, precipitationRange ) {}
	~BorealForest() {}


	static TileIdentity getSubstance( long double heightPercentage, long double substanceNormVal )
	{
		if ( 0.98f <= heightPercentage && heightPercentage <= 1.00f )
		{
			if ( 0.825 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.70 <= substanceNormVal && substanceNormVal <= 0.825 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.475 <= substanceNormVal && substanceNormVal <= 0.70 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.45 <= substanceNormVal && substanceNormVal <= 0.475 )
			{
				return TileIdentity::Podzol;
			}
			else if ( 0.40 <= substanceNormVal && substanceNormVal <= 0.45 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.35 <= substanceNormVal && substanceNormVal <= 0.40 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.30 <= substanceNormVal && substanceNormVal <= 0.35 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.29 <= substanceNormVal && substanceNormVal <= 0.30 )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.20 <= substanceNormVal && substanceNormVal <= 0.29 )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.10 <= substanceNormVal && substanceNormVal <= 0.20 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.075 <= substanceNormVal && substanceNormVal <= 0.10 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.05 <= substanceNormVal && substanceNormVal <= 0.075 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.015 <= substanceNormVal && substanceNormVal <= 0.05 )
			{
				return TileIdentity::Snow;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.015 )
			{
				return TileIdentity::Gravel;
			}
		}
		else if ( 0.9 <= heightPercentage && heightPercentage <= 0.98 )
		{
			if ( 0.80 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.65 <= substanceNormVal && substanceNormVal <= 0.80 )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.60 <= substanceNormVal && substanceNormVal <= 0.65 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.50 <= substanceNormVal && substanceNormVal <= 0.60 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.40 <= substanceNormVal && substanceNormVal <= 0.50 )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.30 <= substanceNormVal && substanceNormVal <= 0.40 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.28 <= substanceNormVal && substanceNormVal <= 0.30 )
			{
				return TileIdentity::Podzol;
			}
			else if ( 0.11 <= substanceNormVal && substanceNormVal <= 0.28 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.09 <= substanceNormVal && substanceNormVal <= 0.11 )
			{
				return TileIdentity::Podzol;
			}
			else //  if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.10 )
			{
				return TileIdentity::Snow;
			}
		}
		else if ( 0.6 <= heightPercentage && heightPercentage <= 0.9 )
		{
			if ( 0.80 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.65 <= substanceNormVal && substanceNormVal <= 0.80 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.64 <= substanceNormVal && substanceNormVal <= 0.65 )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.55 <= substanceNormVal && substanceNormVal <= 0.64 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.525 <= substanceNormVal && substanceNormVal <= 0.55 )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.45 <= substanceNormVal && substanceNormVal <= 0.525 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.425 <= substanceNormVal && substanceNormVal <= 0.45 )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.35 <= substanceNormVal && substanceNormVal <= 0.425 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.20 <= substanceNormVal && substanceNormVal <= 0.35 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.10 <= substanceNormVal && substanceNormVal <= 0.20 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.08 <= substanceNormVal && substanceNormVal <= 0.10 )
			{
				return TileIdentity::Gravel;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.10 )
			{
				return TileIdentity::Dirt;
			}
		}
		else if ( 0.5 <= heightPercentage && heightPercentage <= 0.6 )
		{
			if ( 0.70 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.65 <= substanceNormVal && substanceNormVal <= 0.70 )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.64 <= substanceNormVal && substanceNormVal <= 0.65 )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.55 <= substanceNormVal && substanceNormVal <= 0.64 )
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
		else if ( 0.25 <= heightPercentage && heightPercentage <= 0.5 )
		{
			if ( 0.8 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.65 <= substanceNormVal && substanceNormVal <= 0.8 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.645 <= substanceNormVal && substanceNormVal <= 0.65 )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.34 <= substanceNormVal && substanceNormVal <= 0.645 )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.32 <= substanceNormVal && substanceNormVal <= 0.34 )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.25 <= substanceNormVal && substanceNormVal <= 0.32 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.2475 <= substanceNormVal && substanceNormVal <= 0.25 )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.15 <= substanceNormVal && substanceNormVal <= 0.2475 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.075 <= substanceNormVal && substanceNormVal <= 0.15 )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.04 <= substanceNormVal && substanceNormVal <= 0.075 )
			{
				return TileIdentity::Gravel;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.04 )
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