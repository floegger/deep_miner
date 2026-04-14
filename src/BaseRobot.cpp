#include "../include/BaseRobot.h"
#include "../include/World.h"

#include <algorithm>

BaseRobot::BaseRobot ( std::string name, int startX, int startY )
    : x_ ( startX ), y_ ( startY ), score_ ( 0 ), name_ ( std::move ( name ) ) {}

void BaseRobot::move ( int direction, const World &world ) {

    switch ( direction ) {
    case 1:
        x_ = std::min ( x_ + 1, world.getSizeX() - 1 );
        break;  // right
    case 2:
        x_ = std::max ( x_ - 1, 0 );
        break;  // left
    case 3:
        y_ = std::min ( y_ + 1, world.getSizeY() - 1 );
        break;  // down
    case 4:
        y_ = std::max ( y_ - 1, 0 );
        break;  // up
    default:
        break;  // no move
    }
}

// TODO
// int BaseRobot::decideNextMove ( const World &world ) const
