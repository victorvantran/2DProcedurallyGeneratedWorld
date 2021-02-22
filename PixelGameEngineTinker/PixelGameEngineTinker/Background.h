#pragma once

#include "olcPixelGameEngine.h"


struct BackgroundRenderData
{
	olc::Decal* dayDecal;
	olc::Pixel dayTint;

	olc::Decal* nightDecal;
	olc::Pixel nightTint;

	olc::Decal* sunDecal;
	olc::vf2d sunPosition;

	olc::Decal* landscapeDecal;
	olc::Pixel landscapeTint;


	BackgroundRenderData() :
		dayDecal( nullptr ), dayTint( olc::Pixel{ 0, 0, 0, 255 } ),
		nightDecal( nullptr ), nightTint( olc::Pixel{ 0, 0, 0, 255 } ),
		sunDecal( nullptr ), sunPosition( olc::vf2d{ 0.0f, 0.0f }  ),
		landscapeDecal( nullptr ), landscapeTint( olc::Pixel{ 0, 0, 0, 255 } )
	{}


	BackgroundRenderData(
		olc::Decal* dayDecal_, olc::Pixel dayTint_,
		olc::Decal* nightDecal_, olc::Pixel nightTint_,
		olc::Decal* sunDecal_, olc::vf2d sunPosition_,
		olc::Decal* landscapeDecal_, olc::Pixel landscapeTint_
	) :
		dayDecal( dayDecal_ ), dayTint( dayTint_ ),
		nightDecal( nightDecal_ ), nightTint( nightTint_ ),
		sunDecal( sunDecal_ ), sunPosition( sunPosition_ ),
		landscapeDecal( landscapeDecal_ ), landscapeTint( landscapeTint_ )
	{}


	~BackgroundRenderData() {}

};



class Background
{
private:
	olc::Sprite* _daySprite;
	olc::Decal* _dayDecal;

	olc::Sprite* _nightSprite;
	olc::Decal* _nightDecal;

	olc::Sprite* _sunSprite;
	olc::Decal* _sunDecal;

	olc::Sprite* _landscapeSprite;
	olc::Decal* _landscapeDecal;

	const float& _worldSecond;
	const std::uint16_t& _worldDay;
	const std::uint64_t& _worldYear;

public:
	Background() = delete; // Cannot use default constructor for intializing member reference variables 

	Background( 
		olc::Sprite* daySprite, olc::Decal* dayDecal,
		olc::Sprite* nightSprite, olc::Decal* nightDecal,
		olc::Sprite* sunSprite, olc::Decal* sunDecal,
		olc::Sprite* landscapeSprite, olc::Decal* landscapeDecal,
		float& worldSecond, std::uint16_t& worldDay, std::uint64_t& worldYear
		) :
		_daySprite( daySprite ), _dayDecal( dayDecal ),
		_nightSprite( nightSprite ), _nightDecal( nightDecal ),
		_sunSprite( sunSprite ), _sunDecal( sunDecal ),
		_landscapeSprite( landscapeSprite ), _landscapeDecal( landscapeDecal ),
		_worldSecond( worldSecond ), _worldDay( worldDay ), _worldYear( worldYear )
	{}


	~Background() {}


	static olc::Pixel getDayTint( float worldSecond ) 
	{
		// Return the day tint information to establish ambient lighting
		if ( worldSecond >= 21600 && worldSecond < 39600 ) // Morning
		{
			float difference = worldSecond - 21600;
			float absoluteDifference = 18000;
			float p = difference / ( absoluteDifference - 1 );

			return olc::Pixel{
				( std::uint8_t )( ( 1.0 - p ) * 4 + p * 255 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 4 + p * 255 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 9 + p * 255 + 0.5 ),
				( std::uint8_t )255
			};
		}
		else if ( worldSecond >= 39600 && worldSecond < 61200 ) // Afternoon
		{
			float difference = worldSecond - 39600;
			float absoluteDifference = 21600;
			float p = difference / ( absoluteDifference - 1 );

			return olc::Pixel{
				( std::uint8_t )( ( 1.0 - p ) * 255 + p * 146 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 255 + p * 50 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 255 + p * 50 + 0.5 ),
				( std::uint8_t )255
			};
		}
		else if ( worldSecond >= 61200 && worldSecond < 75600 ) // Evening
		{
			float difference = worldSecond - 61200;
			float absoluteDifference = 14400;
			float p = difference / ( absoluteDifference - 1 );

			return olc::Pixel{
				( std::uint8_t )( ( 1.0 - p ) * 146 + p * 4 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 50 + p * 4 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 50 + p * 9 + 0.5 ),
				( std::uint8_t )255
			};
		}
		else if ( ( worldSecond >= 75600 && worldSecond < 86400 ) || ( worldSecond >= 0 && worldSecond < 21600 ) ) // Night
		{
			float unwrappedSecond = ( worldSecond >= 0 && worldSecond < 21600 ) ? worldSecond + 86400 : worldSecond;
			float difference = unwrappedSecond - 75600;
			float absoluteDifference = 32400;
			float p = difference / ( absoluteDifference - 1 );

			return olc::Pixel{
				( std::uint8_t )( ( 1.0 - p ) * 4 + p * 1 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 4 + p * 1 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 9 + p * 1 + 0.5 ),
				( std::uint8_t )255
			};
		}
		else
		{
			return olc::Pixel{ 0, 0, 0, 0 };
		}
	}



	static olc::Pixel getNightTint( float worldSecond )
	{
		// Return the night tint information to establish ambient lighting
		if ( worldSecond >= 21600 && worldSecond < 25200 ) // Morning
		{
			float difference = worldSecond - 21600;
			float absoluteDifference = 3600;
			float p = difference / ( absoluteDifference - 1 );

			return olc::Pixel{
				( std::uint8_t )( 255 ),
				( std::uint8_t )( 255 ),
				( std::uint8_t )( 255 ),
				( std::uint8_t )( ( 1.0 - p ) * 255 + p * 0 + 0.5 ),
			};
		}
		else if ( worldSecond >= 25200 && worldSecond < 64800 ) // Afternoon
		{
			float difference = worldSecond - 25200;
			float absoluteDifference = 39600;
			float p = difference / ( absoluteDifference - 1 );

			return olc::Pixel{
				( std::uint8_t )( 0 ),
				( std::uint8_t )( 0 ),
				( std::uint8_t )( 0 ),
				( std::uint8_t )0
			};
		}
		else if ( worldSecond >= 64800 && worldSecond < 75600 ) // Evening
		{
			float difference = worldSecond - 64800;
			float absoluteDifference = 10800;
			float p = difference / ( absoluteDifference - 1 );

			return olc::Pixel{
				( std::uint8_t )( 255 ),
				( std::uint8_t )( 255 ),
				( std::uint8_t )( 255 ),
				( std::uint8_t )( ( 1.0 - p ) * 0 + p * 255 + 0.5 ),
			};
		}
		else if ( ( worldSecond >= 75600 && worldSecond < 86400 ) || ( worldSecond >= 0 && worldSecond < 21600 ) ) // Night
		{
			float unwrappedSecond = ( worldSecond >= 0 && worldSecond < 21600 ) ? worldSecond + 86400 : worldSecond;
			float difference = unwrappedSecond - 75600;
			float absoluteDifference = 32400;
			float p = difference / ( absoluteDifference - 1 );

			return olc::Pixel{
				( std::uint8_t )( 255 ),
				( std::uint8_t )( 255 ),
				( std::uint8_t )( 255 ),
				( std::uint8_t )255
			};
		}
		else
		{
			return olc::Pixel{ 0, 0, 0, 0 };
		}
	}


	/*
	static olc::Pixel getNightTint( float worldSecond )
	{
		// Return the night tint information to establish ambient lighting
		if ( worldSecond >= 21600 && worldSecond < 39600 ) // Morning
		{
			float difference = worldSecond - 21600;
			float absoluteDifference = 18000;
			float p = difference / ( absoluteDifference - 1 );

			return olc::Pixel{
				( std::uint8_t )( 255 ),
				( std::uint8_t )( 255 ),
				( std::uint8_t )( 255 ),
				( std::uint8_t )( ( 1.0 - p ) * 255 + p * 0 + 0.5 ),
			};
		}
		else if ( worldSecond >= 39600 && worldSecond < 61200 ) // Afternoon
		{
			float difference = worldSecond - 39600;
			float absoluteDifference = 21600;
			float p = difference / ( absoluteDifference - 1 );

			return olc::Pixel{
				( std::uint8_t )( 0 ),
				( std::uint8_t )( 0 ),
				( std::uint8_t )( 0 ),
				( std::uint8_t )0
			};
		}
		else if ( worldSecond >= 61200 && worldSecond < 75600 ) // Evening
		{
			float difference = worldSecond - 61200;
			float absoluteDifference = 14400;
			float p = difference / ( absoluteDifference - 1 );

			return olc::Pixel{
				( std::uint8_t )( 255 ),
				( std::uint8_t )( 255 ),
				( std::uint8_t )( 255 ),
				( std::uint8_t )( ( 1.0 - p ) * 0 + p * 255 + 0.5 ),
			};
		}
		else if ( ( worldSecond >= 75600 && worldSecond < 86400 ) || ( worldSecond >= 0 && worldSecond < 21600 ) ) // Night
		{
			float unwrappedSecond = ( worldSecond >= 0 && worldSecond < 21600 ) ? worldSecond + 86400 : worldSecond;
			float difference = unwrappedSecond - 75600;
			float absoluteDifference = 32400;
			float p = difference / ( absoluteDifference - 1 );

			return olc::Pixel{
				( std::uint8_t )( 255 ),
				( std::uint8_t )( 255 ),
				( std::uint8_t )( 255 ),
				( std::uint8_t )255
			};
		}
		else
		{
			return olc::Pixel{ 0, 0, 0, 0 };
		}
	}
	*/

	static olc::vf2d getSunPosition( float worldSecond )
	{
		// Return sun position for render
		float difference = worldSecond - 10800;
		float absoluteDifference = 75600;
		float p = difference / ( absoluteDifference - 1 );

		float x = ( 1.0f - p ) * 0.0f + p * 1980.0f + 0.5f;
		//float y = 0.0000007430269776f * worldSecond * worldSecond - 0.0641975308641982f * worldSecond + 1456.6666666666742458f;
		float y = 0.0000007811309252f * worldSecond * worldSecond - 0.0674897119341570f * worldSecond + 1487.7777777777855590f;
		return olc::vf2d{ x, y };
	}


	static olc::Pixel getLandscapeTint( float worldSecond )
	{
		// Return the landscape tint information to establish ambient lighting
		if ( worldSecond >= 21600 && worldSecond < 39600 ) // Morning
		{
			float difference = worldSecond - 21600;
			float absoluteDifference = 18000;
			float p = difference / ( absoluteDifference - 1 );

			return olc::Pixel{
				( std::uint8_t )( ( 1.0 - p ) * 4 + p * 255 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 4 + p * 255 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 9 + p * 255 + 0.5 ),
				( std::uint8_t )255
			};
		}
		else if ( worldSecond >= 39600 && worldSecond < 61200 ) // Afternoon
		{
			float difference = worldSecond - 39600;
			float absoluteDifference = 21600;
			float p = difference / ( absoluteDifference - 1 );

			return olc::Pixel{
				( std::uint8_t )( ( 1.0 - p ) * 255 + p * 146 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 255 + p * 50 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 255 + p * 50 + 0.5 ),
				( std::uint8_t )255
			};
		}
		else if ( worldSecond >= 61200 && worldSecond < 75600 ) // Evening
		{
			float difference = worldSecond - 61200;
			float absoluteDifference = 14400;
			float p = difference / ( absoluteDifference - 1 );

			return olc::Pixel{
				( std::uint8_t )( ( 1.0 - p ) * 146 + p * 4 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 50 + p * 4 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 50 + p * 9 + 0.5 ),
				( std::uint8_t )255
			};
		}
		else if ( ( worldSecond >= 75600 && worldSecond < 86400 ) || ( worldSecond >= 0 && worldSecond < 21600 ) ) // Night
		{
			float unwrappedSecond = ( worldSecond >= 0 && worldSecond < 21600 ) ? worldSecond + 86400 : worldSecond;
			float difference = unwrappedSecond - 75600;
			float absoluteDifference = 32400;
			float p = difference / ( absoluteDifference - 1 );

			return olc::Pixel{
				( std::uint8_t )( ( 1.0 - p ) * 4 + p * 1 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 4 + p * 1 + 0.5 ),
				( std::uint8_t )( ( 1.0 - p ) * 9 + p * 1 + 0.5 ),
				( std::uint8_t )255
			};
		}
		else
		{
			return olc::Pixel{ 0, 0, 0, 0 };
		}
	}


	const BackgroundRenderData& getRenderData() const
	{
		return BackgroundRenderData(
			this->_dayDecal, this->getDayTint( this->_worldSecond ),
			this->_nightDecal, this->getNightTint( this->_worldSecond ),
			this->_sunDecal, this->getSunPosition( this->_worldSecond ),
			this->_landscapeDecal, this->getLandscapeTint( this->_worldSecond )
		);
	}
};