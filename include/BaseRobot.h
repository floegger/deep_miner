#pragma once

#include "Robot.h"

#include <string>

/* Base robot implementation shared by all concrete robot types. */
class BaseRobot : public Robot {
  public:
    BaseRobot(std::string name, int startX, int startY);

    void move(int direction, const World& world) override;
    int decideNextMove(const World& world) const override;

    void setPosition(int x, int y) override {
        x_ = x;
        y_ = y;
    }

    int getScore() const override { return score_; }
    void addScore(int points) override { score_ += points; }
    int getX() const override { return x_; }
    int getY() const override { return y_; }
    std::string getName() const override { return name_; }

  protected:
    int x_;
    int y_;
    int score_;
    std::string name_;
};
