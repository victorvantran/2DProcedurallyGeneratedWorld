#include "Zombie.h"
#include "World.h"


// Constructors/Destructors
Zombie::Zombie() :
	DynamicObject(),
	_ledgeTile( olc::v2d_generic<std::int64_t>{ 0, 0 } ), _cannotGoLeftFrames( 0 ), _cannotGoRightFrames( 0 ),
	_framesFromJumpStart( 0 ),
	_currentState( ZombieState::Stand ), _runSpeed( 0 ), _jumpSpeed( 0 ), _prevCommands{ false }, _currCommands{ false },
	_world( nullptr )
{}


Zombie::Zombie( const olc::v2d_generic<long double>& center, const olc::vf2d& halfSize, const olc::vf2d& scale, ZombieState characterState, float runSpeed, float jumpSpeed,
	World* world, long double time ) :
	DynamicObject( center, halfSize, scale, world, time ),
	_ledgeTile( olc::v2d_generic<std::int64_t>{ 0, 0 } ), _cannotGoLeftFrames( 0 ), _cannotGoRightFrames( 0 ),
	_framesFromJumpStart( 0 ),
	_currentState( characterState ), _runSpeed( runSpeed ), _jumpSpeed( jumpSpeed ), _prevCommands{ false }, _currCommands{ false },
	_world( world )
{}


Zombie::~Zombie() {}



// Getters



// Setters



// Methods
void Zombie::resetCurrCommands()
{
	std::size_t count = ( std::size_t )ZombieCommand::count;

	for ( std::size_t i = 0; i < ( std::size_t )ZombieCommand::count; i++ )
	{
		this->_currCommands[i] = false;
	}
}

void Zombie::updateCurrCommands( bool* commands )
{
	std::size_t count = ( std::size_t )ZombieCommand::count;

	for ( std::size_t i = 0; i < ( std::size_t )ZombieCommand::count; i++ )
	{
		this->_currCommands[i] = commands[i];
	}
}

void Zombie::updatePrevCommands()
{
	// Cache inputs from previous frame
	std::size_t count = ( std::size_t )ZombieCommand::count;

	for ( std::size_t i = 0; i < ( std::size_t )ZombieCommand::count; i++ )
	{
		this->_prevCommands[i] = this->_currCommands[i];
	}

	return;
}


void Zombie::updateState( float deltaTime )
{
	switch ( this->_currentState )
	{
	case ZombieState::Stand:
		this->_currVelocity = olc::v2d_generic<long double>{ 0.0, 0.0 };
		// [!] animateStand

		// Natural transition(s)
		if ( !this->_pushingDown )
		{
			this->transitionState( ZombieState::Jump );
			break;
		}

		// Command transition(s)
		if ( this->commandState( ZombieCommand::GoLeft ) != this->commandState( ZombieCommand::GoRight ) ) // One of it is pressed while the other is not
		{
			this->transitionState( ZombieState::Run );
			break;
		}
		else if ( this->commandState( ZombieCommand::Jump ) )
		{
			this->_currVelocity.y = this->_jumpSpeed; // [?] on first line of jump case?
			// play jump audio [!]
			this->transitionState( ZombieState::Jump );
			break;
		}
		else if ( this->commandState( ZombieCommand::Drop ) )
		{
			if ( this->_onOneWayPlatform )
			{
				this->_currPosition.y += Settings::DynamicObject::ONE_WAY_PLATFORM_THRESHOLD;
			}
		}
		break;

	case ZombieState::Run:
		// [!] animateRun

		// Natural transition(s)

		// Command transitions(s)
		if ( this->commandState( ZombieCommand::GoRight ) == this->commandState( ZombieCommand::GoLeft ) )
		{
			this->_currVelocity = olc::v2d_generic<long double>{ 0.0, 0.0 }; // [?] on first line of stand case?
			this->transitionState( ZombieState::Stand );
			break;
		}
		else if ( this->commandState( ZombieCommand::GoRight ) )
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
		else if ( this->commandState( ZombieCommand::GoLeft ) )
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

		if ( this->commandState( ZombieCommand::Jump ) )
		{
			this->_currVelocity.y = this->_jumpSpeed; // [?] on first line of jump? NO because Jump stands for jump and fall. and when fall, no currVelocity. If we explicitly mutally exclusive states, then we can do that!
			// [!] Play jump sound
			this->transitionState( ZombieState::Jump ); // Jump
			break;
		}
		else if ( !this->_pushingDown )
		{
			// Airborn -> jump -> fall
			this->transitionState( ZombieState::Jump );
			break;
		}


		if ( this->commandState( ZombieCommand::Drop ) )
		{
			if ( this->_onOneWayPlatform )
			{
				this->_currPosition.y += Settings::DynamicObject::ONE_WAY_PLATFORM_THRESHOLD;
			}
		}
		break;


	case ZombieState::Jump:
		// [!] animateJump

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
			else if ( this->commandState( ZombieCommand::Jump ) )
			{
				this->_currVelocity.y = this->_jumpSpeed;
			}
		}



		// Gravity
		this->_currVelocity.y += Settings::World::GRAVITY * deltaTime;
		this->_currVelocity.y = std::max( this->_currVelocity.y, Settings::World::TERMINAL_VELOCITY ); // Termial velocity cap

		// Command transitions(s)
		if ( this->commandState( ZombieCommand::GoRight ) == this->commandState( ZombieCommand::GoLeft ) )
		{
			this->_currVelocity.x = 0.0;
		}
		else if ( this->commandState( ZombieCommand::GoRight ) )
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
		else if ( this->commandState( ZombieCommand::GoLeft ) )
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
		if ( !this->commandState( ZombieCommand::Jump ) && this->_currVelocity.y > 0.0f )
		{
			this->_currVelocity.y = std::min<float>( this->_currVelocity.y, this->_jumpSpeed * Settings::Player::Character::DEFAULT_MIN_JUMP_RATIO );
		}


		// Conditions of falling
		if ( this->_pushingDown )
		{
			// No movement so change stand to statnding
			if ( this->commandState( ZombieCommand::GoRight ) == this->commandState( ZombieCommand::GoLeft ) )
			{
				this->_currVelocity = olc::v2d_generic<long double>{ 0.0, 0.0 };
				// Play hit fround audio [!]
				this->transitionState( ZombieState::Stand );
			}
			// Either go right or go left command, so change the state to run
			else
			{
				this->_currVelocity.y = 0.0;
				this->transitionState( ZombieState::Run );

			}
		}


		// Prevents immediate grab ledge for a few constant frames after letting go
		// Falsifies GoLeft/GoRight command after letting go
		if ( this->_cannotGoLeftFrames > 0 )
		{
			this->_cannotGoLeftFrames--;
			this->setCommandState( ZombieCommand::GoLeft, false );
		}
		if ( this->_cannotGoRightFrames > 0 )
		{
			this->_cannotGoRightFrames--;
			this->setCommandState( ZombieCommand::GoRight, false );
		}

		// Conditions to grab ledge
		// [!] add grabLedgeEnabled Condition
		if ( this->_currVelocity.y <= 0.0 &&
			!this->_pushingUp &&
			( ( this->_pushingRight && this->commandState( ZombieCommand::GoRight ) ) || ( this->_pushingLeft && this->commandState( ZombieCommand::GoLeft ) ) )
			)
		{
			// Look for ledge to grab ( Calculate Top Sensor (left or right) )

			std::uint16_t tileSize = Settings::World::CELL_SIZE;
			long double give = ( 1.0 / ( long double )tileSize );

			olc::v2d_generic<long double> cornerOffset;
			if ( this->_pushingRight && this->commandState( ZombieCommand::GoRight ) )
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
						this->transitionState( ZombieState::GrabLedge );
						break;
					}

				}

			}
		}
		break;

	case ZombieState::GrabLedge:
		// Determine if the ledge is to the left or right of the character
		bool ledgeOnLeft = this->_ledgeTile.x < this->_currPosition.x;
		bool ledgeOnRight = !ledgeOnLeft;

		// Command to let go of ledge
		if ( this->commandState( ZombieCommand::Drop ) ||
			( this->commandState( ZombieCommand::GoLeft ) && ledgeOnRight ) ||
			( this->commandState( ZombieCommand::GoRight ) && ledgeOnLeft )
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

			this->transitionState( ZombieState::Jump );
			break;
		}
		else if ( this->commandState( ZombieCommand::Jump ) )
		{
			this->_currVelocity.y = this->_jumpSpeed;
			this->transitionState( ZombieState::Jump );
			break;
		}
		break;
	}

	return;
}



void Zombie::update( float deltaTime, bool* commands )
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
}
