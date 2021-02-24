// Credits to Daniel Branicki 2D Physics tutorial

#pragma once

#include "Settings.h"
#include "DynamicObject.h"
#include "AlphaAnimator.h"


enum class Command
{
	GoRight,
	GoLeft,
	Jump,
	Drop,

	count
};


enum class CharacterState
{
	Stand,
	Run,
	Jump, // Airborn
	GrabLedge,

	count
};


class World;

class Character : public DynamicObject
{
private:
	CharacterState _currentState;
	float _runSpeed;
	float _jumpSpeed;

	// LedgeWork
	olc::v2d_generic<std::int64_t> _ledgeTile;
	std::int16_t _cannotGoLeftFrames;
	std::int16_t _cannotGoRightFrames;

	// Jump
	std::int32_t _framesFromJumpStart;


	// World Interation
	World* _world;


	void transitionState( CharacterState characterState )
	{
		this->_currentState = characterState;
		return;
	}


	// Animator
	AlphaAnimator _animator;

protected:
	bool _prevCommands[( std::size_t )Command::count];
	bool _currCommands[( std::size_t )Command::count];

	bool cease( Command command ) const
	{
		return !this->_currCommands[( std::size_t )command] && this->_prevCommands[( std::size_t )command];
	}

	bool commandState( Command command ) const
	{
		return this->_currCommands[( std::size_t )command];
	}


	void setCommandState( Command command, bool setting )
	{
		this->_currCommands[( std::size_t )command] = setting;
		return;
	}


	bool execute( Command command ) const
	{
		return this->_currCommands[( std::size_t )command] && !this->_prevCommands[( std::size_t )command];
	}

public:
	// Constructors/Destructors
	Character();
	Character( const olc::v2d_generic<long double>& center, const olc::vf2d& halfSize, const olc::vf2d& scale, CharacterState characterState, float runSpeed, float jumpSpeed,
		World* world, float time,
		olc::Sprite* sprite, olc::Decal* decal );
	~Character();

	
	// Getters
	const AlphaAnimator& getAnimator() const;

	// Setters


	// Methods
	void resetCurrCommands();
	void updateCurrCommands( bool* commands );
	void updatePrevCommands();
	void updateState( float deltaTime );
	//void updateState();

	void update( float deltaTime, bool* commands );
	//void update( bool* commands );

};