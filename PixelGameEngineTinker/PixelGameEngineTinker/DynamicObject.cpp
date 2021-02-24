#include "DynamicObject.h"
#include "World.h"
#include "CollisionData.h"


// Constructors/Destructors
DynamicObject::DynamicObject() :
	_aabb(),
	_aabbOffset( olc::vf2d{ 0.0f, 0.0f } ), _scale( olc::vf2d{ 1.0, 1.0 } ),
	_prevPosition( olc::v2d_generic<long double>{ 0.0, 0.0 } ), _currPosition( olc::v2d_generic<long double>{ 0.0, 0.0 } ),
	_prevVelocity( olc::vf2d{ 0.0f, 0.0f } ), _currVelocity( olc::vf2d{ 0.0f, 0.0f } ),
	_pushedRight( false ), _pushingRight( false ),
	_pushedLeft( false ), _pushingLeft( false ),
	_pushedUp( false ), _pushingUp( false ),
	_pushedDown( false ), _pushingDown( false ),
	_pushedLeftObject( false ), _pushingLeftObject( false ),
	_pushedRightObject( false ), _pushingRightObject( false ),
	_pushedBottomObject( false ), _pushingBottomObject( false ),
	_pushedTopObject( false ), _pushingTopObject( false ),
	_onOneWayPlatform( false ),
	_world( nullptr ), _time( 0 ),
	_spaces(),
	_allCollisions(),
	_isKinematic( false )
{

}


DynamicObject::DynamicObject( const olc::v2d_generic<long double>& center, const olc::vf2d& halfSize, const olc::vf2d& scale, World* world, long double time ) :
	_aabb( center, halfSize, olc::vf2d{ std::abs( scale.x ), std::abs( scale.y ) } ),
	_aabbOffset( olc::vf2d{ 0.0f, 0.0f } ), _scale( scale ),
	_prevPosition( olc::v2d_generic<long double>{ center.x, center.y } ), _currPosition( olc::v2d_generic<long double>{ center.x, center.y } ),
	_prevVelocity( olc::vf2d{ 0.0f, 0.0f } ), _currVelocity( olc::vf2d{ 0.0f, 0.0f } ),
	_pushedRight( false ), _pushingRight( false ),
	_pushedLeft( false ), _pushingLeft( false ),
	_pushedUp( false ), _pushingUp( false ),
	_pushedDown( false ), _pushingDown( false ),
	_pushedLeftObject( false ), _pushingLeftObject( false ),
	_pushedRightObject( false ), _pushingRightObject( false ),
	_pushedBottomObject( false ), _pushingBottomObject( false ),
	_pushedTopObject( false ), _pushingTopObject( false ),
	_onOneWayPlatform( false ),
	_world( world ), _time( time ),
	_spaces(),
	_allCollisions(),
	_isKinematic( false )
{

}


DynamicObject::~DynamicObject()
{

}


// Getters
olc::vf2d DynamicObject::getScale() const
{
	return this->_scale;
}


float DynamicObject::getScaleX() const
{
	return this->_scale.x;
}


float DynamicObject::getScaleY() const
{
	return this->_scale.y;
}


AABB DynamicObject::getAABB() const
{
	return this->_aabb;
}


olc::vf2d DynamicObject::getAABBOffset() const
{
	return this->_aabbOffset * this->_scale;
}


float DynamicObject::getAABBOffsetX() const
{
	return this->_aabbOffset.x * this->_scale.x;
}


float DynamicObject::getAABBOffsetY() const
{
	return this->_aabbOffset.y * this->_scale.y;
}


olc::v2d_generic<long double> DynamicObject::getPrevPosition() const
{
	return this->_prevPosition;
}


olc::v2d_generic<long double> DynamicObject::getCurrPosition() const
{
	return this->_currPosition;
}


olc::vf2d DynamicObject::getPrevVelocity() const
{
	return this->_prevVelocity;
}


olc::vf2d DynamicObject::getCurrVelocity() const
{
	return this->_currVelocity;
}



// Setters
void DynamicObject::setScale( const olc::vf2d& scale )
{
	this->_scale = scale;
	this->_aabb.setScale( scale );
	return;
}


void DynamicObject::setScaleX( float x )
{
	this->_scale.x = x;
	this->_aabb.setScaleX( x );
	return;
}


void DynamicObject::setScaleY( float y )
{
	this->_scale.y = y;
	this->_aabb.setScaleY( y );
	return;
}


void DynamicObject::setAABBOffset( const olc::vf2d& aabbOffset )
{
	this->_aabbOffset = aabbOffset;
	return;
}


void DynamicObject::setAABBOffsetX( float x )
{
	this->_aabbOffset.x = x;
	return;
}


void DynamicObject::setAABBOffsetY( float y )
{
	this->_aabbOffset.y = y;
	return;
}


void DynamicObject::addToSpaces( std::int64_t spaceIndex, std::size_t id )
{
	this->_spaces.push_back( std::pair<std::int64_t, std::size_t>{ spaceIndex, id });
}


bool DynamicObject::getPushingLeft() const
{
	return this->_pushingLeft;
}


bool DynamicObject::getPushingRight() const
{
	return this->_pushingRight;
}


bool DynamicObject::getPushingUp() const
{
	return this->_pushingUp;
}


bool DynamicObject::getPushingDown() const
{
	return this->_pushingDown;
}


// Methods
bool DynamicObject::isCollidingDown( const World* world, long double& worldGroundY, bool& onOneWayPlatform )
{
	olc::v2d_generic<long double> prevCenter = this->_prevPosition + olc::v2d_generic<long double>( this->_aabbOffset.x, -this->_aabbOffset.y );
	olc::v2d_generic<long double> currCenter = this->_currPosition + olc::v2d_generic<long double>( this->_aabbOffset.x, -this->_aabbOffset.y );

	std::uint16_t tileSize = Settings::World::CELL_SIZE;
	long double give = ( 1.0 / ( long double )tileSize );

	// Prev bottom sensor line
	olc::v2d_generic<long double> prevBottomSensorLeft = prevCenter - olc::v2d_generic<long double>( this->_aabb.getHalfSizeX() - give, -this->_aabb.getHalfSizeY() - give );

	// Curr bottom sensor line
	olc::v2d_generic<long double> currBottomSensorLeft = currCenter - olc::v2d_generic<long double>( this->_aabb.getHalfSizeX() - give, - this->_aabb.getHalfSizeY() - give );
	olc::v2d_generic<long double> currBottomSensorRight = olc::v2d_generic( currBottomSensorLeft.x + ( ( long double )this->_aabb.getHalfSizeX() * 2.0 ) - 2.0 * give, currBottomSensorLeft.y );


	// Interpolate on Y range
	std::int64_t endY = ( std::int64_t )std::floor( currBottomSensorLeft.y );
	std::int64_t begY = std::min( ( std::int64_t )(std::floor( prevBottomSensorLeft.y ) + 1 ), endY );
	std::int64_t dist = std::max<std::int64_t>( std::abs( endY - begY ), 1 ); // Mininmum of 1 to use next iteration position for detecting

	std::int64_t checkTileIndexX;

	// Can optimize by only using interpolation checks on objects that go past a certain velocity (at a fixed max fps update),
	// and objects that go slow enough as to never be able to pass through multiple tiles (at a fixed max fps update) will use crude but fast checks [~]
	for ( std::int64_t checkTileIndexY = begY; checkTileIndexY <= endY; checkTileIndexY++ )
	{
		// Interpolate
		long double t = std::abs( endY - checkTileIndexY ) / dist;
		olc::v2d_generic<long double> checkBottomLeft = ( currBottomSensorLeft * t + prevBottomSensorLeft * ( 1 - t ) );
		olc::v2d_generic<long double> checkBottomRight = olc::v2d_generic<long double>{ checkBottomLeft.x + ( ( long double )this->_aabb.getHalfSizeX() * 2.0 ) - 2.0 * give, checkBottomLeft.y };
	
		for ( olc::v2d_generic<long double> checkedTile = checkBottomLeft; ; checkedTile.x += 1.0 )
		{
			checkedTile.x = std::min( checkedTile.x, checkBottomRight.x );

			checkTileIndexX = std::floor( checkedTile.x );
			worldGroundY = ( long double )( checkTileIndexY );

			const Tile* contactTile = world->getTile( checkTileIndexX, checkTileIndexY );
			if ( contactTile == nullptr )
			{
				return true; // [!] Throw error as object is checking collisions of non-existent tile
			}
			else if ( contactTile->isObstacle() )
			{
				onOneWayPlatform = false;
				return true;
			}
			else if ( contactTile->isOneWayPlatform() )
			{
				// Collision ignored if fall beyond the dynamicObject's one way platform threshold when standing on a one way platform
				// Collision not ignored if the dynamicObject does not fall beyond the threshold point when standing on a one way platform
				if ( std::abs( checkTileIndexY - checkBottomLeft.y ) <= Settings::DynamicObject::ONE_WAY_PLATFORM_THRESHOLD - ( this->_prevPosition.y - this->_currPosition.y ) )
				{
					onOneWayPlatform = true;
				}
			}

			// If you finish checking all the left and right tiles at a certain Y
			if ( checkedTile.x >= checkBottomRight.x )
			{
				// If it all happens to be a oneWayPlayform (no block detected), then stand
				if ( onOneWayPlatform )
				{
					return true;
				}
				// Else it has not detected a block nor a platform (otherwise it would have returned true before, so just break out of the loop now
				break;
			}
		}
	}

	return false;
}

bool DynamicObject::isCollidingUp( const World* world, long double& worldCeilingY )
{
	olc::v2d_generic<long double> prevCenter = this->_prevPosition + olc::v2d_generic<long double>( this->_aabbOffset.x, -this->_aabbOffset.y );
	olc::v2d_generic<long double> currCenter = this->_currPosition + olc::v2d_generic<long double>( this->_aabbOffset.x, -this->_aabbOffset.y );

	std::uint16_t tileSize = Settings::World::CELL_SIZE;
	long double give = ( 1.0 / ( long double )tileSize );

	// Prev bottom sensor line
	olc::v2d_generic<long double> prevTopSensorRight = prevCenter + olc::v2d_generic<long double>( this->_aabb.getHalfSizeX() - give, -this->_aabb.getHalfSizeY() - give );

	// Curr bottom sensor line
	olc::v2d_generic<long double> currTopSensorRight = currCenter + olc::v2d_generic<long double>( this->_aabb.getHalfSizeX() - give, -this->_aabb.getHalfSizeY() - give );
	olc::v2d_generic<long double> currTopSensorLeft = olc::v2d_generic<long double>( currTopSensorRight.x - ( ( long double )this->_aabb.getHalfSizeX() * 2.0 ) + 2.0 * give, currTopSensorRight.y );


	// Interpolate on Y range
	std::int64_t endY = ( std::int64_t )std::floor( currTopSensorRight.y );
	std::int64_t begY = std::max( ( std::int64_t )( std::floor( prevTopSensorRight.y ) - 1 ), endY );
	std::int64_t dist = std::max<std::int64_t>( std::abs( endY - begY ), 1 ); // Mininmum of 1 to use next iteration position for detecting

	std::int64_t checkTileIndexX;

	// Can optimize by only using interpolation checks on objects that go past a certain velocity (at a fixed max fps update),
	// and objects that go slow enough as to never be able to pass through multiple tiles (at a fixed max fps update) will use crude but fast checks [~]
	for ( std::int64_t checkTileIndexY = begY; checkTileIndexY >= endY; checkTileIndexY-- )
	{
		// Interpolate
		long double t = std::abs( endY - checkTileIndexY ) / dist;
		olc::v2d_generic<long double> checkTopRight = ( currTopSensorRight * t + prevTopSensorRight * ( 1 - t ) );
		olc::v2d_generic<long double> checkTopLeft = olc::v2d_generic<long double>{ checkTopRight.x - ( ( long double )this->_aabb.getHalfSizeX() * 2.0 ) + 2.0 * give, checkTopRight.y };

		for ( olc::v2d_generic<long double> checkedTile = checkTopLeft; ; checkedTile.x += 1.0 )
		{
			checkedTile.x = std::min( checkedTile.x, checkTopRight.x );

			checkTileIndexX = std::floor( checkedTile.x );

			const Tile* contactTile = world->getTile( checkTileIndexX, checkTileIndexY );
			if ( contactTile == nullptr )
			{
				return true; // [!] Throw error as object is checking collisions of non-existent tile
			}
			else if ( contactTile->isObstacle() )
			{
				worldCeilingY = ( long double )( checkTileIndexY );
				return true;
			}

			// If you finish checking all the left and right tiles at a certain Y, break
			if ( checkedTile.x >= checkTopRight.x ) break;
		}
	}

	return false;
}


bool DynamicObject::isCollidingLeft( const World* world, long double& worldLeftWallX )
{
	olc::v2d_generic<long double> prevCenter = this->_prevPosition + olc::v2d_generic<long double>( this->_aabbOffset.x, -this->_aabbOffset.y );
	olc::v2d_generic<long double> currCenter = this->_currPosition + olc::v2d_generic<long double>( this->_aabbOffset.x, -this->_aabbOffset.y );


	std::uint16_t tileSize = Settings::World::CELL_SIZE;
	long double give = ( 1.0 / ( long double )tileSize );

	// Prev bottom sensor line
	olc::v2d_generic<long double> prevLeftSensorBottom = prevCenter - olc::v2d_generic<long double>( this->_aabb.getHalfSizeX() + give, -this->_aabb.getHalfSizeY() + give );

	// Curr bottom sensor line
	olc::v2d_generic<long double> currLeftSensorBottom = currCenter - olc::v2d_generic<long double>( this->_aabb.getHalfSizeX() + give, -this->_aabb.getHalfSizeY() + give );
	olc::v2d_generic<long double> currLeftSensorTop = olc::v2d_generic<long double>( currLeftSensorBottom.x, currLeftSensorBottom.y - ( this->_aabb.getHalfSizeY() * 2.0 ) + 2.0 * give );


	// Interpolate on X range
	std::int64_t endX = ( std::int64_t )std::floor( currLeftSensorBottom.x );
	std::int64_t begX = std::max( ( std::int64_t )( std::floor( prevLeftSensorBottom.x ) - 1 ), endX );
	std::int64_t dist = std::max<std::int64_t>( std::abs( endX - begX ), 1 ); // Mininmum of 1 to use next iteration position for detecting

	std::int64_t checkTileIndexY;

	// Can optimize by only using interpolation checks on objects that go past a certain velocity (at a fixed max fps update),
	// and objects that go slow enough as to never be able to pass through multiple tiles (at a fixed max fps update) will use crude but fast checks [~]
	for ( std::int64_t checkTileIndexX = begX; checkTileIndexX >= endX; checkTileIndexX-- ) // Right to left
	{
		// Interpolate
		long double t = std::abs( endX - checkTileIndexX ) / dist;
		olc::v2d_generic<long double> checkBottomLeft = ( currLeftSensorBottom * t + prevLeftSensorBottom * ( 1 - t ) );
		olc::v2d_generic<long double> checkTopLeft = olc::v2d_generic<long double>{ checkBottomLeft.x, checkBottomLeft.y - ( this->_aabb.getHalfSizeY() * 2.0 ) + 2.0 * give };

		for ( olc::v2d_generic<long double> checkedTile = checkBottomLeft; ; checkedTile.y -= 1.0 )
		{
			//checkedTile.y = std::min( checkedTile.y, checkTopLeft.y ); [!]
			checkedTile.y = std::max( checkedTile.y, checkTopLeft.y );

			checkTileIndexY = std::floor( checkedTile.y );

			const Tile* contactTile = world->getTile( checkTileIndexX, checkTileIndexY );
			if ( contactTile == nullptr )
			{
				return true; // [!] Throw error as object is checking collisions of non-existent tile
			}
			else if ( contactTile->isObstacle() )
			{
				worldLeftWallX = ( long double )( checkTileIndexX );
				return true;
			}

			// If you finish checking all the down and up tiles at a certain X, break
			if ( checkedTile.y <= checkTopLeft.y ) break;
		}
	}

	return false;
}


bool DynamicObject::isCollidingRight( const World* world, long double& worldRightWallX )
{
	olc::v2d_generic<long double> prevCenter = this->_prevPosition + olc::v2d_generic<long double>( this->_aabbOffset.x, -this->_aabbOffset.y );
	olc::v2d_generic<long double> currCenter = this->_currPosition + olc::v2d_generic<long double>( this->_aabbOffset.x, -this->_aabbOffset.y );

	std::uint16_t tileSize = Settings::World::CELL_SIZE;
	long double give = ( 1.0 / ( long double )tileSize );

	// Prev bottom sensor line
	olc::v2d_generic<long double> prevRightSensorBottom = prevCenter + olc::v2d_generic<long double>( this->_aabb.getHalfSizeX() + give, this->_aabb.getHalfSizeY() - give );

	// Curr bottom sensor line
	olc::v2d_generic<long double> currRightSensorBottom = currCenter + olc::v2d_generic<long double>( this->_aabb.getHalfSizeX() + give, this->_aabb.getHalfSizeY() - give );
	olc::v2d_generic<long double> currRightSensorTop = olc::v2d_generic<long double>( currRightSensorBottom.x, currRightSensorBottom.y - ( this->_aabb.getHalfSizeY() * 2.0 ) + 2.0 * give );


	// Interpolate on X range
	std::int64_t endX = ( std::int64_t )std::floor( currRightSensorBottom.x );
	std::int64_t begX = std::min( ( std::int64_t )( std::floor( prevRightSensorBottom.x ) + 1 ), endX );
	std::int64_t dist = std::max<std::int64_t>( std::abs( endX - begX ), 1 ); // Mininmum of 1 to use next iteration position for detecting

	std::int64_t checkTileIndexY;

	// Can optimize by only using interpolation checks on objects that go past a certain velocity (at a fixed max fps update),
	// and objects that go slow enough as to never be able to pass through multiple tiles (at a fixed max fps update) will use crude but fast checks [~]
	for ( std::int64_t checkTileIndexX = begX; checkTileIndexX <= endX; checkTileIndexX++ ) // Left to Right
	{
		// Interpolate
		long double t = std::abs( endX - checkTileIndexX ) / dist;
		olc::v2d_generic<long double> checkBottomRight = ( currRightSensorBottom * t + prevRightSensorBottom * ( 1 - t ) );
		olc::v2d_generic<long double> checkTopRight = olc::v2d_generic<long double>{ checkBottomRight.x, checkBottomRight.y - ( this->_aabb.getHalfSizeY() * 2.0 ) + 2.0 * give };

		for ( olc::v2d_generic<long double> checkedTile = checkBottomRight; ; checkedTile.y -= 1.0 )
		{
			//checkedTile.y = std::min( checkedTile.y, checkTopLeft.y ); [!]
			checkedTile.y = std::max( checkedTile.y, checkTopRight.y );

			checkTileIndexY = std::floor( checkedTile.y );

			const Tile* contactTile = world->getTile( checkTileIndexX, checkTileIndexY );
			if ( contactTile == nullptr )
			{
				return true; // [!] Throw error as object is checking collisions of non-existent tile
			}
			else if ( contactTile->isObstacle() )
			{
				worldRightWallX = ( long double )( checkTileIndexX );
				return true;
			}

			// If you finish checking all the down and up tiles at a certain X, break
			if ( checkedTile.y <= checkTopRight.y ) break;
		}
	}

	return false;
}



// Collision Detection

std::vector<std::pair<std::int64_t, std::size_t>>& DynamicObject::getSpaces()
{
	return this->_spaces;
}


std::vector<CollisionData>& DynamicObject::getAllCollisions()
{
	return this->_allCollisions;
}


bool DynamicObject::hasCollisionDataFor( DynamicObject* otherObject ) const
{
	for ( std::size_t i = 0; i < this->_allCollisions.size(); i++ )
	{
		if ( this->_allCollisions[i].otherObject == otherObject )
		{
			return true;
		}
	}

	return false;
}



// Collision Response

bool DynamicObject::isKinematic() const
{
	return this->_isKinematic;
}


void DynamicObject::updatePhysicsResponse()
{
	if ( this->_isKinematic ) return;

	this->_pushedLeftObject = this->_pushingLeftObject;
	this->_pushedRightObject = this->_pushingRightObject;
	this->_pushedBottomObject = this->_pushingBottomObject;
	this->_pushedTopObject = this->_pushingTopObject;

	this->_pushingLeftObject = false;
	this->_pushingRightObject = false;
	this->_pushingBottomObject = false;
	this->_pushingTopObject = false;

	olc::v2d_generic<long double> offsetSum = olc::v2d_generic<long double>{ 0.0, 0.0 };

	for ( std::size_t i = 0; i < this->_allCollisions.size(); i++ )
	{
		const CollisionData& collisionData = this->_allCollisions[i];
		DynamicObject* otherObject = collisionData.otherObject;
		olc::v2d_generic<long double> overlap = collisionData.overlap - offsetSum; // [!]

		// Objects just touching each other on x axis
		if ( overlap.x == 0.0 )
		{
			// Other object on right side
			if ( otherObject->getAABB().getCenter().x > this->_aabb.getCenter().x )
			{
				this->_pushingRightObject = true;
				this->_currVelocity.x = std::min<float>( this->_currVelocity.x, 0.0f );
			}
			// Other object on left side
			else
			{
				this->_pushingLeftObject = true;
				this->_currVelocity.x = std::max<float>( this->_currVelocity.x, 0.0f );
			}
			continue;
		}
		// Objects just touching each other on y axis
		else if ( overlap.y == 0.0 )
		{
			// Other object on bottom side
			if ( otherObject->getAABB().getCenter().y > this->_aabb.getCenter().y )
			{
				this->_pushingBottomObject = true;
				this->_currVelocity.y = std::max<float>( this->_currVelocity.y, 0.0f );
			}
			// Other object on left side
			else
			{
				this->_pushingTopObject = true;
				this->_currVelocity.y = std::min<float>( this->_currVelocity.y, 0.0f );
			}
			continue;
		}



		olc::vf2d speed1 = olc::vf2d{ 
			( float )std::abs( collisionData.currPosition1.x - collisionData.prevPosition1.x ), 
			( float )std::abs( collisionData.currPosition1.y - collisionData.prevPosition1.y )
		};

		olc::vf2d speed2 = olc::vf2d{
			( float )std::abs( collisionData.currPosition2.x - collisionData.prevPosition2.x ),
			( float )std::abs( collisionData.currPosition2.y - collisionData.prevPosition2.y )
		};

		olc::vf2d speedSum = speed1 + speed2;


		// Calculate speed ratios to calculate offset
		float speedRatioX, speedRatioY;
		if ( otherObject->isKinematic() )
		{
			speedRatioX = speedRatioY = 1.0f;
		}
		else
		{
			// If both overlap but no moving, split them apart by the middle
			if ( speedSum.x == 0.0f && speedSum.y == 0.0f )
			{
				speedRatioX = speedRatioY = 0.5f;
			}
			else if ( speedSum.x == 0.0f )
			{
				speedRatioX = 0.5f;
				speedRatioY = speed1.y / speedSum.y;
			}
			else if ( speedSum.y == 0.0f )
			{
				speedRatioX = speed1.x / speedSum.x;
				speedRatioY = 0.5f;
			}
			else
			{
				speedRatioX = speed1.x / speedSum.x;
				speedRatioY = speed1.y / speedSum.y;
			}
		}


		//overlap.x = overlap.x / ( this->_aabb.getHalfSizeX() + otherObject->getAABB().getHalfSizeX() );
		//overlap.y = overlap.y / ( this->_aabb.getHalfSizeY() + otherObject->getAABB().getHalfSizeY() );


		float offsetX = overlap.x * speedRatioX;
		float offsetY = overlap.y * speedRatioY;

		// Moving out of the overlap in three cases: horizontally, vertically, or diagonally
		bool overlappedLastFrameX = std::abs( collisionData.prevPosition1.x - collisionData.prevPosition2.x ) < this->_aabb.getHalfSizeX() + otherObject->getAABB().getHalfSizeX();
		bool overlappedLastFrameY = std::abs( collisionData.prevPosition1.y - collisionData.prevPosition2.y ) < this->_aabb.getHalfSizeY() + otherObject->getAABB().getHalfSizeY();

		if ( ( !overlappedLastFrameX && overlappedLastFrameY ) || 
			 ( !overlappedLastFrameX && overlappedLastFrameY && std::abs( overlap.x ) <= std::abs( overlap.y ) ) 
			//( !overlappedLastFrameX && overlappedLastFrameY && std::abs( overlap.x / ( this->_aabb.getHalfSizeX() * 2 ) ) <= std::abs( overlap.y / ( this->_aabb.getHalfSizeY() * 2 ) ) )
			)
		{
			//this->_currPosition.x += offsetX;
			offsetSum.x += offsetX;

			if ( overlap.x < 0.0 )
			{
				//this->_pushingRightObject = true;
				//this->_currVelocity.x = std::min( this->_currVelocity.x, 0.0f );
			}
			else
			{
				//this->_pushingLeftObject = true;
				//this->_currVelocity.x = std::max( this->_currVelocity.x, 0.0f );
			}
		}
		else
		//else if ( ( overlappedLastFrameX && !overlappedLastFrameY ) ||
		//	( overlappedLastFrameX && !overlappedLastFrameY && std::abs( overlap.x ) >= std::abs( overlap.y ) )
		//	)
		{
			// this->_currPosition.y += offsetY;
			offsetSum.y += offsetY;

			if ( overlap.y < 0.0 )
			{
				//this->_pushingBottomObject = true;
				//this->_currVelocity.y = std::max( this->_currVelocity.y, 0.0f );
			}
			else
			{
				//this->_pushingTopObject = true;
				//this->_currVelocity.y = std::min( this->_currVelocity.y, 0.0f );
			}
		}
	}

	return;
}



void DynamicObject::updateStaticPhysics( const World* world, float deltaTime )
{
	// Cache data of previous frame
	this->_prevPosition = this->_currPosition;
	this->_pushedRight = this->_pushingRight;
	this->_pushedLeft = this->_pushingLeft;
	this->_pushedDown = this->_pushingDown;
	this->_pushedUp = this->_pushingUp;
	this->_prevVelocity = this->_currVelocity;

	// Reset states
	this->_onOneWayPlatform = false;

	// Update position using the current speed
	this->_currPosition = this->_currPosition + ( olc::v2d_generic<long double>( this->_currVelocity.x, -this->_currVelocity.y ) * deltaTime );
	//this->_currPosition = this->_currPosition + ( olc::v2d_generic<long double>( this->_currVelocity.x, -this->_currVelocity.y ) * Settings::Game::TICK_RATE );

	long double worldGroundY = this->_currPosition.y + this->_aabb.getHalfSizeY() + this->_aabbOffset.y;
	long double worldCeilingY = this->_currPosition.y - this->_aabb.getHalfSizeY() + this->_aabbOffset.y;
	long double worldLeftWallX = this->_currPosition.x - this->_aabb.getHalfSizeX() + this->_aabbOffset.x;
	long double worldRightWallX = this->_currPosition.x + this->_aabb.getHalfSizeX() + this->_aabbOffset.x;


	// Prioritize horizontal collision detection over verticle
	// Conditions for jutting dynamic object to the right side of the left wall tile:
	//	(1) Horizontal speed is less than or equal to zero ( left movement )
	//	(2) Collidie with left wall
	//	(3) In the previous frame, we did not overlap with tile on horizontal axis ( false not necessarily telling us that object is not pushing the wall )
	// Colliding Left
	if ( this->_currVelocity.x <= 0.0f &&
		this->isCollidingLeft( world, worldLeftWallX ) )
	{
		if ( this->_prevPosition.x - this->_aabb.getHalfSizeX() + this->_aabbOffset.x >= worldLeftWallX )
		{
			this->_currPosition.x = worldLeftWallX + this->_aabb.getHalfSizeX() - this->_aabbOffset.x + 1.0;
			this->_pushingLeft = true;
		}
		this->_currVelocity.x = std::max( this->_currVelocity.x, 0.0f);
	}
	else
	{
		this->_pushingLeft = false;
	}
	
	// Colliding Right
	if ( this->_currVelocity.x >= 0.0f &&
		this->isCollidingRight( world, worldRightWallX ) )
	{
		if ( this->_prevPosition.x + this->_aabb.getHalfSizeX() + this->_aabbOffset.x <= worldRightWallX )
		{
			this->_currPosition.x = worldRightWallX - this->_aabb.getHalfSizeX() - this->_aabbOffset.x;
			this->_pushingRight = true;
		}
		this->_currVelocity.x = std::min( this->_currVelocity.x, 0.0f );
	}
	else
	{
		this->_pushingRight = false;
	}

	// Colliding Down
	if ( this->_currVelocity.y <= 0.0f && 
		this->isCollidingDown( world, worldGroundY, this->_onOneWayPlatform ) )
	{
		this->_currPosition.y = worldGroundY - this->_aabb.getHalfSizeY() + this->_aabbOffset.y;
		this->_currVelocity.y = 0.0;
		this->_pushingDown = true;
	}
	else
	{
		this->_pushingDown = false;
	}

	// Colliding Up
	if ( this->_currVelocity.y >= 0.0f &&
		this->isCollidingUp( world, worldCeilingY ) )
	{
		this->_currPosition.y = worldCeilingY + this->_aabb.getHalfSizeY() + this->_aabbOffset.y + 1.0;
		this->_currVelocity.y = 0.0;
		this->_pushingUp = true;
	}
	else
	{
		this->_pushingUp = false;
	}

	// Update AABB of object to match new position
	this->_aabb.setCenter( this->_currPosition + olc::v2d_generic<long double>( this->_aabbOffset.x, -this->_aabbOffset.y ) ); // negative because negY = worldPosY

	return;
}

/*
void DynamicObject::updateStaticPhysics( const World* world  )
{
	// Cache data of previous frame
	this->_prevPosition = this->_currPosition;
	this->_pushedRight = this->_pushingRight;
	this->_pushedLeft = this->_pushingLeft;
	this->_pushedDown = this->_pushingDown;
	this->_pushedUp = this->_pushingUp;
	this->_prevVelocity = this->_currVelocity;

	// Reset states
	this->_onOneWayPlatform = false;

	// Update position using the current speed
	this->_currPosition = this->_currPosition + ( olc::v2d_generic<long double>( this->_currVelocity.x, -this->_currVelocity.y ) * Settings::Game::TICK_RATE );

	long double worldGroundY = this->_currPosition.y + this->_aabb.getHalfSizeY() + this->_aabbOffset.y;
	long double worldCeilingY = this->_currPosition.y - this->_aabb.getHalfSizeY() + this->_aabbOffset.y;
	long double worldLeftWallX = this->_currPosition.x - this->_aabb.getHalfSizeX() + this->_aabbOffset.x;
	long double worldRightWallX = this->_currPosition.x + this->_aabb.getHalfSizeX() + this->_aabbOffset.x;


	// Prioritize horizontal collision detection over verticle
	// Conditions for jutting dynamic object to the right side of the left wall tile:
	//	(1) Horizontal speed is less than or equal to zero ( left movement )
	//	(2) Collidie with left wall
	//	(3) In the previous frame, we did not overlap with tile on horizontal axis ( false not necessarily telling us that object is not pushing the wall )
	// Colliding Left
	if ( this->_currVelocity.x <= 0.0f &&
		this->isCollidingLeft( world, worldLeftWallX ) )
	{
		if ( this->_prevPosition.x - this->_aabb.getHalfSizeX() + this->_aabbOffset.x >= worldLeftWallX )
		{
			this->_currPosition.x = worldLeftWallX + this->_aabb.getHalfSizeX() - this->_aabbOffset.x + 1.0;
			this->_pushingLeft = true;
		}
		this->_currVelocity.x = std::max( this->_currVelocity.x, 0.0f );
	}
	else
	{
		this->_pushingLeft = false;
	}

	// Colliding Right
	if ( this->_currVelocity.x >= 0.0f &&
		this->isCollidingRight( world, worldRightWallX ) )
	{
		if ( this->_prevPosition.x + this->_aabb.getHalfSizeX() + this->_aabbOffset.x <= worldRightWallX )
		{
			this->_currPosition.x = worldRightWallX - this->_aabb.getHalfSizeX() - this->_aabbOffset.x;
			this->_pushingRight = true;
		}
		this->_currVelocity.x = std::min( this->_currVelocity.x, 0.0f );
	}
	else
	{
		this->_pushingRight = false;
	}

	// Colliding Down
	if ( this->_currVelocity.y <= 0.0f &&
		this->isCollidingDown( world, worldGroundY, this->_onOneWayPlatform ) )
	{
		this->_currPosition.y = worldGroundY - this->_aabb.getHalfSizeY() + this->_aabbOffset.y;
		this->_currVelocity.y = 0.0;
		this->_pushingDown = true;
	}
	else
	{
		this->_pushingDown = false;
	}

	// Colliding Up
	if ( this->_currVelocity.y >= 0.0f &&
		this->isCollidingUp( world, worldCeilingY ) )
	{
		this->_currPosition.y = worldCeilingY + this->_aabb.getHalfSizeY() + this->_aabbOffset.y + 1.0;
		this->_currVelocity.y = 0.0;
		this->_pushingUp = true;
	}
	else
	{
		this->_pushingUp = false;
	}

	// Update AABB of object to match new position
	this->_aabb.setCenter( this->_currPosition + olc::v2d_generic<long double>( this->_aabbOffset.x, -this->_aabbOffset.y ) ); // negative because negY = worldPosY

	return;
}
*/


void DynamicObject::updateDynamicPhysics( const World* world, float deltaTime )
{
	this->updatePhysicsResponse();

	this->_pushingLeft = this->_pushingLeft || this->_pushingLeftObject;
	this->_pushingRight = this->_pushingRight || this->_pushingRightObject;
	this->_pushingDown = this->_pushingDown || this->_pushingBottomObject;
	this->_pushingUp = this->_pushingUp || this->_pushingTopObject;

	// Update AABB
	this->_aabb.setCenter( this->_currPosition );



	return;
}


/*
void DynamicObject::updateDynamicPhysics( const World* world )
{
	this->updatePhysicsResponse();

	this->_pushingLeft = this->_pushingLeft || this->_pushingLeftObject;
	this->_pushingRight = this->_pushingRight || this->_pushingRightObject;
	this->_pushingDown = this->_pushingDown || this->_pushingBottomObject;
	this->_pushingUp = this->_pushingUp || this->_pushingTopObject;

	// Update AABB
	this->_aabb.setCenter( this->_currPosition );



	return;
}
*/