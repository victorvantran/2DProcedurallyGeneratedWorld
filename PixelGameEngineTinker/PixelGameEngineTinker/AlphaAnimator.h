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
		Idle = 4,
		Run = 8,
		Jump = 8,
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
	static constexpr float FRAMES_PER_SECOND = 60.0f;
	static constexpr float REFRESH_RATE = ( 1.0f / FRAMES_PER_SECOND );
	float _localTime;

	// Sprite information
	olc::Sprite* _sprite;
	olc::Decal* _decal;
	static constexpr std::uint8_t PIXEL_SIZE_X = 64;
	static constexpr std::uint8_t PIXEL_SIZE_Y = 64;
	static constexpr std::uint8_t MAX_GRAPHIC_COUNTER = 8; // number of columns in out sprite (greatest common multiple between 4 and 8 = 8)

	GraphicState _graphicState;
	Direction _direction;
	std::uint8_t _graphicCounter;
public:
	AlphaAnimator() : _sprite( nullptr ), _decal( nullptr ), _localTime( 0 ), _graphicState( GraphicState::Idle ), _direction( Direction::Right ), _graphicCounter( 0 ) {}
	AlphaAnimator( olc::Sprite* sprite, olc::Decal* decal ) : _sprite( sprite ), _decal( decal ), _localTime( 0 ), _graphicState( GraphicState::Idle ), _direction( Direction::Right ), _graphicCounter( 0 ) {}
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
		if ( this->_localTime >= AlphaAnimator::REFRESH_RATE )
		{
			this->_localTime -= REFRESH_RATE;
			this->updateGraphicCounter();
		}
		return;
	}


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
			
		this->selectPartialRender();
		return;
	}


	void selectPartialRender()
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
			stateIndex = ( std::uint8_t )( GraphicState::Jump );
			transitionIndex = this->_graphicCounter % ( std::uint8_t )( GraphicStateTransitions::Jump );
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

		std::cout << (int)stateIndex << ", " << (int)transitionIndex << std::endl;

		//( stateIndex + directionIndex ) * ( PIXEL_SIZE_Y ) , transitionIndex * ( PIXEL_SIZE_X )
		return;
	}
};