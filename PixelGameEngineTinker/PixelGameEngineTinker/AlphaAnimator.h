#pragma once

// Animator for Character

class AlphaAnimator
{
public:
	enum class GraphicState
	{
		Idle,
		Run,
		Jump,
		Hurt,
		Melee,
		Bow,
		Dead,
		count
	};


	enum class GraphicStateTransitions
	{
		Idle = 8,
		Run = 8,
		Jump = 1,
		Hurt = 4,
		Melee = 4,
		Bow = 4,
		Dead = 8
	};

	enum class Direction
	{
		Left,
		Right
	};

private:
	float _localTime;
	std::int16_t _localTick;

	// Sprite information
	olc::Sprite* _sprite;
	olc::Decal* _decal;
	static constexpr std::uint64_t PIXEL_SIZE_X = 64;
	static constexpr std::uint64_t PIXEL_SIZE_Y = 64;
	static constexpr std::uint8_t MAX_GRAPHIC_COUNTER = 8; // number of columns in out sprite (greatest common multiple between 4 and 8 = 8)
	static constexpr std::int8_t OFFSET_X = 20;
	static constexpr std::int8_t OFFSET_Y = 15;

	GraphicState _graphicState;
	Direction _direction;
	std::uint8_t _graphicCounter;
public:
	AlphaAnimator() : _sprite( nullptr ), _decal( nullptr ), _localTick( 0 ), _localTime( 0 ), _graphicState( GraphicState::Idle ), _direction( Direction::Right ), _graphicCounter( 0 ) {}
	AlphaAnimator( olc::Sprite* sprite, olc::Decal* decal ) : _sprite( sprite ), _decal( decal ), _localTick( 0 ), _localTime( 0 ), _graphicState( GraphicState::Idle ), _direction( Direction::Right ), _graphicCounter( 0 ) {}
	~AlphaAnimator() {}


	void updateGraphicCounter()
	{
		// Oscillate the graphic counter to know which column of the sprite to choose from
		this->_graphicCounter++;
		this->_graphicCounter %= AlphaAnimator::MAX_GRAPHIC_COUNTER;

		return;
	}


	
	void tick( float deltaTime )
	{
		// Updates the local time based on a fixed refresh rate. Updates the graphic counter for each refresh tick
		this->_localTime += deltaTime;
		if ( this->_localTime >= Settings::Game::TICK_RATE )
		{
			this->_localTime -= Settings::Game::TICK_RATE;
			this->_localTick++;

			if ( this->_localTick >= Settings::Player::Character::ANIMATOR_TICKS_PER_FRAME )
			{
				this->_localTick -= Settings::Player::Character::ANIMATOR_TICKS_PER_FRAME;
				this->updateGraphicCounter();
			}
		}
		return;
	}
	

	/*
	void tick( )
	{
		// Updates the local time based on a fixed refresh rate. Updates the graphic counter for each refresh tick
		this->_localTick++;
		if ( this->_localTick >= Settings::Player::Character::ANIMATOR_TICKS_PER_FRAME )
		{
			this->_localTick -= Settings::Player::Character::ANIMATOR_TICKS_PER_FRAME;
			this->updateGraphicCounter();
		}
		return;
	}
	*/

	/*
	void update( float deltaTime, GraphicState graphicState, float velocityX )
	{
		// Updates the graphic state, direction, and proper graphic transition in preparation for rendering
		this->tick( deltaTime );

		this->_graphicState = graphicState;

		if ( velocityX < 0.0f )
		{
			this->_direction = Direction::Left;
		}
		else if ( velocityX > 0.0f )
		{
			this->_direction = Direction::Right;
		}
			
		return;
	}
	*/

	void update( float deltaTime, GraphicState graphicState, float velocityX )
	{
		// Updates the graphic state, direction, and proper graphic transition in preparation for rendering
		this->_graphicState = graphicState;

		if ( velocityX < 0.0f )
		{
			this->_direction = Direction::Left;
		}
		else if ( velocityX > 0.0f )
		{
			this->_direction = Direction::Right;
		}

		tick( deltaTime );
		
		return;
	}



	std::tuple<olc::Decal*, std::uint64_t, std::uint64_t, std::int8_t, std::int8_t> selectPartialRender() const
	{
		// Based on the graphic state, target the proper graphic cell of the spritesheet
		std::uint8_t stateIndex;
		std::uint8_t transitionIndex;
		std::uint8_t directionIndex;

		switch ( this->_direction )
		{
		case Direction::Right:
			directionIndex = 0;
			break;
		case Direction::Left:
			directionIndex = ( std::uint8_t )( GraphicState::count );
			break;
		default:
			directionIndex = 0;
			break;
		}

		switch ( this->_graphicState )
		{
		case GraphicState::Idle:
			stateIndex = ( std::uint8_t )( GraphicState::Idle );
			transitionIndex = this->_graphicCounter % ( std::uint8_t )( GraphicStateTransitions::Idle );
			break;
		case GraphicState::Run:
			stateIndex = ( std::uint8_t )( GraphicState::Run );
			transitionIndex = this->_graphicCounter % ( std::uint8_t )( GraphicStateTransitions::Run );
			break;
		case GraphicState::Jump:
			//stateIndex = ( std::uint8_t )( GraphicState::Jump );
			//transitionIndex = this->_graphicCounter % ( std::uint8_t )( GraphicStateTransitions::Jump );
			stateIndex = ( std::uint8_t )( GraphicState::Run );
			transitionIndex = this->_graphicCounter % ( std::uint8_t )( GraphicStateTransitions::Run );
			break;
		case GraphicState::Hurt:
			stateIndex = ( std::uint8_t )( GraphicState::Hurt );
			transitionIndex = this->_graphicCounter % ( std::uint8_t )( GraphicStateTransitions::Hurt );
			break;
		case GraphicState::Melee:
			stateIndex = ( std::uint8_t )( GraphicState::Melee );
			transitionIndex = this->_graphicCounter % ( std::uint8_t )( GraphicStateTransitions::Melee );
			break;
		case GraphicState::Bow:
			stateIndex = ( std::uint8_t )( GraphicState::Bow );
			transitionIndex = this->_graphicCounter % ( std::uint8_t )( GraphicStateTransitions::Bow );
			break;
		case GraphicState::Dead:
			stateIndex = ( std::uint8_t )( GraphicState::Dead );
			transitionIndex = this->_graphicCounter % ( std::uint8_t )( GraphicStateTransitions::Dead );
			break;
		default:
			stateIndex = ( std::uint8_t )( GraphicState::Idle );
			transitionIndex = this->_graphicCounter % ( std::uint8_t )( GraphicStateTransitions::Idle );
			break;
		}

		//std::cout << this->_decal << std::endl;
		//std::cout << (int)stateIndex << ", " << (int)transitionIndex << std::endl;

		//( stateIndex + directionIndex ) * ( PIXEL_SIZE_Y ) , transitionIndex * ( PIXEL_SIZE_X )
		
		return std::tuple<olc::Decal*, std::uint64_t, std::uint64_t, std::int8_t, std::int8_t>{ 
			this->_decal, 
			transitionIndex* ( PIXEL_SIZE_X ), ( stateIndex + directionIndex ) * ( PIXEL_SIZE_Y ),
			AlphaAnimator::OFFSET_X, AlphaAnimator::OFFSET_Y
		};
	}
};