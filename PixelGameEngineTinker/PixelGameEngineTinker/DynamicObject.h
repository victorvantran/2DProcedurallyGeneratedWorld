#pragma once

#include "olcPixelGameEngine.h"
#include "AABB.h"

class DynamicObject
{
private:

protected:
	olc::v2d_generic<long double> _prevPosition;
	olc::v2d_generic<long double> _currPosition; // worldPosition
	
	bool _pushedRight;
	bool _pushingRight;

	bool _pushedLeft;
	bool _pushingLeft;

	bool _pushedDown; // wasOnGround
	bool _pushingDown; // onGround

	bool _pushedUp; // wasAtCeiling
	bool _pushingUp; // atCeiling


	olc::vf2d _prevVelocity;
	olc::vf2d _currVelocity;
	
	olc::vf2d _scale;

	AABB _aabb;
	olc::vf2d _aabbOffset;
public:
	DynamicObject() :
		_aabb(),
		_aabbOffset( olc::vf2d{ 0.0f, 0.0f } ), _scale( olc::vf2d{ 1.0, 1.0 } ),
		_prevPosition( olc::v2d_generic<long double>{ 0.0, 0.0 } ), _currPosition( olc::v2d_generic<long double>{ 0.0, 0.0 } ),
		_prevVelocity( olc::vf2d{ 0.0f, 0.0f } ), _currVelocity( olc::vf2d{ 0.0f, 0.0f } ),
		_pushedRight( false ), _pushingRight( false ),
		_pushedLeft( false ), _pushingLeft( false ),
		_pushedUp( false ), _pushingUp( false ),
		_pushedDown( false ), _pushingDown( false )
	{

	}

	DynamicObject( olc::v2d_generic<long double> center, olc::vf2d halfSize ) : 
		_aabb( center, halfSize ),
		_aabbOffset( olc::vf2d{ 0.0f, 0.0f } ), _scale( olc::vf2d{ 1.0, 1.0 } ),
		_prevPosition( olc::v2d_generic<long double>{ 0.0, 0.0 } ), _currPosition( olc::v2d_generic<long double>{ 0.0, 0.0 } ),
		_prevVelocity( olc::vf2d{ 0.0f, 0.0f } ), _currVelocity( olc::vf2d{ 0.0f, 0.0f } ),
		_pushedRight( false ), _pushingRight( false ),
		_pushedLeft( false ), _pushingLeft( false ),
		_pushedUp( false ), _pushingUp( false ),
		_pushedDown( false ), _pushingDown( false )
	{}

	~DynamicObject()
	{

	}


	// Getters
	olc::vf2d getScale() const
	{
		return this->_scale;
	}

	AABB getAABB() const
	{
		return this->_aabb;
	}

	olc::vf2d getAABBOffset() const
	{
		return this->_aabbOffset;
	}


	// Setters


	// Methods
	void updatePhysics( float deltaTime )
	{
		// Cache data of previous frame
		this->_prevPosition = this->_currPosition;

		this->_pushedRight = this->_pushingRight;
		this->_pushedLeft = this->_pushingLeft;
		this->_pushedDown = this->_pushedDown;
		this->_pushedUp = this->_pushingUp;

		this->_prevVelocity = this->_currVelocity;

		// Update position using the current speed
		this->_currPosition = this->_currPosition + ( olc::v2d_generic<long double>( this->_currVelocity.x, -this->_currVelocity.y ) * deltaTime );

		// Temp, if vertical position in world greater than zero assume character is on ground for testing purposes [!]
		if ( this->_currPosition.y > 0.0 ) // posY = worldNegY
		{
			this->_currPosition.y = 0.0;
			this->_pushingDown = true;
		}
		else
		{
			this->_pushingDown = false;
		}


		// Update AABB of object to match new position
		this->_aabb.setCenter( this->_currPosition + olc::v2d_generic<long double>( this->_aabbOffset.x, -this->_aabbOffset.y ) ); // negative because negY = worldPosY

		return;
	}
};