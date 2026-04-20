#pragma once

#include <vector>

class World {
    /* A 3D grid world where robots can mine blocks.
    Blocks have a value, some have special effects. */

  public:
    explicit World ( int x = 5, int y = 5, int z = 10 );

    int getValue ( int x, int y, int z ) const;
    void setValue ( int x, int y, int z, int value );
    int getSurfaceLevel ( int x, int y ) const;
    int mine ( int x, int y );

    void display ( int p1x = -1, int p1y = -1, int p2x = -1, int p2y = -1 ) const; // player positions initialized to -1 -> outside of surface grid

    int getSizeX () const { return _sizeX; }
    int getSizeY () const { return _sizeY; }
    int getSizeZ () const { return _sizeZ; }

    // stage 2
    void rearrange ();

    // stage 3
    int checkEffects ( int x, int y );

  private:
    int _sizeX, _sizeY, _sizeZ;
    std::vector<std::vector<std::vector<int>>> _grid;

    void init ();
};
