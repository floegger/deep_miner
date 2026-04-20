#include "../include/SortBot.h"
#include "../include/World.h"

#include <algorithm>
#include <vector>

SortBot::SortBot ( int startX, int startY ) : BaseRobot ( "SortBot", startX, startY ) {}

int SortBot::mine ( World &world ) {
    /* Collect positive blocks in the current column, sort them so the
       highest value becomes the surface block, then mine that block.
       Complexity: O(z log z) where z = world depth at this column */
    std::vector<std::pair<int, int>> values;
    values.reserve ( static_cast<std::size_t> ( world.getSizeZ() ) );

    for ( int z = 0; z < world.getSizeZ(); ++z ) {
        const int value = world.getValue ( x_, y_, z );
        if ( value > 0 ) {
            values.emplace_back ( value, z );
        }
    }

    if ( values.empty() ) {
        return 0;
    }

    std::sort ( values.begin(), values.end(), [] ( const auto &a, const auto &b ) { return a.first < b.first; } );

    for ( const auto &[ value, z ] : values ) {
        ( void )value;
        world.setValue ( x_, y_, z, 0 );
    }

    for ( std::size_t i = 0; i < values.size(); ++i ) {
        world.setValue ( x_, y_, static_cast<int> ( i ), values[ i ].first );
    }

    const int surface = world.getSurfaceLevel ( x_, y_ );
    if ( surface < 0 ) {
        return 0;
    }

    const int mined = world.getValue ( x_, y_, surface );
    world.setValue ( x_, y_, surface, 0 );
    score_ += mined;
    return mined;
}
