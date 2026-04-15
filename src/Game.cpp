#include "../include/Game.h"
#include "../include/SortBot.h"
#include "../include/DigDeepBot.h"
#include "../include/RandomBot.h"
#include <iostream>
#include <limits>

int validateInput ( const std::string& prompt, int min, int max );

Game::Game() : world_ ( 5, 5, 10 ) {}

void Game::run () {
    std::cout << "=== DEEP MINER ===\n\n";
    setup();
    world_.display ( player_->getX(), player_->getY(), computer_->getX(), computer_->getY() );

    int round = 1;
    while ( !isGameOver() ) {
        std::cout << "\n--- Round " << round ++ << " ---\n";

       // human player
        std::cout << "[Player: " << player_->getName () << "]\n";
        play ( *player_, true );

        // computer
        std::cout << "[Computer: " << computer_->getName() << "]\n";
        play ( *computer_, false );

        world_.display ( player_->getX(), player_->getY(), computer_->getX(), computer_->getY() );
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

void Game::play ( Robot& robot, bool isPlayer ) {
    int direction = ( !isPlayer || autoMode_ ) ? robot.decideNextMove( world_ )
        : validateInput ( "Direction (0 =stay 1=+x 2=-x 3=+y 4=-y): ", 0, 4);
    robot.move( direction, world_ );

    int effect = world_.checkEffects( robot.getX(), robot.getY() );
    if ( effect < 0 ) {
        applyEffect( robot, effect );
    }
    if ( effect != -1 ) {  // -1 = blocked; all other effects still allow mining
        int mined = robot.mine( world_ );
        std::cout << "  " << robot.getName() << " mined " << mined << " points.\n";
    }
    checkRearrange( robot );
}

void Game::checkRearrange(Robot& robot) {
    int score     = robot.getScore();
    int threshold = (score / 50) * 50;
    if (threshold > 0 && threshold > lastThreshold_) {
        lastThreshold_ = threshold;
        std::cout << "*** " << robot.getName() << " reached " << threshold
                  << " points! World rearranged! ***\n";
        world_.rearrange();
        world_.display ( player_->getX(), player_->getY(), computer_->getX(), computer_->getY() );
    }
}

void Game::applyEffect(Robot& robot, int effect) {
    switch (effect) {
        case -1:
            std::cout << "  [EFFECT -1] " << robot.getName() << " blocked this round!\n";
            break;

        case -2: {
            int minVal = std::numeric_limits<int>::max(), bx = 0, by = 0;
            for (int x = 0; x < world_.getSizeX(); ++x)
                for (int y = 0; y < world_.getSizeY(); ++y) {
                    int surf = world_.getSurfaceLevel(x, y);
                    if (surf < 0) continue;
                    int val = world_.getValue(x, y, surf);
                    if (val < minVal) { minVal = val; bx = x; by = y; }
                }
            std::cout << "  [EFFECT -2] " << robot.getName()
                      << " teleported to (" << bx << "," << by << ")!\n";
            robot.setPosition(bx, by);
            break;
        }

        case -3:
            std::cout << "  [EFFECT -3] " << robot.getName() << " loses half score!\n";
            robot.addScore(-(robot.getScore() / 2));
            break;

        default:
            std::cout << "  [EFFECT] Unknown effect " << effect << " ignored.\n";
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

int validateInput (const std::string& prompt, int min, int max) {
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
