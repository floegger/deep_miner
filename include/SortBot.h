#pragma once

#include "../include/BaseRobot.h"

/* Robot that reorders the current column so the highest value reaches the surface,
   then mines that top block. */
class SortBot : public BaseRobot {
  public:
    SortBot ( int startX, int startY );
    int mine ( World &world ) override;
};
