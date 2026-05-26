#include "TempleOfRecursia.h"
using namespace std;

/* A helper function that draws exactly one rectangle. */
Rectangle drawRect(const Rectangle& bounds,
                        double fracWidth,
                        double fracHeight,
                        double addFrac) {
    double width = fracWidth * bounds.width;
    double height = fracHeight * bounds.height;
    double addHeight = addFrac * bounds.height;

    int x_add = (bounds.width - width) / 2;

    int left = bounds.x + x_add;
    int top = bounds.y + bounds.height - addHeight - height;

    return {left, top, int(width), int(height)};
}

/* A helper function that draws exactly a (base + column) pair of rectangles. */
Vector<Rectangle> drawTempleHelper(const Rectangle& bounds, const TempleParameters& params) {
    Rectangle base = drawRect(bounds, params.baseWidth, params.baseHeight, 0);
    Rectangle column = drawRect(bounds, params.columnWidth, params.columnHeight, params.baseHeight);
    return {base, column};
}

/* This helper function adds an upper temple on top of the column last drawn. First, create a
 * new, smaller, upper bounding box. Then, draw a temple within the new bounding box.*/
Vector<Rectangle> drawUpperTemple(const Rectangle& bounds,
                                  const TempleParameters& params,
                                  const Rectangle& input) {
    int newHeight = params.upperTempleHeight * bounds.height;
    Rectangle newBounds = {input.x, input.y - newHeight, input.width, newHeight};
    return makeTemple(newBounds, params);
}

/* This helper function adds smaller temples along the base last drawn. */
Vector<Rectangle> drawSmallerTemples(const Rectangle& bounds,
                                     const TempleParameters& params,
                                     const Rectangle& input) {
    int smallWidth = params.smallTempleWidth * bounds.width;
    int smallHeight = params.smallTempleHeight * bounds.height;
    int spacing = (input.width - params.numSmallTemples * smallWidth) / (params.numSmallTemples - 1);

    /* Looping over all of the smaller temples from left to right */
    Vector<Rectangle> output;
    for (int i = 0; i < params.numSmallTemples; i++) {
        Rectangle smallBounds = {input.x + i * (smallWidth + spacing), input.y - smallHeight, smallWidth, smallHeight};
        output += makeTemple(smallBounds, params);
    }

    return output;
}

Vector<Rectangle> makeTemple(const Rectangle& bounds, const TempleParameters& params) {
    /* Error case elimination */
    if (params.order < 0) {
        error("Order is negative.");
    }

    /* Base case: by definition, an order-0 temple is nothing at all. */
    if (params.order == 0) {
        return {};
    }

    /* Recursive case for drawing the upper temple and the many smaller temples. */
    Vector<Rectangle> output = drawTempleHelper(bounds, params);

    TempleParameters newParams = params;
    newParams.order -= 1;

    output += drawUpperTemple(bounds, newParams, output[1]);
    output += drawSmallerTemples(bounds, newParams, output[0]);

    return output;
}




/* * * * * Unit tests curated by Sunny Zhao * * * * */
#include "GUI/SimpleTest.h"

STUDENT_TEST("Milestone One: Draws the initial temple base.") {
    /* These bounds are chosen so that the resulting x and y coordinates
     * can clearly be read off as a combination of the x and y of the
     * bounds and the width and height.
     */
    Rectangle bounds = { 100000, 400000, 128, 512 };
    TempleParameters params;
    params.order = 1;

    /* Simple case: the base fills the entire rectangle. */
    params.baseHeight = 1.0;
    params.baseWidth  = 1.0;

    auto temple = makeTemple(bounds, params);
    EXPECT_GREATER_THAN(temple.size(), 0);

    /* First rectangle should be the base. */
    EXPECT_EQUAL(temple[0], { 100000, 400000, 128, 512 });

    /* A more realistic base. */
    params.baseHeight = 0.25;
    params.baseWidth  = 0.75;

    temple = makeTemple(bounds, params);
    EXPECT_GREATER_THAN(temple.size(), 0);

    /* First rectangle should be the base. */
    EXPECT_EQUAL(temple[0], { 100016, 400384, 96, 128 });
}

STUDENT_TEST("Milestone Two: Draws the column.") {
    /* These bounds are chosen so that the resulting x and y coordinates
     * can clearly be read off as a combination of the x and y of the
     * bounds and the width and height.
     */
    Rectangle bounds = { 100000, 400000, 128, 512 };

    TempleParameters params;
    params.order = 1;

    /* Simple case: The base has zero height and the tower fills the whole bounds. */
    params.baseHeight = 0.0;
    params.baseWidth  = 1.0;
    params.columnWidth = 1.0;
    params.columnHeight = 1.0;

    auto temple = makeTemple(bounds, params);
    EXPECT_GREATER_THAN(temple.size(), 1);

    /* Second rectangle should be the tower, and it should fill the whole bounds. */
    EXPECT_EQUAL(temple[1], { 100000, 400000, 128, 512 });

    /* More elaborate case: Base is 25% the height, and tower column is 50% of the width
     * of the bounds and 25% the height.
     */
    params.baseHeight = 0.25;
    params.columnWidth = 0.5;
    params.columnHeight = 0.25;

    temple = makeTemple(bounds, params);
    EXPECT_GREATER_THAN(temple.size(), 1);

    /* Second rectangle is the tower. */
    EXPECT_EQUAL(temple[1], { 100032, 400256, 64, 128 });
}

STUDENT_TEST("Milestone Three: Handles zero and negative orders.") {
    Rectangle bounds = { 0, 0, 1, 1 };
    TempleParameters params;
    params.order = 0;

    EXPECT_EQUAL(makeTemple(bounds, params), { });

    params.order = -1;
    EXPECT_ERROR(makeTemple(bounds, params));

    params.order = -137;
    EXPECT_ERROR(makeTemple(bounds, params));
}

STUDENT_TEST("Milestone Three: Draws an upper temple on top of the column.") {
    /* Dimensions chosen so that we can work with quarters and halves without issue.
     * As above, we choose the (x, y) pair to be different and the width/height to
     * be different so that mistakes are easier to spot. Using 100000 and 200000
     * for x and y also makes it easier to see the contributions of each component.
     */
    Rectangle bounds = { 100000, 200000, 128, 512 };
    TempleParameters params;
    params.order = 2;

    /* Base has height 25% and inset 25%. Column has height 25% and with 50%. */
    params.baseHeight = 0.25;
    params.baseWidth  = 0.50;
    params.columnWidth = 0.50;
    params.columnHeight = 0.25;

    /* Upper temple has height 25%. */
    params.upperTempleHeight = 0.25;

    /* We expect to get back
     *
     * Order-2 base
     * Order-2 column
     * Order-1 base
     * Order-1 column
     */
    auto temple = makeTemple(bounds, params);
    EXPECT_GREATER_THAN(temple.size(), 3);

    /* Base. */
    EXPECT_EQUAL(temple[0], { 100032, 200384, 64, 128 });

    /* Column. */
    EXPECT_EQUAL(temple[1], { 100032, 200256, 64, 128 });

    /* Recursive base. */
    EXPECT_EQUAL(temple[2], { 100048, 200224, 32, 32 });

    /* Recursive column. */
    EXPECT_EQUAL(temple[3], { 100048, 200192, 32, 32 });
}

STUDENT_TEST("Milestone Three: Draws multiple levels of upper temples.") {
    /* Dimensions chosen so that we can work with quarters and halves without issue.
     * As above, we choose the (x, y) pair to be different and the width/height to
     * be different so that mistakes are easier to spot. Using 100000 and 200000
     * for x and y also makes it easier to see the contributions of each component.
     */
    Rectangle bounds = { 100000, 200000, 64, 512 };
    TempleParameters params;
    params.order = 3;

    /* Base has height 25% and width 75%. Column has height 25% and with 50%. */
    params.baseHeight = 0.25;
    params.baseWidth  = 0.75;
    params.columnWidth = 0.50;
    params.columnHeight = 0.25;

    /* Upper temple has height 50%. */
    params.upperTempleHeight = 0.5;

    /* We expect to get back
     *
     * Order-3 base
     * Order-3 column
     * Order-2 base
     * Order-2 column
     * Order-1 base
     * Order-1 column
     */
    auto temple = makeTemple(bounds, params);
    EXPECT_GREATER_THAN(temple.size(), 5);

    /* Order-3 Base. */
    EXPECT_EQUAL(temple[0], { 100008, 200384, 48, 128 });

    /* Order-3 Column. */
    EXPECT_EQUAL(temple[1], { 100016, 200256, 32, 128 });

    /* Order-2 Base. */
    EXPECT_EQUAL(temple[2], { 100020, 200192, 24, 64 });

    /* Order-2 Column. */
    EXPECT_EQUAL(temple[3], { 100024, 200128, 16, 64 });

    /* Order-1 Base. */
    EXPECT_EQUAL(temple[4], { 100026, 200096, 12, 32 });

    /* Order-1 Column. */
    EXPECT_EQUAL(temple[5], { 100028, 200064, 8, 32 });
}

STUDENT_TEST("Milestone Four: Draws two smaller towers of the correct size.") {
    /* Dimensions chosen so that we can work with quarters and halves without issue.
     * As above, we choose the (x, y) pair to be different and the width/height to
     * be different so that mistakes are easier to spot. Using 100000 and 200000
     * for x and y also makes it easier to see the contributions of each component.
     */
    Rectangle bounds = { 100000, 200000, 64, 512 };
    TempleParameters params;
    params.order = 2;

    /* Base has height 25% and width 75%. Column has height 25% and width 50%. */
    params.baseHeight = 0.25;
    params.baseWidth  = 0.75;
    params.columnWidth = 0.50;
    params.columnHeight = 0.25;

    /* Upper temple has height 25%. */
    params.upperTempleHeight = 0.25;

    /* Two smaller temples. */
    params.numSmallTemples = 2;

    /* Each smaller temple is 25% the width of the figure and 50% the height. */
    params.smallTempleWidth  = 0.25;
    params.smallTempleHeight = 0.50;

    /* We expect to get back
     *
     * Order-2 large base
     * Order-2 large column
     * Order-1 large base
     * Order-1 large column
     * Order-1 left small base
     * Order-1 left small column
     * Order-1 right small base
     * Order-1 right small column
     */
    auto temple = makeTemple(bounds, params);
    EXPECT_GREATER_THAN(temple.size(), 7);

    /* Order-2 Base. */
    EXPECT_EQUAL(temple[0], { 100008, 200384, 48, 128 });

    /* Order-2 Column. */
    EXPECT_EQUAL(temple[1], { 100016, 200256, 32, 128 });

    /* Order-1 (Upper) Base. */
    EXPECT_EQUAL(temple[2], { 100020, 200224, 24, 32 });

    /* Order-1 (Upper) Column. */
    EXPECT_EQUAL(temple[3], { 100024, 200192, 16, 32 });

    /* Order-1 (Left Small) Base. */
    EXPECT_EQUAL(temple[4], { 100010, 200320, 12, 64 });

    /* Order-1 (Left Small) Column. */
    EXPECT_EQUAL(temple[5], { 100012, 200256, 8, 64 });

    /* Order-1 (Right Small) Base. */
    EXPECT_EQUAL(temple[6], { 100042, 200320, 12, 64 });

    /* Order-1 (Right Small) Column. */
    EXPECT_EQUAL(temple[7], { 100044, 200256, 8, 64 });

}

STUDENT_TEST("Milestone Four: Draws four smaller towers of the correct size.") {
    /* The width and height here are chosen to be powers of two so that we can
     * exactly represent them using the double type without rounding.
     *
     * As above, we choose the (x, y) pair to be different and the width/height to
     * be different so that mistakes are easier to spot. Using 100000 and 200000
     * for x and y also makes it easier to see the contributions of each component.
     */
    Rectangle bounds = { 10000000, 20000000, 2048, 256 };
    TempleParameters params;
    params.order = 2;

    /* Base has height 25% and width 50%. We pick these choices because these
     * are exactly representable as doubles and so that they're nonzero, making
     * it easier to spot arithmetic errors when positioning the temple.
     *
     * Column has height 25% and width 50% - again, nice powers of two that
     * make it easy to spot math errors and mixups in quantities.
     */
    params.baseHeight = 0.25;
    params.baseWidth  = 0.50;
    params.columnWidth = 0.50;
    params.columnHeight = 0.25;

    /* Upper temple has height 50%, again, to make everything work nicely with
     * round numbers.
     */
    params.upperTempleHeight = 0.5;

    /* Four smaller temples. */
    params.numSmallTemples = 4;

    /* Each smaller tower is 1/8 the width of the figure and 50% the height. */
    params.smallTempleWidth  = 0.125;
    params.smallTempleHeight = 0.50;

    /* We expect to get back
     *
     * Order-2 large base
     * Order-2 large column
     * Order-1 large base
     * Order-1 large column
     * Order-1 leftmost small base
     * Order-1 leftmost small column
     * Order-1 left middle small base
     * Order-1 left middle small column
     * Order-1 right middle small base
     * Order-1 right middle small column
     * Order-1 rightmost small base
     * Order-1 rightmost small column
     */
    auto temple = makeTemple(bounds, params);
    EXPECT_GREATER_THAN(temple.size(), 11);

    /* Order-2 Base. */
    EXPECT_EQUAL(temple[0], { 10000512, 20000192, 1024, 64 });

    /* Order-2 Column. */
    EXPECT_EQUAL(temple[1], { 10000512, 20000128, 1024, 64 });

    /* Order-1 (Upper) Base. */
    EXPECT_EQUAL(temple[2], { 10000768, 20000096, 512, 32 });

    /* Order-1 (Upper) Column. */
    EXPECT_EQUAL(temple[3], { 10000768, 20000064, 512, 32 });

    /* Order-1 (Leftmost Small) Base. */
    EXPECT_EQUAL(temple[4], { 10000576, 20000160, 128, 32 });

    /* Order-1 (Leftmost Small) Column. */
    EXPECT_EQUAL(temple[5], { 10000576, 20000128, 128, 32 });

    /* Order-1 (Left Middle Small) Base. */
    EXPECT_EQUAL(temple[6], { 10000832, 20000160, 128, 32 });

    /* Order-1 (Left Middle Small) Column. */
    EXPECT_EQUAL(temple[7], { 10000832, 20000128, 128, 32 });

    /* Order-1 (Right Middle Small) Base. */
    EXPECT_EQUAL(temple[8], { 10001088, 20000160, 128, 32 });

    /* Order-1 (Right Middle Small) Column. */
    EXPECT_EQUAL(temple[9], { 10001088, 20000128, 128, 32 });

    /* Order-1 (Rightmost Middle Small) Base. */
    EXPECT_EQUAL(temple[10], { 10001344, 20000160, 128, 32 });

    /* Order-1 (Rightmost Middle Small) Column. */
    EXPECT_EQUAL(temple[11], { 10001344, 20000128, 128, 32 });
}

STUDENT_TEST("Milestone Four: Draws a fully-recursive, elaborate temple.") {
    /* We've chosen these values so that all widths and heights are powers
     * of two, which can be stored as doubles with perfect precision and no
     * rounding errors.
     */
    Rectangle bounds = { 1048576, 2097152, 65536, 16384 };
    TempleParameters params;
    params.order = 3;

    /* All these scaling factors are perfect powers of two. */
    params.baseHeight = 0.125;
    params.baseWidth  = 0.5;
    params.columnWidth = 0.0625;
    params.columnHeight = 0.3125;
    params.upperTempleHeight = 0.5;

    /* Four smaller temples. */
    params.numSmallTemples = 4;

    /* Each smaller temple is 1/8 the width of the figure and 50% the height. */
    params.smallTempleWidth  = 0.125;
    params.smallTempleHeight = 0.50;

    auto temple = makeTemple(bounds, params);

    Vector<Rectangle> expected = {
        { 1064960, 2111488, 32768, 2048 },
        { 1079296, 2106368,  4096, 5120 },
        { 1080320, 2105344,  2048, 1024 },
        { 1081216, 2102784,   256, 2560 },
        { 1081280, 2102272,   128,  512 },
        { 1081336, 2100992,    16, 1280 },
        { 1080448, 2104832,   256,  512 },
        { 1080560, 2103552,    32, 1280 },
        { 1080960, 2104832,   256,  512 },
        { 1081072, 2103552,    32, 1280 },
        { 1081472, 2104832,   256,  512 },
        { 1081584, 2103552,    32, 1280 },
        { 1081984, 2104832,   256,  512 },
        { 1082096, 2103552,    32, 1280 },
        { 1067008, 2110464,  4096, 1024 },
        { 1068800, 2107904,   512, 2560 },
        { 1068928, 2107392,   256,  512 },
        { 1069040, 2106112,    32, 1280 },
        { 1067264, 2109952,   512,  512 },
        { 1067488, 2108672,    64, 1280 },
        { 1068288, 2109952,   512,  512 },
        { 1068512, 2108672,    64, 1280 },
        { 1069312, 2109952,   512,  512 },
        { 1069536, 2108672,    64, 1280 },
        { 1070336, 2109952,   512,  512 },
        { 1070560, 2108672,    64, 1280 },
        { 1075200, 2110464,  4096, 1024 },
        { 1076992, 2107904,   512, 2560 },
        { 1077120, 2107392,    256, 512 },
        { 1077232, 2106112,    32, 1280 },
        { 1075456, 2109952,   512,  512 },
        { 1075680, 2108672,    64, 1280 },
        { 1076480, 2109952,   512,  512 },
        { 1076704, 2108672,    64, 1280 },
        { 1077504, 2109952,   512,  512 },
        { 1077728, 2108672,    64, 1280 },
        { 1078528, 2109952,   512,  512 },
        { 1078752, 2108672,    64, 1280 },
        { 1083392, 2110464,  4096, 1024 },
        { 1085184, 2107904,   512, 2560 },
        { 1085312, 2107392,   256,  512 },
        { 1085424, 2106112,    32, 1280 },
        { 1083648, 2109952,   512,  512 },
        { 1083872, 2108672,    64, 1280 },
        { 1084672, 2109952,   512,  512 },
        { 1084896, 2108672,    64, 1280 },
        { 1085696, 2109952,   512,  512 },
        { 1085920, 2108672,    64, 1280 },
        { 1086720, 2109952,   512,  512 },
        { 1086944, 2108672,    64, 1280 },
        { 1091584, 2110464,  4096, 1024 },
        { 1093376, 2107904,   512, 2560 },
        { 1093504, 2107392,   256,  512 },
        { 1093616, 2106112,    32, 1280 },
        { 1091840, 2109952,   512,  512 },
        { 1092064, 2108672,    64, 1280 },
        { 1092864, 2109952,   512,  512 },
        { 1093088, 2108672,    64, 1280 },
        { 1093888, 2109952,   512,  512 },
        { 1094112, 2108672,    64, 1280 },
        { 1094912, 2109952,   512,  512 },
        { 1095136, 2108672,    64, 1280 }
    };

    /* We don't use EXPECT_EQUALS here, as it will fill the display with a lot of
     * text.
     *
     * This test case checks the full end-to-end behavior of your temple-generation
     * code. If you have any errors in that code, they're likely to show up here.
     * You may find it useful to write additional test cases that look for more
     * specific errors if you're failing this test.
     */
    EXPECT(temple == expected);
}
