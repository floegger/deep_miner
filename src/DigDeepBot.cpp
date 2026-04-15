#include "../include/DigDeepBot.h"
#include "../include/World.h"

DigDeepBot::DigDeepBot ( const int startX, const int startY ) :
BaseRobot( "DigDeepBot", startX, startY) {}

int DigDeepBot::mine ( World& world ) {
    int total = 0;
    int grabbed = 0;

    for ( int z = world.getSizeZ() - 1; z >= 0 && grabbed < 3; --z ) {
        int v = world.getValue ( x_, y_, z );
        if ( v > 0 ) {
            total += v;
            world.setValue ( x_, y_, z, 0 );
            ++grabbed;
        }
    }
    score_ += total;
    return total;
}
