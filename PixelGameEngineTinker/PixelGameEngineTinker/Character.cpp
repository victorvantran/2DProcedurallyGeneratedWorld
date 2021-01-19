#include "Character.h"
#include "World.h"


// Constructors/Destructors
Character::Character() :
	DynamicObject(),
	_currentState( CharacterState::Stand ), _runSpeed( 0 ), _jumpSpeed( 0 ), _prevCommands{ false }, _currCommands{ false },
	_world( nullptr )
{}


Character::Character( olc::v2d_generic<long double> center, olc::vf2d halfSize, CharacterState characterState, float runSpeed, float jumpSpeed,
	World* world ) :
	DynamicObject( center, halfSize ),
	_currentState( characterState ), _runSpeed( runSpeed ), _jumpSpeed( jumpSpeed ), _prevCommands{ false }, _currCommands{ false },
	_world( world )
{}


Character::~Character() {}



// Getters



// Setters



// Methods
void Character::resetCurrCommands()
{
	std::size_t count = ( std::size_t )Command::count;

	for ( std::size_t i = 0; i < ( std::size_t )Command::count; i++ )
	{
		this->_currCommands[i] = false;
	}
}

void Character::updateCurrCommands( bool* commands )
{
	std::size_t count = ( std::size_t )Command::count;

	for ( std::size_t i = 0; i < ( std::size_t )Command::count; i++ )
	{
		this->_currCommands[i] = commands[i];
	}
}

void Character::updatePrevCommands()
{
	// Cache inputs from previous frame
	std::size_t count = ( std::size_t )Command::count;

	for ( std::size_t i = 0; i < ( std::size_t )Command::count; i++ )
	{
		this->_prevCommands[i] = this->_currCommands[i];
	}

	return;
}


void Character::updateState( float deltaTime )
{
	switch ( this->_currentState )
	{
	case CharacterState::Stand:
		this->_currVelocity = olc::v2d_generic<long double>{ 0.0, 0.0 };
		// [!] animateStand

		// Natural transition(s)
		if ( !this->_pushingDown )
		{
			this->transitionState( CharacterState::Jump );
			break;
		}

		// Command transition(s)
		if ( this->commandState( Command::GoLeft ) != this->commandState( Command::GoRight ) ) // One of it is pressed while the other is not
		{
			this->transitionState( CharacterState::Run );
			break;
		}
		else if ( this->commandState( Command::Jump ) )
		{
			this->_currVelocity.y = this->_jumpSpeed; // [?] on first line of jump case?
			// play jump audio [!]
			this->transitionState( CharacterState::Jump );
			break;
		}
		break;


	case CharacterState::Run:
		// [!] animateRun

		// Natural transition(s)

		// Command transitions(s)
		if ( this->commandState( Command::GoRight ) == this->commandState( Command::GoLeft ) )
		{
			this->_currVelocity = olc::v2d_generic<long double>{ 0.0, 0.0 }; // [?] on first line of stand case?
			this->transitionState( CharacterState::Stand );
			break;
		}
		else if ( this->commandState( Command::GoRight ) )
		{
			if ( this->_pushingRight )
			{
				// pushing against object
				this->_currVelocity.x = 0.0;
			}
			else
			{
				// not pushing against object
				this->_currVelocity.x = this->_runSpeed;
			}

			// [!] Flip Sprite Horizontally
		}
		else if ( this->commandState( Command::GoLeft ) )
		{
			if ( this->_pushingLeft )
			{
				this->_currVelocity.x = 0.0;
			}
			else
			{
				this->_currVelocity.x = -this->_runSpeed;
			}

			// [!] Flip Sprite Horizontally
		}

		if ( this->commandState( Command::Jump ) )
		{
			this->_currVelocity.y = this->_jumpSpeed; // [?] on first line of jump? NO because Jump stands for jump and fall. and when fall, no currVelocity. If we explicitly mutally exclusive states, then we can do that!
			// [!] Play jump sound
			this->transitionState( CharacterState::Jump ); // Jump
			break;
		}
		else if ( !this->_pushingDown )
		{
			// Airborn -> jump -> fall
			this->transitionState( CharacterState::Jump );
			break;
		}
		break;


	case CharacterState::Jump:
		// [!] animateJump

		// Gravity
		this->_currVelocity.y += Settings::World::GRAVITY * deltaTime;
		this->_currVelocity.y = std::max( this->_currVelocity.y, Settings::World::TERMINAL_VELOCITY ); // Termial velocity cap

		// Command transitions(s)
		if ( this->commandState( Command::GoRight ) == this->commandState( Command::GoLeft ) )
		{
			this->_currVelocity.x = 0.0;
		}
		else if ( this->commandState( Command::GoRight ) )
		{
			if ( this->_pushingRight )
			{
				// pushing against object
				this->_currVelocity.x = 0.0;
			}
			else
			{
				// not pushing against object
				this->_currVelocity.x = this->_runSpeed;
			}

			// [!] Flip Sprite Horizontally
		}
		else if ( this->commandState( Command::GoLeft ) )
		{
			if ( this->_pushingLeft )
			{
				this->_currVelocity.x = 0.0;
			}
			else
			{
				this->_currVelocity.x = -this->_runSpeed;
			}

			// [!] Flip Sprite Horizontally
		}

		// Jump higher based on how long button is pressed
		if ( !this->commandState( Command::Jump ) && this->_currVelocity.y > 0.0f )
		{
			this->_currVelocity.y = std::min<float>( this->_currVelocity.y, this->_jumpSpeed * Settings::Player::Character::DEFAULT_MIN_JUMP_RATIO );
		}



		if ( this->_pushingDown )
		{
			// No movement so change stand to statnding
			if ( this->commandState( Command::GoRight ) == this->commandState( Command::GoLeft ) )
			{
				this->_currVelocity = olc::v2d_generic<long double>{ 0.0, 0.0 };
				// Play hit fround audio [!]
				this->transitionState( CharacterState::Stand );
			}
			// Either go right or go left command, so change the state to run
			else
			{
				this->_currVelocity.y = 0.0;
				this->transitionState( CharacterState::Run );

			}
		}


		break;


	case CharacterState::GrabLedge:
		break;
	}


	// std::cout << (int)this->_currentState << std::endl;
	return;
}



void Character::update( float deltaTime, bool* commands )
{
	this->resetCurrCommands();
	this->updateCurrCommands( commands );

	this->updateState( deltaTime );

	if (
		( !this->_pushedDown && this->_pushingDown ) ||
		( !this->_pushedUp && this->_pushingUp ) ||
		( !this->_pushedLeft && this->_pushingLeft ) ||
		( !this->_pushedRight && this->_pushingRight )
		)
	{
		// Play contact audio [!]
	}

	this->updatePhysics( this->_world, deltaTime );
	this->updatePrevCommands();
}