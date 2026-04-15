#include <iostream>
#include "include/Game.h"

int main () {
    try {
        Game game;
        game.run ();
    } catch ( const std::exception& e ) {
        std::cerr << "Fatal Error: " << e.what() << "\n";
        return 1;
    }
    return 0;

}
