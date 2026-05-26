#pragma once

#include "Demos/2DTypes.h"
#include "vector.h"

Vector<Point> makeMountainRange(const Point& left,
                                const Point& right,
                                int amplitude,
                                double decayRate);
