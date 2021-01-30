#pragma once

#include "Tile.h"

// https://8kmdbsavannastropicalgrasslands.weebly.com/soil.html
// https://www.britannica.com/science/savanna/Environment
// https://www.blueplanetbiomes.org/savanna.php#:~:text=A%20savanna%20is%20a%20rolling,also%20known%20as%20tropical%20grasslands.&text=Savannas%20have%20warm%20temperature%20year%20round.
// https://ucmp.berkeley.edu/exhibits/biomes/grasslands.php

// 0.40


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
	static TileIdentity getSubstance( long double heightPercentage, long double substanceNormVal )
	{

		if ( 0.99 <= heightPercentage && heightPercentage <= 1.00 )
		{
			return TileIdentity::Dirt;
		}
		else if ( 0.989 <= heightPercentage && heightPercentage <= 0.99 )
		{
			if ( 0.92 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.90 <= substanceNormVal && substanceNormVal <= 0.92 )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.875 <= substanceNormVal && substanceNormVal <= 0.90 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.870 <= substanceNormVal && substanceNormVal <= 0.875 )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.869 <= substanceNormVal && substanceNormVal <= 0.870 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.860 <= substanceNormVal && substanceNormVal <= 0.869 )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.775 <= substanceNormVal && substanceNormVal <= 0.860 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.770 <= substanceNormVal && substanceNormVal <= 0.775 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.70 <= substanceNormVal && substanceNormVal <= 0.770 )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.6875 <= substanceNormVal && substanceNormVal <= 0.70 )
			{
				return TileIdentity::RedClay;
			}
			else if ( 0.650 <= substanceNormVal && substanceNormVal <= 0.6875 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.575 <= substanceNormVal && substanceNormVal <= 0.650 )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.45 <= substanceNormVal && substanceNormVal <= 0.575 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.448 <= substanceNormVal && substanceNormVal <= 0.45 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.375 <= substanceNormVal && substanceNormVal <= 0.448 )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.3749 <= substanceNormVal && substanceNormVal <= 0.375 )
			{
				return TileIdentity::RedClay;
			}
			else if ( 0.25 <= substanceNormVal && substanceNormVal <= 0.3749 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.15 <= substanceNormVal && substanceNormVal <= 0.25 )
			{
				return TileIdentity::Mor;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.15 )
			{
				return TileIdentity::Dirt;
			}
		}
		else if ( 0.85 <= heightPercentage && heightPercentage <= 0.989 )
		{
			if ( 0.855 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.840 <= substanceNormVal && substanceNormVal <= 0.855 )
			{
				return TileIdentity::Sand;
			}
			else if ( 0.70 <= substanceNormVal && substanceNormVal <= 0.840 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.6875 <= substanceNormVal && substanceNormVal <= 0.70 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.68 <= substanceNormVal && substanceNormVal <= 0.6875 )
			{
				return TileIdentity::RedClay;
			}
			else if ( 0.6795 <= substanceNormVal && substanceNormVal <= 0.68 )
			{
				return TileIdentity::Laterite;
			}
			else if ( 0.645 <= substanceNormVal && substanceNormVal <= 0.6795 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.425 <= substanceNormVal && substanceNormVal <= 0.645 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.375 <= substanceNormVal && substanceNormVal <= 0.425 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.37475 <= substanceNormVal && substanceNormVal <= 0.375 )
			{
				return TileIdentity::RedClay;
			}
			else if ( 0.10 <= substanceNormVal && substanceNormVal <= 0.37475 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.05 <= substanceNormVal && substanceNormVal <= 0.10 )
			{
				return TileIdentity::Stone;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.05 )
			{
				return TileIdentity::Dirt;
			}
		}
		else if ( 0.50 <= heightPercentage && heightPercentage <= 0.85 )
		{
			if ( 0.775 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.68 <= substanceNormVal && substanceNormVal <= 0.775 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.67 <= substanceNormVal && substanceNormVal <= 0.68 )
			{
				return TileIdentity::BleachedSand;
			}
			else if ( 0.65 <= substanceNormVal && substanceNormVal <= 0.67 )
			{
				return TileIdentity::Laterite;
			}
			else if ( 0.60 <= substanceNormVal && substanceNormVal <= 0.65 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.59 <= substanceNormVal && substanceNormVal <= 0.60 )
			{
				return TileIdentity::Gravel;
			}
			else if ( 0.45 <= substanceNormVal && substanceNormVal <= 0.59 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.425 <= substanceNormVal && substanceNormVal <= 0.45 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.35 <= substanceNormVal && substanceNormVal <= 0.425 )
			{
				return TileIdentity::Laterite;
			}
			else if ( 0.25 <= substanceNormVal && substanceNormVal <= 0.35 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.10 <= substanceNormVal && substanceNormVal <= 0.25 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.05 <= substanceNormVal && substanceNormVal <= 0.10 )
			{
				return TileIdentity::Dirt;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.05 )
			{
				return TileIdentity::Stone;
			}
		}
		else if ( 0.40 <= heightPercentage && heightPercentage <= 0.50 )
		{
			if ( 0.975 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.90 <= substanceNormVal && substanceNormVal <= 0.975 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.855 <= substanceNormVal && substanceNormVal <= 0.90 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.725 <= substanceNormVal && substanceNormVal <= 0.855 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.70 <= substanceNormVal && substanceNormVal <= 0.725 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.66 <= substanceNormVal && substanceNormVal <= 0.70 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.655 <= substanceNormVal && substanceNormVal <= 0.66 )
			{
				return TileIdentity::Laterite;
			}
			else if ( 0.475 <= substanceNormVal && substanceNormVal <= 0.655 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.445 <= substanceNormVal && substanceNormVal <= 0.475 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.40 <= substanceNormVal && substanceNormVal <= 0.445 )
			{
				return TileIdentity::Laterite;
			}
			else if ( 0.28 <= substanceNormVal && substanceNormVal <= 0.40 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.25 <= substanceNormVal && substanceNormVal <= 0.28 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.06 <= substanceNormVal && substanceNormVal <= 0.25 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.05 <= substanceNormVal && substanceNormVal <= 0.06 )
			{
				return TileIdentity::Dirt;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.05 )
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