#pragma once

#include "Settings.h"
#include "DynamicObject.h"


enum class ZombieCommand
{
	GoRight,
	GoLeft,
	Jump,
	Drop,

	count
};


enum class ZombieState
{
	Stand,
	Run,
	Jump, // Airborn
	GrabLedge,

	count
};



class World;

class Zombie : public DynamicObject
{
private:
	ZombieState _currentState;
	float _runSpeed;
	float _jumpSpeed;


	// LedgeWork
	olc::v2d_generic<std::int64_t> _ledgeTile;
	std::int16_t _cannotGoLeftFrames;
	std::int16_t _cannotGoRightFrames;

	// Jump
	std::int32_t _framesFromJumpStart;



	World* _world;


	void transitionState( ZombieState characterState )
	{
		this->_currentState = characterState;
		return;
	}

protected:
	bool _prevCommands[( std::size_t )ZombieCommand::count];
	bool _currCommands[( std::size_t )ZombieCommand::count];

	bool cease( ZombieCommand command ) const
	{
		return !this->_currCommands[( std::size_t )command] && this->_prevCommands[( std::size_t )command];
	}

	bool commandState( ZombieCommand command ) const
	{
		return this->_currCommands[( std::size_t )command];
	}


	void setCommandState( ZombieCommand command, bool setting )
	{
		this->_currCommands[( std::size_t )command] = setting;
		return;
	}


	bool execute( ZombieCommand command ) const
	{
		return this->_currCommands[( std::size_t )command] && !this->_prevCommands[( std::size_t )command];
	}

public:
	// Constructors/Destructors
	Zombie();
	Zombie( const olc::v2d_generic<long double>& center, const olc::vf2d& halfSize, const olc::vf2d& scale, ZombieState characterState, float runSpeed, float jumpSpeed,
		World* world, long double time );
	~Zombie();


	// Getters


	// Setters


	// Methods
	void resetCurrCommands();
	void updateCurrCommands( bool* commands );
	void updatePrevCommands();
	void updateState( float deltaTime );
	void update( float deltaTime, bool* commands );
};