#pragma once

#include <cstdint>

#include "Tile.h"
#include "Cell.h"
#include "TileBorder.h"

class TileRender : public Cell
{
private:
	TileIdentity _id;
	std::uint8_t _bordersDecalIndex;
public:
	TileRender();
	~TileRender();
	TileRender( TileIdentity id, std::uint8_t bordersDecalIndex, const BoundingBox<std::int64_t>& bounds );

	TileIdentity getId() const;
	TileIdentity setId( TileIdentity id );

	bool exists() const;

	void setBordersDecalIndex( std::uint8_t bordersDecalIndex );
	std::uint8_t getBordersDecalIndex() const;


	inline bool operator==( const TileRender& rhs ) const
	{
		/*
		return this->getId() == rhs.getId() &&
			this->exists() == rhs.exists() &&
			this->getWidth() == rhs.getWidth() &&
			this->getHeight() == rhs.getHeight();
			*/

		return this->getId() == rhs.getId() &&
			this->exists() == rhs.exists() &&
			this->getBordersDecalIndex() == rhs.getBordersDecalIndex() &&
			this->getWidth() == rhs.getWidth() &&
			this->getHeight() == rhs.getHeight();
	}

	void clear();
};