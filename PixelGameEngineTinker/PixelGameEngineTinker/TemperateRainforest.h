#pragma once

#include "Tile.h"


// https://temperaterainforestess.weebly.com/soil-and-land.html
// http://temperate-rainforest-facts.com/Temperate-Rainforest-Biome/Temperate-Rainforest-Soil.shtml 

// Like temperate seasonal but more rich

// Less diverse




// Moss dirt
// Moss stone

// alfisol under and along with moss dirt/stone and mor and mull
// alfisol along with clay
// red and yellow clay

// 0.30


class TemperateRainforest
{
private:

public:
	static TileIdentity getSubstance( long double heightPercentage, long double substanceNormVal )
	{
		if ( 0.999 <= heightPercentage && heightPercentage <= 1.0 )
		{
			return TileIdentity::MossDirt;
		}
		else if ( 0.975 <= heightPercentage && heightPercentage <= 0.999 )
		{
			if ( 0.75 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::MossDirt;
			}
			else if ( 0.65 <= substanceNormVal && substanceNormVal <= 0.75 )
			{
				return TileIdentity::Mull;
			}
			else if ( 0.50 <= substanceNormVal && substanceNormVal <= 0.65 )
			{
				return TileIdentity::Alfisol;
			}
			else if ( 0.35 <= substanceNormVal && substanceNormVal <= 0.50 )
			{
				return TileIdentity::MossDirt;
			}
			else if ( 0.34 <= substanceNormVal && substanceNormVal <= 0.35 )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.3 <= substanceNormVal && substanceNormVal <= 0.34 )
			{
				return TileIdentity::MossStone;
			}
			else if ( 0.15 <= substanceNormVal && substanceNormVal <= 0.3 )
			{
				return TileIdentity::MossDirt;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.15 )
			{
				return TileIdentity::Alfisol;
			}
		}
		else if ( 0.60 <= heightPercentage && heightPercentage <= 0.975 )
		{
			if ( 0.95 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Alfisol;
			}
			else if ( 0.925 <= substanceNormVal && substanceNormVal <= 0.95 )
			{
				return TileIdentity::Mull;
			}
			else if ( 0.875 <= substanceNormVal && substanceNormVal <= 0.925 )
			{
				return TileIdentity::MossDirt;
			}
			else if ( 0.850 <= substanceNormVal && substanceNormVal <= 0.875 )
			{
				return TileIdentity::MossStone;
			}
			else if ( 0.815 <= substanceNormVal && substanceNormVal <= 0.850 )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.80 <= substanceNormVal && substanceNormVal <= 0.815 )
			{
				return TileIdentity::Alfisol;
			}
			else if ( 0.7875 <= substanceNormVal && substanceNormVal <= 0.80 )
			{
				return TileIdentity::YellowClay;
			}
			else if ( 0.785 <= substanceNormVal && substanceNormVal <= 0.7875 )
			{
				return TileIdentity::MossStone;
			}
			else if ( 0.75 <= substanceNormVal && substanceNormVal <= 0.785 )
			{
				return TileIdentity::MossDirt;
			}
			else if ( 0.70 <= substanceNormVal && substanceNormVal <= 0.75 )
			{
				return TileIdentity::Mull;
			}
			else if ( 0.695 <= substanceNormVal && substanceNormVal <= 0.70 )
			{
				return TileIdentity::YellowClay;
			}
			else if ( 0.675 <= substanceNormVal && substanceNormVal <= 0.695 )
			{
				return TileIdentity::Alfisol;
			}
			else if ( 0.60 <= substanceNormVal && substanceNormVal <= 0.675 )
			{
				return TileIdentity::MossDirt;
			}
			else if ( 0.50 <= substanceNormVal && substanceNormVal <= 0.60 )
			{
				return TileIdentity::MossStone;
			}
			else if ( 0.35 <= substanceNormVal && substanceNormVal <= 0.50 )
			{
				return TileIdentity::MossDirt;
			}
			else if ( 0.34 <= substanceNormVal && substanceNormVal <= 0.35 )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.30 <= substanceNormVal && substanceNormVal <= 0.34 )
			{
				return TileIdentity::MossDirt;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.30 )
			{
				return TileIdentity::Alfisol;
			}
		}
		else if ( 0.55 <= heightPercentage && heightPercentage <= 0.60 )
		{
			// Red clay, dirt, stone, charcoal
			if ( 0.95 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::MossDirt;
			}
			else if ( 0.90 <= substanceNormVal && substanceNormVal <= 0.95 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.850 <= substanceNormVal && substanceNormVal <= 0.90 )
			{
				return TileIdentity::MossStone;
			}
			else if ( 0.849 <= substanceNormVal && substanceNormVal <= 0.850 )
			{
				return TileIdentity::RedClay;
			}
			else if ( 0.815 <= substanceNormVal && substanceNormVal <= 0.849 )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.81 <= substanceNormVal && substanceNormVal <= 0.815 )
			{
				return TileIdentity::Alfisol;
			}
			else if ( 0.70 <= substanceNormVal && substanceNormVal <= 0.81 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.685 <= substanceNormVal && substanceNormVal <= 0.70 )
			{
				return TileIdentity::MossDirt;
			}
			else if ( 0.6 <= substanceNormVal && substanceNormVal <= 0.685 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.55 <= substanceNormVal && substanceNormVal <= 0.6 )
			{
				return TileIdentity::MossStone;
			}
			else if ( 0.54 <= substanceNormVal && substanceNormVal <= 0.55 )
			{
				return TileIdentity::RedClay;
			}
			else if ( 0.475 <= substanceNormVal && substanceNormVal <= 0.54 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.375 <= substanceNormVal && substanceNormVal <= 0.475 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.3725 <= substanceNormVal && substanceNormVal <= 0.375 )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.37 <= substanceNormVal && substanceNormVal <= 0.3725 )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.25 <= substanceNormVal && substanceNormVal <= 0.37 )
			{
				return TileIdentity::Dirt;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.25 )
			{
				return TileIdentity::Stone;
			}
		}
		else if ( 0.40 <= heightPercentage && heightPercentage <= 0.55 )
		{
			// dirt, stone, light clay, charcoal
			if ( 0.875 <= substanceNormVal && substanceNormVal <= 1.0 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.8735 <= substanceNormVal && substanceNormVal <= 0.875 )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.80 <= substanceNormVal && substanceNormVal <= 0.8735 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.70 <= substanceNormVal && substanceNormVal <= 0.80 )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.67 <= substanceNormVal && substanceNormVal <= 0.70 )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.4 <= substanceNormVal && substanceNormVal <= 0.67 )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.399 <= substanceNormVal && substanceNormVal <= 0.4 )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.10 <= substanceNormVal && substanceNormVal <= 0.399 )
			{
				return TileIdentity::Dirt;
			}
			else // if ( 0.0 <= substanceNormVal && substanceNormVal <= 0.1 )
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