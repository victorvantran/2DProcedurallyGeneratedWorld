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
bool DynamicObject::isCollidingDown( const World* world, long double& worldGroundY )
{
	olc::v2d_generic<long double> prevCenter = this->_prevPosition + olc::v2d_generic<long double>( this->_aabbOffset.x, -this->_aabbOffset.y );
	olc::v2d_generic<long double> currCenter = this->_currPosition + olc::v2d_generic<long double>( this->_aabbOffset.x, -this->_aabbOffset.y );

	std::uint16_t tileSize = Settings::World::CELL_SIZE;
	long double give = ( 1.0 / ( long double )tileSize );

	// Prev bottom sensor line
	olc::v2d_generic<long double> prevBottomSensorLeft = prevCenter - olc::v2d_generic<long double>( this->_aabb.getHalfSize().x - give, -this->_aabb.getHalfSize().y - give );

	// Curr bottom sensor line
	olc::v2d_generic<long double> currBottomSensorLeft = currCenter - olc::v2d_generic<long double>( this->_aabb.getHalfSize().x - give, - this->_aabb.getHalfSize().y - give );
	olc::v2d_generic<long double> currBottomSensorRight = olc::v2d_generic( currBottomSensorLeft.x + ( ( long double )this->_aabb.getHalfSize().x * 2.0 ) - 2.0 * give, currBottomSensorLeft.y );


	// Interpolate on Y range
	std::int64_t endY = ( std::int64_t )std::floor( currBottomSensorLeft.y );
	std::int64_t begY = std::min( ( std::int64_t )(std::floor( prevBottomSensorLeft.y ) + 1 ), endY );
	std::int64_t dist = std::max<int>( std::abs( endY - begY ), 1 ); // Mininmum of 1 to use next iteration position for detecting

	std::int64_t tileIndexX;

	// Can optimize by only using interpolation checks on objects that go past a certain velocity (at a fixed max fps update),
	// and objects that go slow enough as to never be able to pass through multiple tiles (at a fixed max fps update) will use crude but fast checks [~]
	for ( std::int64_t tileIndexY = begY; tileIndexY <= endY; tileIndexY++ )
	{
		// Interpolate
		long double t = std::abs( endY - tileIndexY ) / dist;
		olc::v2d_generic<long double> checkBottomLeft = ( currBottomSensorLeft * t + prevBottomSensorLeft * ( 1 - t ) );
		olc::v2d_generic<long double> checkBottomRight = olc::v2d_generic<long double>{ checkBottomLeft.x + ( ( long double )this->_aabb.getHalfSize().x * 2.0 ) - 2.0 * give, checkBottomLeft.y };
	
		for ( olc::v2d_generic<long double> checkedTile = checkBottomLeft; ; checkedTile.x += 1.0 )
		{
			checkedTile.x = std::min( checkedTile.x, checkBottomRight.x );

			std::int64_t tileIndexX = std::floor( checkedTile.x );
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

			if ( checkedTile.x >= checkBottomRight.x ) break;
		}
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

	long double worldGroundY = this->_currPosition.y + this->_aabbOffset.y;
	if ( this->_currVelocity.y <= 0.0f && this->isCollidingDown( world, worldGroundY ) )
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