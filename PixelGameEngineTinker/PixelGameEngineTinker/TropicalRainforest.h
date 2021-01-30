#pragma once

#include "Tile.h"

// Thin humus
// thick masses of iron, alumininum oxides, quartz
// Shallow Dirt -> Mull -> Ioron and aluminum oxide ( thick ) -> clay ->


// Clay


class TropicalRainforest
{
private:

public:
	static TileIdentity getSubstance( long double heightPercentage, long double substanceNormVal )
	{
		if ( 0.999 <= heightPercentage && heightPercentage <= 1.00 )
		{
			return TileIdentity::MossDirt;
		}
		else if ( 0.99 <= heightPercentage && heightPercentage <= 0.999 )
		{
			if ( 0.86 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.85 <= substanceNormVal && substanceNormVal <= 0.86 )
			{
				return TileIdentity::MossStone;
			}
			else if ( 0.20 <= substanceNormVal && substanceNormVal <= 0.85 )
			{
				return TileIdentity::MossDirt;
			}
			else if ( 0.125 <= substanceNormVal && substanceNormVal <= 0.20 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.120 <= substanceNormVal && substanceNormVal <= 0.125 )
			{
				return TileIdentity::Stone;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.120 )
			{
				return TileIdentity::Mor;
			}
		}
		else if ( 0.90 <= heightPercentage && heightPercentage <= 0.99 )
		{
			if ( 0.90 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.88 <= substanceNormVal && substanceNormVal <= 0.90 )
			{
				return TileIdentity::Gravel;
			}
			if ( 0.80 <= substanceNormVal && substanceNormVal <= 0.88 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.75 <= substanceNormVal && substanceNormVal <= 0.80 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.50 <= substanceNormVal && substanceNormVal <= 0.75 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.45 <= substanceNormVal && substanceNormVal <= 0.50 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.435 <= substanceNormVal && substanceNormVal <= 0.45 )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.20 <= substanceNormVal && substanceNormVal <= 0.435 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.175 <= substanceNormVal && substanceNormVal <= 0.20 )
			{
				return TileIdentity::Stone;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.175 )
			{
				return TileIdentity::Dirt;
			}
		}
		else if ( 0.85 <= heightPercentage && heightPercentage <= 0.90 )
		{
			// Iron Ore
			// Aluminium Ore
			// Oxides
			if ( 0.80 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.78 <= substanceNormVal && substanceNormVal <= 0.80 )
			{
				return ( ( std::int32_t )( substanceNormVal * 100 ) % 2 ) ? TileIdentity::AluminiumOxide : TileIdentity::IronOxide;
			}
			else if ( 0.70 <= substanceNormVal && substanceNormVal <= 0.78 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.60 <= substanceNormVal && substanceNormVal <= 0.70 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.59 <= substanceNormVal && substanceNormVal <= 0.60 )
			{
				return TileIdentity::AluminiumOxide;
			}
			else if ( 0.5875 <= substanceNormVal && substanceNormVal <= 0.59 )
			{
				return TileIdentity::AluminiumOre;
			}
			else if ( 0.580 <= substanceNormVal && substanceNormVal <= 0.5875 )
			{
				return TileIdentity::AluminiumOxide;
			}
			else if ( 0.50 <= substanceNormVal && substanceNormVal <= 0.580 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.45 <= substanceNormVal && substanceNormVal <= 0.50 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.435 <= substanceNormVal && substanceNormVal <= 0.45 )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.20 <= substanceNormVal && substanceNormVal <= 0.435 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.19 <= substanceNormVal && substanceNormVal <= 0.20 )
			{
				return TileIdentity::IronOxide;
			}
			else if ( 0.185 <= substanceNormVal && substanceNormVal <= 0.19 )
			{
				return TileIdentity::IronOre;
			}
			else if ( 0.180 <= substanceNormVal && substanceNormVal <= 0.185 )
			{
				return TileIdentity::IronOxide;
			}
			else if ( 0.125 <= substanceNormVal && substanceNormVal <= 0.180 )
			{
				return TileIdentity::Stone;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.125 )
			{
				return TileIdentity::Dirt;
			}
		}
		else if ( 0.70 <= heightPercentage && heightPercentage <= 0.85 )
		{
			if ( 0.90 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.80 <= substanceNormVal && substanceNormVal <= 0.90 )
			{
				return ( ( int )( substanceNormVal * 100 ) % 2 ) ? TileIdentity::AluminiumOxide : TileIdentity::IronOxide;
			}
			else if ( 0.725 <= substanceNormVal && substanceNormVal <= 0.80 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.70 <= substanceNormVal && substanceNormVal <= 0.725 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.60 <= substanceNormVal && substanceNormVal <= 0.70 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.30 <= substanceNormVal && substanceNormVal <= 0.60 )
			{
				return ( ( int )( substanceNormVal * 100 ) % 2 ) ? TileIdentity::AluminiumOxide : TileIdentity::IronOxide;
			}
			else if ( 0.2999 <= substanceNormVal && substanceNormVal <= 0.30 )
			{
				return TileIdentity::AluminiumOre;
			}
			else if ( 0.10 <= substanceNormVal && substanceNormVal <= 0.2999 )
			{
				return ( ( std::int32_t )( substanceNormVal * 100 ) % 2 ) ? TileIdentity::AluminiumOxide : TileIdentity::IronOxide;
			}
			else if ( 0.0999 <= substanceNormVal && substanceNormVal <= 0.10 )
			{
				return TileIdentity::IronOre;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.0999 )
			{
				return TileIdentity::Stone;
			}
		}
		else if ( 0.55 <= heightPercentage && heightPercentage <= 0.70 )
		{
			if ( 0.91 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.905 <= substanceNormVal && substanceNormVal <= 0.91 )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.90 <= substanceNormVal && substanceNormVal <= 0.905 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.60 <= substanceNormVal && substanceNormVal <= 0.90 )
			{
				return ( ( std::int32_t )( substanceNormVal * 100 ) % 2 ) ? TileIdentity::AluminiumOxide : TileIdentity::IronOxide;
			}
			else if ( 0.59 <= substanceNormVal && substanceNormVal <= 0.60 )
			{
				return TileIdentity::DarkClay;
			}
			else if ( 0.55 <= substanceNormVal && substanceNormVal <= 0.59 )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.54 <= substanceNormVal && substanceNormVal <= 0.55 )
			{
				return TileIdentity::DarkClay;
			}
			else if ( 0.525 <= substanceNormVal && substanceNormVal <= 0.54 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.5225 <= substanceNormVal && substanceNormVal <= 0.525 )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.50 <= substanceNormVal && substanceNormVal <= 0.5225 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.425 <= substanceNormVal && substanceNormVal <= 0.50 )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.415 <= substanceNormVal && substanceNormVal <= 0.425 )
			{
				return TileIdentity::DarkClay;
			}
			else if ( 0.40 <= substanceNormVal && substanceNormVal <= 0.415 )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.15 <= substanceNormVal && substanceNormVal <= 0.40 )
			{
				return ( ( int )( substanceNormVal * 100 ) % 2 ) ? TileIdentity::AluminiumOxide : TileIdentity::IronOxide;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.15 )
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

