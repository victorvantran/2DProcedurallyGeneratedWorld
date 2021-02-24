/*
#include "Background.h"
#include "World.h"

const BackgroundRenderData& Background::getRenderData() const
{

	// Return the information needed to render the world background
	//return BackgroundRenderData(
	//	this->_dayDecal, this->getDayTint( this->_worldSecond ),
	//	this->_nightDecal, this->getNightTint( this->_worldSecond ),
	//	this->_sunDecal, this->getSunPosition( this->_worldSecond ),
	//	this->_landscapeDecal, this->getLandscapeTint( this->_worldSecond )
	//);


	long double worldSecond = World::getSecond( this->_worldTick );

	return BackgroundRenderData(
		this->_dayDecal, this->getDayTint( worldSecond ),
		this->_nightDecal, this->getNightTint( worldSecond ),
		this->_sunDecal, this->getSunPosition( worldSecond ),
		this->_landscapeDecal, this->getLandscapeTint( worldSecond )
	);

}
*/