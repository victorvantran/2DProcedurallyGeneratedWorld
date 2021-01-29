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
	static TileIdentity getSubstance( float heightPercentage, float substanceNormVal )
	{
		if ( 0.999f <= heightPercentage && heightPercentage <= 1.00f )
		{
			return TileIdentity::MossDirt;
		}
		else if ( 0.99f <= heightPercentage && heightPercentage <= 0.999f )
		{
			if ( 0.86 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.85f <= substanceNormVal && substanceNormVal <= 0.86f )
			{
				return TileIdentity::MossStone;
			}
			else if ( 0.20f <= substanceNormVal && substanceNormVal <= 0.85f )
			{
				return TileIdentity::MossDirt;
			}
			else if ( 0.125f <= substanceNormVal && substanceNormVal <= 0.20f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.120f <= substanceNormVal && substanceNormVal <= 0.125f )
			{
				return TileIdentity::Stone;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.1 )
			{
				return TileIdentity::Mor;
			}
		}
		else if ( 0.90f <= heightPercentage && heightPercentage <= 0.99f )
		{
			if ( 0.9f <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.88f <= substanceNormVal && substanceNormVal <= 0.90f )
			{
				return TileIdentity::Gravel;
			}
			if ( 0.80f <= substanceNormVal && substanceNormVal <= 0.88f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.75f <= substanceNormVal && substanceNormVal <= 0.80f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.50f <= substanceNormVal && substanceNormVal <= 0.75f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.45f <= substanceNormVal && substanceNormVal <= 0.50f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.435f <= substanceNormVal && substanceNormVal <= 0.45f )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.20f <= substanceNormVal && substanceNormVal <= 0.435f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.175f <= substanceNormVal && substanceNormVal <= 0.20f )
			{
				return TileIdentity::Stone;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.175f )
			{
				return TileIdentity::Dirt;
			}
		}
		else if ( 0.85f <= heightPercentage && heightPercentage <= 0.90f )
		{
			// Iron Ore
			// Aluminium Ore
			// Oxides
			if ( 0.80f <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.78f <= substanceNormVal && substanceNormVal <= 0.80f )
			{
				return ( ( int )( substanceNormVal * 100 ) % 2 ) ? TileIdentity::AluminiumOxide : TileIdentity::IronOxide;
			}
			else if ( 0.70f <= substanceNormVal && substanceNormVal <= 0.78f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.60f <= substanceNormVal && substanceNormVal <= 0.70f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.59f <= substanceNormVal && substanceNormVal <= 0.60f )
			{
				return TileIdentity::AluminiumOxide;
			}
			else if ( 0.5875f <= substanceNormVal && substanceNormVal <= 0.59f )
			{
				return TileIdentity::AluminiumOre;
			}
			else if ( 0.580f <= substanceNormVal && substanceNormVal <= 0.5875f )
			{
				return TileIdentity::AluminiumOxide;
			}
			else if ( 0.50f <= substanceNormVal && substanceNormVal <= 0.580f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.45f <= substanceNormVal && substanceNormVal <= 0.50f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.435f <= substanceNormVal && substanceNormVal <= 0.45f )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.20f <= substanceNormVal && substanceNormVal <= 0.435f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.19f <= substanceNormVal && substanceNormVal <= 0.20f )
			{
				return TileIdentity::IronOxide;
			}
			else if ( 0.185f <= substanceNormVal && substanceNormVal <= 0.19f )
			{
				return TileIdentity::IronOre;
			}
			else if ( 0.180f <= substanceNormVal && substanceNormVal <= 0.185f )
			{
				return TileIdentity::IronOxide;
			}
			else if ( 0.125f <= substanceNormVal && substanceNormVal <= 0.180f )
			{
				return TileIdentity::Stone;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.125f )
			{
				return TileIdentity::Dirt;
			}
		}
		else if ( 0.70f <= heightPercentage && heightPercentage <= 0.85f )
		{
			if ( 0.9f <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.80f <= substanceNormVal && substanceNormVal <= 0.90f )
			{
				return ( ( int )( substanceNormVal * 100 ) % 2 ) ? TileIdentity::AluminiumOxide : TileIdentity::IronOxide;
			}
			else if ( 0.725f <= substanceNormVal && substanceNormVal <= 0.80f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.70f <= substanceNormVal && substanceNormVal <= 0.725f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.60f <= substanceNormVal && substanceNormVal <= 0.70f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.30f <= substanceNormVal && substanceNormVal <= 0.60f )
			{
				return ( ( int )( substanceNormVal * 100 ) % 2 ) ? TileIdentity::AluminiumOxide : TileIdentity::IronOxide;
			}
			else if ( 0.2999f <= substanceNormVal && substanceNormVal <= 0.30f )
			{
				return TileIdentity::AluminiumOre;
			}
			else if ( 0.10f <= substanceNormVal && substanceNormVal <= 0.2999f )
			{
				return ( ( int )( substanceNormVal * 100 ) % 2 ) ? TileIdentity::AluminiumOxide : TileIdentity::IronOxide;
			}
			else if ( 0.0999f <= substanceNormVal && substanceNormVal <= 0.10f )
			{
				return TileIdentity::IronOre;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.0999f )
			{
				return TileIdentity::Stone;
			}
		}
		else if ( 0.55f <= heightPercentage && heightPercentage <= 0.70f )
		{
			if ( 0.91f <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.905f <= substanceNormVal && substanceNormVal <= 0.91 )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.90f <= substanceNormVal && substanceNormVal <= 0.905 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.60f <= substanceNormVal && substanceNormVal <= 0.90f )
			{
				return ( ( int )( substanceNormVal * 100 ) % 2 ) ? TileIdentity::AluminiumOxide : TileIdentity::IronOxide;
			}
			else if ( 0.59f <= substanceNormVal && substanceNormVal <= 0.60f )
			{
				return TileIdentity::DarkClay;
			}
			else if ( 0.55f <= substanceNormVal && substanceNormVal <= 0.59f )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.54f <= substanceNormVal && substanceNormVal <= 0.55f )
			{
				return TileIdentity::DarkClay;
			}
			else if ( 0.525f <= substanceNormVal && substanceNormVal <= 0.54f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.5225f <= substanceNormVal && substanceNormVal <= 0.525f )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.50f <= substanceNormVal && substanceNormVal <= 0.5225f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.425f <= substanceNormVal && substanceNormVal <= 0.50f )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.415f <= substanceNormVal && substanceNormVal <= 0.425f )
			{
				return TileIdentity::DarkClay;
			}
			else if ( 0.40f <= substanceNormVal && substanceNormVal <= 0.415f )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.15f <= substanceNormVal && substanceNormVal <= 0.40f )
			{
				return ( ( int )( substanceNormVal * 100 ) % 2 ) ? TileIdentity::AluminiumOxide : TileIdentity::IronOxide;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.15f )
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
