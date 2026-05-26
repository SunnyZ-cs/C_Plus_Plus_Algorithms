/* File: Ball.h
 * Author:  (), with updates
 *         and revisions by  ().
 *
 * Type representing a ball that bounces within a set of rectangular
 * bounds.
 */
#pragma once
#include "Demos/Rectangle.h"
#include "GUI/GUIUtils.h"
#include "GUI/Font.h"
#include "gwindow.h"
#include "gfont.h"
#include <string>

class Ball {
public:
    /* Creates a new ball with the specified ID number that bounces
     * inside of the specified rectangle.
     */
    Ball(int ballID, const Rectangle& bounceBounds);

    /* Draws the ball on the screen. */
    void draw() const;

    /* Moves the ball one step. */
    void move();

private:
    /* Ball x and y coordinate. */
    int x, y;

    /* Ball velocity: x and y are updated by adding vx and vy each time
     * move() is called.
     */
    int vx;
    int vy;

    /* Which number displays on this ball. */
    int id;

    /* Rectangular area the ball must stay inside while bouncing. */
    Rectangle bounceArea;
};

/* Graphics helper functions. */
void drawOval(int x, int y, int width, int height);
void drawCenteredText(double x, double y, double width, double height, const std::string& text);
void clearWindow();
void updateWindow();

/* Creates a bunch of balls and bounces them around. */
void bounceBalls(int numBalls, int numSteps, const Rectangle& bounceBounds);
