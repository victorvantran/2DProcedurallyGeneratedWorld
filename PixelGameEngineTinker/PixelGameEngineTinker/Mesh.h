#pragma once

#include "olcPixelGameEngine.h"

// Skin overlay purely used for rendering a world with smoother tiles

namespace mesh
{
	class Mesh
	{
	private:
		int* _meshMapping;
		olc::vi2d _meshDimension; // [!] 1 row and column less than the tile (or make it same size and make the default valueso f it 16 (solid block) )

	public:
		Mesh();
		~Mesh();

		Mesh( int* mapping, const olc::vi2d& meshDimension );

		int* getMapping();
		olc::vi2d getMeshDimension();
	};
}


mesh::Mesh::Mesh()
{

}

mesh::Mesh::~Mesh()
{
	delete this->_meshMapping;
	this->_meshMapping = nullptr;
}

mesh::Mesh::Mesh( int* mapping, const olc::vi2d& meshDimension )
{
	this->_meshMapping = mapping;
	this->_meshDimension = meshDimension;
}