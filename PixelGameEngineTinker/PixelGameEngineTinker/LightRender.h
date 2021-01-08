#pragma once

#include "Cell.h"


class LightRender : public Cell
{
public:
	// [!] Can change uint8_t[4] to int32_t and do bit operations to disect the 4-wide for optimizations
	std::uint8_t corner0[4] = { 0, 0, 0, 0 };
	std::uint8_t corner1[4] = { 0, 0, 0, 0 };
	std::uint8_t corner2[4] = { 0, 0, 0, 0 };
	std::uint8_t corner3[4] = { 0, 0, 0, 0 };

	bool exist;

	LightRender();
	~LightRender();
	LightRender( std::uint8_t* corner0_, std::uint8_t* corner1_, std::uint8_t* corner2_, std::uint8_t* corner3_, bool exist_, const BoundingBox<std::int64_t>& bounds );


	bool exists() const;
	bool isSolid() const;

	inline bool operator==( const LightRender& rhs ) const
	{
		return corner0[0] == rhs.corner0[0] && corner1[0] == rhs.corner1[0] && corner2[0] == rhs.corner2[0] && corner3[0] == rhs.corner3[0] &&
			this->isSolid() && rhs.isSolid();
	}
};