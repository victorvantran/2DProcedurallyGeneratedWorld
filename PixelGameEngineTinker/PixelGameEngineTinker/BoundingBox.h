#pragma once

template<typename T>
struct BoundingBox
{
	T x = -1;
	T y = -1;
	T width = -1;
	T height = -1;

	BoundingBox( T x = -1, T y = -1, T width = -1, T height = -1 );
	~BoundingBox();

	bool operator==( const BoundingBox<T>& rhs ) const;
	bool operator>=( const BoundingBox<T>& rhs ) const; // encapsulate (can be same size)
	bool operator>( const BoundingBox<T>& rhs ) const; // strictly encapsulate


	bool intersects( const BoundingBox<T>& otherBoundingBox ) const;

	void clear();
};


template<typename T>
BoundingBox<T>::BoundingBox( T x, T y, T width, T height )
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
}

template<typename T>
BoundingBox<T>::~BoundingBox()
{
}



template<typename T>
bool BoundingBox<T>::operator==( const BoundingBox<T>& rhs ) const
{
	return ( this->x == rhs.x &&
		this->y == rhs.y &&
		this->width == rhs.width &&
		this->height == rhs.height
		);
}



template<typename T>
bool BoundingBox<T>::operator>=( const BoundingBox<T>& rhs ) const
{
	return ( this->x <= rhs.x &&
		this->y <= rhs.y &&
		( this->x + this->width ) >= ( rhs.x + rhs.width ) &&
		( this->y + this->height ) >= ( rhs.y + rhs.height )
		);
}



template<typename T>
bool BoundingBox<T>::operator>( const BoundingBox<T>& rhs ) const
{
	if ( this->x >= rhs.x )
	{
		return false;
	}

	if ( this->y >= rhs.y )
	{
		return false;
	}

	if ( ( this->x + this->width ) <= ( rhs.x + rhs.width ) )
	{
		return false;
	}

	if ( ( this->y + this->height ) <= ( rhs.y + rhs.height ) )
	{
		return false;
	}

	return true;
}

template<typename T>
bool BoundingBox<T>::intersects( const BoundingBox<T>& otherBoundingBox ) const // intersect is stricter than merely touching
{
	if ( ( this->x + this->width ) <= otherBoundingBox.x )
	{
		return false;
	}
	if ( ( otherBoundingBox.x + otherBoundingBox.width ) <= this->x )
	{
		return false;
	}
	if ( ( this->y + this->height ) <= otherBoundingBox.y )
	{
		return false;
	}
	if ( ( otherBoundingBox.y + otherBoundingBox.height ) <= this->y )
	{
		return false;
	}

	return true;
}


template<typename T>
void BoundingBox<T>::clear()
{
	this->x = -1;
	this->y = -1;
	this->width = -1;
	this->height = -1;
	return;
}

