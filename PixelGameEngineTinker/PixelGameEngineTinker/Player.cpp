#include "Player.h"
#include "World.h"


void Player::render()
{
	this->_world->getCamera()->renderPlayer( *this );
}
