#include "../include/RandomBot.h"
#include "../include/World.h"

#include <random>

RandomBot::RandomBot ( const int startX, const int startY ) : BaseRobot ( "RandomBot", startX, startY ) {}

int RandomBot::mine ( World &world ) {
    /* Mine up to a random number of positive blocks from the current column,
       scanning from top to bottom and stopping once the random limit is reached.
       Complexity: O(z) where z = world height */
    int total = 0;
    int grabbed = 0;
    const int limit = randomNumber();

    for ( int z = world.getSizeZ() - 1; z >= 0 && grabbed < limit; --z ) {
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

int RandomBot::randomNumber () {
    /* Return a uniformly distributed mining limit in [0, 9].
       Complexity: O(1) */
    static std::mt19937 rng ( std::random_device {}() );
    std::uniform_int_distribution<int> distr ( 0, 9 );
    return distr ( rng );
}
