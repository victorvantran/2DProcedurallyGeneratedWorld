#pragma once

#include "olcPixelGameEngine.h"
#include <cmath>

class AABB
{
private:
	olc::v2d_generic<long double> _center;
	olc::vf2d _halfSize;
public:

	// Constructors/Destructor
	AABB() : _center( olc::v2d_generic<long double>{ 0.0, 0.0 } ), _halfSize( olc::v2d_generic<long double>{ 0.0, 0.0 } ) {}
	AABB( const olc::v2d_generic<long double>& center, const olc::vf2d& halfSize ) : _center( center ), _halfSize( halfSize ) {}
	~AABB() {}


	// Getters
	olc::v2d_generic<long double> getCenter() const
	{
		return this->_center;
	}

	olc::vf2d getHalfSize() const
	{
		return this->_halfSize;
	}

	olc::v2d_generic<long double> getTopLeft() const
	{
		return this->_center - olc::v2d_generic<long double>{ ( long double )this->_halfSize.x, ( long double )this->_halfSize.y };
	}


	// Setters
	void setCenter( const olc::v2d_generic<long double>& center )
	{
		this->_center = center;
		return;
	}


	void setHalfSize( const olc::vf2d& halfSize )
	{
		this->_halfSize = halfSize;
		return;
	}

	// Methods
	bool overlaps( const AABB& otherAABB )
	{
		if ( std::abs( this->_center.x - otherAABB.getCenter().x ) > ( ( long double )this->_halfSize.x + otherAABB.getHalfSize().x ) ) return false;
		if ( std::abs( this->_center.y - otherAABB.getCenter().y ) > ( ( long double )this->_halfSize.y + otherAABB.getHalfSize().y ) ) return false;
		return true;
	}


};