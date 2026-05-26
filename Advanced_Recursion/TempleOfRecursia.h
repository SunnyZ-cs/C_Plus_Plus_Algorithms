#pragma once

#include "Demos/2DTypes.h"

struct TempleParameters {
    /* How wide is the base, relative to the width of the bounding box? */
    double baseHeight = 0.1;

    /* How tall is the base, relative to the height of the bounding box? */
    double baseWidth = 0.9;

    /* How wide is the column, relative to the width of the bounding box? */
    double columnWidth = 0.5;

    /* How tall is the column, relative to the height of the bounding box? */
    double columnHeight = 0.3;

    /* How tall is the upper temple, relative to the height of the bounding box? */
    double upperTempleHeight = 0.6;

    /* Order of the figure. An order-0 temple is nothing at all. An order-n
     * temple is a base and column with an order-(n-1) temple placed on top
     * of it.
     */
    int order = 6;

    /* How many smaller temples are there? (Will always be at least two.) */
    int numSmallTemples = 4;

    /* How wide is each smaller temples, relative to the width of the bounding box? */
    double smallTempleWidth = 0.2;

    /* How tall is each smaller temples, relative to the height of the bounding box? */
    double smallTempleHeight = 0.5;
};

Vector<Rectangle> makeTemple(const Rectangle& bounds,
                             const TempleParameters& params);
