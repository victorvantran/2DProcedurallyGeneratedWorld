#pragma once

#include "olcPixelGameEngine.h"
#include "settings.h"
#include "DynamicObject.h"


enum class KeyInput
{
	UpKey = 0,
	DownKey = 1,
	LeftKey = 2,
	RightKey = 3,
	JumpKey = 4,
	count
};



enum class CharacterState
{
	Stand = 0,
	Walk = 1,
	Jump = 2,
	GrabLedge = 3,
	count
};


class Character : public aabb::DynamicObject
{
private:
	bool* _currInputs;
	bool* _prevInputs;

	CharacterState _currState;
	float _walkSpeed;
	float _jumpSpeed;
	float _gravity;
	float _terminalFallingSpeed;
	float _minJumpSpeed;
	
	olc::vi2d _ledgeTileIndex;
	float _grabLedgeStart;
	float _grabLedgeEnd;
	float _grabLedgeOffset;
	float _grabLedgeJumpSpeed;


	void runStandState();
	void runWalkState();
	void runJumpState( float deltaTime, World& world );
	void runGrabLedgeState();

	

public:
	Character();
	~Character();

	Character( bool currInputs[], bool prevInputs[], olc::vf2d startPosition = olc::vf2d{ 0.0f, 0.0f } );

	void constructCharacter( bool currInputs[], bool prevInputs[], olc::vf2d startPosition );
	void destructCharacter();

	void updateCharacter( float deltaTime, World& world );

	void updatePrevInputs();


	bool keyPressed(KeyInput key);
	bool keyReleased(KeyInput key);
	bool keyState(KeyInput key);


};


Character::Character()
{
	this->constructCharacter( nullptr, nullptr, olc::vf2d{ 0.0f, 0.0f } );
}


Character::~Character()
{
	this->destructCharacter();
}


Character::Character( bool currInputs[], bool prevInputs[], olc::vf2d startPosition )
{
	this->constructCharacter( currInputs, prevInputs, startPosition );
}



void Character::constructCharacter( bool currInputs[], bool prevInputs[], olc::vf2d startPosition )
{
	this->_currInputs = currInputs;
	this->_prevInputs = prevInputs;

	this->_currState = CharacterState::Stand;
	this->_walkSpeed = settings::CHARACTER::WALK_SPEED;
	this->_jumpSpeed = settings::CHARACTER::JUMP_SPEED;
	this->_gravity = settings::CHARACTER::GRAVITY;
	this->_terminalFallingSpeed = settings::CHARACTER::TERMINAL_FALLING_SPEED;
	this->_minJumpSpeed = settings::CHARACTER::MIN_JUMP_SPEED;

	this->_prevCenterPosition = startPosition;
	this->_currCenterPosition = startPosition;
	this->_aabb.halfSize = settings::CHARACTER::AABB_HALF_SIZE;
	this->_aabbOffset = settings::CHARACTER::AABB_OFFSET;
	this->_scale = settings::CHARACTER::SCALE;

	this->_oneWayPlatformThreshold = settings::CHARACTER::ONE_WAY_PLATFORM_THRESHOLD;
	this->_grabLedgeStart = settings::CHARACTER::GRAB_LEDGE_START;
	this->_grabLedgeEnd = settings::CHARACTER::GRAB_LEDGE_END;
	this->_grabLedgeOffset = settings::CHARACTER::GRAB_LEDGE_OFFSET;
	this->_grabLedgeJumpSpeed = settings::CHARACTER::GRAB_LEDGE_JUMP_SPEED;


	return;
}


void Character::destructCharacter()
{
	return;
}


void Character::updateCharacter( float deltaTime, World& world )
{
	switch ( this->_currState )
	{
	case CharacterState::Stand:
		this->runStandState();
		break;
	case CharacterState::Walk:
		this->runWalkState();
		break;
	case CharacterState::Jump:
		this->runJumpState( deltaTime, world );
		break;
	case CharacterState::GrabLedge:
		this->runGrabLedgeState();
		break;
	}

	this->updatePhysics( deltaTime, world );

	// Play an audio when a character collides with an object (just touched ceiling, ground, or walls)
	if ( !this->_prevPushUp && this->_currPushUp
		|| !this->_prevPushDown && this->_currPushDown
		|| !this->_prevPushLeft && this->_currPushLeft
		|| !this->_prevPushRight && this->_currPushRight
		)
	{
		// [!] Play touch audio;
	}

	this->updatePrevInputs();

	return;
}


void Character::runStandState()
{
	this->_currVelocity = olc::vf2d{ 0.0f, 0.0f };
	// [!] Animate stand

	// If there is no floor to push down on, then transition to jump state
	if ( !this->_currPushDown )
	{
		this->_currState = CharacterState::Jump;
		return;
	}

	// If left xor Right key is pressed, then transition to walk state
	if ( this->keyState( KeyInput::RightKey ) != this->keyState( KeyInput::LeftKey ) )
	{
		this->_currState = CharacterState::Walk;
		return;
	}
	// Next priority is Jump key is pressed, then transition to jump state
	else if ( this->keyState( KeyInput::JumpKey ) )
	{
		this->_currVelocity.y = -this->_jumpSpeed;
		this->_currState = CharacterState::Jump;
		return;
	}

	// If the character presses down and is standing on a one way platform, move down the threshold as to pass through it
	if ( this->keyState( KeyInput::DownKey ) )
	{
		if ( _onOneWayPlatform )
		{
			this->_currCenterPosition.y += this->_oneWayPlatformThreshold;
		}
	}

	return;
}


void Character::runWalkState()
{
	// [!] Animate walk

	// If left and right key are both pressed or unpressed, then transition to standing state
	if ( this->keyState( KeyInput::LeftKey ) == this->keyState( KeyInput::RightKey ) )
	{
		this->_currVelocity = olc::vf2d{ 0.0f, 0.0f };
		this->_currState = CharacterState::Stand;
		return;
	}
	// If left key is pressed, set horizotal speed based on if there is no obstacle to push left on
	else if ( this->keyState( KeyInput::LeftKey ) )
	{
		if ( this->_currPushLeft )
		{
			this->_currVelocity.x = 0.0f;
		}
		else
		{
			this->_currVelocity.x = -this->_walkSpeed;
		}
		// [!] Flip sprite horitzontally
	}
	// If right key is pressed, set horizotal speed based on if there is no obstacle to push right on
	else if ( this->keyState( KeyInput::RightKey ) )
	{
		if ( this->_currPushRight )
		{
			this->_currVelocity.x = 0.0f;
		}
		else
		{
			this->_currVelocity.x = this->_walkSpeed;
		}
		// [!] Flip sprite horitzontally
	}


	// Next check for jumping. If the jump key is pressed, set the verticle speed
	if ( this->keyState( KeyInput::JumpKey ) )
	{
		this->_currVelocity.y = -this->_jumpSpeed;
		// [!] Play jump audio
		this->_currState = CharacterState::Jump;
		return;
	}
	// If the character is not pushing down on a floor, transition to falling (jump state)
	else if ( !this->_currPushDown )
	{
		this->_currState = CharacterState::Jump;
		return;
	}

	// If the character presses down and is standing on a one way platform, move down the threshold as to pass through it
	if ( this->keyState( KeyInput::DownKey ) )
	{
		if ( _onOneWayPlatform )
		{
			this->_currCenterPosition.y += this->_oneWayPlatformThreshold;
		}
	}

	return;
}


void Character::runJumpState( float deltaTime, World& world )
{
	// [!] Animate jump

	// Character is affected by gravity
	this->_currVelocity.y = this->_currVelocity.y + this->_gravity * deltaTime;

	// Lock to the limiting terminal velocity. Remember that downwards is higher y value, so we are using max
	this->_currVelocity.y = std::min<float>( this->_currVelocity.y, this->_terminalFallingSpeed);

	// If left and right key are both pressed or unpressed, then no strafing (velocity.x = 0)
	if ( this->keyState( KeyInput::LeftKey ) == this->keyState( KeyInput::RightKey ) )
	{
		this->_currVelocity.x = 0.0f;
	}
	// If left key is pressed, set strafing horizotal speed based on if there is no obstacle to push left on
	else if ( this->keyState( KeyInput::LeftKey ) )
	{
		if ( this->_currPushLeft )
		{
			this->_currVelocity.x = 0.0f;
		}
		else
		{
			this->_currVelocity.x = -this->_walkSpeed;
		}
		// [!] Flip sprite horitzontally
	}
	// If right key is pressed, set strafing horizotal speed based on if there is no obstacle to push right on
	else if ( this->keyState( KeyInput::RightKey ) )
	{
		if ( this->_currPushRight )
		{
			this->_currVelocity.x = 0.0f;
		}
		else
		{
			this->_currVelocity.x = this->_walkSpeed;
		}
		// [!] Flip sprite horitzontally
	}

	// If we hit the ground
	if ( this->_currPushDown )
	{
		// If no movement, then transition to stand
		if ( this->keyState( KeyInput::LeftKey ) == this->keyState( KeyInput::RightKey ) )
		{
			this->_currVelocity.x = 0.0f;
			this->_currVelocity.y = 0.0f;
			this->_currState = CharacterState::Stand;
			// [!] Play sound
		}
		else
		{
			this->_currVelocity.y = 0.0f;
			this->_currState = CharacterState::Walk;
			// [!] Play sound
		}
	}


	// Ledge grab conditions:
	// 1) If the character is falling
	// 2) If the character is not at the ground
	// 3) If the character is not at the ceiling
	// 4) If the character collides with a wall
	if ( this->_currVelocity.y >= 0.0f &&
		!this->_currPushDown &&
		!this->_currPushUp &&
		( ( this->_prevPushLeft && this->keyState( KeyInput::LeftKey ) ) || ( this->_prevPushRight && this->keyState( KeyInput::RightKey ) ) )
		)
	{
		// Look for a ledge to grab
		// Calculate the top position of the sensor which is either top left or top right
		olc::vf2d cornerOffset;

		if ( this->_prevPushLeft && this->_currInputs[ (int)KeyInput::LeftKey ] )
		{
			cornerOffset = -this->getHalfSize() - olc::vf2d{ 1.0f / world.getTileDimension().x, 0.0f };
		}
		else
		{
			cornerOffset = olc::vf2d{ this->getHalfSize().x + ( 1.0f / world.getTileDimension().x ), -this->getHalfSize().y };
		}

		// Check a certain column of tiles from top to bottom (falling)
		int tileX, topY, bottomY;
		tileX = ( int )( this->_currCenterPosition.x + cornerOffset.x );

		if ( ( this->_prevPushLeft && this->_currPushLeft ) || ( this->_prevPushRight && this->_currPushRight ) )
		{
			topY = (int)(this->_prevCenterPosition.y - this->getOffset().y + cornerOffset.y + this->_grabLedgeStart);
			bottomY = ( int )( this->_currCenterPosition.y + cornerOffset.y + this->_grabLedgeEnd );
		}
		else
		{
			topY = ( int )( this->_currCenterPosition.y + cornerOffset.y + this->_grabLedgeStart );
			bottomY = ( int )( this->_currCenterPosition.y + cornerOffset.y + this->_grabLedgeEnd );
		}


		Tile* topTile;
		Tile* bottomTile;
		for ( int y = topY; y <= bottomY; y++ )
		{
			// Conditions for grabbing the tile:
			// 1) The tile is empty
			// 2) The tile directly below is a Block or OneWayPlatform

			topTile = world.getTileFromIndex( tileX, y );
			bottomTile = world.getTileFromIndex( tileX, y + 1 );
			

			if ( topTile != nullptr && bottomTile != nullptr )
			{
				if ( !topTile->isObstacle() &&
					bottomTile->isLedge() )
				{

					//olc::vf2d tileCorner = olc::vi2d{ tileX + ( cornerOffset.x > 0 ) ? 1 : 0, y + 1 };
					olc::vf2d tileCorner = olc::vi2d{ tileX, y + 1 };


					//tileCorner.x += ( cornerOffset.x > 0 ) ? 1 : ( ( cornerOffset.x < 0 ) ? 0 : 0 );


					//tileCorner.x -= ( cornerOffset.x > 0 ) ? 1 : ( ( cornerOffset.x < 0 ) ? -1 : 0 );
					//tileCorner.y -= 0.5f;

					if ( ( y < bottomY ) ||
						( tileCorner.y - ( this->_currCenterPosition.y + cornerOffset.y ) <= this->_grabLedgeEnd ) &&
						( ( this->_currCenterPosition.y + cornerOffset.y ) - tileCorner.y >= this->_grabLedgeStart ) )
					{
						this->_ledgeTileIndex = olc::vi2d{ tileX, y + 1 };
					}

					// Align character with the ledge by the top of the character and top of the tile, and offset
					this->_currCenterPosition.y = tileCorner.y - cornerOffset.y + this->getOffset().y - this->_grabLedgeStart - this->_grabLedgeEnd;
					this->_currVelocity = olc::vf2d{ 0.0f, 0.0f };
					this->_currState = CharacterState::GrabLedge;
					break;
				}
			}

		}

	}
	
	
	// Make the jump higher the longer the button is pressed
	if ( !this->keyState( KeyInput::JumpKey ) && this->_currVelocity.y < 0.0f )
	{
		this->_currVelocity.y = -std::min<float>( this->_currVelocity.y, this->_minJumpSpeed );
	}
	



	return;
}


void Character::runGrabLedgeState()
{ 
	// Determine if the ledge is to the left or right of the character
	bool ledgeOnLeft = this->_ledgeTileIndex.x < this->_currCenterPosition.x;
	bool ledgeOnRight = !ledgeOnLeft;


	// To drop down from the ledge, the player can:
	// 1) press Down
	// 2) press left if grab right
	// 3) press right if grab left
	if ( this->_currInputs[( int )KeyInput::DownKey] ||
		( !this->_currInputs[( int )KeyInput::LeftKey] && ledgeOnLeft ) ||
		( !this->_currInputs[( int )KeyInput::RightKey] && ledgeOnRight ) 
		)
	{
		this->_currState = CharacterState::Jump;
	}
	// To jump up to the ledge, the player simply presses the jump key
	else if ( this->_currInputs[( int )KeyInput::JumpKey] )
	{
		this->_currVelocity.y = -this->_grabLedgeJumpSpeed;
		this->_currState = CharacterState::Jump;
	}
	return;
}



void Character::updatePrevInputs()
{
	/// Move the key values from currInputs to prevInputs

	for ( int i = 0; i < ( int )KeyInput::count; i++ )
	{
		this->_prevInputs[i] = this->_currInputs[i];
	}
	return;
}


bool Character::keyPressed( KeyInput key )
{
	return this->_currInputs[( int )key] && !this->_prevInputs[( int )key];
}


bool Character::keyReleased( KeyInput key )
{
	return !this->_currInputs[( int )key] && this->_prevInputs[( int )key];
}


bool Character::keyState( KeyInput key )
{
	return this->_currInputs[( int )key];
}




