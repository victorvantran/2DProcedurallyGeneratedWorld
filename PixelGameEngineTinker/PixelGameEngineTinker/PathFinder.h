#pragma once

#include <cmath>
#include <cstdint>
#include <vector>
#include <queue>
// Based on Franco Gustavo's implementation of A* fast

struct PathFinderNode
{
    std::int64_t     f; // f = gone + heuristic
    std::int64_t     g;
    PathFinderNode* pX; // Parent
    PathFinderNode* pY;
    std::uint8_t pZ; // parent z index
    float jumpValue;
    std::uint8_t    status;

    PathFinderNode() : f( 0 ), g( 0 ), pX( nullptr ), pY( nullptr ), pZ( 0 ), jumpValue( 0.0f ), status( 0 ) {}
    ~PathFinderNode() {}
};



struct Location
{
    // Used to index x, y, jump value
    std::size_t xy;
    std::size_t z;

    Location( std::size_t xy_, std::size_t z_ ) : xy( xy_ ), z( z_ ) {}
    ~Location() {}
};



struct Comparator
{
    std::vector<PathFinderNode*>* matrix;

    bool operator()( const Location& a, const Location& b )
    {
        return this->matrix[a.xy][a.z]->f >= this->matrix[b.xy][b.z]->f;
    }

    Comparator( std::vector<PathFinderNode*>* matrix_ ) : matrix( matrix_ ) {}
    ~Comparator() {}
};


class PathFinder
{
private:
    std::vector<PathFinderNode*>* _nodes;
    std::vector<std::int64_t> _touchedLocations;
    std::priority_queue<Location, std::vector<Location>, Comparator> _open;
public:

    PathFinder()
    {

    }

    ~PathFinder()
    {
        delete[] this->_nodes;
        this->_nodes = nullptr;
    }
};