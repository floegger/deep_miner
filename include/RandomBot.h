#pragma once

#include "BaseRobot.h"

class World;

class RandomBot : public BaseRobot {
/* A robot that digs at a random depth */
    public:
        RandomBot ( const int startX, const int startY );
        int mine ( World& world ) override;
    private:
        int randomNumber();
};
