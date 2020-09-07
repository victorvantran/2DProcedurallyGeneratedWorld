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
		olc::vf2d _currCenterPosition;

		olc::vf2d _prevVelocity;
		olc::vf2d _currVelocity;

		olc::vf2d _scale;

		
		float _oneWayPlatformThreshold;
		bool _onOneWayPlatform;



		AABB _aabb;
		olc::vf2d _aabbOffset; // needed to align AABB to anything such as an object's sprite

		// Position state of object
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


		void constructDynamicObject();
		void destructDynamicObject();

		void updatePhysics( float deltaTime, World& world );


		olc::vf2d getCurrPosition();
		olc::vf2d getSize();
		olc::vf2d getHalfSize();
		olc::vf2d getOffset();
		olc::vf2d getScale();


		/// Object detection methods (tile-based using boundary senors)
		bool isCollidingUp( olc::vf2d prevPosition, olc::vf2d currPosition, World& world, float& contactUp );
		bool isCollidingDown( olc::vf2d prevPosition, olc::vf2d currPosition, World& world, float& contactDown, bool& onOneWayPlatform );
		bool isCollidingLeft( olc::vf2d prevPosition, olc::vf2d currPosition, World& world, float& contactLeft );
		bool isCollidingRight( olc::vf2d prevPosition, olc::vf2d currPosition, World& world, float& contactRight );

	};
}


aabb::DynamicObject::DynamicObject()
{
	this->constructDynamicObject();
}

aabb::DynamicObject::~DynamicObject()
{
	this->destructDynamicObject();
}


void aabb::DynamicObject::constructDynamicObject()
{
	this->_prevPushUp = false;
	this->_currPushUp = false;
	this->_prevPushDown = false;
	this->_currPushDown = false;
	this->_prevPushLeft = false;
	this->_currPushLeft = false;
	this->_prevPushRight = false;
	this->_currPushRight = false;

	this->_onOneWayPlatform = false;
	this->_oneWayPlatformThreshold = settings::DYNAMIC_OBJECT::ONE_WAY_PLATFORM_THRESHOLD;
	return;
}


void aabb::DynamicObject::destructDynamicObject()
{
	return;
}


void aabb::DynamicObject::updatePhysics( float deltaTime, World& world )
{
	this->_prevCenterPosition = this->_currCenterPosition;
	this->_prevVelocity = this->_currVelocity;

	this->_prevPushUp = this->_currPushUp;
	this->_prevPushDown = this->_currPushDown;
	this->_prevPushLeft = this->_currPushLeft;
	this->_prevPushRight = this->_currPushRight;
	

	// Pushing right detection
	float contactRight = 0.0f;
	if ( this->_currVelocity.x >= 0.0f &&
		this->isCollidingRight( this->_prevCenterPosition, this->_currCenterPosition, world, contactRight ) )
	{
		if ( this->_prevCenterPosition.x - this->getHalfSize().x + this->getOffset().x <= contactRight )
		{
			this->_currCenterPosition.x = contactRight - this->getHalfSize().x + this->getOffset().x;
			this->_currPushRight = true;
		}
		this->_currVelocity.x = std::min<float>( 0.0f, this->_currVelocity.x );
	}
	else
	{
		this->_currPushRight = false;
	}


	
	// Pushing left detection (mirrored right)
	float contactLeft = 0.0f;
	if ( this->_currVelocity.x <= 0.0f && 
		this->isCollidingLeft( this->_prevCenterPosition, this->_currCenterPosition, world, contactLeft ) )
	{
		if ( this->_prevCenterPosition.x + this->getHalfSize().x + this->getOffset().x >= contactLeft )
		{
			this->_currCenterPosition.x = contactLeft + this->getHalfSize().x - this->getOffset().x;
			this->_currPushLeft = true;
		}
		this->_currVelocity.x = std::max<float>( 0.0f, this->_currVelocity.x );
	}
	else
	{
		this->_currPushLeft = false;
	}
	

	
	// Pushing up detection
	float contactUp = 0.0f;
	if ( this->_currVelocity.y <= 0.0f && this->isCollidingUp( this->_prevCenterPosition, this->_currCenterPosition, world, contactUp ) )
	{
		this->_currCenterPosition.y = contactUp + this->getHalfSize().y + this->getOffset().y;
		this->_currVelocity.y = 0.0f;
		this->_currPushUp = true;
	}
	else
	{
		this->_currPushUp = false;
	}


	// Pushing down detection
	float contactDown = 0.0f;
	if ( this->_currVelocity.y >= 0.0f && this->isCollidingDown( this->_prevCenterPosition, this->_currCenterPosition, world, contactDown, _onOneWayPlatform ) )
	{
		this->_currCenterPosition.y = contactDown - this->getHalfSize().y + this->getOffset().y;
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


// [!] need an efficient way for region of checking (perhaps quadtree?)
/*
bool aabb::DynamicObject::isCollidingDown( olc::vf2d prevPosition, olc::vf2d currPosition, World& world, float& contactDown )
{
	// If collided downwards (like hitting a ground), calculate the beginning point and end point of the bottom sensor line. Note that it is the index of the cell one pixel below and one pixel short on each side
	// Updates the groundLevel where the contact occured

	// Get the bottom censors
	olc::vf2d center = currPosition + this->_aabbOffset;
	olc::vf2d bottomLeft = center + this->getHalfSize() - olc::vf2d{ this->getHalfSize().x * 2.0f, 0.0f } +olc::vf2d{ 1.0f / world.getTileDimension().x, 1.0f / world.getTileDimension().y };
	olc::vf2d bottomRight = center + this->getHalfSize() - olc::vf2d{ 1.0f / world.getTileDimension().x, -1.0f / world.getTileDimension().y };

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
		contactDown = ( float )checkTileIndex.y;
		checkTile = world.getTileFromIndex( checkTileIndex );

		if ( checkTile != nullptr && checkTile->isBlock() )
		{
			return true;
		}
	}

	return false;
}
*/



bool aabb::DynamicObject::isCollidingUp( olc::vf2d prevPosition, olc::vf2d currPosition, World& world, float& contactUp )
{
	// If collided up (like hitting a ceiling), calculate the beginning point and end point of the bottom sensor line.
	// Note that it is the index of the cell one pixel below and one pixel short on each side
	// Iterpolate through the tiles possibly touched between the prev and current, to ensure character does not push too fast through a block
	// Updates the y-level where the contact occured
	contactUp = 0.0f;

	// Get the bottom censors
	olc::vf2d prevCenter = prevPosition + this->_aabbOffset;
	olc::vf2d currCenter = currPosition + this->_aabbOffset;


	olc::vf2d prevTopRight = prevCenter - olc::vf2d{ -this->getHalfSize().x, this->getHalfSize().y } -olc::vf2d{ 1.0f / world.getTileDimension().x, 1.0f / world.getTileDimension().y };
	olc::vf2d currTopRight = currCenter - olc::vf2d{ -this->getHalfSize().x, this->getHalfSize().y } -olc::vf2d{ 1.0f / world.getTileDimension().x, 1.0f / world.getTileDimension().y };
	olc::vf2d currTopLeft = currCenter - olc::vf2d{ this->getHalfSize().x, this->getHalfSize().y } -olc::vf2d{ -1.0f / world.getTileDimension().x, 1.0f / world.getTileDimension().y };


	olc::vi2d prevTopRightIndex = olc::vi2d{ prevTopRight };
	olc::vi2d currTopRightIndex = olc::vi2d{ currTopRight };
	olc::vi2d currTopLeftIndex = olc::vi2d{ currTopLeft };


	int endY = currTopRightIndex.y;
	int begY = std::min<int>( prevTopRightIndex.y, endY );
	int dist = std::max<int>( std::abs( endY - begY ), 1 );

	int tileIndexX;


	Tile* checkTile;
	olc::vi2d checkTileIndex;
	WorldChunk* worldChunk;

	for ( int tileIndexY = begY; tileIndexY <= endY; ++tileIndexY )
	{
		// Interpolate
		float t = std::abs( endY - tileIndexY ) / dist;
		olc::vf2d checkTopRight = ( currTopRight * t + prevTopRight * ( 1 - t ) );
		olc::vf2d checkTopLeft = olc::vf2d{ checkTopRight.x - this->getHalfSize().x * 2.0f + ( 2.0f / world.getTileDimension().x ), checkTopRight.y };

		olc::vi2d checkTopRightIndex = olc::vi2d{ checkTopRight };
		olc::vi2d checkTopLeftIndex = olc::vi2d{ checkTopLeft };

		for ( checkTileIndex = checkTopLeftIndex; checkTileIndex.x <= checkTopRightIndex.x; checkTileIndex.x += 1 )
		{
			worldChunk = world.getWorldChunkFromIndex( checkTileIndex );
			if ( worldChunk == nullptr )
			{
				return true; // [!] Raise exception for trying to touch an index on a tile that does not exist because the worldchunk does not exist
			}

			checkTile = world.getTileFromIndex( checkTileIndex );

			if ( checkTile != nullptr && checkTile->exists() )
			{
				if ( checkTile->isBlock() )
				{
					// Calculate the potential top contact point
					contactUp = ( float )checkTileIndex.y + 1.0f; // contactIndex ( the real contactUp = checkTopRight.y ) Add 1.0f because it's the cell size
					return true;
				}
			}

		}
	}

	return false;
}



bool aabb::DynamicObject::isCollidingDown( olc::vf2d prevPosition, olc::vf2d currPosition, World& world, float& contactDown, bool& onOneWayPlatform )
{
	// If collided downwards (like hitting a ground), calculate the beginning point and end point of the bottom sensor line.
	// Note that it is the index of the cell one pixel below and one pixel short on each side
	// Iterpolate through the tiles possibly touched between the prev and current, to ensure character does not fall too fast through a block
	// This is more computationally expensive, so perhaps use a simpler ground detection (the commented function above) for slow moving objects. Have a speed parameter flag
	// to decide which algorithm to use
	// Updates the y-level where the contact occured

	onOneWayPlatform = false;
	contactDown = 0.0f;

	// Get the bottom censors
	olc::vf2d prevCenter = prevPosition + this->_aabbOffset;
	olc::vf2d currCenter = currPosition + this->_aabbOffset;

	olc::vf2d prevBottomLeft = prevCenter + this->getHalfSize() - olc::vf2d{ this->getHalfSize().x * 2.0f, 0.0f } +olc::vf2d{ 1.0f / world.getTileDimension().x, 1.0f / world.getTileDimension().y };
	olc::vf2d currBottomLeft = currCenter + this->getHalfSize() - olc::vf2d{ this->getHalfSize().x * 2.0f, 0.0f } +olc::vf2d{ 1.0f / world.getTileDimension().x, 1.0f / world.getTileDimension().y };
	olc::vf2d currBottomRight = currCenter + this->getHalfSize() - olc::vf2d{ 1.0f / world.getTileDimension().x, -1.0f / world.getTileDimension().y };


	olc::vi2d prevBottomLeftIndex = olc::vi2d{ prevBottomLeft };
	olc::vi2d currBottomLeftIndex = olc::vi2d{ currBottomLeft };
	olc::vi2d currBottomRightIndex = olc::vi2d{ currBottomRight };


	int endY = currBottomLeftIndex.y;
	int begY = std::min<int>( prevBottomLeftIndex.y, endY);
	int dist = std::max<int>( std::abs(endY - begY), 1);

	int tileIndexX;


	Tile* checkTile;
	olc::vi2d checkTileIndex;
	WorldChunk* worldChunk;

	for ( int tileIndexY = begY; tileIndexY <= endY; ++tileIndexY )
	{
		// interpolate
		float t = std::abs(endY - tileIndexY) / dist;
		olc::vf2d checkBottomLeft = ( currBottomLeft * t + prevBottomLeft * (1 - t) );
		olc::vf2d checkBottomRight = olc::vf2d{ checkBottomLeft.x + this->getHalfSize().x * 2.0f - (2.0f / world.getTileDimension().x ), checkBottomLeft.y};

		olc::vi2d checkBottomLeftIndex = olc::vi2d{ checkBottomLeft };
		olc::vi2d checkBottomRightIndex = olc::vi2d{ checkBottomRight };

		for ( checkTileIndex = checkBottomLeftIndex; ; checkTileIndex.x += 1 )
		{
			worldChunk = world.getWorldChunkFromIndex( checkTileIndex );
			if ( worldChunk == nullptr )
			{
				return true; // [!] Raise exception for trying to touch an index on a tile that does not exist because the worldchunk does not exist
			}

			// Calculate the potential bottom contact point
			contactDown = ( float )checkTileIndex.y; // contactIndex ( the real contactDown = checkBottomLeft.y )
			checkTile = world.getTileFromIndex( checkTileIndex );

			if ( checkTile != nullptr && checkTile->exists() )
			{
				if ( checkTile->isBlock() ) 
				{
					onOneWayPlatform = false; // In case the character is on both a block and a onewayplatform (prevent from using the oneway platform)
					return true;
				}
				else if ( checkTile->isOneWayPlatform() )
				{
					// Collision ignored if fall beyond the character's one way platform threshold when standing on a one way platform
					// Collision not ignoted if the character does not fall beyond the threshold point when standing on a one way platform
					if ( std::abs(checkTileIndex.y - checkBottomLeft.y) <= settings::CHARACTER::ONE_WAY_PLATFORM_THRESHOLD + currPosition.y - prevPosition.y )
					{
						onOneWayPlatform = true;
					}
				}
			}

			// If you finish checking all the potential down tiles
			if ( checkTileIndex.x >= checkBottomRightIndex.x )
			{
				// If it all happens to be a oneWayPlayform (no block detected), then stand
				if ( onOneWayPlatform )
				{
					return true;
				}
				// Else it has not detected a block (otherwise it would have returned true before, so just break out of the loop now
				break;
			}

		}
	}

	return false;
}





bool aabb::DynamicObject::isCollidingLeft( olc::vf2d prevPosition, olc::vf2d currPosition, World& world, float& contactLeft )
{
	// If collided left (like hitting a wall), calculate the beginning point and end point of the bottom sensor line.
	// Note that it is the index of the cell one pixel below and one pixel short on each side
	// Iterpolate through the tiles possibly touched between the prev and current, to ensure character does not push too fast through a block
	// Updates the y-level where the contact occured
	contactLeft = 0.0f;

	// Get the bottom censors
	olc::vf2d prevCenter = prevPosition + this->_aabbOffset;
	olc::vf2d currCenter = currPosition + this->_aabbOffset;

	/*
	olc::vf2d prevBottomLeft = prevCenter - olc::vf2d{ this->getHalfSize().x, -this->getHalfSize().y } - olc::vf2d{ 1.0f / world.getTileDimension().x, 0.0f };
	olc::vf2d currBottomLeft = currCenter - olc::vf2d{ this->getHalfSize().x, -this->getHalfSize().y } - olc::vf2d{ 1.0f / world.getTileDimension().x, 0.0f };
	olc::vf2d currTopLeft = currCenter - this->getHalfSize() - olc::vf2d{ 1.0f / world.getTileDimension().x, 0.0f };
	*/
	olc::vf2d prevBottomLeft = prevCenter - olc::vf2d{ this->getHalfSize().x, -this->getHalfSize().y } - olc::vf2d{ 1.0f / world.getTileDimension().x, 1.0f / world.getTileDimension().y };
	olc::vf2d currBottomLeft = currCenter - olc::vf2d{ this->getHalfSize().x, -this->getHalfSize().y } - olc::vf2d{ 1.0f / world.getTileDimension().x, 1.0f / world.getTileDimension().y };
	olc::vf2d currTopLeft = currCenter - this->getHalfSize() - olc::vf2d{ 1.0f / world.getTileDimension().x, - 1.0f / world.getTileDimension().y };



	olc::vi2d prevBottomLeftIndex = olc::vi2d{ prevBottomLeft };
	olc::vi2d currBottomLeftIndex = olc::vi2d{ currBottomLeft };
	olc::vi2d currTopLeftIndex = olc::vi2d{ currTopLeft };


	int endX = currBottomLeftIndex.x;
	int begX = std::max<int>( prevBottomLeftIndex.x, endX ); // [?] min, prevBottomLeftIndex.x - 1
	int dist = std::max<int>( std::abs( endX - begX ), 1 );

	int tileIndexY;

	Tile* checkTile;
	olc::vi2d checkTileIndex;
	//olc::vf2d checkTilePosition;
	WorldChunk* worldChunk;

	for ( int tileIndexX = begX; tileIndexX >= endX; --tileIndexX ) // right to left
	{
		// Interpolate
		float t = std::abs( endX - tileIndexX ) / dist;
		olc::vf2d checkBottomLeft = ( currBottomLeft * t + prevBottomLeft * ( 1 - t ) );
		olc::vf2d checkTopLeft = checkBottomLeft - olc::vf2d{ 0.0f, this->getHalfSize().y * 2.0f };

		olc::vi2d checkBottomLeftIndex = olc::vi2d{ checkBottomLeft };
		olc::vi2d checkTopLeftIndex = olc::vi2d{ checkTopLeft };

		for ( checkTileIndex = checkBottomLeftIndex; checkTileIndex.y >= checkTopLeft.y; checkTileIndex.y -= 1 )
		{
			worldChunk = world.getWorldChunkFromIndex( checkTileIndex );
			if ( worldChunk == nullptr )
			{
				return true; // [!] Raise exception for trying to touch an index on a tile that does not exist because the worldchunk does not exist
			}

			checkTile = world.getTileFromIndex( checkTileIndex );

			if ( checkTile != nullptr && checkTile->exists() )
			{
				if ( checkTile->isBlock() )
				{
					// Calculate the potential top contact point
					contactLeft = ( float )checkTileIndex.x + 1.0f; 
					//contactLeft = ( float )checkBottomLeft.x; // contactIndex ( the real contactUp = checkBottomLeft.x ) Add 1.0f because it's the cell size
					return true;
				}
			}

		}

		/*
		for ( checkTilePosition = checkBottomLeft; checkTilePosition.y >= checkTopLeft.y; checkTilePosition.y -= 1 )
		{
			worldChunk = world.getWorldChunkFromIndex( olc::vi2d{ checkTilePosition } );
			if ( worldChunk == nullptr )
			{
				return true; // [!] Raise exception for trying to touch an index on a tile that does not exist because the worldchunk does not exist
			}

			checkTile = world.getTileFromIndex( olc::vi2d{ checkTilePosition } );

			if ( checkTile != nullptr && checkTile->exists() )
			{
				if ( checkTile->isBlock() )
				{
					// Calculate the potential top contact point
					//contactLeft = ( float )checkTileIndex.x + 1.0f; 
					contactLeft = ( float )checkTilePosition.x; // contactIndex ( the real contactUp = checkBottomLeft.x ) Add 1.0f because it's the cell size
					return true;
				}
			}

		}
		*/
	}


	return false;
}







bool aabb::DynamicObject::isCollidingRight( olc::vf2d prevPosition, olc::vf2d currPosition, World& world, float& contactRight )
{
	// If collided left (like hitting a wall), calculate the beginning point and end point of the bottom sensor line.
	// Note that it is the index of the cell one pixel below and one pixel short on each side
	// Iterpolate through the tiles possibly touched between the prev and current, to ensure character does not push too fast through a block
	// Updates the y-level where the contact occured
	contactRight = 0.0f;

	// Get the bottom censors
	olc::vf2d prevCenter = prevPosition + this->_aabbOffset;
	olc::vf2d currCenter = currPosition + this->_aabbOffset;

	/*
	olc::vf2d prevBottomRight = prevCenter + this->getHalfSize() + olc::vf2d{ 1.0f / world.getTileDimension().x, 0.0f };
	olc::vf2d currBottomRight = currCenter + this->getHalfSize() + olc::vf2d{ 1.0f / world.getTileDimension().x, 0.0f };
	olc::vf2d currTopRight = currCenter + this->getHalfSize() - olc::vf2d{ -1.0f / world.getTileDimension().x, this->getHalfSize().y * 2.0f };
	*/	

	olc::vf2d prevBottomRight = prevCenter + this->getHalfSize() + olc::vf2d{ 1.0f / world.getTileDimension().x, -1.0f / world.getTileDimension().y };
	olc::vf2d currBottomRight = currCenter + this->getHalfSize() + olc::vf2d{ 1.0f / world.getTileDimension().x, -1.0f / world.getTileDimension().y };
	olc::vf2d currTopRight = currCenter + this->getHalfSize() - olc::vf2d{ -1.0f / world.getTileDimension().x, ( this->getHalfSize().y * 2.0f ) - ( 1.0f / world.getTileDimension().y ) };

	olc::vi2d prevBottomRightIndex = olc::vi2d{ prevBottomRight };
	olc::vi2d currBottomRightIndex = olc::vi2d{ currBottomRight };
	olc::vi2d currTopRightIndex = olc::vi2d{ currTopRight };


	int endX = currBottomRightIndex.x;
	int begX = std::min<int>( prevBottomRightIndex.x, endX ); // [?] man, prevBottomRightIndex.x + 1
	int dist = std::max<int>( std::abs( endX - begX ), 1 );

	int tileIndexY;

	Tile* checkTile;
	olc::vi2d checkTileIndex;
	WorldChunk* worldChunk;

	for ( int tileIndexX = begX; tileIndexX <= endX; ++tileIndexX ) // left to right
	{
		// Interpolate
		float t = std::abs( endX - tileIndexX ) / dist;
		olc::vf2d checkBottomRight = ( currBottomRight * t + prevBottomRight * ( 1 - t ) );
		olc::vf2d checkTopRight = checkBottomRight - olc::vf2d{ 0.0f, this->getHalfSize().y * 2.0f };

		olc::vi2d checkBottomRightIndex = olc::vi2d{ checkBottomRight };
		olc::vi2d checkTopRightIndex = olc::vi2d{ checkTopRight };

		for ( checkTileIndex = checkBottomRightIndex; checkTileIndex.y >= checkTopRight.y; checkTileIndex.y -= 1 )
		{
			worldChunk = world.getWorldChunkFromIndex( checkTileIndex );
			if ( worldChunk == nullptr )
			{
				return true; // [!] Raise exception for trying to touch an index on a tile that does not exist because the worldchunk does not exist
			}

			checkTile = world.getTileFromIndex( checkTileIndex );

			if ( checkTile != nullptr && checkTile->exists() )
			{
				if ( checkTile->isBlock() )
				{
					// Calculate the potential top contact point
					contactRight = ( float )checkTileIndex.x; 
					//contactRight = ( float )checkBottomRight.x;
					return true;
				}
			}

		}

	}


	return false;
}