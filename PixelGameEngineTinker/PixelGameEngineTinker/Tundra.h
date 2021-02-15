#pragma once

#include "Biome.h"
#include "Tile.h"


// https://ucmp.berkeley.edu/exhibits/biomes/tundra.php

// Soil, permafrost, talik


// 0.98 -> 1.0/ // Snow, Dirt, Mor, Stone 

// 0.90 -> 0.98: Snow, Stone, Permafrost



class Tundra : public Biome
{
private:

public:
	Tundra() {}
	Tundra( const FoliageMap& foliageMap, const std::pair<long double, long double>& temperatureRange, const std::pair<long double, long double>& precipitationRange ) :
		Biome( foliageMap, temperatureRange, precipitationRange ) {}
	~Tundra() {}


	static TileIdentity getSubstance( long double heightPercentage, long double substanceNormVal )
	{
		if ( 0.98 <= heightPercentage && heightPercentage <= 1.00 )
		{
			if ( 0.75 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.50 <= substanceNormVal && substanceNormVal <= 0.75 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.49 <= substanceNormVal && substanceNormVal <= 0.50 )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.30 <= substanceNormVal && substanceNormVal <= 0.49 )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.25 <= substanceNormVal && substanceNormVal <= 0.30 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.245 <= substanceNormVal && substanceNormVal <= 0.25 )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.20 <= substanceNormVal && substanceNormVal <= 0.245 )
			{
				return TileIdentity::Dirt;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.20 )
			{
				return TileIdentity::Snow;
			}
		}
		else if ( 0.90 <= heightPercentage && heightPercentage <= 0.98 )
		{
			if ( 0.80 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.60 <= substanceNormVal && substanceNormVal <= 0.80 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.58 <= substanceNormVal && substanceNormVal <= 0.60 )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.45 <= substanceNormVal && substanceNormVal <= 0.58 )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.40 <= substanceNormVal && substanceNormVal <= 0.45 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.35 <= substanceNormVal && substanceNormVal <= 0.40 )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.30 <= substanceNormVal && substanceNormVal <= 0.35 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.275 <= substanceNormVal && substanceNormVal <= 0.30 )
			{
				return TileIdentity::Permafrost;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.275 )
			{
				return TileIdentity::Snow;
			}
		}
		else if ( 0.60 <= heightPercentage && heightPercentage <= 0.90 )
		{
			// 0.60 -> 0.90 permafrost, talik( dirt, mor, stone, light clay, shale ), snow

			if ( 0.95 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.90 <= substanceNormVal && substanceNormVal <= 0.95 )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.895 <= substanceNormVal && substanceNormVal <= 0.90 )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.875 <= substanceNormVal && substanceNormVal <= 0.895 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.825 <= substanceNormVal && substanceNormVal <= 0.875 )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.775 <= substanceNormVal && substanceNormVal <= 0.825 )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.70 <= substanceNormVal && substanceNormVal <= 0.775 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.675 <= substanceNormVal && substanceNormVal <= 0.70 )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.50 <= substanceNormVal && substanceNormVal <= 0.675 )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.495 <= substanceNormVal && substanceNormVal <= 0.50 )
			{
				return TileIdentity::Shale;
			}
			else if ( 0.475 <= substanceNormVal && substanceNormVal <= 0.495 )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.30 <= substanceNormVal && substanceNormVal <= 0.475 )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.25 <= substanceNormVal && substanceNormVal <= 0.30 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.245 <= substanceNormVal && substanceNormVal <= 0.25 )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.205 <= substanceNormVal && substanceNormVal <= 0.245 )
			{
				return TileIdentity::Snow;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.205f )
			{
				return TileIdentity::Permafrost;
			}
		}
		else // if ( 0.0 <= heightPercentage && heightPercentage <= 0.60 )
		{
			// 0.20 -> 0.60 talik ( dirt, mor, gravel, stone, charcoal, light clay, dark clay, shale -> Uranium Ore ) perma frost, snow, stone

			if ( 0.98 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.95 <= substanceNormVal && substanceNormVal <= 0.98 )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.90 <= substanceNormVal && substanceNormVal <= 0.95 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.89 <= substanceNormVal && substanceNormVal <= 0.90 )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.80 <= substanceNormVal && substanceNormVal <= 0.89 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.775 <= substanceNormVal && substanceNormVal <= 0.80 )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.75 <= substanceNormVal && substanceNormVal <= 0.775 )
			{
				return TileIdentity::Snow;
			}
			else if ( 0.55 <= substanceNormVal && substanceNormVal <= 0.75 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.545 <= substanceNormVal && substanceNormVal <= 0.55 )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.445 <= substanceNormVal && substanceNormVal <= 0.545 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.425 <= substanceNormVal && substanceNormVal <= 0.445 )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.40 <= substanceNormVal && substanceNormVal <= 0.425 )
			{
				return TileIdentity::Shale;
			}
			else if ( 0.39 <= substanceNormVal && substanceNormVal <= 0.40 )
			{
				return TileIdentity::Permafrost;
			}
			else if ( 0.35 <= substanceNormVal && substanceNormVal <= 0.39 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.345 <= substanceNormVal && substanceNormVal <= 0.35 )
			{
				return TileIdentity::Snow;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.345 )
			{
				return TileIdentity::Stone;
			}
		}
	}
};