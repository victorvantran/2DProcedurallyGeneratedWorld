#pragma once

#include "olcPixelGameEngine.h"
#include "Character.h"

enum class PlayerKeyInput
{
	ZERO,
	ONE,
	TWO,
	THREE,
	FOUR,
	FIVE,
	SIX,
	SEVEN,
	EIGHT,
	NINE,
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	SPACE,
	count
};


 
// Player -> key Inputs
// Player ->update Commands
// Character ->read Commands
// Fixed Update



class World;

class Player
{
private:
	Character _character;
	World* _world;

	bool _prevInputs[( std::size_t )PlayerKeyInput::count];
	bool _currInputs[( std::size_t )PlayerKeyInput::count];
public:
	// Constructors/Destructors
	Player() : _character(), _world( nullptr ), _prevInputs{ false }, _currInputs{ false } {}

	Player( olc::v2d_generic<long double> center, olc::vf2d halfSize, CharacterState characterState, float runSpeed, float jumpSpeed, World* world ) :
		_character( center, halfSize, characterState, runSpeed, jumpSpeed, world ),
		_world( world ),
		_prevInputs{ false }, _currInputs{ false } {}
	/*
	Player( olc::v2d_generic<long double> center, olc::vf2d halfSize, CharacterState characterState, float runSpeed, float jumpSpeed ) :
		_character( center, halfSize, CharacterState::Stand, Settings::Player::Character::RUN_SPEED, Settings::Player::Character::JUMP_SPEED ) {}
		*/

	// _prevInputs{ false }, _currInputs{ false } {}
	~Player() {}


	// Getters
	Character& getCharacter()
	{
		return this->_character;
	}


	// Setters
	

	// Methods
	void resetInputs()
	{
		for ( std::size_t i = 0; i < ( std::size_t )PlayerKeyInput::count; i++ )
		{
			this->_currInputs[i] = false;
		}
		return;
	}


	void updateInputs( olc::PixelGameEngine& pge )
	{
		/*
		if ( pge.GetKey( olc::Key::K0 ).bPressed || pge.GetKey( olc::Key::K0 ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::ZERO] = true;
		if ( pge.GetKey( olc::Key::K1 ).bPressed || pge.GetKey( olc::Key::K0 ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::ONE] = true;
		if ( pge.GetKey( olc::Key::K2 ).bPressed || pge.GetKey( olc::Key::K0 ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::TWO] = true;
		if ( pge.GetKey( olc::Key::K3 ).bPressed || pge.GetKey( olc::Key::K0 ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::THREE] = true;
		if ( pge.GetKey( olc::Key::K4 ).bPressed || pge.GetKey( olc::Key::K0 ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::FOUR] = true;
		if ( pge.GetKey( olc::Key::K5 ).bPressed || pge.GetKey( olc::Key::K0 ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::FIVE] = true;
		if ( pge.GetKey( olc::Key::K6 ).bPressed || pge.GetKey( olc::Key::K0 ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::SIX] = true;
		if ( pge.GetKey( olc::Key::K7 ).bPressed || pge.GetKey( olc::Key::K0 ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::SEVEN] = true;
		if ( pge.GetKey( olc::Key::K8 ).bPressed || pge.GetKey( olc::Key::K0 ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::EIGHT] = true;
		if ( pge.GetKey( olc::Key::K9 ).bPressed || pge.GetKey( olc::Key::K0 ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::NINE] = true;
		*/

		if ( pge.GetKey( olc::Key::W ).bPressed || pge.GetKey( olc::Key::W ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::W] = true;
		if ( pge.GetKey( olc::Key::A ).bPressed || pge.GetKey( olc::Key::A ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::A] = true;
		if ( pge.GetKey( olc::Key::S ).bPressed || pge.GetKey( olc::Key::S ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::S] = true;
		if ( pge.GetKey( olc::Key::D ).bPressed || pge.GetKey( olc::Key::D ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::D] = true;
		if ( pge.GetKey( olc::Key::SPACE ).bPressed || pge.GetKey( olc::Key::SPACE ).bHeld ) this->_currInputs[( std::size_t )PlayerKeyInput::SPACE] = true;

		return;
	}


	void updatePrevInputs()
	{
		// Cache inputs from previous frame
		std::size_t count = ( std::size_t )PlayerKeyInput::count;

		for ( std::size_t i = 0; i < ( std::size_t )PlayerKeyInput::count; i++ )
		{
			this->_prevInputs[i] = this->_currInputs[i];
		}

		return;
	}

	bool* createCharacterCommands()
	{
		// Create current commands for character based on current player inputs
		bool characterCommands[( std::size_t )Command::count];

		characterCommands[( std::size_t )Command::GoRight] = this->_currInputs[( std::size_t )PlayerKeyInput::D];
		characterCommands[( std::size_t )Command::GoLeft] = this->_currInputs[( std::size_t )PlayerKeyInput::A];
		characterCommands[( std::size_t )Command::Jump] = this->_currInputs[( std::size_t )PlayerKeyInput::W];
		characterCommands[( std::size_t )Command::Drop] = this->_currInputs[( std::size_t )PlayerKeyInput::S];
		//characterCommands[( std::size_t )Command::Drop] = false; // [!] no drop implemented yet

		return characterCommands;
	}

	void update( float deltaTime, olc::PixelGameEngine& pge )
	{
		this->resetInputs();
		this->updateInputs( pge );
		bool* characterCommands = this->createCharacterCommands();
		this->_character.update( deltaTime, characterCommands );
		this->updatePrevInputs();
		return;
	}

};