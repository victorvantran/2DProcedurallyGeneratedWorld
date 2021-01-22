#pragma once

#include <cmath>
#include "olcPixelGameEngine.h"
#include "Settings.h"
#include "AABB.h"


class World;

class DynamicObject
{
private:

protected:
	olc::v2d_generic<long double> _prevPosition;
	olc::v2d_generic<long double> _currPosition; // worldPosition ( Everything follows x,y except Position which follows x, -y to match worldPosition). Center position
	
	bool _pushedRight;
	bool _pushingRight;

	bool _pushedLeft;
	bool _pushingLeft;

	bool _pushedDown; // wasOnGround
	bool _pushingDown; // onGround

	bool _pushedUp; // wasAtCeiling
	bool _pushingUp; // atCeiling

	bool _onOneWayPlatform;

	olc::vf2d _prevVelocity;
	olc::vf2d _currVelocity;
	
	olc::vf2d _scale;

	AABB _aabb;
	olc::vf2d _aabbOffset;
public:
	// Constructors/Destructors
	DynamicObject();
	DynamicObject( const olc::v2d_generic<long double>& center, const olc::vf2d& halfSize, const olc::vf2d& scale );
	~DynamicObject();

	// Getters
	olc::vf2d getScale() const;
	float getScaleX() const;
	float getScaleY() const;
	AABB getAABB() const;
	olc::vf2d getAABBOffset() const;
	float getAABBOffsetX() const;
	float getAABBOffsetY() const;

	// Setters
	void setScale( const olc::vf2d& scale );
	void setScaleX( float x );
	void setScaleY( float y );
	void setAABBOffset( const olc::vf2d& aabbOffset );
	void setAABBOffsetX( float x );
	void setAABBOffsetY( float y );

	// Methods
	bool isCollidingDown( const World* world, long double& worldGroundY, bool& onOneWayPlatform );
	bool isCollidingUp( const World* world, long double& worldCeilingY );
	bool isCollidingLeft( const World* world, long double& worldLeftWallX );
	bool isCollidingRight( const World* world, long double& worldRightWallX );

	void updatePhysics( const World* world, float deltaTime );
};