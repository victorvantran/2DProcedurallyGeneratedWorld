#pragma once

#include "olcPixelGameEngine.h"
/// With credits to Daniel Branicki and his tutorials

namespace aabb
{
	struct AABB
	{
		olc::vf2d center;
		olc::vf2d halfSize;


		AABB();
		~AABB();

		AABB( const olc::vf2d& center, const olc::vf2d& halfSize );

		bool overlaps( AABB target );
	};


	AABB::AABB()
	{

	}

	AABB::~AABB()
	{

	}

	AABB::AABB( const olc::vf2d& center, const olc::vf2d& halfSize )
	{
		this->center = center;
		this->halfSize = halfSize;
	}


	bool AABB::overlaps( AABB target )
	{
		if ( std::abs( this->center.x - target.center.x ) > ( this->halfSize.x + target.halfSize.x ) ) return false;
		if ( std::abs( this->center.y - target.center.y ) > ( this->halfSize.y + target.halfSize.y ) ) return false;
		return true;
	}
}