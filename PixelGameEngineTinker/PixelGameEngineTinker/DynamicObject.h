#pragma once

#include "olcPixelGameEngine.h"
#include "AABB.h"
#include "WorldChunk.h"
#include "World.h"

namespace aabb
{
	class DynamicObject
	{
	protected:
		olc::vf2d _prevCenterPosition;
		olc::vf2d _currCenterPosition; // [!] position needs to be normalized

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

		void updatePhysics( float deltaTime, World& world );


		olc::vf2d getCurrPosition();
		olc::vf2d getSize();
		olc::vf2d getHalfSize();
		olc::vf2d getOffset();
		olc::vf2d getScale();


		/// Object detection methods (tile-based using boundary senors)
		bool isCollidingDown( olc::vf2d prevPosition, olc::vf2d currPosition, olc::vf2d speed, World& world, float& contactY );
	};
}


aabb::DynamicObject::DynamicObject()
{

}

aabb::DynamicObject::~DynamicObject()
{

}

void aabb::DynamicObject::updatePhysics( float deltaTime, World& world )
{
	this->_prevCenterPosition = this->_currCenterPosition;
	this->_prevVelocity = this->_currVelocity;

	this->_prevPushUp = this->_currPushUp;
	this->_prevPushDown = this->_currPushDown;
	this->_prevPushLeft = this->_currPushLeft;
	this->_prevPushRight = this->_currPushRight;
	
	float contactY = 0.0f;
	if ( this->_currVelocity.y > 0.0f && this->isCollidingDown( this->_prevCenterPosition, this->_currCenterPosition, this->_currVelocity, world, contactY ) )
	{
		this->_currCenterPosition.y = contactY - this->getHalfSize().y + this->getOffset().y;
		this->_currVelocity.y = 0.0f;
		this->_currPushDown = true;
	}
	else
	{
		this->_currPushDown = false;
	}
	

	this->_currCenterPosition = this->_currCenterPosition + this->_currVelocity * deltaTime;


	/// Update center position
	this->_aabb.center = this->_currCenterPosition + this->_aabbOffset;


	return;
}



olc::vf2d aabb::DynamicObject::getCurrPosition()
{
	return this->_currCenterPosition;
}


olc::vf2d aabb::DynamicObject::getSize()
{
	return this->_aabb.halfSize * 2.0f;
}


olc::vf2d aabb::DynamicObject::getHalfSize()
{
	return this->_aabb.halfSize;
}

olc::vf2d aabb::DynamicObject::getOffset()
{
	return this->_aabbOffset;
}


olc::vf2d aabb::DynamicObject::getScale()
{
	return this->_scale;
}


// need a region of checking
bool aabb::DynamicObject::isCollidingDown( olc::vf2d prevPosition, olc::vf2d currPosition, olc::vf2d speed, World& world, float& contactY )
{
/// If collided downwards (like hitting a ground), calculate the beginning point and end point of the bottom sensor line. Note that it is the index of the cell one pixel below and one pixel short on each side
/// Updates the groundLevel where the contact occured

	olc::vf2d center = currPosition + this->_aabbOffset;
	olc::vf2d bottomLeft = center + this->getHalfSize() - olc::vf2d{ this->getHalfSize().x * 2.0f, 0.0f } +olc::vf2d{ 1.0f / world.getTileDimension().x, 1.0f / world.getTileDimension().y }; // [!] may need to divided by 8.0 (the tile dimension)
	olc::vf2d bottomRight = center + this->getHalfSize() - olc::vf2d{ 1.0f / world.getTileDimension().x, -1.0f / world.getTileDimension().y };////olc::vf2d( bottomLeft.x + this->getHalfSize().x * 2.0f - ( 2.0f / world.getTileDimension().x ), bottomLeft.y );

	olc::vi2d bottomLeftIndex = olc::vi2d{ (int)bottomLeft.x, ( int )bottomLeft.y };
	olc::vi2d bottomRightIndex = olc::vi2d{ ( int )bottomRight.x, ( int )bottomRight.y };

	// Based on the sensors, we can get the position of tiles needed to check
	Tile* checkTile;
	olc::vi2d checkTileIndex;
	WorldChunk* worldChunk;



	for ( checkTileIndex = bottomLeftIndex; checkTileIndex.x <= bottomRightIndex.x; checkTileIndex.x += 1.0f )
	{
		worldChunk = world.getWorldChunkFromIndex( checkTileIndex );
		if ( worldChunk == nullptr )
		{
			return true; // [!] Raise exception for trying to touch an index on a tile that does not exist because the worldchunk does not exist
		}

		// Calculate the potential bottom contact point
		contactY = ( float )checkTileIndex.y;
		checkTile = world.getTileFromIndex( checkTileIndex );

		if ( checkTile != nullptr && checkTile->isBlock() )
		{
			//std::cout << checkTileIndex << std::endl;
			return true;
		}
	}


	return false;
}