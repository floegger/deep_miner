#include "../include/BaseRobot.h"
#include "../include/World.h"

#include <algorithm>
#include <climits>

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

int BaseRobot::decideNextMove ( const World &world ) const {
    struct Option {
        int dir, val;
    };
    std::vector<Option> options;
    options.reserve ( 5 );

    auto evaluate = [ & ] ( int dx, int dy, int dir ) {
        int nx = x_ + dx, ny = y_ + dy;
        if ( nx < 0 || nx >= world.getSizeX() )
            return;
        if ( ny < 0 || ny >= world.getSizeY() )
            return;
        int surf = world.getSurfaceLevel ( nx, ny );
        if ( surf < 0 )
            return;
        options.push_back ( { dir, world.getValue ( nx, ny, surf ) } );
    };

    evaluate ( 0, 0, 0 );
    evaluate ( 1, 0, 1 );
    evaluate ( -1, 0, 2 );
    evaluate ( 0, 1, 3 );
    evaluate ( 0, -1, 4 );

    if ( options.empty() ) {
        // All immediate neighbours are empty – scan the whole grid for the
        // nearest non-empty column and step one cell toward it.
        int bestDist = INT_MAX, tx = x_, ty = y_;
        for ( int cx = 0; cx < world.getSizeX(); ++cx ) {
            for ( int cy = 0; cy < world.getSizeY(); ++cy ) {
                if ( world.getSurfaceLevel ( cx, cy ) >= 0 ) {
                    int dist = std::abs ( cx - x_ ) + std::abs ( cy - y_ );
                    if ( dist < bestDist ) { bestDist = dist; tx = cx; ty = cy; }
                }
            }
        }
        if ( tx == x_ && ty == y_ ) return 0;  // grid is truly empty
        if ( tx > x_ ) return 1;
        if ( tx < x_ ) return 2;
        if ( ty > y_ ) return 3;
        return 4;
    }

    auto best = std::max_element ( options.begin(), options.end(),
                                   [] ( const Option &a, const Option &b ) { return a.val < b.val; } );
    return best->dir;
}
