#include "../include/World.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>

/* Construct a 3D grid world and initialize all cells.
   Complexity: O(x × y × z) */
World::World ( int x, int y, int z )
    : _sizeX ( x ), _sizeY ( y ), _sizeZ ( z ),
      _grid ( x, std::vector<std::vector<int>> ( y, std::vector<int> ( z, 0 ) ) ) {
    init();
}

/* Fill the world with random positive values and occasional effect cells.
   Effects are encoded as negative values in [-3, -1].
   Complexity: O(x × y × z) */
void World::init () {
    std::mt19937 rng ( std::random_device {}() );
    std::uniform_int_distribution<int> valueDist ( 1, 9 );
    std::uniform_int_distribution<int> effectChance ( 0, 9 );
    std::uniform_int_distribution<int> effectType ( -3, -1 );

    for ( int x = 0; x < _sizeX; ++x ) {
        for ( int y = 0; y < _sizeY; ++y ) {
            for ( int z = 0; z < _sizeZ; ++z ) {
                _grid[ x ][ y ][ z ] = ( effectChance ( rng ) == 1 ) ? effectType ( rng ) : valueDist ( rng );
            }
        }
    }
}

/* Return the value at (x, y, z), throwing on invalid coordinates.
   Complexity: O(1) */
int World::getValue ( int x, int y, int z ) const {
    if ( x < 0 || x >= _sizeX || y < 0 || y >= _sizeY || z < 0 || z >= _sizeZ ) {
        throw std::out_of_range ( "Coordinates out of bounds" );
    }
    return _grid[ x ][ y ][ z ];
}

/* Set the value at (x, y, z), throwing on invalid coordinates.
   Complexity: O(1) */
void World::setValue ( int x, int y, int z, int value ) {
    if ( x < 0 || x >= _sizeX || y < 0 || y >= _sizeY || z < 0 || z >= _sizeZ ) {
        throw std::out_of_range ( "Coordinates out of bounds" );
    }
    _grid[ x ][ y ][ z ] = value;
}

/* Return the highest z with a positive value in column (x, y).
   Negative effect cells are ignored for mining surface purposes.
   Complexity: O(z) */
int World::getSurfaceLevel ( int x, int y ) const {
    for ( int z = _sizeZ - 1; z >= 0; --z ) {
        if ( _grid[ x ][ y ][ z ] > 0 ) {
            return z;
        }
    }
    return -1;
}

/* Mine one topmost positive block from column (x, y) and return its value.
   Returns 0 when the column has no mineable block.
   Complexity: O(z) */
int World::mine ( int x, int y ) {
    const int z = getSurfaceLevel ( x, y );
    if ( z == -1 ) {
        return 0;
    }

    const int value = _grid[ x ][ y ][ z ];
    if ( value > 0 ) {
        _grid[ x ][ y ][ z ] = 0;
    }
    return value;
}

/* Print a 2D surface view with optional player/computer markers.
   Marker legend is shown when at least one robot position is provided.
   Complexity: O(x × y × z) due to per-cell surface lookup */
void World::display ( int p1x, int p1y, int p2x, int p2y ) const {
    std::cout << "=== SURFACE VIEW ===\n";

    if ( p1x >= 0 || p2x >= 0 ) {
        std::cout << "  P = Player   C = Computer   ! = both\n";
    }

    std::cout << "   ";
    for ( int y = 0; y < _sizeY; ++y ) {
        std::cout << std::setw ( 4 ) << y;
    }
    std::cout << "\n";

    for ( int x = 0; x < _sizeX; ++x ) {
        std::cout << std::setw ( 2 ) << x << " ";
        for ( int y = 0; y < _sizeY; ++y ) {
            const int surface = getSurfaceLevel ( x, y );
            const int val = ( surface >= 0 ? _grid[ x ][ y ][ surface ] : 0 );
            const bool hasP1 = ( x == p1x && y == p1y );
            const bool hasP2 = ( x == p2x && y == p2y );

            if ( hasP1 && hasP2 ) {
                std::cout << std::setw ( 3 ) << val << '!';
            } else if ( hasP1 ) {
                std::cout << std::setw ( 3 ) << val << 'P';
            } else if ( hasP2 ) {
                std::cout << std::setw ( 3 ) << val << 'C';
            } else {
                std::cout << std::setw ( 4 ) << val;
            }
        }
        std::cout << "\n";
        std::cout << "=========================\n";
    }
}

/* Rearrange each column by applying one random operation:
   shuffle, ascending sort, or descending sort.
   Zero cells remain untouched; only non-zero entries are rearranged in place.
   Complexity: O(x × y × z log z) in the worst case */
void World::rearrange () {
    std::mt19937 rng ( std::random_device {}() );
    std::uniform_int_distribution<int> opDist ( 0, 2 );

    for ( int x = 0; x < _sizeX; ++x ) {
        for ( int y = 0; y < _sizeY; ++y ) {
            std::vector<int> values;
            std::vector<int> positions;

            for ( int z = 0; z < _sizeZ; ++z ) {
                if ( _grid[ x ][ y ][ z ] != 0 ) {
                    values.push_back ( _grid[ x ][ y ][ z ] );
                    positions.push_back ( z );
                }
            }

            switch ( opDist ( rng ) ) {
            case 0:
                std::shuffle ( values.begin(), values.end(), rng );
                break;
            case 1:
                std::sort ( values.begin(), values.end() );
                break;
            case 2:
                std::sort ( values.begin(), values.end(), std::greater<int>() );
                break;
            }

            for ( std::size_t i = 0; i < positions.size(); ++i ) {
                _grid[ x ][ y ][ positions[ i ] ] = values[ i ];
            }
        }
    }
}

/* Find and consume the first negative effect value in column (x, y).
   Returns 0 when no effect exists in that column.
   Complexity: O(z) */
int World::checkEffects ( int x, int y ) {
    auto &col = _grid[ x ][ y ];
    auto it = std::find_if ( col.begin(), col.end(), [] ( int v ) { return v < 0; } );

    if ( it == col.end() ) {
        return 0;
    }

    const int effect = *it;
    *it = 0;
    return effect;
}
