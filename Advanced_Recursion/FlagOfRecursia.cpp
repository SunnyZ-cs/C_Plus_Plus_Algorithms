/******************************************************************************
 * File: FlagOfRecursia.cpp
 * Author:  ()
 * Code to draw the Flag of Recursia. You do not need to edit this file when
 * working through project 3. Instead, you'll just work through it in the
 * debugger.
 */
#include "FlagOfRecursia.h"
#include "vector.h"
using namespace std;

/* Constants controlling the colors on the flag. */
const Color kCardinal   = Color(196,  30,  58);
const Color kSandstone  = Color(245, 242, 225);

/* The golden ratio, used to subdivide the triangles. */
const double kPhi = (1 + sqrt(5.0)) / 2;

int drawObtuseTriangle(const Point& apex,
                       const Point& base1,
                       const Point& base2,
                       int order);

/* Draws an acute triangle with the specified apex and base points and of
 * the given order, then returns the number of triangles drawn.
 */
int drawAcuteTriangle(const Point& apex,
                      const Point& base1,
                      const Point& base2,
                      int order) {
    /* An order-0 triangle is just a literal triangle. */
    if (order == 0) {
        drawTriangle(apex, base1, base2, kCardinal);
        return 1;
    }
    /* Otherwise, subdivide the triangle into an acute triangle and an obtuse
     * triangle. You're welcome to work through the math here that determines
     * the new corner points if you'd like, but you are not required to do so.
     */
    else {
        Point sideMid = apex + (base1 - apex) / kPhi;

        int trianglesDrawn = 0;
        trianglesDrawn += drawAcuteTriangle(base2, sideMid, base1, order - 1);
        trianglesDrawn += drawObtuseTriangle(sideMid, base2, apex, order - 1);
        return trianglesDrawn;
    }
}

/* Draws an obtuse triangle with the specified apex and base points and of
 * the given order, then returns the number of triangles drawn.
 */
int drawObtuseTriangle(const Point& apex,
                       const Point& base1,
                       const Point& base2,
                       int order) {
    /* An order-0 triangle is just a literal triangle. */
    if (order == 0) {
        drawTriangle(apex, base1, base2, kSandstone);
        return 1;
    }
    /* Otherwise, subdivide the triangle into an two obtuse triangles and an
     * acute triangle. You are welcome to work through the math that computes
     * the new corner points if you'd like, but you are not required to do so.
     */
    else {
        Point baseMid = base1 + (base2 - base1) / kPhi;
        Point sideMid = base1 + (apex - base1) / kPhi;

        int trianglesDrawn = 0;
        trianglesDrawn += drawObtuseTriangle(sideMid, baseMid, base1, order - 1);
        trianglesDrawn += drawObtuseTriangle(baseMid, base2, apex, order - 1);
        trianglesDrawn += drawAcuteTriangle(baseMid, sideMid, apex, order - 1);
        return trianglesDrawn;
    }
}

/* Computes the points on the boundary of regular decagon (10-sided polygon) that
 * is centered within the given rectangle.
 */
Vector<Point> placeDecagonIn(const Rectangle& bounds) {
    /* Find a square that fits within the bounds. To do this, see if we're taller
     * than wide or vice-versa and use the shorter side as the dimensions of
     * our bounding box.
     */
    Rectangle square;
    if (bounds.width >= bounds.height) {
        square.x = bounds.x + (bounds.width - bounds.height) / 2;
        square.y = bounds.y;
        square.width = square.height = bounds.height;
    } else {
        square.x = bounds.x;
        square.y = bounds.y + (bounds.height - bounds.width) / 2;
        square.width = square.height = bounds.width;
    }

    /* Points on the boundary of a regular decagon. */
    Point center = { square.x + square.width / 2, square.y + square.height / 2 };
    int radius   = square.width * 0.4;

    /* Compute the points on the border of the decagon using some trig. */
    Vector<Point> decagonPoints;
    for (int i = 0; i < 10; i++) {
        Point pt;
        pt.x = center.x - radius * cos(i * M_PI / 5 + M_PI / 10);
        pt.y = center.y + radius * sin(i * M_PI / 5 + M_PI / 10);
        decagonPoints += pt;
    }

    return decagonPoints;
}

int drawFlagOfRecursia(const Rectangle& bounds) {
    /* We will draw ten triangles around the border of a decagon (10-sided
     * regular polygon). Compute the points on the decagon, along with
     * the center point of the rectangular region.
     */
    Vector<Point> decagonPoints = placeDecagonIn(bounds);
    Point center = { bounds.x + bounds.width / 2, bounds.y + bounds.height / 2 };

    /* Track how many triangles we drew. */
    int numTriangles = 0;
    for (int i = 0; i < 10; i++) {
        /* These two points, plus the center, give the corners of the triangle. */
        Point p0 = decagonPoints[i];
        Point p1 = decagonPoints[(i + 1) % 10];

        /* Draw one recursive triangle, remembering how many triangles were drawn in
         * the process.
         */
        int triangles = drawAcuteTriangle(center, p0, p1, i);
        numTriangles += triangles;
    }

    return numTriangles;
}
