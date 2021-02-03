#pragma once

#include "olcPixelGameEngine.h"
#include <cmath>

class AABB
{
private:
	olc::v2d_generic<long double> _center;
	olc::vf2d _halfSize;
	olc::vf2d _scale;
public:

	// Constructors/Destructor
	AABB() : _center( olc::v2d_generic<long double>{ 0.0, 0.0 } ), _halfSize( olc::vf2d{ 0.0f, 0.0f } ), _scale( olc::vf2d{ 0.0f, 0.0f } ) {}
	AABB( const olc::v2d_generic<long double>& center, const olc::vf2d& halfSize, const olc::vf2d& scale ) : _center( center ), _halfSize( halfSize ), _scale( scale ) {}
	~AABB() {}


	// Getters
	olc::v2d_generic<long double> getCenter() const
	{
		return this->_center;
	}

	olc::vf2d getHalfSize() const
	{
		return this->_halfSize * this->_scale;
	}

	float getHalfSizeX() const
	{
		return this->_halfSize.x * this->_scale.x;
	}
	
	float getHalfSizeY() const
	{
		return this->_halfSize.y * this->_scale.y;
	}

	olc::vf2d getScale() const
	{
		return this->_scale;
	}

	float getScaleX() const
	{
		return this->_scale.x;
	}

	float getScaleY() const
	{
		return this->_scale.y;
	}


	olc::v2d_generic<long double> getTopLeft() const
	{
		return this->_center - olc::v2d_generic<long double>{ ( long double )this->getHalfSizeX(), ( long double )this->getHalfSizeY() };
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


	void setHalfSizeX( float x )
	{
		this->_halfSize.x = x;
		return;
	}


	void setHalfSizeY( float y )
	{
		this->_halfSize.y = y;
		return;
	}


	void setScale( const olc::vf2d& scale )
	{
		this->_scale = olc::vf2d{ std::abs( scale.x ), std::abs( scale.y ) };
		return;
	}


	void setScaleX( float x )
	{
		this->_scale.x = std::abs( x );
		return;
	}


	void setScaleY( float y )
	{
		this->_scale.y = std::abs( y );
		return;
	}

	// Methods
	bool overlaps( const AABB& otherAABB )
	{
		if ( std::abs( this->_center.x - otherAABB.getCenter().x ) > ( ( long double )this->getHalfSizeX() + otherAABB.getHalfSizeX() ) ) return false;
		if ( std::abs( this->_center.y - otherAABB.getCenter().y ) > ( ( long double )this->getHalfSizeY() + otherAABB.getHalfSizeY() ) ) return false;
		return true;
	}

	bool overlapsSigned( const AABB& otherAABB, olc::v2d_generic<long double>& overlap )
	{
		overlap = olc::v2d_generic<long double>{ 0.0, 0.0 };
		if ( ( this->_halfSize.x == 0.0f || this->_halfSize.y == 0.0f || otherAABB.getHalfSizeX() == 0.0f || otherAABB.getHalfSizeY() == 0.0f ) ||
			( std::abs( this->_center.x - otherAABB.getCenter().x ) > ( ( long double )this->getHalfSizeX() + otherAABB.getHalfSizeX() ) ) ||
			( std::abs( this->_center.y - otherAABB.getCenter().y ) > ( ( long double )this->getHalfSizeY() + otherAABB.getHalfSizeY() ) ) )
		{
			return false;
		}


		/*
		if ( this->_halfSize.x == 0.0f || this->_halfSize.y == 0.0f || otherAABB.getHalfSizeX() == 0.0f || otherAABB.getHalfSizeY() == 0.0f ) return false;
		if ( std::abs( this->_center.x - otherAABB.getCenter().x ) > ( ( long double )this->getHalfSizeX() + otherAABB.getHalfSizeX() ) ) return false;
		if ( std::abs( this->_center.y - otherAABB.getCenter().y ) > ( ( long double )this->getHalfSizeY() + otherAABB.getHalfSizeY() ) ) return false;
		*/

		
		overlap = olc::v2d_generic<long double>{
			( std::signbit( this->_center.x - otherAABB.getCenter().x ) ? -1 : 1 ) * ( ( this->_halfSize.x + otherAABB.getHalfSizeX() ) - std::abs( this->_center.x - otherAABB.getCenter().x ) ),
			( std::signbit( this->_center.y - otherAABB.getCenter().y ) ? -1 : 1 ) * ( ( this->_halfSize.y + otherAABB.getHalfSizeY() ) - std::abs( this->_center.y - otherAABB.getCenter().y ) )
		};
		


		return true;
	}
};