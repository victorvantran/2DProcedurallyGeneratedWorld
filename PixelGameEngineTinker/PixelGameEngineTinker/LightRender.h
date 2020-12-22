#pragma once

#include "Cell.h"


class LightRender : public Cell
{
public:
	LightRender() {}
	~LightRender() {}

	inline bool operator==( const LightRender& rhs ) const
	{
		return this->getId() == rhs.getId() &&
			this->getExist() == rhs.getExist() &&
			this->getWidth() == rhs.getWidth() &&
			this->getHeight() == rhs.getHeight();
	}
};