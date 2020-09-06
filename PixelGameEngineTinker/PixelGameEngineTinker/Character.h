#pragma once

#include "olcPixelGameEngine.h"
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


class Character : private aabb::DynamicObject
{
private:
	bool _currInputs[(int)KeyInput::count];
	bool _prevInputs[(int)KeyInput::count];

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

	void constructCharacter();
	void destructCharacter();

	void updateCharacter( float deltaTime );

	void updatePrevInputs();


	bool keyPressed(KeyInput key);
	bool keyReleased(KeyInput key);
	bool keyState(KeyInput key);

};


Character::Character()
{
	this->constructCharacter();
}


Character::~Character()
{
	this->destructCharacter();
}


void Character::constructCharacter()
{
	this->_currState = CharacterState::Stand;
	this->_walkSpeed = 10.0f;
	this->_jumpSpeed = 30.0f;
	this->_gravity = 9.8f;
	this->_terminalFallingSpeed = 10.0f;
	this->_minJumpSpeed = this->_jumpSpeed / 2.0f;
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
		this->_currVelocity.y = this->_jumpSpeed;
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
		this->_currVelocity.y = this->_jumpSpeed;
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
	this->_currVelocity.y = std::max<float>( this->_currVelocity.y, this->_terminalFallingSpeed);

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

	// Make the jump higher the longer the button is pressed
	if ( !this->keyState( KeyInput::JumpKey ) && this->_currVelocity.y > 0.0f )
	{
		this->_currVelocity.y = std::min<float>( this->_currVelocity.y, this->_minJumpSpeed );
	}

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

