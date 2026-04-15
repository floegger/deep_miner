#include "../include/World.h"
#include <iomanip>
#include <iostream>
#include <random>

#include <algorithm>
#include <atomic>

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
// int World::checkEffects ( int x, int y ) const
void World::rearrange() {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> opDist(0, 2);

    for (int x = 0; x < _sizeX; ++x) {
        for (int y = 0; y < _sizeY; ++y) {
            std::vector<int> values;
            std::vector<int> positions;
            for (int z = 0; z < _sizeZ; ++z) {
                if (_grid[x][y][z] != 0) {
                    values.push_back(_grid[x][y][z]);
                    positions.push_back(z);
                }
            }

            switch (opDist(rng)) {
                case 0: std::shuffle(values.begin(), values.end(), rng); break;
                case 1: std::sort(values.begin(), values.end());         break;
                case 2: std::sort(values.begin(), values.end(), std::greater<int>()); break;
            }

            for (std::size_t i = 0; i < positions.size(); ++i)
                _grid[x][y][positions[i]] = values[i];
        }
    }
}

int World::checkEffects ( int x, int y ) {
    auto& col = _grid[x][y];
    auto it = std::find_if(col.begin(), col.end(), [](int v){ return v < 0; });
    if (it == col.end()) return 0;
    int effect = *it;
    *it = 0;  // consume: the effect fires exactly once
    return effect;
}
