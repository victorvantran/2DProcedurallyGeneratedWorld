#include "DynamicObject.h"
#include "World.h"


// Constructors/Destructors
DynamicObject::DynamicObject() :
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


DynamicObject::DynamicObject( olc::v2d_generic<long double> center, olc::vf2d halfSize ) :
	_aabb( center, halfSize ),
	_aabbOffset( olc::vf2d{ 0.0f, 0.0f } ), _scale( olc::vf2d{ 1.0, 1.0 } ),
	_prevPosition( olc::v2d_generic<long double>{ 0.0, 0.0 } ), _currPosition( olc::v2d_generic<long double>{ 0.0, 0.0 } ),
	_prevVelocity( olc::vf2d{ 0.0f, 0.0f } ), _currVelocity( olc::vf2d{ 0.0f, 0.0f } ),
	_pushedRight( false ), _pushingRight( false ),
	_pushedLeft( false ), _pushingLeft( false ),
	_pushedUp( false ), _pushingUp( false ),
	_pushedDown( false ), _pushingDown( false )
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

AABB DynamicObject::getAABB() const
{
	return this->_aabb;
}

olc::vf2d DynamicObject::getAABBOffset() const
{
	return this->_aabbOffset;
}


// Setters


// Methods
bool DynamicObject::hasGround( const World* world, long double& worldGroundY )
{
	olc::v2d_generic center = this->_currPosition + olc::v2d_generic<long double>( this->_aabbOffset.x, -this->_aabbOffset.y );

	// Bottom sensor line
	std::uint16_t tileSize = Settings::World::CELL_SIZE;
	long double give = ( 1.0 / ( long double )tileSize ); // Give
	olc::v2d_generic bottomSensorLeft = center - olc::v2d_generic<long double>( this->_aabb.getHalfSize().x - give, - this->_aabb.getHalfSize().y - give );
	olc::v2d_generic bottomSensorRight = olc::v2d_generic( bottomSensorLeft.x + ( ( long double )this->_aabb.getHalfSize().x * 2.0 ) - 2.0 * give, bottomSensorLeft.y );

	// Check the tiles along the sensor line
	for ( olc::v2d_generic<long double> checkedTile = bottomSensorLeft; ; checkedTile.x += 1.0 )
	{
		checkedTile.x = std::min( checkedTile.x, bottomSensorRight.x );

		std::int64_t tileIndexX = std::floor( checkedTile.x );
		std::int64_t tileIndexY = std::floor( checkedTile.y );

		worldGroundY = ( long double )( tileIndexY );

		const Tile* contactTile = world->getTile( tileIndexX, tileIndexY );
		if ( contactTile == nullptr )
		{
			return true; // [!] Throw error
		}
		else if ( contactTile->isObstacle() )
		{
			return true;
		}

		if ( checkedTile.x >= bottomSensorRight.x ) break;
	}

	return false;
}



void DynamicObject::updatePhysics( const World* world, float deltaTime )
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

	/*
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
	*/

	long double worldGroundY = this->_currPosition.y + this->_aabbOffset.y;
	if ( this->_currVelocity.y <= 0.0f && this->hasGround( world, worldGroundY ) )
	{
		this->_currPosition.y = worldGroundY - this->_aabb.getHalfSize().y + this->_aabbOffset.y;
		this->_currVelocity.y = 0.0;
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