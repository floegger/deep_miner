#include "../include/Game.h"
#include "../include/SortBot.h"
#include "../include/DigDeepBot.h"
#include "../include/RandomBot.h"
#include <iostream>
#include <limits>

int validateInput ( const std::string& prompt, int min, int max );

Game::Game() : world_ ( 5, 5, 10 ) {}

void Game::run () {
    std::cout << "=== DEEP MINER ===/n/n";
    setup();
    world_.display ();

    int round = 1;
    while ( !isGameOver() ) {
        std::cout << "\n--- Round " << round ++ << " ---\n";

       // human player
        std::cout << "[Player: " << player_->getName () << "]\n";
        play ( *player_, true );

        // computer
        std::cout << "[Computer: " << computer_->getName() << "]\n";
        play ( *computer_, false );

        world_.display();
        printScores();
   }
    printResult();
}

void Game::setup () {
    autoMode_ = ( validateInput("Mode - 1: Player vs. Computer  2: Computer vs. Computer: ", 1, 2) == 2 );

    std::cout   << "\nRobots: \n"
                << " 1 = SortBot    (sorts column and mines highest value)\n"
                << " 2 = DigDeepBot (grabs the top 3 values)]\n"
                << " 3 = RandomBot  (grabs a random Number of values)\n";

    player_ =       createRobot ( validateInput(
                    "Your robot: ", 1, 3), 0, 0 );
    computer_ =     createRobot ( validateInput(
                    "Computer robot: ", 1, 3 ), world_.getSizeX() - 1, world_.getSizeY() - 1 );
}

std::unique_ptr < Robot > Game::createRobot ( int choice, int x, int y ) const {
    switch ( choice ) {
        case 1:
            return std::make_unique < SortBot > ( x, y );
        case 2 :
            return std::make_unique < DigDeepBot > ( x, y );
        case 3 :
            return std::make_unique < RandomBot > ( x, y );
        default:
            return std::make_unique < SortBot > ( x, y );
    }
}

bool Game::isGameOver () const {
    for ( int x = 0; x < world_.getSizeX (); ++x ) {
        for ( int y = 0; y < world_.getSizeY(); ++y ){
            if ( world_.getSurfaceLevel( x, y) >= 0 )
                return false;
        }
    }
    return true;
}

void Game::printScores() const {
    std::cout   << "Score: "
                << player_->getName () << " = " << player_->getScore() << " | "
                << computer_->getName() << " = " << computer_->getScore() << "\n";
}

void Game::printResult() const {
    std::cout << "=== GAME OVER ===\n";
    printScores();
    int scorePlayer = player_->getScore(), scoreComputer = computer_->getScore();
    if ( scorePlayer > scoreComputer )
        std::cout << player_->getName() << " WINS!\n";
    else if ( scoreComputer > scorePlayer )
        std::cout << computer_->getName() << " WINS!\n";
    else
        std::cout << "DRAW!\n";
}

int getValidatedInput(const std::string& prompt, int min, int max) {
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val && val >= min && val <= max)
            return val;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "  Invalid. Enter a number between "
                  << min << " and " << max << ": ";
    }
}
