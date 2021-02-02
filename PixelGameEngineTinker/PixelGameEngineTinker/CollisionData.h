#pragma once

#include "olcPixelGameEngine.h"
#include "DynamicObject.h"

struct CollisionData
{
	DynamicObject* otherObject;
	olc::v2d_generic<long double> overlap;
	olc::vf2d velocity1;
	olc::vf2d velocity2;
	olc::v2d_generic<long double> prevPosition1;
	olc::v2d_generic<long double> prevPosition2;
	olc::v2d_generic<long double> currPosition1;
	olc::v2d_generic<long double> currPosition2;

	CollisionData()
		: otherObject( nullptr ), overlap(), velocity1(), velocity2(), prevPosition1(), prevPosition2(), currPosition1(), currPosition2()
	{

	}

	CollisionData( DynamicObject* otherObject,
		olc::v2d_generic<long double> overlap_,
		olc::vf2d velocity1_, olc::vf2d velocity2_,
		olc::v2d_generic<long double> prevPosition1_, olc::v2d_generic<long double> prevPosition2_,
		olc::v2d_generic<long double> currPosition1_, olc::v2d_generic<long double> currPosition2_ ) 
		: otherObject( otherObject ),
		overlap( overlap_ ), 
		velocity1( velocity1_ ), velocity2( velocity2_ ),
		prevPosition1( prevPosition1_ ), prevPosition2( prevPosition2_ ), 
		currPosition1( currPosition1_ ), currPosition2( currPosition2_ )
	{

	}

	~CollisionData()
	{

	}

};