#include "Character.h"
#include "World.h"


// Constructors/Destructors
Character::Character() :
	DynamicObject(),
	_ledgeTile( olc::v2d_generic<std::int64_t>{ 0, 0 } ), _cannotGoLeftFrames( 0 ), _cannotGoRightFrames( 0 ),
	_framesFromJumpStart( 0 ),
	_currentState( CharacterState::Stand ), _runSpeed( 0 ), _jumpSpeed( 0 ), _prevCommands{ false }, _currCommands{ false },
	_world( nullptr )
{}


Character::Character( const olc::v2d_generic<long double>& center, const olc::vf2d& halfSize, const olc::vf2d& scale, CharacterState characterState, float runSpeed, float jumpSpeed,
	World* world, float time ) :
	DynamicObject( center, halfSize, scale, world, time ),
	_ledgeTile( olc::v2d_generic<std::int64_t>{ 0, 0 } ), _cannotGoLeftFrames( 0 ), _cannotGoRightFrames( 0 ),
	_framesFromJumpStart( 0 ),
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
		this->_animator.update( deltaTime, AlphaAnimator::GraphicState::Idle, this->_currVelocity.x ); // [!] animateStand

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
		else if ( this->commandState( Command::Drop ) )
		{
			if ( this->_onOneWayPlatform )
			{
				this->_currPosition.y += Settings::DynamicObject::ONE_WAY_PLATFORM_THRESHOLD;
			}
		}
		break;

	case CharacterState::Run:
		this->_animator.update( deltaTime, AlphaAnimator::GraphicState::Run, this->_currVelocity.x ); // [!] animateRun

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


		if ( this->commandState( Command::Drop ) )
		{
			if ( this->_onOneWayPlatform )
			{
				this->_currPosition.y += Settings::DynamicObject::ONE_WAY_PLATFORM_THRESHOLD;
			}
		}
		break;


	case CharacterState::Jump:
		this->_animator.update( deltaTime, AlphaAnimator::GraphicState::Jump, this->_currVelocity.x ); // [!] animateJump

		//this->_framesFromJumpStart++;
		//std::cout << this->_framesFromJumpStart << std::endl;

		if ( this->_framesFromJumpStart <= Settings::Player::Character::JUMP_FRAMES_THREASHOLD )
		{
			this->_framesFromJumpStart++;
			// No "midair jump" if hit ceiling or at verticle speed already

			if ( this->_pushingUp || this->_currVelocity.y > 0.0 )
			{
				this->_framesFromJumpStart = Settings::Player::Character::JUMP_FRAMES_THREASHOLD + 1;
			}
			else if ( this->commandState( Command::Jump ) )
			{
				this->_currVelocity.y = this->_jumpSpeed;
			}
		}



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


		// Conditions of falling
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


		// Prevents immediate grab ledge for a few constant frames after letting go
		// Falsifies GoLeft/GoRight command after letting go
		if ( this->_cannotGoLeftFrames > 0 )
		{
			this->_cannotGoLeftFrames--;
			this->setCommandState( Command::GoLeft, false );
		}
		if ( this->_cannotGoRightFrames > 0 )
		{
			this->_cannotGoRightFrames--;
			this->setCommandState( Command::GoRight, false );
		}

		// Conditions to grab ledge
		// [!] add grabLedgeEnabled Condition
		if ( this->_currVelocity.y <= 0.0 && 
			!this->_pushingUp &&
			( ( this->_pushingRight && this->commandState( Command::GoRight ) ) || ( this->_pushingLeft && this->commandState( Command::GoLeft) ) )
			)
		{
			// Look for ledge to grab ( Calculate Top Sensor (left or right) )
			
			std::uint16_t tileSize = Settings::World::CELL_SIZE;
			long double give = ( 1.0 / ( long double )tileSize );

			olc::v2d_generic<long double> cornerOffset;
			if ( this->_pushingRight && this->commandState( Command::GoRight ) )
			{
				cornerOffset = olc::v2d_generic<long double>( this->_aabb.getHalfSizeX() + give, -this->_aabb.getHalfSizeY() );
			}
			else
			{
				cornerOffset = olc::v2d_generic<long double>( -this->_aabb.getHalfSizeX() - give, -this->_aabb.getHalfSizeY() );
			}

			std::int64_t checkTileX = ( std::int64_t )std::floor( this->_currPosition.x + cornerOffset.x );
			std::int64_t topY;
			std::int64_t bottomY;

			if ( ( this->_pushedLeft && this->_pushingLeft ) || ( this->_pushedRight && this->_pushingRight ) )
			{
				topY = ( std::int64_t )std::floor( this->_prevPosition.y - this->_aabbOffset.y + cornerOffset.y + Settings::Player::Character::GRAB_LEDGE_START_Y ); // [!] + aabboffset
				bottomY = ( std::int64_t )std::floor( this->_currPosition.y + cornerOffset.y + Settings::Player::Character::GRAB_LEDGE_END_Y );
			}
			else
			{
				topY = ( std::int64_t )std::floor( this->_currPosition.y + cornerOffset.y + Settings::Player::Character::GRAB_LEDGE_START_Y );
				bottomY = ( std::int64_t )std::floor( this->_currPosition.y + cornerOffset.y + Settings::Player::Character::GRAB_LEDGE_END_Y );
			}
			
			for ( std::int64_t y = topY; y <= bottomY; y++ ) // Top to bottom
			{
				const Tile* oneOverLedge = this->_world->getTile( checkTileX, y );
				const Tile* ledge = this->_world->getTile( checkTileX, y + 1 );
				if ( oneOverLedge == nullptr || ledge == nullptr )
				{
					// Throw error [!]
					break;
				}
				// If ledge is valid
				else if ( !oneOverLedge->isObstacle() && ledge->isObstacle() )
				{
					// Position character to the ledge
					olc::v2d_generic<long double> tileCornerPos = olc::v2d_generic<long double>{ ( long double )checkTileX, ( long double )( y + 1 ) };

					if ( ( y < bottomY ) ||
						( ( ( this->_currPosition.y + cornerOffset.y ) - tileCornerPos.y >= Settings::Player::Character::GRAB_LEDGE_END_Y ) &&
						( tileCornerPos.y - ( this->_currPosition.y + cornerOffset.y ) <= Settings::Player::Character::GRAB_LEDGE_START_Y ) ) )
					{
						this->_ledgeTile = olc::v2d_generic<std::int64_t>{ checkTileX, y + 1 };

						this->_currPosition.y = tileCornerPos.y - cornerOffset.y + this->_aabbOffset.y + Settings::Player::Character::GRAB_LEDGE_START_Y - Settings::Player::Character::GRAB_LEDGE_TILE_OFFSET_Y;
						this->_currVelocity = olc::v2d_generic<long double>{ 0.0, 0.0 };
						this->transitionState( CharacterState::GrabLedge );
						break;
					}
					
				}

			}
		}
		break;

	case CharacterState::GrabLedge:
		// Determine if the ledge is to the left or right of the character
		bool ledgeOnLeft = this->_ledgeTile.x < this->_currPosition.x;
		bool ledgeOnRight = !ledgeOnLeft;

		// Command to let go of ledge
		if ( this->commandState( Command::Drop ) ||
			( this->commandState( Command::GoLeft ) && ledgeOnRight ) ||
			( this->commandState( Command::GoRight ) && ledgeOnLeft )
			)
		{
			if ( ledgeOnLeft )
			{
				this->_cannotGoLeftFrames = Settings::Player::Character::GRAB_LEDGE_LET_GO_FRAMES;
			}
			else
			{
				this->_cannotGoRightFrames = Settings::Player::Character::GRAB_LEDGE_LET_GO_FRAMES;
			}

			this->transitionState( CharacterState::Jump );
			break;
		}
		else if ( this->commandState( Command::Jump ) )
		{
			this->_currVelocity.y = this->_jumpSpeed;
			this->transitionState( CharacterState::Jump );
			break;
		}
		break;
	}

	return;
}



void Character::update( float deltaTime, bool* commands )
{
	this->resetCurrCommands();
	this->updateCurrCommands( commands );

	// reset

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


	//std::cout << (int)this->_currentState << std::endl;

	this->updateStaticPhysics( this->_world, deltaTime );


	// Every time we leave the ground, set frame start to 0. Incremented every time in the jump state
	if ( this->_pushedDown && !this->_pushingDown )
	{
		this->_framesFromJumpStart = 0;
	}

	this->updatePrevCommands();

	this->_time += deltaTime;
}