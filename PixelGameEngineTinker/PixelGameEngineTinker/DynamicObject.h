#pragma once

#include "olcPixelGameEngine.h"
#include "Settings.h"
#include "AABB.h"


class World;

class DynamicObject
{
private:

protected:
	olc::v2d_generic<long double> _prevPosition;
	olc::v2d_generic<long double> _currPosition; // worldPosition ( Everything follows x,y except Position which follows x, -y to match worldPosition)
	
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
	DynamicObject( olc::v2d_generic<long double> center, olc::vf2d halfSize );
	~DynamicObject();

	// Getters
	olc::vf2d getScale() const;
	AABB getAABB() const;
	olc::vf2d getAABBOffset() const;

	// Setters


	// Methods
	bool isCollidingDown( const World* world, long double& worldGroundY, bool& onOneWayPlatform );
	void updatePhysics( const World* world, float deltaTime );
};