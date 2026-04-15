#include "../include/SortBot.h"
#include "../include/World.h"

#include <algorithm>
#include <vector>

SortBot::SortBot ( const int startX, const int startY ) : BaseRobot ( "SortBot", startX, startY ) {}

int SortBot::mine ( World &world ) {
    // create ( value, z ) pairs for positive values ( exclude mined blocks and effects )
    std::vector<std::pair<int, int>> values;
    for ( int z = 0; z < world.getSizeZ(); ++z ) {
        int v = world.getValue(x_, y_, z );
        if ( v > 0 ) values.emplace_back(v, z );
    }
    if ( values.empty()) return 0;
    // sort ascending so highest value ends up at top (surface) and gets mined
    std::sort ( values.begin(), values.end(), [] ( const auto& a, const auto& b ) { return a.first < b.first;} );

for ( auto& [x,  z] : values ) {
    world.setValue ( x_, y_, z, 0);
}
    for ( std::size_t i = 0; i < values.size(); ++i ) {
        world.setValue( x_, y_, static_cast<int>(i), values[i].first );
    }
    int surface = world.getSurfaceLevel(x_, y_);
    if ( surface < 0 ) return 0;
    int mined = world.getValue ( x_, y_, surface );
    world.setValue ( x_, y_, surface, 0 );
    score_ += mined;
    return mined;
}
