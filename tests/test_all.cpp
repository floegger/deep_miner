#include "../include/World.h"
#include "../include/BaseRobot.h"
#include "../include/SortBot.h"
#include "../include/DigDeepBot.h"
#include "../include/RandomBot.h"

#include <iostream>
#include <sstream>
#include <streambuf>

static int s_passed = 0;
static int s_failed = 0;

#define TEST(name, body)                                                          \
    do {                                                                          \
        try {                                                                     \
            body                                                                  \
            std::cout << "  [PASS] " name "\n";                                  \
            ++s_passed;                                                           \
        } catch ( const std::exception& e ) {                                    \
            std::cout << "  [FAIL] " name ": " << e.what() << "\n";             \
            ++s_failed;                                                           \
        } catch ( ... ) {                                                         \
            std::cout << "  [FAIL] " name ": unknown exception\n";               \
            ++s_failed;                                                           \
        }                                                                         \
    } while(0)

#define ASSERT(expr)                                                              \
    if ( !(expr) ) throw std::runtime_error ( "ASSERT failed: " #expr )

#define ASSERT_EQ(a, b)                                                           \
    if ( (a) != (b) ) {                                                           \
        std::ostringstream _msg;                                                  \
        _msg << #a << " == " << #b << "  (" << (a) << " != " << (b) << ")";     \
        throw std::runtime_error ( "ASSERT_EQ failed: " + _msg.str() );          \
    }

/* Redirect std::cout to a sink for the duration of the scope.
   Useful for robot methods that print as a side-effect. */
struct SilentOut {
    std::streambuf* saved;
    SilentOut()  { saved = std::cout.rdbuf ( nullptr ); }
    ~SilentOut() { std::cout.rdbuf ( saved ); }
};

/* Create a World with every cell zeroed so tests start from a known state.
   init() is called by the constructor; we then overwrite all cells to 0. */
static World makeEmptyWorld ( int x = 5, int y = 5, int z = 10 ) {
    World w ( x, y, z );
    for ( int xi = 0; xi < x; ++xi )
        for ( int yi = 0; yi < y; ++yi )
            for ( int zi = 0; zi < z; ++zi )
                w.setValue ( xi, yi, zi, 0 );
    return w;
}

/* World: getValue / setValue */

static void test_world_get_set () {
    std::cout << "\n── World: getValue / setValue ──\n";

    TEST ( "set and get round-trips correctly", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 1, 2, 3, 42 );
        ASSERT_EQ ( w.getValue ( 1, 2, 3 ), 42 );
    } );

    TEST ( "initial zeroed cell returns 0", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        ASSERT_EQ ( w.getValue ( 0, 0, 0 ), 0 );
    } );

    TEST ( "overwrite cell with new value", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 0, 5 );
        w.setValue ( 0, 0, 0, 9 );
        ASSERT_EQ ( w.getValue ( 0, 0, 0 ), 9 );
    } );

    TEST ( "getValue throws on x out of range", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        bool threw = false;
        try { w.getValue ( 3, 0, 0 ); } catch ( const std::out_of_range& ) { threw = true; }
        ASSERT ( threw );
    } );

    TEST ( "getValue throws on y out of range", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        bool threw = false;
        try { w.getValue ( 0, 3, 0 ); } catch ( const std::out_of_range& ) { threw = true; }
        ASSERT ( threw );
    } );

    TEST ( "getValue throws on z out of range", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        bool threw = false;
        try { w.getValue ( 0, 0, 5 ); } catch ( const std::out_of_range& ) { threw = true; }
        ASSERT ( threw );
    } );

    TEST ( "setValue throws on negative x", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        bool threw = false;
        try { w.setValue ( -1, 0, 0, 5 ); } catch ( const std::out_of_range& ) { threw = true; }
        ASSERT ( threw );
    } );

    TEST ( "setValue throws on negative z", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        bool threw = false;
        try { w.setValue ( 0, 0, -1, 5 ); } catch ( const std::out_of_range& ) { threw = true; }
        ASSERT ( threw );
    } );
}

/* World: getSurfaceLevel */

static void test_world_surface_level () {
    std::cout << "\n── World: getSurfaceLevel ──\n";

    TEST ( "empty column returns -1", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        ASSERT_EQ ( w.getSurfaceLevel ( 0, 0 ), -1 );
    } );

    TEST ( "single block at z=0 returns 0", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 0, 7 );
        ASSERT_EQ ( w.getSurfaceLevel ( 0, 0 ), 0 );
    } );

    TEST ( "returns highest positive z", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 0, 3 );
        w.setValue ( 0, 0, 2, 5 );
        w.setValue ( 0, 0, 4, 8 );
        ASSERT_EQ ( w.getSurfaceLevel ( 0, 0 ), 4 );
    } );

    TEST ( "ignores negative effect cells", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 3, -2 );  // effect at z=3
        w.setValue ( 0, 0, 1,  6 );  // mineable block at z=1
        ASSERT_EQ ( w.getSurfaceLevel ( 0, 0 ), 1 );
    } );

    TEST ( "effect above positive block is ignored; block is still surface", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 1, 4 );
        w.setValue ( 0, 0, 3, -1 );  // effect above
        ASSERT_EQ ( w.getSurfaceLevel ( 0, 0 ), 1 );
    } );

    TEST ( "all-effect column returns -1", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 0, -1 );
        w.setValue ( 0, 0, 1, -2 );
        w.setValue ( 0, 0, 2, -3 );
        ASSERT_EQ ( w.getSurfaceLevel ( 0, 0 ), -1 );
    } );

    TEST ( "does not bleed across columns", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 4, 9 );
        ASSERT_EQ ( w.getSurfaceLevel ( 1, 0 ), -1 );
    } );
}

/* World: mine */

static void test_world_mine () {
    std::cout << "\n── World: mine ──\n";

    TEST ( "returns 0 on empty column", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        ASSERT_EQ ( w.mine ( 0, 0 ), 0 );
    } );

    TEST ( "returns surface value and zeroes that cell", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 2, 9 );
        ASSERT_EQ ( w.mine ( 0, 0 ), 9 );
        ASSERT_EQ ( w.getValue ( 0, 0, 2 ), 0 );
    } );

    TEST ( "lower block becomes new surface after mining", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 1, 4 );
        w.setValue ( 0, 0, 3, 7 );
        w.mine ( 0, 0 );  // mines z=3
        ASSERT_EQ ( w.getSurfaceLevel ( 0, 0 ), 1 );
    } );

    TEST ( "column empty after mining sole block", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 0, 3 );
        w.mine ( 0, 0 );
        ASSERT_EQ ( w.getSurfaceLevel ( 0, 0 ), -1 );
    } );
}

/* World: checkEffects */

static void test_world_check_effects () {
    std::cout << "\n── World: checkEffects ──\n";

    TEST ( "returns 0 when no effects in column", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 0, 5 );
        ASSERT_EQ ( w.checkEffects ( 0, 0 ), 0 );
    } );

    TEST ( "returns the effect value when present", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 2, -2 );
        ASSERT_EQ ( w.checkEffects ( 0, 0 ), -2 );
    } );

    TEST ( "consumes the effect cell (becomes 0)", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 1, -3 );
        w.checkEffects ( 0, 0 );
        ASSERT_EQ ( w.getValue ( 0, 0, 1 ), 0 );
    } );

    TEST ( "second effect still present after first is consumed", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 0, -1 );
        w.setValue ( 0, 0, 2, -2 );
        w.checkEffects ( 0, 0 );
        const int remaining = w.checkEffects ( 0, 0 );
        ASSERT ( remaining < 0 );
    } );

    TEST ( "returns 0 for fully empty column", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        ASSERT_EQ ( w.checkEffects ( 0, 0 ), 0 );
    } );
}

/* World: rearrange (effect semantics) */

static void test_world_rearrange_effects () {
    std::cout << "\n── World: rearrange (effect semantics) ──\n";

    TEST ( "effect cells stay at their original z-position across rearrangements", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 0, -1 );  // effect at z=0
        w.setValue ( 0, 0, 1,  5 );  // minable blocks
        w.setValue ( 0, 0, 2,  3 );
        for ( int i = 0; i < 20; ++i ) {
            SilentOut s;
            w.rearrange ();
            ASSERT_EQ ( w.getValue ( 0, 0, 0 ), -1 );
        }
    } );

    TEST ( "positive values are preserved (same sum) after rearrange", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 1, 3 );
        w.setValue ( 0, 0, 3, 7 );
        SilentOut s;
        w.rearrange ();
        int sum = 0;
        for ( int z = 0; z < w.getSizeZ (); ++z )
            if ( w.getValue ( 0, 0, z ) > 0 )
                sum += w.getValue ( 0, 0, z );
        ASSERT_EQ ( sum, 10 );
    } );

    TEST ( "effect value is preserved after rearrange", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 2, -3 );
        w.setValue ( 0, 0, 4,  6 );
        SilentOut s;
        w.rearrange ();
        ASSERT_EQ ( w.getValue ( 0, 0, 2 ), -3 );
    } );
}

/* BaseRobot: move */

static void test_baserobot_move () {
    std::cout << "\n── BaseRobot: move ──\n";

    TEST ( "direction 1 moves right (+x)", {
        World w = makeEmptyWorld ();
        SortBot bot ( 0, 0 );
        bot.move ( 1, w );
        ASSERT_EQ ( bot.getX (), 1 );
    } );

    TEST ( "direction 2 moves left (-x)", {
        World w = makeEmptyWorld ();
        SortBot bot ( 2, 2 );
        bot.move ( 2, w );
        ASSERT_EQ ( bot.getX (), 1 );
    } );

    TEST ( "direction 3 moves down (+y)", {
        World w = makeEmptyWorld ();
        SortBot bot ( 0, 0 );
        bot.move ( 3, w );
        ASSERT_EQ ( bot.getY (), 1 );
    } );

    TEST ( "direction 4 moves up (-y)", {
        World w = makeEmptyWorld ();
        SortBot bot ( 2, 2 );
        bot.move ( 4, w );
        ASSERT_EQ ( bot.getY (), 1 );
    } );

    TEST ( "direction 0 stays in place", {
        World w = makeEmptyWorld ();
        SortBot bot ( 2, 3 );
        bot.move ( 0, w );
        ASSERT_EQ ( bot.getX (), 2 );
        ASSERT_EQ ( bot.getY (), 3 );
    } );

    TEST ( "clamps at right boundary (x = sizeX - 1)", {
        World w = makeEmptyWorld ();
        SortBot bot ( 4, 0 );
        bot.move ( 1, w );
        ASSERT_EQ ( bot.getX (), 4 );
    } );

    TEST ( "clamps at left boundary (x = 0)", {
        World w = makeEmptyWorld ();
        SortBot bot ( 0, 0 );
        bot.move ( 2, w );
        ASSERT_EQ ( bot.getX (), 0 );
    } );

    TEST ( "clamps at bottom boundary (y = sizeY - 1)", {
        World w = makeEmptyWorld ();
        SortBot bot ( 0, 4 );
        bot.move ( 3, w );
        ASSERT_EQ ( bot.getY (), 4 );
    } );

    TEST ( "clamps at top boundary (y = 0)", {
        World w = makeEmptyWorld ();
        SortBot bot ( 0, 0 );
        bot.move ( 4, w );
        ASSERT_EQ ( bot.getY (), 0 );
    } );
}

/* BaseRobot: decideNextMove */

static void test_baserobot_decide () {
    std::cout << "\n── BaseRobot: decideNextMove ──\n";

    TEST ( "returns 0 (stay) when world is fully empty", {
        World w = makeEmptyWorld ();
        SortBot bot ( 2, 2 );
        ASSERT_EQ ( bot.decideNextMove ( w ), 0 );
    } );

    TEST ( "chooses neighbour with highest surface value", {
        World w = makeEmptyWorld ();
        w.setValue ( 3, 2, 0, 9 );  // right neighbour of (2,2): value 9
        w.setValue ( 1, 2, 0, 2 );  // left  neighbour of (2,2): value 2
        SortBot bot ( 2, 2 );
        ASSERT_EQ ( bot.decideNextMove ( w ), 1 );  // direction 1 = right (+x)
    } );

    TEST ( "stays on current cell if it has highest value", {
        World w = makeEmptyWorld ();
        w.setValue ( 2, 2, 0, 9 );  // current position: highest
        w.setValue ( 3, 2, 0, 3 );
        w.setValue ( 1, 2, 0, 2 );
        SortBot bot ( 2, 2 );
        ASSERT_EQ ( bot.decideNextMove ( w ), 0 );  // direction 0 = stay
    } );

    TEST ( "fallback: steps right toward the only non-empty column", {
        World w = makeEmptyWorld ();
        w.setValue ( 4, 4, 0, 5 );  // only non-empty column is at (4,4)
        SortBot bot ( 0, 0 );       // no mineable neighbours
        ASSERT_EQ ( bot.decideNextMove ( w ), 1 );  // tx=4 > x_=0, so direction 1 (right)
    } );

    TEST ( "fallback: steps down when target is directly below", {
        World w = makeEmptyWorld ();
        w.setValue ( 0, 4, 0, 5 );  // only non-empty column is at (0,4)
        SortBot bot ( 0, 0 );
        ASSERT_EQ ( bot.decideNextMove ( w ), 3 );  // tx==x_ and ty=4 > y_=0, so direction 3 (down)
    } );
}

/* DigDeepBot: mine */

static void test_digdeepbot_mine () {
    std::cout << "\n── DigDeepBot: mine ──\n";

    TEST ( "returns 0 on empty column", {
        World w = makeEmptyWorld ();
        DigDeepBot bot ( 0, 0 );
        ASSERT_EQ ( bot.mine ( w ), 0 );
    } );

    TEST ( "mines the only block in the column", {
        World w = makeEmptyWorld ();
        w.setValue ( 0, 0, 2, 5 );
        DigDeepBot bot ( 0, 0 );
        ASSERT_EQ ( bot.mine ( w ), 5 );
        ASSERT_EQ ( w.getValue ( 0, 0, 2 ), 0 );
    } );

    TEST ( "mines top 3 blocks and returns their sum", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 0, 1 );  // z=0: below limit, should remain
        w.setValue ( 0, 0, 1, 2 );  // z=1: mined 3rd
        w.setValue ( 0, 0, 2, 3 );  // z=2: mined 2nd
        w.setValue ( 0, 0, 3, 4 );  // z=3: mined 1st (top)
        DigDeepBot bot ( 0, 0 );
        ASSERT_EQ ( bot.mine ( w ), 4 + 3 + 2 );
        ASSERT_EQ ( w.getValue ( 0, 0, 0 ), 1 );  // z=0 untouched
    } );

    TEST ( "mines fewer than 3 if column has only 2 blocks", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 1, 3 );
        w.setValue ( 0, 0, 3, 6 );
        DigDeepBot bot ( 0, 0 );
        ASSERT_EQ ( bot.mine ( w ), 6 + 3 );
    } );

    TEST ( "skips effect cells (negative values)", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 2, -1 );  // effect; must not be counted
        w.setValue ( 0, 0, 3,  7 );  // positive block above
        DigDeepBot bot ( 0, 0 );
        ASSERT_EQ ( bot.mine ( w ), 7 );
    } );

    TEST ( "score accumulates correctly across two mines", {
        World w = makeEmptyWorld ();
        w.setValue ( 0, 0, 0, 4 );
        w.setValue ( 0, 0, 1, 6 );
        DigDeepBot bot ( 0, 0 );
        bot.mine ( w );  // score = 10
        w.setValue ( 0, 0, 5, 3 );
        bot.mine ( w );  // score = 13
        ASSERT_EQ ( bot.getScore (), 13 );
    } );
}

/* SortBot: mine */

static void test_sortbot_mine () {
    std::cout << "\n── SortBot: mine ──\n";

    TEST ( "returns 0 on empty column", {
        World w = makeEmptyWorld ();
        SortBot bot ( 0, 0 );
        ASSERT_EQ ( bot.mine ( w ), 0 );
    } );

    TEST ( "returns the single block value", {
        World w = makeEmptyWorld ();
        w.setValue ( 0, 0, 0, 7 );
        SortBot bot ( 0, 0 );
        ASSERT_EQ ( bot.mine ( w ), 7 );
    } );

    TEST ( "always mines the highest value in the column", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 0, 3 );
        w.setValue ( 0, 0, 2, 7 );  // highest
        w.setValue ( 0, 0, 4, 1 );
        SortBot bot ( 0, 0 );
        ASSERT_EQ ( bot.mine ( w ), 7 );
    } );

    TEST ( "column has correct remaining sum after mining highest", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 0, 2 );
        w.setValue ( 0, 0, 1, 5 );
        w.setValue ( 0, 0, 2, 8 );  // mined
        SortBot bot ( 0, 0 );
        bot.mine ( w );
        int remaining = 0;
        for ( int z = 0; z < w.getSizeZ (); ++z )
            if ( w.getValue ( 0, 0, z ) > 0 )
                remaining += w.getValue ( 0, 0, z );
        ASSERT_EQ ( remaining, 2 + 5 );
    } );

    TEST ( "score accumulates correctly", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        w.setValue ( 0, 0, 0, 4 );
        w.setValue ( 0, 0, 1, 9 );
        SortBot bot ( 0, 0 );
        bot.mine ( w );  // mines 9
        w.setValue ( 0, 0, 2, 6 );
        bot.mine ( w );  // mines 6 (new highest)
        ASSERT_EQ ( bot.getScore (), 15 );
    } );
}

/* RandomBot: mine */

static void test_randombot_mine () {
    std::cout << "\n── RandomBot: mine ──\n";

    TEST ( "returns 0 on empty column regardless of limit", {
        World w = makeEmptyWorld ();
        RandomBot bot ( 0, 0 );
        for ( int i = 0; i < 10; ++i )
            ASSERT_EQ ( bot.mine ( w ), 0 );
    } );

    TEST ( "mined value is always non-negative", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        for ( int z = 0; z < 5; ++z )
            w.setValue ( 0, 0, z, z + 1 );
        RandomBot bot ( 0, 0 );
        for ( int i = 0; i < 20; ++i ) {
            /* Restore column before each mine so the test is repeatable. */
            for ( int z = 0; z < 5; ++z )
                w.setValue ( 0, 0, z, z + 1 );
            ASSERT ( bot.mine ( w ) >= 0 );
        }
    } );

    TEST ( "never mines more than 9 blocks per turn", {
        World w = makeEmptyWorld ( 3, 3, 10 );
        for ( int z = 0; z < 10; ++z )
            w.setValue ( 0, 0, z, 1 );  // 10 blocks each worth 1
        RandomBot bot ( 0, 0 );
        const int mined = bot.mine ( w );
        ASSERT ( mined <= 9 );  // limit drawn from [0, 9]
    } );

    TEST ( "score is always non-negative after mining", {
        World w = makeEmptyWorld ( 3, 3, 5 );
        for ( int z = 0; z < 5; ++z )
            w.setValue ( 0, 0, z, 3 );
        RandomBot bot ( 0, 0 );
        bot.mine ( w );
        ASSERT ( bot.getScore () >= 0 );
    } );
}

/* BaseRobot: score / position utilities */

static void test_baserobot_misc () {
    std::cout << "\n── BaseRobot: score / position utilities ──\n";

    TEST ( "initial score is 0", {
        SortBot bot ( 0, 0 );
        ASSERT_EQ ( bot.getScore (), 0 );
    } );

    TEST ( "addScore adds positive points", {
        SortBot bot ( 0, 0 );
        bot.addScore ( 10 );
        ASSERT_EQ ( bot.getScore (), 10 );
    } );

    TEST ( "addScore with negative value decreases score", {
        SortBot bot ( 0, 0 );
        bot.addScore ( 20 );
        bot.addScore ( -10 );
        ASSERT_EQ ( bot.getScore (), 10 );
    } );

    TEST ( "setPosition updates x and y", {
        SortBot bot ( 0, 0 );
        bot.setPosition ( 3, 4 );
        ASSERT_EQ ( bot.getX (), 3 );
        ASSERT_EQ ( bot.getY (), 4 );
    } );

    TEST ( "getName returns correct name for SortBot", {
        SortBot bot ( 0, 0 );
        ASSERT ( bot.getName () == "SortBot" );
    } );

    TEST ( "getName returns correct name for DigDeepBot", {
        DigDeepBot bot ( 0, 0 );
        ASSERT ( bot.getName () == "DigDeepBot" );
    } );

    TEST ( "getName returns correct name for RandomBot", {
        RandomBot bot ( 0, 0 );
        ASSERT ( bot.getName () == "RandomBot" );
    } );
}

int main () {
    std::cout << "=== Deep Miner Tests ===\n";

    test_world_get_set ();
    test_world_surface_level ();
    test_world_mine ();
    test_world_check_effects ();
    test_world_rearrange_effects ();
    test_baserobot_move ();
    test_baserobot_decide ();
    test_digdeepbot_mine ();
    test_sortbot_mine ();
    test_randombot_mine ();
    test_baserobot_misc ();

    std::cout << "\n=== Results: "
              << s_passed << " passed, "
              << s_failed << " failed ===\n";

    return s_failed > 0 ? 1 : 0;
}
