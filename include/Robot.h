#pragma once

#include <string>
#include <system_error>
#include <type_traits>

class World;

class Robot {
    /* A virtual interface for a robot */
  public:
    virtual ~Robot () = default;
    Robot ( const Robot & ) = default;
    Robot &operator= ( const Robot & ) = default;
    Robot ( Robot && ) = default;
    Robot &operator= ( Robot && ) = default;

    virtual void move ( int direction, const World &world ) = 0;
    virtual int mine ( World &world ) = 0;
    virtual int decideNextMove ( const World &world ) const = 0;

    virtual void setPosition ( int x, int y ) = 0;
    virtual int getScore () const = 0;
    virtual void addScore ( int points ) = 0;
    virtual int getX () const = 0;
    virtual int getY () const = 0;
    virtual std::string getName () const = 0;

  protected:
    Robot () = default;
};
