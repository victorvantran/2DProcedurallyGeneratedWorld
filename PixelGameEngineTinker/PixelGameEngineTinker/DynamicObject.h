#pragma once

#include "olcPixelGameEngine.h"
#include "AABB.h"
#include "WorldChunk.h"

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
		olc::vf2d getOffset();
		olc::vf2d getScale();


		/// Object detection methods (tile-based using boundary senors)
		bool isCollidingDown( olc::vf2d prevPosition, olc::vf2d currPosition, olc::vf2d speed, float& groundY, WorldChunk& worldChunk );
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

olc::vf2d aabb::DynamicObject::getOffset()
{
	return this->_aabbOffset;
}


olc::vf2d aabb::DynamicObject::getScale()
{
	return this->_scale;
}


// need a region of checking
bool aabb::DynamicObject::isCollidingDown( olc::vf2d prevPosition, olc::vf2d currPosition, olc::vf2d speed, float& contactY, WorldChunk& worldChunk )
{
/// If collided downwards (like hitting a ground), calculate the beginning point and end point of the bottom sensor line. Note that it is one pixel below and one pixel short on each side
/// Updates the groundLevel where the contact occured
	olc::vf2d center = currPosition + this->_aabbOffset;
	olc::vf2d bottomLeft = center - this->getHalfSize() + olc::vf2d{ 1.0f, 1.0f };
	olc::vf2d bottomRight = olc::vf2d( bottomLeft.x + this->getHalfSize().x * 2.0f - 2.0f, bottomLeft.y );

	// Based on the sensors, we can get the position of tiles needed to check

	// [!] settings::ATLAS::TILE_DIMENSION is temporary; the dynamic object needs to know the tile dimension given by either by a world, worldchunk, or layer to reference
	olc::vi2d checkTileIndex;
	Tile* checkTile;

	for ( olc::vf2d checkTilePosition = bottomLeft; checkTilePosition.x >= bottomRight.x; checkTilePosition.x += settings::ATLAS::TILE_DIMENSION.x )
	{
		checkTilePosition.x = std::min<float>( checkTilePosition.x, bottomRight.x); // Make sure our checked tile is not out of range of the bottomRight endpoint

		checkTileIndex = worldChunk.getIndexFromPixelPosition( checkTilePosition );

		// Calculate the potential bottom contact point
		contactY = ( float )checkTileIndex.y * worldChunk.getAtlas().getTileDimension().y + worldChunk.getAtlas().getTileDimension().y / 2.0f + worldChunk.getPosition().y;

		checkTile = worldChunk.getTileFromPixelPosition( checkTilePosition );

		if ( checkTile != nullptr && checkTile->isBlock() )
		{
			return true;
		}
	}

	return false;
}