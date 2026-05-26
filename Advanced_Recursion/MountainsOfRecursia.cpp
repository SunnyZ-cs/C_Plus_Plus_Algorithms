#include "MountainsOfRecursia.h"
using namespace std;

Vector<Point> makeMountainRange(const Point& left,
                                const Point& right,
                                int amplitude,
                                double decayRate) {
    /* Error cases below.*/
    if (left.x > right.x) {
        error("The left point is to the right of the right point.");
    }
    if (amplitude < 0) {
        error("The amplitude value is negative.");
    }
    if (decayRate < 0 || decayRate > 1) {
        error("The decayRate parameter is negative or greater than one.");
    }

    Vector<Point> soFar;
    if (abs(left.x - right.x) <= 3) {
        /* Base case: if distance between left and right is
         * sufficiently close, draw a straight line and we're done.
         */
        soFar += left;
        soFar += right;
    } else {
        /* Recursive case: compute new midpoint and draw two smaller
         * mountain ranges.
         */
        int y_add = randomInteger(-1 * amplitude, amplitude);
        Point mid = {(left.x + right.x)/2, ((left.y + right.y)/2) + y_add};
        soFar += makeMountainRange(left, mid, amplitude * decayRate, decayRate);
        soFar.remove(soFar.size() - 1);
        soFar += makeMountainRange(mid, right, amplitude * decayRate, decayRate);
    }
    return soFar;
}




/* * * * * Unit tests curated by Sunny Zhao * * * * */
#include "GUI/SimpleTest.h"
#include "Demos/ChiSquaredTesting.h"

STUDENT_TEST("Handles invalid inputs.") {
    /* Left point to the right of the right point. */
    EXPECT_ERROR(makeMountainRange({0, 0}, {-1, 0}, 10, 1));

    /* Invalid amplitude. */
    EXPECT_ERROR(makeMountainRange({0, 0}, {10, 10}, -137, 1));

    /* Bad decay rates. */
    EXPECT_ERROR(makeMountainRange({0, 0}, {10, 10}, 137, -0.1));
    EXPECT_ERROR(makeMountainRange({0, 0}, {10, 10}, 137, +1.1));
}

STUDENT_TEST("Works on points that are very close together.") {
    /* Points are within 3 pixels, so we should just get a straight line. */
    auto mountain = makeMountainRange({0, 0}, {1, 0}, 100, 0.1);
    EXPECT_EQUAL(mountain, {{ 0, 0 }, { 1, 0 }});
}

STUDENT_TEST("Works with close points and amplitude zero.") {
    /* We should get back (0, 0), (3, 3), (6, 6) because that's the midpoint
     * and the displacement amplitude is zero.
     */
    auto mountain = makeMountainRange({0, 0}, {6, 6}, 0, 1);
    EXPECT_EQUAL(mountain, {{ 0, 0 }, { 3, 3 }, { 6, 6 }});
}

STUDENT_TEST("Works with far points and zero amplitude.") {
    /* Create a number of points spaced out by threes. This must be one more
     * than a power of two for the math to work out.
     */
    const int kNumPoints = 33;

    Vector<Point> points;
    for (int i = 0; i < kNumPoints; i++) {
        points += Point{ 3 * i, 3 * i };
    }

    /* If we make a mountain range between the first and last point with zero
     * amplitude, we should get all the points on the line in the order in which
     * they appear.
     */
    EXPECT_EQUAL(makeMountainRange(points[0], points[points.size() - 1], 0, 1), points);
}

STUDENT_TEST("Works with close points and amplitude 1.") {
    /* The endpoints are (10, 0) and (16, 0), so the midpoint is (13, 0). It should
     * be displaced by +1 / -1, so we should get back (13, -1), (13, 0), or (13, 1)
     * uniformly at random.
     */
    if (!ChiSquaredTesting::isClose({ 1.0 / 3.0, 1.0 / 3.0, 1.0 / 3.0 }, [] {
            auto mountain = makeMountainRange({10, 0}, {16, 0}, 1, 1);

            /* Confirm everything else is okay. */
            EXPECT_EQUAL(mountain.size(), 3);
            EXPECT_EQUAL(mountain[0], {10, 0});
            EXPECT_EQUAL(mountain[2], {16, 0});

            /* Make sure the middle point is one of (13, -1), (13, 0), or (31, +1). */
            EXPECT_EQUAL(mountain[1].x, 13);
            EXPECT_GREATER_THAN_OR_EQUAL_TO(mountain[1].y, -1);
            EXPECT_LESS_THAN_OR_EQUAL_TO(mountain[1].y, 1);

            /* Map from -1, 0, 1 to 0, 1, 2. */
            return mountain[1].y + 1;
        })) {
        SHOW_ERROR("Not all midpoint displacements are equally likely.");
    }
}

STUDENT_TEST("Works with close points and amplitude 10.") {
    /* The endpoints are (10, 0) and (16, 0), so the midpoint is (13, 0). It should
     * be displaced by +1 / -1, so we should get back (13, -10), (13, -9),
     * ..., or (13, 10) uniformly at random.
     */
    if (!ChiSquaredTesting::isClose(vector<double>(21, 1.0 / 21), [] {
            auto mountain = makeMountainRange({10, 0}, {16, 0}, 10, 1);

            /* Confirm everything else is okay. */
            EXPECT_EQUAL(mountain.size(), 3);
            EXPECT_EQUAL(mountain[0], {10, 0});
            EXPECT_EQUAL(mountain[2], {16, 0});

            /* Make sure the middle point is one of (13, -10), ... (13, +10). */
            EXPECT_EQUAL(mountain[1].x, 13);
            EXPECT_GREATER_THAN_OR_EQUAL_TO(mountain[1].y, -10);
            EXPECT_LESS_THAN_OR_EQUAL_TO(mountain[1].y, 10);

            return mountain[1].y + 10;
        })) {
        SHOW_ERROR("Not all midpoint displacements are equally likely.");
    }
}

STUDENT_TEST("Works on faraway points with decaying amplitude.") {
    /* Endpoints are (10, 0) and (22, 12). Amplitude is 2. Decay rate is 0.5.
     * This means that our points should be
     *
     * (10, 0), (13, X), (16, Y), (19, Z), (22, 12)
     *
     * where Y is displaced from the expected midpoint of 6 by -2 to +2. The values
     * of X and Z will depend on what Y is.
     */
    if (!ChiSquaredTesting::isClose(vector<double>(5, 1.0 / 5), [] {
            auto mountain = makeMountainRange({10, 0}, {22, 12}, 2, 0.5);

            /* Confirm the endpoints are correct. */
            EXPECT_EQUAL(mountain.size(), 5);
            EXPECT_EQUAL(mountain[0], {10, 0});
            EXPECT_EQUAL(mountain[4], {22, 12});

            /* Make sure the middle points have the right x coordinates. */
            EXPECT_EQUAL(mountain[1].x, 13);
            EXPECT_EQUAL(mountain[2].x, 16);
            EXPECT_EQUAL(mountain[3].x, 19);

            /* Make sure the middle point's y coordinate is within tolerance of
             * the actual midpoint.
             */
            EXPECT_GREATER_THAN_OR_EQUAL_TO(mountain[2].y, 4);
            EXPECT_LESS_THAN_OR_EQUAL_TO(mountain[2].y, 8);

            return mountain[2].y - 4;
        })) {
        SHOW_ERROR("Not all central midpoint displacements are equally likely.");
    }

    /* Now, look at the Y coordinate of the point after the first. The probabilities of
     * its value can be determined based on the probabilities that the midpoint takes
     * on different values. That's shown here:
     *
     *   First Point    Midpoint    True Midpoint   Possible Outcome     Probability
     *   (10, 0)        (16, 4)     (13, 2)         (13, 1)              1/5 * 1/3
     *   (10, 0)        (16, 4)     (13, 2)         (13, 2)              1/5 * 1/3
     *   (10, 0)        (16, 4)     (13, 2)         (13, 3)              1/5 * 1/3
     *   (10, 0)        (16, 5)     (13, 2)         (13, 1)              1/5 * 1/3
     *   (10, 0)        (16, 5)     (13, 2)         (13, 2)              1/5 * 1/3
     *   (10, 0)        (16, 5)     (13, 2)         (13, 3)              1/5 * 1/3
     *   (10, 0)        (16, 6)     (13, 3)         (13, 2)              1/5 * 1/3
     *   (10, 0)        (16, 6)     (13, 3)         (13, 3)              1/5 * 1/3
     *   (10, 0)        (16, 6)     (13, 3)         (13, 4)              1/5 * 1/3
     *   (10, 0)        (16, 7)     (13, 3)         (13, 2)              1/5 * 1/3
     *   (10, 0)        (16, 7)     (13, 3)         (13, 3)              1/5 * 1/3
     *   (10, 0)        (16, 7)     (13, 3)         (13, 4)              1/5 * 1/3
     *   (10, 0)        (16, 8)     (13, 4)         (13, 3)              1/5 * 1/3
     *   (10, 0)        (16, 8)     (13, 4)         (13, 4)              1/5 * 1/3
     *   (10, 0)        (16, 8)     (13, 4)         (13, 5)              1/5 * 1/3
     *
     *   Summing this up, we get the following
     *
     *   Outcome | Probability
     *  ---------+----------------
     *   (13, 1) | 2/15
     *   (13, 2) | 4/15
     *   (13, 3) | 5/15
     *   (13, 4) | 3/15
     *   (13, 5) | 1/15
     */
    if (!ChiSquaredTesting::isClose({ 2.0 / 15, 4.0 / 15, 5.0 / 15, 3.0 / 15, 1.0 / 15 }, [] {
            auto mountain = makeMountainRange({10, 0}, {22, 12}, 2, 0.5);

            /* Confirm the endpoints are correct. */
            EXPECT_EQUAL(mountain.size(), 5);
            EXPECT_EQUAL(mountain[0], {10, 0});
            EXPECT_EQUAL(mountain[4], {22, 12});

            /* Make sure the middle points have the right x coordinates. */
            EXPECT_EQUAL(mountain[1].x, 13);
            EXPECT_EQUAL(mountain[2].x, 16);
            EXPECT_EQUAL(mountain[3].x, 19);

            /* Make sure the left midpoint's y coordinate is within tolerance of
             * the actual midpoint.
             */
            EXPECT_GREATER_THAN_OR_EQUAL_TO(mountain[1].y, 1);
            EXPECT_LESS_THAN_OR_EQUAL_TO(mountain[1].y, 5);

            return mountain[1].y - 1;
        })) {
        SHOW_ERROR("Not all left midpoint displacements are equally likely.");
    }

    /* We get the same table for the right midpoint, except that
     *
     * 1. The Y coordinate is 19, not 13.
     * 2. All values are increased by 6, because we're taking the midpoint of what's
     *    on the right and the midpoint, not what's on the left and the midpoint.
     */
    if (!ChiSquaredTesting::isClose({ 2.0 / 15, 4.0 / 15, 5.0 / 15, 3.0 / 15, 1.0 / 15 }, [] {
            auto mountain = makeMountainRange({10, 0}, {22, 12}, 2, 0.5);

            /* Confirm the endpoints are correct. */
            EXPECT_EQUAL(mountain.size(), 5);
            EXPECT_EQUAL(mountain[0], {10, 0});
            EXPECT_EQUAL(mountain[4], {22, 12});

            /* Make sure the middle points have the right x coordinates. */
            EXPECT_EQUAL(mountain[1].x, 13);
            EXPECT_EQUAL(mountain[2].x, 16);
            EXPECT_EQUAL(mountain[3].x, 19);

            /* Make sure the left midpoint's y coordinate is within tolerance of
             * the actual midpoint.
             */
            EXPECT_GREATER_THAN_OR_EQUAL_TO(mountain[3].y, 7);
            EXPECT_LESS_THAN_OR_EQUAL_TO(mountain[3].y, 11);

            return mountain[3].y - 7;
        })) {
        SHOW_ERROR("Not all left midpoint displacements are equally likely.");
    }
}
