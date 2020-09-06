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


		olc::vf2d getCurrPosition();
		olc::vf2d getSize();
		olc::vf2d getHalfSize();
		olc::vf2d getScale();
	};
}


aabb::DynamicObject::DynamicObject()
{

}

aabb::DynamicObject::~DynamicObject()
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


	/// Update center position
	this->_aabb.center = this->_currPosition + this->_aabbOffset;


	return;
}



olc::vf2d aabb::DynamicObject::getCurrPosition()
{
	return this->_currPosition;
}


olc::vf2d aabb::DynamicObject::getSize()
{
	return this->_aabb.halfSize * 2.0f;
}


olc::vf2d aabb::DynamicObject::getHalfSize()
{
	return this->_aabb.halfSize;
}


olc::vf2d aabb::DynamicObject::getScale()
{
	return this->_scale;
}