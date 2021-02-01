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

	template<typename S>
	bool intersects( const BoundingBox<S>& otherBoundingBox ) const
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


	template<typename S>
	bool encapsulates( const BoundingBox<S>& otherBoundingBox ) const
	{
		if ( this->x <= otherBoundingBox.x &&
			this->x + this->width >= otherBoundingBox.width &&
			this->y <= otherBoundingBox.x &&
			this->y + this->height >= otherBoundingBox.y )
		{
			return true;
		}
		
		return false;
	}




	void clear();


	T getX() const;
	T getY() const;
	T getCenterX() const;
	T getCenterY() const;
	T getWidth() const;
	T getHeight() const;


	T setX( T x );
	T setY( T y );
	T setCenterX( T x );
	T setCenterY( T y );
	T setWidth( T width );
	T setHeight( T height );
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


template<typename T>
T BoundingBox<T>::getX() const
{
	return this->x;
}

template<typename T>
T BoundingBox<T>::getY() const
{
	return this->y;
}


template<typename T>
T BoundingBox<T>::getCenterX() const
{
	return this->x + this->height / 2;
}

template<typename T>
T BoundingBox<T>::getCenterY() const
{
	return this->y + this->height / 2;
}


template<typename T>
T BoundingBox<T>::getWidth() const
{
	return this->width;
}


template<typename T>
T BoundingBox<T>::getHeight() const
{
	return this->height;
}



template<typename T>
T BoundingBox<T>::setX( T x )
{
	this->x = x;
	return this->x;
}


template<typename T>
T BoundingBox<T>::setY( T y )
{
	this->y = y;
	return this->y;
}


template<typename T>
T BoundingBox<T>::setCenterX( T x )
{
	this->x = x - ( this->width / 2.0f );
	return this->x;
}


template<typename T>
T BoundingBox<T>::setCenterY( T y )
{
	this->y = y - ( this->height / 2.0f );
	return this->y;
}

template<typename T>
T BoundingBox<T>::setWidth( T width )
{
	this->width = width;
	return this->width;
}


template<typename T>
T BoundingBox<T>::setHeight( T height )
{
	this->height = height;
	return this->height;
}