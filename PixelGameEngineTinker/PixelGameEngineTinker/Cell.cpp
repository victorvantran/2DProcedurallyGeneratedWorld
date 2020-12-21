#include "Cell.h"

Cell::Cell()
	: _id( 0 ), _bounds( BoundingBox<int>() )
{
}


Cell::~Cell()
{
}


Cell::Cell( int id, BoundingBox<int> bounds )
	: _id( id ), _bounds( bounds )
{
}


int Cell::getId() const
{
	return this->_id;
}


BoundingBox<int> Cell::getBounds() const
{
	return this->_bounds;
}

bool Cell::getExist() const
{
	return this->_id != 0;
}



int Cell::getX() const
{
	return this->_bounds.getX();
}


int Cell::getY() const
{
	return this->_bounds.getY();
}



int Cell::getWidth() const
{
	return this->_bounds.getWidth();
}


int Cell::getHeight() const
{
	return this->_bounds.getHeight();
}


int Cell::setId( int id )
{
	this->_id = id;
	return this->_id;
}


BoundingBox<int> Cell::setBounds( const BoundingBox<int>& bounds )
{
	this->_bounds = bounds;
	return this->_bounds;
}


int Cell::setWidth( int width )
{
	this->_bounds.setWidth( width );
	return width;
}


int Cell::setHeight( int height )
{
	this->_bounds.setHeight( height );
	return height;
}





void Cell::clear()
{
	this->_id = 0;
	this->_bounds.clear();
}