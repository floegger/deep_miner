#include "../include/World.h"
#include <iomanip>
#include <iostream>
#include <random>

// #include <algorithm>
// #include <atomic>

World::World ( int x, int y, int z )
    : _sizeX ( x ), _sizeY ( y ), _sizeZ ( z ),
      _grid ( x, std::vector<std::vector<int>> ( y, std::vector<int> ( z, 0 ) ) ) {
    init();
}

void World::init () {
    std::mt19937 rng ( std::random_device {}() );  // random number generator (RNG)
    std::uniform_int_distribution<int> distribution ( 1, 9 );
    std::uniform_int_distribution<int> effectProbability ( 0, 9 );  // 10% chance for an effect
    std::uniform_int_distribution<int> effectType ( 1, 3 );         // 3 types of effects

    for ( int x = 0; x < _sizeX; ++x ) {
        for ( int y = 0; y < _sizeY; ++y ) {
            for ( int z = 0; z < _sizeZ; ++z ) {
                _grid[ x ][ y ][ z ] = ( effectProbability ( rng ) == 1 )
                                           ? effectType ( rng )
                                           : distribution ( rng );  // 10% chance for effect
            }
        }
    }
}  // init

int World::getValue ( int x, int y, int z ) const {
    if ( x < 0 || x >= _sizeX || y < 0 || y >= _sizeY || z < 0 || z >= _sizeZ ) {
        throw std::out_of_range ( "Coordinates out of bounds" );
    }
    return _grid[ x ][ y ][ z ];
}

void World::setValue ( int x, int y, int z, int value ) {
    if ( x < 0 || x >= _sizeX || y < 0 || y >= _sizeY || z < 0 || z >= _sizeZ ) {
        throw std::out_of_range ( "Coordinates out of bounds" );
    }
    _grid[ x ][ y ][ z ] = value;
}

int World::getSurfaceLevel ( int x, int y ) const {
    for ( int z = _sizeZ - 1; z >= 0; --z ) {
        if ( _grid[ x ][ y ][ z ] != 0 ) {
            return z;
        }
    }
    return -1;  // no surface found
}

int World::mine ( int x, int y ) {
    int z = getSurfaceLevel ( x, y );
    if ( z == -1 )
        return 0;
    int value = _grid[ x ][ y ][ z ];
    if ( value > 0 ) {
        _grid[ x ][ y ][ z ] = 0;  // mine the block
    }
    return value;
}

void World::display () const {
    for ( int y = 0; y < _sizeY; ++y ) {
        std::cout << std::setw ( 4 ) << y;
    }

    for ( int x = 0; x < _sizeX; ++x ) {
        std::cout << std::setw ( 2 ) << x << ": ";
        for ( int y = 0; y < _sizeY; ++y ) {
            int surface = getSurfaceLevel ( x, y );
            std::cout << std::setw ( 4 ) << ( surface >= 0 ? _grid[ x ][ y ][ surface ] : 0 );
        }
        std::cout << "\n";
    }
}

// TODO:
// void World::rearrange ()
// int World::checkEffects ( int x, int y ) const
