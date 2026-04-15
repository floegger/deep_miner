#pragma once
#include "../include/BaseRobot.h"

#include <type_traits>
class World;

class DigDeepBot : public BaseRobot {
    public:
        DigDeepBot ( const int startX, const int startY );
        int mine ( World& world ) override;
};
