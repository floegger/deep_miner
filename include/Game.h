#pragma once

#include <memory>

#include "World.h"
#include "Robot.h"


class Game {
    public:
        Game ();
        void run ();

    private:
        World world_;
        std::unique_ptr< Robot > player_;
        std::unique_ptr < Robot > computer_;
        bool autoMode_ = false;
        int lastThreshold_ = 0;

        void setup ();
        void play ( Robot& robot, bool isPlayer );
        void checkRearrange ( Robot& robot );
        void applyEffects ( Robot& robot, int effect );
        bool isGameOver() const;
        void printScores() const;
        void printResult() const;

        std::unique_ptr < Robot > createRobot ( int choice, int x, int y ) const;

};
