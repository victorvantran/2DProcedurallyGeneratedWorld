#pragma once

#include "olcPixelGameEngine.h"
#include "AABB.h"

namespace aabb
{
	class DynamicObject
	{
	protected:
		olc::vf2d _prevPosition;
		olc::vf2d _currPosition;

		olc::vf2d _prevVelocity;
		olc::vf2d _currVelocity;

		olc::vf2d _scale;


		AABB _aabb;
		olc::vf2d _aabbOffset; /// needed to align AABB to anything such as an object's sprite

		/// Position state of object
		bool _prevPushUp;
		bool _currPushUp;
		bool _prevPushDown;
		bool _currPushDown;
		bool _prevPushLeft;
		bool _currPushLeft;
		bool _prevPushRight;
		bool _currPushRight;


	public:
		DynamicObject();
		~DynamicObject();

		void updatePhysics( float deltaTime );

	};
}


aabb::DynamicObject::DynamicObject()
{

}

aabb::DynamicObject::DynamicObject()
{

}

void aabb::DynamicObject::updatePhysics( float deltaTime )
{
	this->_prevPosition = this->_currPosition;
	this->_prevVelocity = this->_currVelocity;

	this->_prevPushUp = this->_currPushUp;
	this->_prevPushDown = this->_currPushDown;
	this->_prevPushLeft = this->_currPushLeft;
	this->_prevPushRight = this->_currPushRight;


	this->_currPosition = this->_currPosition + this->_currVelocity * deltaTime;

	/// Temporary
	if ( this->_currPosition.y < 0.0f )
	{
		this->_currPosition.y = 0.0f;
		this->_currPushDown = true;
	}
	else
	{
		this->_currPushDown = false;
	}


	/// Update center position
	this->_aabb.center = this->_currPosition + this->_aabbOffset;


	return;
}