#pragma once

#include "BaseRobot.h"

class World;

/* Robot that mines up to three positive blocks per turn from its current column. */
class DigDeepBot : public BaseRobot {
  public:
    DigDeepBot ( int startX, int startY );
    int mine ( World &world ) override;
};
