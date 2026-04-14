#pragma once

#include "../include/BaseRobot.h"

class SortBot : public BaseRobot {
  public:
    SortBot ( int startX, int startY );
    int mine ( World &world ) override;
};
