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


class TemperateRainforest
{
private:

public:
	static TileIdentity getSubstance( float heightPercentage, float substanceNormVal )
	{
		if ( 0.999f <= heightPercentage && heightPercentage <= 1.0f )
		{
			return TileIdentity::MossDirt;
		}
		else if ( 0.975f <= heightPercentage && heightPercentage <= 0.999f )
		{
			if ( 0.75f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::MossDirt;
			}
			else if ( 0.65f <= substanceNormVal && substanceNormVal <= 0.75f )
			{
				return TileIdentity::Mull;
			}
			else if ( 0.50f <= substanceNormVal && substanceNormVal <= 0.65f )
			{
				return TileIdentity::Alfisol;
			}
			else if ( 0.35f <= substanceNormVal && substanceNormVal <= 0.5f )
			{
				return TileIdentity::MossDirt;
			}
			else if ( 0.34f <= substanceNormVal && substanceNormVal <= 0.35f )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.3f <= substanceNormVal && substanceNormVal <= 0.34f )
			{
				return TileIdentity::MossStone;
			}
			else if ( 0.15f <= substanceNormVal && substanceNormVal <= 0.3f )
			{
				return TileIdentity::MossDirt;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.15f )
			{
				return TileIdentity::Alfisol;
			}
		}
		else if ( 0.60f <= heightPercentage && heightPercentage <= 0.975f )
		{
			if ( 0.95f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::Alfisol;
			}
			else if ( 0.925f <= substanceNormVal && substanceNormVal <= 0.95f )
			{
				return TileIdentity::Mull;
			}
			else if ( 0.875f <= substanceNormVal && substanceNormVal <= 0.925f )
			{
				return TileIdentity::MossDirt;
			}
			else if ( 0.850f <= substanceNormVal && substanceNormVal <= 0.875f )
			{
				return TileIdentity::MossStone;
			}
			else if ( 0.815f <= substanceNormVal && substanceNormVal <= 0.850f )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.80f <= substanceNormVal && substanceNormVal <= 0.815f )
			{
				return TileIdentity::Alfisol;
			}
			else if ( 0.7875f <= substanceNormVal && substanceNormVal <= 0.80f )
			{
				return TileIdentity::YellowClay;
			}
			else if ( 0.785f <= substanceNormVal && substanceNormVal <= 0.7875f )
			{
				return TileIdentity::MossStone;
			}
			else if ( 0.75f <= substanceNormVal && substanceNormVal <= 0.785f )
			{
				return TileIdentity::MossDirt;
			}
			else if ( 0.70f <= substanceNormVal && substanceNormVal <= 0.75f )
			{
				return TileIdentity::Mull;
			}
			else if ( 0.695f <= substanceNormVal && substanceNormVal <= 0.70f )
			{
				return TileIdentity::YellowClay;
			}
			else if ( 0.675f <= substanceNormVal && substanceNormVal <= 0.695f )
			{
				return TileIdentity::Alfisol;
			}
			else if ( 0.60f <= substanceNormVal && substanceNormVal <= 0.675f )
			{
				return TileIdentity::MossDirt;
			}
			else if ( 0.50f <= substanceNormVal && substanceNormVal <= 0.60f )
			{
				return TileIdentity::MossStone;
			}
			else if ( 0.35f <= substanceNormVal && substanceNormVal <= 0.50f )
			{
				return TileIdentity::MossDirt;
			}
			else if ( 0.34f <= substanceNormVal && substanceNormVal <= 0.35f )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.30f <= substanceNormVal && substanceNormVal <= 0.34f )
			{
				return TileIdentity::MossDirt;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.30f )
			{
				return TileIdentity::Alfisol;
			}
		}
		else if ( 0.55f <= heightPercentage && heightPercentage <= 0.60f )
		{
			// Red clay, dirt, stone, charcoal
			if ( 0.95f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::MossDirt;
			}
			else if ( 0.90f <= substanceNormVal && substanceNormVal <= 0.95f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.850f <= substanceNormVal && substanceNormVal <= 0.90f )
			{
				return TileIdentity::MossStone;
			}
			else if ( 0.849f <= substanceNormVal && substanceNormVal <= 0.850f )
			{
				return TileIdentity::RedClay;
			}
			else if ( 0.815f <= substanceNormVal && substanceNormVal <= 0.849f )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.81f <= substanceNormVal && substanceNormVal <= 0.815f )
			{
				return TileIdentity::Alfisol;
			}
			else if ( 0.70f <= substanceNormVal && substanceNormVal <= 0.81f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.685f <= substanceNormVal && substanceNormVal <= 0.70f )
			{
				return TileIdentity::MossDirt;
			}
			else if ( 0.6f <= substanceNormVal && substanceNormVal <= 0.685f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.55f <= substanceNormVal && substanceNormVal <= 0.6f )
			{
				return TileIdentity::MossStone;
			}
			else if ( 0.54f <= substanceNormVal && substanceNormVal <= 0.55f )
			{
				return TileIdentity::RedClay;
			}
			else if ( 0.475f <= substanceNormVal && substanceNormVal <= 0.54f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.375 <= substanceNormVal && substanceNormVal <= 0.475f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.3725f <= substanceNormVal && substanceNormVal <= 0.375 )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.37f <= substanceNormVal && substanceNormVal <= 0.3725f )
			{
				return TileIdentity::Mor;
			}
			else if ( 0.25f <= substanceNormVal && substanceNormVal <= 0.37f )
			{
				return TileIdentity::Dirt;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.25f )
			{
				return TileIdentity::Stone;
			}
		}
		else if ( 0.40f <= heightPercentage && heightPercentage <= 0.55f )
		{
			// dirt, stone, light clay, charcoal
			if ( 0.875f <= substanceNormVal && substanceNormVal <= 1.0f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.8735f <= substanceNormVal && substanceNormVal <= 0.875f )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.80f <= substanceNormVal && substanceNormVal <= 0.8735f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.70f <= substanceNormVal && substanceNormVal <= 0.80f )
			{
				return TileIdentity::Dirt;
			}
			else if ( 0.67f <= substanceNormVal && substanceNormVal <= 0.70f )
			{
				return TileIdentity::LightClay;
			}
			else if ( 0.4f <= substanceNormVal && substanceNormVal <= 0.67f )
			{
				return TileIdentity::Stone;
			}
			else if ( 0.399f <= substanceNormVal && substanceNormVal <= 0.4f )
			{
				return TileIdentity::Charcoal;
			}
			else if ( 0.10f <= substanceNormVal && substanceNormVal <= 0.399f )
			{
				return TileIdentity::Dirt;
			}
			else // if ( 0.0f <= substanceNormVal && substanceNormVal <= 0.1f )
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