#pragma once

#include <vector>

/* A 3-D grid world where robots mine positive-value blocks and trigger effects. */
class World {
  public:
    explicit World ( int x = 5, int y = 5, int z = 10 );

    /* Return the value at (x, y, z).
       Throws std::out_of_range for invalid coordinates.
       Complexity: O(1) */
    int getValue ( int x, int y, int z ) const;

    /* Set the value at (x, y, z).
       Throws std::out_of_range for invalid coordinates.
       Complexity: O(1) */
    void setValue ( int x, int y, int z, int value );

    /* Return the highest z in column (x, y) containing a positive value.
       Returns -1 if no minable block exists in the column.
       Complexity: O(sizeZ) */
    int getSurfaceLevel ( int x, int y ) const;

    /* Mine the current surface block at (x, y), set it to 0, and return its value.
       Returns 0 if the column has no minable surface.
       Complexity: O(sizeZ) */
    int mine ( int x, int y );

    /* Print a 2-D surface view of the world with optional robot markers.
       Pass default coordinates (-1, -1) to omit a robot marker.
       Complexity: O(sizeX * sizeY * sizeZ) */
    void display ( int p1x = -1, int p1y = -1, int p2x = -1, int p2y = -1 ) const;

    int getSizeX () const { return _sizeX; }
    int getSizeY () const { return _sizeY; }
    int getSizeZ () const { return _sizeZ; }

    /* Rearrange each column by applying a random operation
       (shuffle, ascending sort, or descending sort) to non-zero entries.
       Complexity: O(sizeX * sizeY * sizeZ log sizeZ) */
    void rearrange ();

    /* Find and consume one negative effect value in column (x, y).
       Returns 0 if no effect exists.
       Complexity: O(sizeZ) */
    int checkEffects ( int x, int y );

  private:
    int _sizeX, _sizeY, _sizeZ;
    std::vector<std::vector<std::vector<int>>> _grid;

    /* Initialize the grid with random positive values and occasional effects.
       Complexity: O(sizeX * sizeY * sizeZ) */
    void init ();
};
