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
	TileRender( int id, const BoundingBox<int>& bounds );

	void setBordersDecalIndex( std::uint8_t bordersDecalIndex );
	std::uint8_t getBordersDecalIndex() const;

	bool exists() const;
};