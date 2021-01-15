#pragma once

#include "Cell.h"


class LightRender : public Cell
{
public:
	// [!] Can change uint8_t[4] to int32_t and do bit operations to disect the 4-wide for optimizations
	std::uint32_t corner0;
	std::uint32_t corner1;
	std::uint32_t corner2;
	std::uint32_t corner3;

	bool exist;

	LightRender();
	~LightRender();
	LightRender( std::uint32_t corner0_, std::uint32_t corner1_, std::uint32_t corner2_, std::uint32_t corner3_, bool exist_, const BoundingBox<std::int64_t>& bounds );


	bool exists() const;
	bool isSolid() const;

	inline bool operator==( const LightRender& rhs ) const
	{
		return corner0 == rhs.corner0 && corner1 == rhs.corner1 && corner2 == rhs.corner2 && corner3 == rhs.corner3 &&
			this->isSolid() && rhs.isSolid();
	}

	void clear();

};