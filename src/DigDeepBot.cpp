#include "../include/DigDeepBot.h"
#include "../include/World.h"

DigDeepBot::DigDeepBot ( const int startX, const int startY ) : BaseRobot ( "DigDeepBot", startX, startY ) {}

int DigDeepBot::mine ( World &world ) {
    /* Mine up to three positive blocks from the current column, scanning
       from top depth to bottom depth.
       Complexity: O(z) where z = column depth */
    int total = 0;
    int grabbed = 0;

    for ( int z = world.getSizeZ() - 1; z >= 0 && grabbed < 3; --z ) {
        const int value = world.getValue ( x_, y_, z );
        if ( value > 0 ) {
            total += value;
            world.setValue ( x_, y_, z, 0 );
            ++grabbed;
        }
    }

    score_ += total;
    return total;
}
