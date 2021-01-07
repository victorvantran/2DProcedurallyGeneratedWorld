#include "Cell.h"

/*
Cell::Cell()
	: _id( 0 ), _bounds( BoundingBox<std::int64_t>() )
{
}
*/

Cell::Cell()
	: _bounds( BoundingBox<std::int64_t>() )
{
}

Cell::~Cell()
{
}

/*
Cell::Cell( std::uint64_t id, BoundingBox<std::int64_t> bounds )
	: _id( id ), _bounds( bounds )
{
}
*/


Cell::Cell( BoundingBox<std::int64_t> bounds )
	: _bounds( bounds )
{
}

/*
std::uint64_t Cell::getId() const
{
	return this->_id;
}
*/

BoundingBox<std::int64_t> Cell::getBounds() const
{
	return this->_bounds;
}

/*
bool Cell::getExist() const
{
	return this->_id != 0;
}
*/


std::int64_t Cell::getX() const
{
	return this->_bounds.getX();
}


std::int64_t Cell::getY() const
{
	return this->_bounds.getY();
}



std::int64_t Cell::getWidth() const
{
	return this->_bounds.getWidth();
}


std::int64_t Cell::getHeight() const
{
	return this->_bounds.getHeight();
}

/*
std::uint64_t Cell::setId( std::uint64_t id )
{
	this->_id = id;
	return this->_id;
}
*/

BoundingBox<std::int64_t> Cell::setBounds( const BoundingBox<std::int64_t>& bounds )
{
	this->_bounds = bounds;
	return this->_bounds;
}


std::int64_t Cell::setWidth( std::int64_t width )
{
	this->_bounds.setWidth( width );
	return width;
}


std::int64_t Cell::setHeight( std::int64_t height )
{
	this->_bounds.setHeight( height );
	return height;
}


void Cell::clear()
{
	//this->_id = 0;
	this->_bounds.clear();
}