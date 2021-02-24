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
		sunDecal( nullptr ), sunPosition( olc::vf2d{ 0.0f, 0.0f } ),
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

