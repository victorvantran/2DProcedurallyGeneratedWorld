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
void BoundingBox<T>::clear()
{
	this->x = -1;
	this->y = -1;
	this->width = -1;
	this->height = -1;
	return;
}

