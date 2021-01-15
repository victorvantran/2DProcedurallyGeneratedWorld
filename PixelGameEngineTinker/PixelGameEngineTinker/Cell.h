#pragma once
#include "olcPixelGameEngine.h"
#include "BoundingBox.h"


class Cell
{
protected:
	//std::uint64_t _id;
	BoundingBox<std::int64_t> _bounds;

	Cell();
	~Cell();

	//Cell( std::uint64_t id, BoundingBox<std::int64_t> _bounds );
	Cell( BoundingBox<std::int64_t> _bounds );

public:
	//std::uint64_t getId() const;
	BoundingBox<std::int64_t> getBounds() const;
	std::int64_t getX() const;
	std::int64_t getY() const;
	std::int64_t getWidth() const;
	std::int64_t getHeight() const;
	//bool getExist() const;

	//std::uint64_t setId( std::uint64_t id );
	BoundingBox<std::int64_t> setBounds( const BoundingBox<std::int64_t>& _bounds );
	std::int64_t setWidth( std::int64_t width );
	std::int64_t setHeight( std::int64_t height );

	virtual void clear() = 0;
	//virtual void clear();
};
