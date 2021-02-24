#pragma once

#include <cmath>
#include <set>
#include "olcPixelGameEngine.h"
#include "Settings.h"
#include "AABB.h"
// #include "CollisionData.h"


class World;
class CollisionData;

class DynamicObject
{
private:

protected:
	olc::v2d_generic<long double> _prevPosition;
	olc::v2d_generic<long double> _currPosition; // worldPosition ( Everything follows x,y except Position which follows x, -y to match worldPosition). Center position
	
	// Optimize by setting these booleans as bits
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

	// World interation
	World* _world;
	long double _time;

	// Collision Detection/Response
	std::vector<std::pair<std::int64_t,std::size_t>> _spaces;
	std::vector<CollisionData> _allCollisions;
	bool _isKinematic;

	bool _pushedLeftObject;
	bool _pushingLeftObject;

	bool _pushedRightObject;
	bool _pushingRightObject;

	bool _pushedBottomObject;
	bool _pushingBottomObject;

	bool _pushedTopObject;
	bool _pushingTopObject;

public:
	// Constructors/Destructors
	DynamicObject();
	DynamicObject( const olc::v2d_generic<long double>& center, const olc::vf2d& halfSize, const olc::vf2d& scale, World* world, long double time );
	~DynamicObject();


	// Getters
	olc::vf2d getScale() const;
	float getScaleX() const;
	float getScaleY() const;
	AABB getAABB() const;
	olc::vf2d getAABBOffset() const;
	float getAABBOffsetX() const;
	float getAABBOffsetY() const;
	olc::v2d_generic<long double> getPrevPosition() const;
	olc::v2d_generic<long double> getCurrPosition() const;
	olc::vf2d getPrevVelocity() const;
	olc::vf2d getCurrVelocity() const;
	bool getPushingLeft() const;
	bool getPushingRight() const;
	bool getPushingUp() const;
	bool getPushingDown() const;


	// Setters
	void setScale( const olc::vf2d& scale );
	void setScaleX( float x );
	void setScaleY( float y );
	void setAABBOffset( const olc::vf2d& aabbOffset );
	void setAABBOffsetX( float x );
	void setAABBOffsetY( float y );
	void addToSpaces( std::int64_t spaceIndex, std::size_t id );


	// Methods
	bool isCollidingDown( const World* world, long double& worldGroundY, bool& onOneWayPlatform );
	bool isCollidingUp( const World* world, long double& worldCeilingY );
	bool isCollidingLeft( const World* world, long double& worldLeftWallX );
	bool isCollidingRight( const World* world, long double& worldRightWallX );


	// Collision Detection
	std::vector<std::pair<std::int64_t, std::size_t>>& getSpaces();
	std::vector<CollisionData>& getAllCollisions();
	bool hasCollisionDataFor( DynamicObject* otherObject ) const;


	// Collision Response
	bool isKinematic() const;
	void updatePhysicsResponse();


	// Physics Update
	void updateStaticPhysics( const World* world, float deltaTime );
	void updateDynamicPhysics( const World* world, float deltaTime );
	virtual void update( float deltaTime, bool* commands ) = 0;

};