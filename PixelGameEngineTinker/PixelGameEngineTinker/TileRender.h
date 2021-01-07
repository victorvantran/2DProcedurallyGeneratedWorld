#pragma once

#include <cstdint>

#include "Cell.h"
#include "TileBorder.h"

class TileRender : public Cell
{
private:
	std::uint64_t _id;
	std::uint8_t _bordersDecalIndex;
public:
	TileRender();
	~TileRender();
	TileRender( std::uint64_t id, const BoundingBox<std::int64_t>& bounds );

	std::uint64_t getId() const;
	std::uint64_t setId( std::uint64_t id );

	bool exists() const;

	void setBordersDecalIndex( std::uint8_t bordersDecalIndex );
	std::uint8_t getBordersDecalIndex() const;


	inline bool operator==( const TileRender& rhs ) const
	{
		return this->getId() == rhs.getId() &&
			this->exists() == rhs.exists() &&
			this->getWidth() == rhs.getWidth() &&
			this->getHeight() == rhs.getHeight();
	}

};