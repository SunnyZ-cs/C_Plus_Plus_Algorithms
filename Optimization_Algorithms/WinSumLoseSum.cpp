#include "WinSumLoseSum.h"
using namespace std;

Optional<Set<int>> makeTarget(const Set<int>& elems, int target) {
    /* Base case 1: when target is 0, add up no values.
     */
    if (target == 0) {
        return {};
    }

    /* Base case 2: when the target is in the set elems, add up the
     * elem(target) by itself.
     */
    if (elems.contains(target)) {
        return {target};
    }

    /* Recursive case: The current target can be made if the new set
     * exluding one of the elems in the set elems can make the new target
     * which is exactly the elem fewer than the current target.
     */
    for (int elem: elems) {
        auto remaining = elems - elem;
        auto soFar = makeTarget(remaining, target - elem);
        if (soFar != Nothing) {
            return soFar.value() + elem;
        }
    }

    /* After looping over the whole set elems, there is simply no way that
     * the target can be made.
     */
    return Nothing;
}

/* * * * * Unit tests curated by Sunny Zhao * * * * */
#include "GUI/SimpleTest.h"


STUDENT_TEST("Works for a doubleton set that contains positive and negative numbers.") {
    EXPECT_EQUAL(makeTarget({ 1, -1 }, 1), {1});
    EXPECT_EQUAL(makeTarget({ 1, -1 }, -1), {-1});
    EXPECT_NOT_EQUAL(makeTarget({ 1, -1 }, 0), Nothing);
    EXPECT_EQUAL(makeTarget({ 1, -1 }, 5), Nothing);
}



/* * * * * Unit tests curated by Sunny Zhao * * * * */
STUDENT_TEST("Works for an empty set of numbers.") {
    /* Can make 0, but not others. */
    EXPECT_EQUAL(makeTarget({ },  0), {});
    EXPECT_EQUAL(makeTarget({ },  1), Nothing);
    EXPECT_EQUAL(makeTarget({ }, -1), Nothing);
}

STUDENT_TEST("Works for a one-element (singleton) set.") {
    /* Can make 0 and 137, but not others. */
    EXPECT_EQUAL(makeTarget({ 137 }, 0),   {});
    EXPECT_EQUAL(makeTarget({ 137 }, 137), {137});
    EXPECT_EQUAL(makeTarget({ 137 }, 1),   Nothing);
    EXPECT_EQUAL(makeTarget({ 137 }, -1),  Nothing);
}

STUDENT_TEST("Works for a two-element (doubleton) set.") {
    EXPECT_EQUAL(makeTarget({ 1, 2 }, -1), Nothing);
    EXPECT_EQUAL(makeTarget({ 1, 2 }, 0), {});
    EXPECT_EQUAL(makeTarget({ 1, 2 }, 1), {1});
    EXPECT_EQUAL(makeTarget({ 1, 2 }, 2), {2});
    EXPECT_EQUAL(makeTarget({ 1, 2 }, 3), {1, 2});
    EXPECT_EQUAL(makeTarget({ 1, 2 }, 4), Nothing);
}

STUDENT_TEST("Works for a three-element set.") {

    /* Can make 0, 1, 3, 4, 5, 7, and 8, but not others. */
    EXPECT_EQUAL(makeTarget({ 1, 3, 4 }, -1), Nothing);
    EXPECT_EQUAL(makeTarget({ 1, 3, 4 },  0), {});
    EXPECT_EQUAL(makeTarget({ 1, 3, 4 },  1), {1});
    EXPECT_EQUAL(makeTarget({ 1, 3, 4 },  2), Nothing);
    EXPECT_EQUAL(makeTarget({ 1, 3, 4 },  3), {3});
    EXPECT_NOT_EQUAL(makeTarget({ 1, 3, 4 }, 4), Nothing);
    EXPECT_EQUAL(makeTarget({ 1, 3, 4 },  5), {1, 4});
    EXPECT_EQUAL(makeTarget({ 1, 3, 4 },  6), Nothing);
    EXPECT_EQUAL(makeTarget({ 1, 3, 4 },  7), {3, 4});
    EXPECT_EQUAL(makeTarget({ 1, 3, 4 },  8), {1, 3, 4});
    EXPECT_EQUAL(makeTarget({ 1, 3, 4 },  9), Nothing);
}
