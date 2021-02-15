#pragma once

#include "Biome.h"
#include "Tile.h"
// https://cforest.weebly.com/uploads/1/3/4/9/13495202/7326878.gif?1348042161
// [!] bleached sand , clay and iron oxide

class TemperateSeasonalForest : public Biome
{
private:

public:
	TemperateSeasonalForest() {}
	TemperateSeasonalForest( const FoliageMap& foliageMap, const std::pair<long double, long double>& temperatureRange, const std::pair<long double, long double>& precipitationRange ) :
		Biome( foliageMap, temperatureRange, precipitationRange ) {}
	~TemperateSeasonalForest() {}


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



	FoliageIdentity getFoliage( std::int64_t tileX, long double temperatureNormalizedValue, long double precipitationNormalizedValue )
	{
		long double foliagePerlinValue = this->_foliageMap.getPerlinValue( tileX );
		long double foliageNormalizedValue = Biome::normalizeHistogram( foliagePerlinValue );

		long double temperatureLocalNormalizedValue = this->getTemperatureLocalNormalizedValue( temperatureNormalizedValue );
		long double precipitationLocalNormalizedValue = this->getPrecipitationLocalNormalizedValue( precipitationNormalizedValue );

		std::uint32_t random = Biome::randomHash32( std::uint32_t( tileX ) );
		std::uint32_t chance = random % 1000;

		std::uint8_t size = 0;
		std::uint8_t color = 0;
		std::uint8_t variant = ( std::uint8_t )( ( random ) % 100 );


		// Maple Tree
		if ( chance <= 100 &&
			( ( 0.85 <= foliageNormalizedValue && foliageNormalizedValue <= 1.0 ) ||
				( 0.55 <= foliageNormalizedValue && foliageNormalizedValue <= 0.60 ) ||
				( 0.25 <= foliageNormalizedValue && foliageNormalizedValue <= 0.40 ) )
			)
		{
			// Size
			if ( 0.8 <= precipitationLocalNormalizedValue )
			{
				size = 2;
			}
			else if ( 0.55 <= precipitationLocalNormalizedValue && precipitationLocalNormalizedValue <= 0.8 )
			{
				size = 1;
			}
			else // if ( precipitationLocalNormalizedValue <= 0.6 )
			{
				size = 0;
			}

			// Color
			if ( 0.7 <= temperatureLocalNormalizedValue && temperatureLocalNormalizedValue <= 1.0 )
			{
				color = 0;
			}
			else if ( 0.5 <= temperatureLocalNormalizedValue && temperatureLocalNormalizedValue <= 0.7 )
			{
				color = ( ( std::int32_t )( foliageNormalizedValue * 10 ) % 2 == 0 ) ? 0 : 1;
			}
			else if ( 0.4 <= temperatureLocalNormalizedValue && temperatureLocalNormalizedValue <= 0.5 )
			{
				color = 1;
			}
			else if ( 0.2 <= temperatureLocalNormalizedValue && temperatureLocalNormalizedValue <= 0.4 )
			{
				color = ( ( std::int32_t )( foliageNormalizedValue * 10 ) % 2 == 0 ) ? 1 : 2;
			}
			else //  if ( 0.0 <= temperatureNormalizedValue && temperatureNormalizedValue <= 0.2 )
			{
				color = 2;
			}

			// Chance
			if ( ( size == 0 && chance <= 20 ) ||
				( size == 1 && chance <= 5 ) ||
				( size == 2 && chance <= 5 )
				)
			{
				return Foliage::getMapleTree( size, color, variant );
			}
			else
			{
				return FoliageIdentity::Void;

			}
		}

		// Elm
		else if ( 100 <= chance && chance <= 200 && ( 0.0 <= foliageNormalizedValue && foliageNormalizedValue <= 1.0 )
			)

		{
			// Size
			if ( 0.7 <= precipitationLocalNormalizedValue && precipitationLocalNormalizedValue <= 1.0 )
			{
				size = 1;
			}
			else // if ( 0.0 <= temperatureLocalNormalizedValue && temperatureLocalNormalizedValue <= 0.7 )
			{
				size = 0;
			}


			// Chance
			if (
				( 0.45 <= temperatureLocalNormalizedValue && temperatureLocalNormalizedValue <= 1.0 ) &&
				(
					( size == 0 && temperatureLocalNormalizedValue >= 0.75 && chance <= 120 ) ||
					( size == 0 && temperatureLocalNormalizedValue <= 0.75 && chance <= 110 ) ||
					( size == 1 && temperatureLocalNormalizedValue >= 0.75 && chance <= 110 ) ||
					( size == 1 && temperatureLocalNormalizedValue <= 0.75 && chance <= 105 )
					)
				)
			{
				return Foliage::getElmTree( size, variant );
			}
			else
			{
				return FoliageIdentity::Void;

			}

		}


		// Hickory
		else if ( 200 <= chance && chance <= 300 && ( 0.0 <= foliageNormalizedValue && foliageNormalizedValue <= 1.0 )
			)

		{
			// Size
			if ( 0.7 <= precipitationLocalNormalizedValue && precipitationLocalNormalizedValue <= 1.0 )
			{
				size = 1;
			}
			else // if ( 0.0 <= temperatureLocalNormalizedValue && temperatureLocalNormalizedValue <= 0.7 )
			{
				size = 0;
			}


			// Chance
			if (
				( 0.0 <= temperatureLocalNormalizedValue && temperatureLocalNormalizedValue <= 0.75 ) &&
				(
					( size == 0 && temperatureLocalNormalizedValue >= 0.45 && chance <= 240 ) ||
					( size == 0 && temperatureLocalNormalizedValue <= 0.45 && chance <= 220 ) ||
					( size == 1 && temperatureLocalNormalizedValue >= 0.45 && chance <= 220 ) ||
					( size == 1 && temperatureLocalNormalizedValue <= 0.45 && chance <= 210 )
					)
				)
			{
				return Foliage::getHickoryTree( size, variant );
			}
			else
			{
				return FoliageIdentity::Void;

			}

		}

		// Fern
		else if ( 300 <= chance && chance <= 700 &&
			(
				( 0.8 <= foliageNormalizedValue && foliageNormalizedValue <= 1.0 ) ||
				( 0.45 <= foliageNormalizedValue && foliageNormalizedValue <= 0.6 ) ||
				( 0.1 <= foliageNormalizedValue && foliageNormalizedValue <= 0.35 )
				)
			)

		{
			// Size
			if ( 0.7 <= precipitationLocalNormalizedValue && precipitationLocalNormalizedValue <= 1.0 )
			{
				size = 1;
			}
			else // if ( 0.0 <= temperatureLocalNormalizedValue && temperatureLocalNormalizedValue <= 0.7 )
			{
				size = 0;
			}

			// Chance
			if (
				( size == 0 && chance <= 550 ) ||
				( size == 1 && chance <= 325 )
				)
			{
				return Foliage::getFern( size );
			}
		}

		return FoliageIdentity::Void;
	}

};


