#include "Player.h"
#include "World.h"




void Player::update( olc::PixelGameEngine& pge )
{
	// Update the player inputs and character physics in the world. Modify world chunks is needed for when world chunk switches pointers
	std::unique_lock<std::mutex> lockModify( this->_world->_modifyWorldChunksMutex );

	this->resetInputs();
	this->updateInputs( pge );
	bool* characterCommands = this->createCharacterCommands();
	this->_character.update( pge.GetElapsedTime() * Settings::Game::TIME_SCALE, characterCommands );
	this->updatePrevInputs();
	return;
}


void Player::render()
{
	this->_world->getCamera()->renderPlayer( *this );
}
