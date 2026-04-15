#include "../include/RandomBot.h"
#include "../include/World.h"

#include <random>

RandomBot::RandomBot ( const int startX, const int startY ):
BaseRobot ( "RandomBot", startX, startY ) {}

int RandomBot::mine ( World& world ) {
    int total = 0;
    int grabbed = 0;
    const int limit = randomNumber();  // computed once; calling it every iteration re-rolls the limit

    for ( int z = world.getSizeZ() -1; z >= 0 && grabbed < limit;  --z ) {
        int value = world.getValue ( x_, y_, z );
        if ( value > 0 ) {
            total += value;
            world.setValue ( x_, y_, z, 0 );
            ++grabbed;
        }
    }
    score_ += total;
    return total;
}

int RandomBot::randomNumber () {
    static std ::mt19937 rng ( std::random_device{}());
    std::uniform_int_distribution < int > distr ( 0, 9 );
    return distr ( rng );
};
