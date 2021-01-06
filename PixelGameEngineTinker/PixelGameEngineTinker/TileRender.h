#pragma once

#include <cstdint>

#include "Cell.h"
#include "TileBorder.h"

class TileRender : public Cell
{
private:
	std::uint8_t _bordersDecalIndex;
public:
	TileRender();
	~TileRender();
	TileRender( std::uint64_t id, const BoundingBox<std::int64_t>& bounds );

	void setBordersDecalIndex( std::uint8_t bordersDecalIndex );
	std::uint8_t getBordersDecalIndex() const;

	bool exists() const;


	inline bool operator==( const TileRender& rhs ) const
	{
		return this->getId() == rhs.getId() &&
			this->getExist() == rhs.getExist() &&
			this->getWidth() == rhs.getWidth() &&
			this->getHeight() == rhs.getHeight();
	}

};