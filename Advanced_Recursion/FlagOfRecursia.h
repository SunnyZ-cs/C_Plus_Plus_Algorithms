#pragma once

#include "Demos/2DTypes.h"
#include "GUI/Color.h"
using MiniGUI::Color;

/* Draws a single triangle given the positions of its corners and a color. */
void drawTriangle(const Point& p0, const Point& p1, const Point& p2, Color c);

/* Draws the Flag of Recursia within the rectangle specified by bounds. */
int drawFlagOfRecursia(const Rectangle& bounds);
