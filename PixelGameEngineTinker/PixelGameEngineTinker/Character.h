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

	void runStandState();
	void runWalkState();
	void runJumpState( float deltaTime );
	void runGrabLedgeState();

	

public:
	Character();
	~Character();

	Character( bool currInputs[], bool prevInputs[], olc::vf2d startPosition = olc::vf2d{ 0.0f, 0.0f } );

	void constructCharacter( bool currInputs[], bool prevInputs[], olc::vf2d startPosition );
	void destructCharacter();

	void updateCharacter( float deltaTime );

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

	this->_prevPosition = startPosition;
	this->_currPosition = startPosition;
	this->_aabb.halfSize = settings::CHARACTER::AABB_HALF_SIZE;
	this->_aabbOffset = settings::CHARACTER::AABB_OFFSET;
	this->_scale = settings::CHARACTER::SCALE;
	return;
}


void Character::destructCharacter()
{
	return;
}


void Character::updateCharacter( float deltaTime )
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
		this->runJumpState( deltaTime );
		break;
	case CharacterState::GrabLedge:
		this->runGrabLedgeState();
		break;
	}

	this->updatePhysics( deltaTime );

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

	return;
}


void Character::runJumpState( float deltaTime )
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

	/*
	// Make the jump higher the longer the button is pressed
	if ( !this->keyState( KeyInput::JumpKey ) && this->_currVelocity.y > 0.0f )
	{
		this->_currVelocity.y = -std::min<float>( this->_currVelocity.y, this->_minJumpSpeed );
	}
	*/


	/// Temporary make object not fall off screen (the bottom of screen "simulates" a floor)
	/*
	if ( this->_currPosition.y > settings::RESOLUTION::SCREEN_DIMENSION.y - this->getHalfSize().y )
	{
		if ( !this->keyState( KeyInput::JumpKey ) )
		{
			this->_currPosition.y = settings::RESOLUTION::SCREEN_DIMENSION.y - this->getHalfSize().y;
			this->_currPushDown = true;
			this->_currState = CharacterState::Stand;
		}
	}
	else
	{
		this->_currPushDown = false;
	}
	*/

	/*
	float contactY = 0.0f;
	if ( this->_currVelocity.y <= 0.0f && this->isCollidingDown(this->_prevPosition, this->_currPosition, this->_currVelocity, contactY, world) )
	{
		this->_currPosition.y = contactY + this->getHalfSize().y - this->getOffset().y;
		this->_currVelocity.y = 0.0f;
		this->_currPushDown = true;
	}
	else
	{
		this->_currPushDown = false;
	}
	*/

	return;
}


void Character::runGrabLedgeState()
{
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




