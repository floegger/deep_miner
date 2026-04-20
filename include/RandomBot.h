#pragma once

#include "BaseRobot.h"

class World;

/* Robot that mines a random number of positive blocks from its current column. */
class RandomBot : public BaseRobot {
  public:
    RandomBot ( int startX, int startY );
    int mine ( World &world ) override;

  private:
    int randomNumber ();
};
