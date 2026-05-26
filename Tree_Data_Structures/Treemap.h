#pragma once

#include "GUI/MemoryDiagnostics.h"
#include "vector.h"
#include "map.h"
#include <string>
#include <istream>
#include <ostream>

/* Type representing a node in a tree. Each node has an associated
 * weight, which must be positive, and a list of children. The
 * weight of each node is the sum of its childrens' weights, and
 * children are stored sorted in descending order by weight.
 */
struct Node {
    std::string name;
    double      weight;     // Must be positive

    Vector<Node*> children; // Sorted by weight

    /* These constructors make it easier to set up a node. */
    Node() = default;
    Node(const std::string& name, double weight, const Vector<Node*>& children = {})  :
        name(name), weight(weight), children(children) {
    }

    /* Help find memory leaks. */
    TRACK_ALLOCATIONS_OF(Node);
};

/* Type representing a rectangle in 2D space. The rectangle is defined by
 * the (x, y) coordinate of its upper-left corner, plus a width and height.
 * Rectangles are such that the top and left sides of the rectangle are
 * included within the rectangle, while the bottom and right sides are
 * not.
 */
struct Rectangle {
    int x = 0, y = 0, width = 0, height = 0;
};

/* Helper functions for working with rectangles in SimpleTest. These functions
 * allow us to compare rectangles for equality and to print a rectangle to
 * cout.
 */
bool operator== (const Rectangle& one, const Rectangle& two);
std::ostream& operator<< (std::ostream& out, const Rectangle& rect);

/* Type representing a pair of rectangles. */
struct Rectangles {
    Rectangle one, two;
};

/* Provided helper functions. */
void freeTree(Node* root);
Rectangles splitRectangle(const Rectangle& input, double alpha);

/* You need to implement these functions. */
Map<Node*, int> depthsOf(Node* root);
Map<Node*, Rectangle> partitionChildrenOf(Node* node, const Rectangle& bounds);
Map<Node*, Rectangle> formTreemapOf(Node* root, const Rectangle& bounds);
Node* loadGovernmentData(std::istream& in, const std::string& name);
