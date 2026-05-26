#include "Treemap.h"
#include "GUI/SimpleTest.h"
#include "CSVReader.h"
#include "priorityqueue.h"
using namespace std;

/* Main recursive implementation for depthsOf. */
Map<Node*, int> depthHelp(Node* root, int start) {
    Map<Node*, int> depths; // initializes an empty map.
    depths[root] = start; // look at root and store height so far.

    /* Loop over children recursively.*/
    for (Node* child: root->children) {
        depths += depthHelp(child, start + 1);
    }

    return depths;
}

Map<Node*, int> depthsOf(Node* root) {
    /* Handles edge case where there isn't a tree. */
    if (root == nullptr) return {};

    /* There is an actual tree. */
    return depthHelp(root, 0);
}

/* Main recursive implementation for partitionChildrenOf. */
Map<Node*, Rectangle> partitionHelp(const Vector<Node*>& children, const Rectangle& bounds, int total) {
    Map<Node*, Rectangle> partition; // initializes an empty map.

    /* Base case: one child takes all love. */
    if (children.size() == 1) {
        partition[children[0]] = bounds;

    /* Recursive case: two or more children split the love. */
    } else {
        int index = 0;
        double scannedSum = 0;
        Vector<Node*> scanned;

        /* Iteratively look at all children in order of descending weights. */
        while (index < children.size() - 1 && scannedSum < total / 2) {
            scanned += children[index];
            scannedSum += children[index]->weight;
            index++;
        }

        Vector<Node*> unscanned = children.subList(index);
        double unscannedSum = total - scannedSum;

        double alpha = scannedSum / total;
        Rectangles rects = splitRectangle(bounds, alpha);
        Rectangle scannedBounds = rects.one;
        Rectangle unscannedBounds = rects.two;

        /* Recursively complete further sub-splitting. */
        partition += partitionHelp(scanned, scannedBounds, scannedSum);
        partition += partitionHelp(unscanned, unscannedBounds, unscannedSum);
    }

    return partition;
}

Map<Node*, Rectangle> partitionChildrenOf(Node* node, const Rectangle& bounds) {
    /* Error case elimination. */
    if (node == nullptr || node->children.isEmpty()) {
        error("input node doesn't exist or it has no children.");
    }

    return partitionHelp(node->children, bounds, node->weight);
}

/* Recursively build the tree map of a given tree represented by its root. */
Map<Node*, Rectangle> formTreemapOf(Node* root, const Rectangle& bounds) {
    /* Error case elimination. */
    if (root == nullptr) {
        error("input node doesn't exist.");
    }

    Map<Node*, Rectangle> treeMap; // initializes an empty map.
    treeMap[root] = bounds; // root node's rectangle is always just the full rectangle itself.

    /* Base case: if the root has no children, then the final tree map is just (the tree map of
     * the single root).
     */
    if (root->children.isEmpty()) return treeMap;

    /* Recursive case: the root has children. The final tree map is thus (the tree map of the
     * single root) plus (the tree maps of all its children).
     */
    Map<Node*, Rectangle> childrenInfo = partitionChildrenOf(root, bounds);
    for (Node* child: childrenInfo) {
        treeMap += formTreemapOf(child, childrenInfo[child]);
    }

    return treeMap;
}

/* A helper function that takes in a list of unsorted children and returns the sorted (descending order) version.*/
Vector<Node*> sortChildren(const Vector<Node*>& children) {
    PriorityQueue<Node*> temp;

    for (Node* child: children) {
        temp.enqueue(child, -1 * child->weight); // -1 here makes sure that child with high weight gets to the front of the queue.
    }

    Vector<Node*> sorted;
    while (!temp.isEmpty()) {
        sorted += temp.dequeue();
    }

    return sorted;
}

Node* loadGovernmentData(istream& in, const string& name) {
    CSVReader data = CSVReader::parse(in);

    double totalWeight = 0; // Counter that keeps track of the total amount of spending by the federal government as a whole.
    Vector<Node*> allAgencies; // A list of all agencies as the children of the federal government.

    Map<string, Vector<Node*>> agencyAccounts; // Each key is the name of an agency, its associated value is all accounts owned by the agency.
    Map<string, double> agencyWeights; // Each key is the name of an agency, its associated value is the total amount of spending by all accounts owned by the agency.

    /* Look at data and extract information one row at a time to build the account nodes.*/
    for (int row = 0; row < data.CSVReader::numRows(); row++) {
        string agency = data[row]["owning_agency_name"];
        string account = data[row]["federal_account_name"];
        double weight = stringToReal(data[row]["gross_outlay_amount"]);

        /* Do not include meaningless data points (i.e. spending that is 0 or negative).*/
        if (weight > 0) {
            Node* node = new Node(account, weight); // Account node has no children.
            agencyAccounts[agency] += node;
            agencyWeights[agency] += weight;
            totalWeight += weight;
        }
    }

    /* Reports errors if no valid data is read.*/
    if (agencyAccounts.isEmpty()) {
        error("There are no accounts spending any money meaningfully in the given year.");
    }

    /* Build the agency nodes. The children(accounts) of each agency node should be stored in sorted order. */
    for (string agency: agencyAccounts) {
        Node* node = new Node(agency, agencyWeights[agency], sortChildren(agencyAccounts[agency]));
        allAgencies += node;
    }

    /* Build the federal government(root) node. The children(agencies) of it should be stored in sorted order. */
    Node* fedGov = new Node(name, totalWeight, sortChildren(allAgencies));

    return fedGov;
}


/* * * * * Provided Helper Functions * * * * */

#include "GUI/TextUtils.h"

/* Recursively frees all the nodes in the given tree. */
void freeTree(Node* root) {
    /* Edge case: Empty trees require no work. */
    if (root == nullptr) return;

    /* Postorder traversal to ensure we don't read from dead objects. */
    for (Node* child: root->children) {
        freeTree(child);
    }

    delete root;
}

/* Rectangle-splitting algorithm. This algorithm splits along the long axis, so that wide
 * rectangles get split vertically and tall rectangles get split horizontally.
 */
Rectangles splitRectangle(const Rectangle& bounds, double alpha) {
    /* Validate input. */
    if (0 > alpha|| alpha > 1) {
        error("Invalid fractional split.");
    }

    /* Begin with the original bounds. */
    Rectangles result = { bounds, bounds };

    /* Unify code paths by selecting which fields of Rectangle
     * we'll be modifying.
     */
    int Rectangle::* coordinate;
    int Rectangle::* dimension;

    /* Wider than tall? */
    if (bounds.width >= bounds.height) {
        coordinate = &Rectangle::x;
        dimension  = &Rectangle::width;
    }
    /* Taller than wide? */
    else {
        coordinate = &Rectangle::y;
        dimension  = &Rectangle::height;
    }

    /* Shrink the smaller of the two rectangles so that its width (or
     * height) is a 1 - alpha fraction of what it started with. This
     * ensures the tiebreaking of sizes makes the smaller rectangle
     * indeed smaller. Then, subtract that width off of the larger
     * rectangle so that widths still sum up appropriately.
     */

    /* We use the round() function here to avoid floating-point shenanigans. */
    result.two.*dimension =  round(result.two.*dimension * (1 - alpha));
    result.one.*dimension -= result.two.*dimension;

    /* Adjust the x (or y) coordinate of the smaller rectangle so that
     * it remains flush with the first rectangle.
     */
    result.two.*coordinate += result.one.*dimension;

    return result;
}

bool operator== (const Rectangle& one, const Rectangle& two) {
    return one.x == two.x &&
           one.y == two.y &&
           one.width == two.width &&
           one.height == two.height;
}

ostream& operator<< (ostream& out, const Rectangle& rect) {
    return out << format("{ %s, %s, %s, %s }", rect.x, rect.y, rect.width, rect.height);
}


/* * * * * Unit tests curated by Sunny Zhao * * * * */
STUDENT_TEST("depthsOf works on a tree of height 2.") {
    /* The Tree looks like this:
     *            A
     *            |
     *            B
     *            |
     *            C
     */

    Node* nodeC = new Node{ "C", 1 };
    Node* nodeB = new Node{ "B", 1, {nodeC}};
    Node* nodeA = new Node{ "A", 1, {nodeB}};

    auto map = depthsOf(nodeA);

    EXPECT_EQUAL(map[nodeA], 0);
    EXPECT_EQUAL(map[nodeB], 1);
    EXPECT_EQUAL(map[nodeC], 2);

    freeTree(nodeA);
}

STUDENT_TEST("partitionChildrenOf splits 4 equal children 25/25/25/25."){
    auto* tree = new Node { "A", 20, {
                     new Node{ "B", 5 },
                     new Node{ "C", 5 },
                     new Node{ "D", 5 },
                     new Node{ "E", 5 },
                 }
    };

    auto* nodeB = tree->children[0];
    auto* nodeC = tree->children[1];
    auto* nodeD = tree->children[2];
    auto* nodeE = tree->children[3];

    /* Eight times as wide as tall. */
    Rectangle bounds = { 10, 20, 80, 10 };
    auto partition = partitionChildrenOf(tree, bounds);

    EXPECT_EQUAL(partition.size(), 4);
    EXPECT(partition.containsKey(nodeB));
    EXPECT(partition.containsKey(nodeC));
    EXPECT(partition.containsKey(nodeD));
    EXPECT(partition.containsKey(nodeE));

    EXPECT_EQUAL(partition[nodeB], { 10, 20, 20, 10 });
    EXPECT_EQUAL(partition[nodeC], { 30, 20, 20, 10 });
    EXPECT_EQUAL(partition[nodeD], { 50, 20, 20, 10 });
    EXPECT_EQUAL(partition[nodeE], { 70, 20, 20, 10 });

    freeTree(tree);
}

STUDENT_TEST("formTreemapOf works on a depth-2 tree") {
    /* Tree looks like this:
     *            A
     *            |
     *            B
     *            |
     *            C
     *
     * Node A, B, C all encompasses the full bounds.
     */

    Node* nodeC = new Node{ "C", 1 };
    Node* nodeB = new Node{ "B", 1, {nodeC}};
    Node* nodeA = new Node{ "A", 1, {nodeB}};

    Rectangle bounds = { 10, 20, 50, 30 };
    auto treemap = formTreemapOf(nodeA, bounds);

    EXPECT_EQUAL(treemap.size(), 3);
    EXPECT_EQUAL(treemap[nodeA], bounds);
    EXPECT_EQUAL(treemap[nodeB], bounds);
    EXPECT_EQUAL(treemap[nodeC], bounds);

    freeTree(nodeA);
}

STUDENT_TEST("depthsOf works in simple cases.") {
    EXPECT_EQUAL(depthsOf(nullptr), { });

    Node* singleton = new Node{ "Just Me!", 1 };
    EXPECT_EQUAL(depthsOf(singleton), { { singleton, 0 } });
    freeTree(singleton);
}

STUDENT_TEST("depthsOf works on a tree of height 1.") {
    Node* childA = new Node{ "Child A", 3 };
    Node* childB = new Node{ "Child B", 2 };
    Node* childC = new Node{ "Child C", 1 };

    Node* tree = new Node{ "Root", 6, { childA, childB, childC } };

    auto map = depthsOf(tree);
    EXPECT_EQUAL(map.size(), 4);

    EXPECT_EQUAL(map[tree], 0);
    EXPECT_EQUAL(map[childA], 1);
    EXPECT_EQUAL(map[childB], 1);
    EXPECT_EQUAL(map[childC], 1);

    freeTree(tree);
}

STUDENT_TEST("depthsOf works on a tree of unequal depths.") {
    /* The tree looks like this:
     *
     *                 A
     *               / | \
     *              B  E  F
     *             / \   /|\
     *            C   D G H I
     *                     / \
     *                    J   K
     */
    Node* nodeC = new Node{ "C", 1 };
    Node* nodeD = new Node{ "D", 1 };
    Node* nodeB = new Node{ "B", 2, { nodeC, nodeD } };
    Node* nodeE = new Node{ "E", 1 };
    Node* nodeG = new Node{ "G", 1 };
    Node* nodeH = new Node{ "H", 1 };
    Node* nodeJ = new Node{ "J", 1 };
    Node* nodeK = new Node{ "K", 1 };
    Node* nodeI = new Node{ "I", 2, { nodeJ, nodeK } };
    Node* nodeF = new Node{ "F", 4, { nodeG, nodeH, nodeI } };
    Node* nodeA = new Node{ "A", 7, { nodeB, nodeE, nodeF } };

    auto map = depthsOf(nodeA);
    EXPECT_EQUAL(map.size(), 11);

    EXPECT_EQUAL(map[nodeA], 0);
    EXPECT_EQUAL(map[nodeB], 1);
    EXPECT_EQUAL(map[nodeE], 1);
    EXPECT_EQUAL(map[nodeF], 1);
    EXPECT_EQUAL(map[nodeC], 2);
    EXPECT_EQUAL(map[nodeD], 2);
    EXPECT_EQUAL(map[nodeG], 2);
    EXPECT_EQUAL(map[nodeH], 2);
    EXPECT_EQUAL(map[nodeI], 2);
    EXPECT_EQUAL(map[nodeJ], 3);
    EXPECT_EQUAL(map[nodeK], 3);

    freeTree(nodeA);
}



STUDENT_TEST("partitionChildrenOf splits two equal children 50/50.") {
    auto* tree = new Node { "A", 10, {
                     new Node{ "B", 5 },
                     new Node{ "C", 5 }
                 }
    };

    auto* nodeB = tree->children[0];
    auto* nodeC = tree->children[1];

    /* Twice as wide as tall. */
    Rectangle bounds = { 10, 20, 40, 20 };
    auto partition = partitionChildrenOf(tree, bounds);

    EXPECT_EQUAL(partition.size(), 2);
    EXPECT(partition.containsKey(nodeB));
    EXPECT(partition.containsKey(nodeC));

    EXPECT_EQUAL(partition[nodeB], { 10, 20, 20, 20 });
    EXPECT_EQUAL(partition[nodeC], { 30, 20, 20, 20 });

    freeTree(tree);
}

STUDENT_TEST("partitionChildrenOf splits two unequal children 75/25.") {
    auto* tree = new Node { "A", 20, {
                     new Node{ "B", 15 },
                     new Node{ "C",  5 }
                 }
    };

    auto* nodeB = tree->children[0];
    auto* nodeC = tree->children[1];

    /* Four times as wide as tall. */
    Rectangle bounds = { 10, 20, 80, 20 };
    auto partition = partitionChildrenOf(tree, bounds);

    EXPECT_EQUAL(partition.size(), 2);
    EXPECT(partition.containsKey(nodeB));
    EXPECT(partition.containsKey(nodeC));

    EXPECT_EQUAL(partition[nodeB], { 10, 20, 60, 20 });
    EXPECT_EQUAL(partition[nodeC], { 70, 20, 20, 20 });

    freeTree(tree);
}

STUDENT_TEST("partitionChildrenOf splits two equal children 50/50.") {
    auto* tree = new Node { "A", 10, {
                     new Node{ "B", 5 },
                     new Node{ "C", 5 }
                 }
    };

    auto* nodeB = tree->children[0];
    auto* nodeC = tree->children[1];

    /* Twice as wide as tall. */
    Rectangle bounds = { 10, 20, 40, 20 };
    auto partition = partitionChildrenOf(tree, bounds);

    EXPECT_EQUAL(partition.size(), 2);
    EXPECT(partition.containsKey(nodeB));
    EXPECT(partition.containsKey(nodeC));

    EXPECT_EQUAL(partition[nodeB], { 10, 20, 20, 20 });
    EXPECT_EQUAL(partition[nodeC], { 30, 20, 20, 20 });

    freeTree(tree);
}

STUDENT_TEST("partitionChildrenOf splits when half the weight exceeds the total.") {
    /* The "correct" way to do this is to first split into A/BCD, then
     * to split BCD into B/CD, then to split CD as C/D.
     */
    auto* tree = new Node { "X", 8, {
                     new Node{ "A",  4 },
                     new Node{ "B",  2 },
                     new Node{ "C",  1 },
                     new Node{ "D",  1 }
                 }
    };

    auto* nodeA = tree->children[0];
    auto* nodeB = tree->children[1];
    auto* nodeC = tree->children[2];
    auto* nodeD = tree->children[3];

    /* This rectangle is chosen so that the splits go like this:
     *
     * +-------+-------+
     * |       |       |
     * |       |   B   |
     * |       |       |
     * |   A   +---+---+
     * |       |   |   |
     * |       | C | D |
     * |       |   |   |
     * +-------+---+---+
     */
    Rectangle bounds = { 0, 0, 20, 16 };
    auto partition = partitionChildrenOf(tree, bounds);

    EXPECT_EQUAL(partition.size(), 4);
    EXPECT_EQUAL(partition[nodeA], {  0,  0, 10, 16 });
    EXPECT_EQUAL(partition[nodeB], { 10,  0, 10,  8 });
    EXPECT_EQUAL(partition[nodeC], { 10,  8,  5,  8 });
    EXPECT_EQUAL(partition[nodeD], { 15,  8,  5,  8 });

    freeTree(tree);
}

STUDENT_TEST("partitionChildrenOf works on a trickier split.") {
    /* These end up getting split apart like this:
     *
     *             ABCDEFGHI
     *              /     \
     *           ABC     DEFGHI
     *           / \      /   \
     *          AB  C   DE   FGHI
     *         /  \     /\   /   \
     *        A   B    D  E FG    HI
     *                      /\    /\
     *                     F  G  H  I
     */
    auto* tree = new Node { "X", 32, {
                     new Node{ "A",  8 },
                     new Node{ "B",  7 },
                     new Node{ "C",  4 },
                     new Node{ "D",  4 },
                     new Node{ "E",  3 },
                     new Node{ "F",  2 },
                     new Node{ "G",  2 },
                     new Node{ "H",  1 },
                     new Node{ "I",  1 },
                 }
    };

    auto* nodeA = tree->children[0];
    auto* nodeB = tree->children[1];
    auto* nodeC = tree->children[2];
    auto* nodeD = tree->children[3];
    auto* nodeE = tree->children[4];
    auto* nodeF = tree->children[5];
    auto* nodeG = tree->children[6];
    auto* nodeH = tree->children[7];
    auto* nodeI = tree->children[8];

    /* This rectangle's dimensions are chosen so that all the boundaries
     * come out nicely as whole numbers.
     */
    Rectangle bounds = { 0, 0, 384, 78 };
    auto partition = partitionChildrenOf(tree, bounds);

    Map<Node*, Rectangle> expected = {
        { nodeA, {   0,   0,  96,  78 } },
        { nodeB, {  96,   0,  84,  78 } },
        { nodeC, { 180,   0,  48,  78 } },
        { nodeD, { 228,   0,  48,  78 } },
        { nodeE, { 276,   0,  36,  78 } },
        { nodeF, { 312,   0,  36,  52 } },
        { nodeG, { 348,   0,  36,  52 } },
        { nodeH, { 312,  52,  36,  26 } },
        { nodeI, { 348,  52,  36,  26 } },
    };

    EXPECT_EQUAL(partition.size(), expected.size());
    EXPECT(partition.containsKey(nodeA));
    EXPECT(partition.containsKey(nodeB));
    EXPECT(partition.containsKey(nodeC));
    EXPECT(partition.containsKey(nodeD));
    EXPECT(partition.containsKey(nodeE));
    EXPECT(partition.containsKey(nodeF));
    EXPECT(partition.containsKey(nodeG));
    EXPECT(partition.containsKey(nodeH));
    EXPECT(partition.containsKey(nodeI));

    EXPECT_EQUAL(partition[nodeA], expected[nodeA]);
    EXPECT_EQUAL(partition[nodeB], expected[nodeB]);
    EXPECT_EQUAL(partition[nodeC], expected[nodeC]);
    EXPECT_EQUAL(partition[nodeD], expected[nodeD]);
    EXPECT_EQUAL(partition[nodeE], expected[nodeE]);
    EXPECT_EQUAL(partition[nodeF], expected[nodeF]);
    EXPECT_EQUAL(partition[nodeG], expected[nodeG]);
    EXPECT_EQUAL(partition[nodeH], expected[nodeH]);
    EXPECT_EQUAL(partition[nodeI], expected[nodeI]);

    freeTree(tree);
}

STUDENT_TEST("formTreemapOf works on a single node.") {
    Node* tree = new Node{ "Just Me", 1 };

    Rectangle bounds = { 0, 0, 2, 3 };
    auto treemap = formTreemapOf(tree, bounds);

    EXPECT_EQUAL(treemap.size(), 1);
    EXPECT_EQUAL(treemap[tree], bounds);

    freeTree(tree);
}

STUDENT_TEST("formTreemapOf works on a depth-1 tree.") {
    /* Tree looks like this:
     *
     *          A
     *        / | \
     *       B  C  D
     *       3  2  1
     *
     * The correct split should look like this:
     *
     * +---------+---------+
     * |         |         |
     * |         |    C    |
     * |    B    |         |
     * |         +---------+
     * |         |    D    |
     * +---------+---------+
     *
     * Node A then encompasses the full bounds.
     */
    Node* nodeB = new Node{ "B", 3 };
    Node* nodeC = new Node{ "C", 2 };
    Node* nodeD = new Node{ "D", 1 };
    Node* nodeA = new Node{ "A", 6, { nodeB, nodeC, nodeD } };

    Rectangle bounds = { 10, 20, 50, 30 };
    auto treemap = formTreemapOf(nodeA, bounds);

    EXPECT_EQUAL(treemap.size(), 4);
    EXPECT_EQUAL(treemap[nodeA], bounds);
    EXPECT_EQUAL(treemap[nodeB], { 10, 20, 25, 30 });
    EXPECT_EQUAL(treemap[nodeC], { 35, 20, 25, 20 });
    EXPECT_EQUAL(treemap[nodeD], { 35, 40, 25, 10 });

    freeTree(nodeA);
}

STUDENT_TEST("formTreemapOf works on a tree of uneven depths") {
    /* The tree looks like this:
     *
     *           A 8
     *          / \
     *       4 B   C 4
     *            / \
     *         2 D   E 2
     *          / \
     *         F   G
     *         1   1
     *
     * The treemap should split the original rectangle in half 50/50, then
     * recursively keep splitting one part of the rectangle 50/50 from there.
     */
    auto* nodeB = new Node{ "B", 4 };
    auto* nodeE = new Node{ "E", 2 };
    auto* nodeF = new Node{ "F", 1 };
    auto* nodeG = new Node{ "G", 1 };
    auto* nodeD = new Node{ "D", 2, { nodeF, nodeG } };
    auto* nodeC = new Node{ "C", 4, { nodeD, nodeE } };
    auto* nodeA = new Node{ "A", 8, { nodeB, nodeC } };

    Rectangle bounds = { 100, 200, 8, 6 };
    auto treemap = formTreemapOf(nodeA, bounds);
    EXPECT_EQUAL(treemap.size(), 7);

    /* Node A gets the full bounds. */
    EXPECT_EQUAL(treemap[nodeA], bounds);

    /* Node B gets the left half after a vertical split. */
    EXPECT_EQUAL(treemap[nodeB], { 100, 200, 4, 6 });

    /* Node C gets the right half after a vertical split. */
    EXPECT_EQUAL(treemap[nodeC], { 104, 200, 4, 6 });

    /* Node D gets the top half after a horizontal split. */
    EXPECT_EQUAL(treemap[nodeD], { 104, 200, 4, 3 });

    /* Node E gets the bottom half after a horizontal split. */
    EXPECT_EQUAL(treemap[nodeE], { 104, 203, 4, 3 });

    /* Node F gets the left half after a vertical split. */
    EXPECT_EQUAL(treemap[nodeF], { 104, 200, 2, 3 });

    /* Node G gets the right half after a vertical split. */
    EXPECT_EQUAL(treemap[nodeG], { 106, 200, 2, 3 });

    freeTree(nodeA);
}

#include <sstream>
#include <algorithm>

namespace {
    struct DataRow {
        string agency, account;
        double amount;
    };

    /* Utility function to let us call loadGovernmentData on a list of rows. */
    Node* loadGovernmentData(const Vector<DataRow>& rows, const string& name) {
        stringstream data;

        /* Write the header. */
        data << "owning_agency_name,federal_account_name,gross_outlay_amount" << endl;

        /* Write rows. */
        for (auto row: rows) {
            data << quotedVersionOf(row.agency)  << ","
                 << quotedVersionOf(row.account) << ","
                 << fixed << setprecision(2) << row.amount << endl;
        }

        return ::loadGovernmentData(data, name);
    }

    /* Utility function that compares two trees for equality. */
    bool areEqual(Node* one, Node* two) {
        if (one == nullptr || two == nullptr) return one == two;

        if (one->name != two->name) {
            return false;
        }

        if (!SimpleTest::Internal::areEqual(one->weight, two->weight)) {
            return false;
        }

        if (one->children.size() != two->children.size()) {
            return false;
        }

        for (int i = 0; i < one->children.size(); i++) {
            if (!areEqual(one->children[i], two->children[i])) {
                return false;
            }
        }

        return true;
    }
}

STUDENT_TEST("loadGovernmentData computes corresponding weights correctly.") {
    auto* result = loadGovernmentData({ { "A", "w", 4 },
                                       { "A", "x", 3 },
                                       { "B", "y", 2 },
                                       { "B", "z", 1 } }, "Test Data");

    auto* expected = new Node{
        "Test Data", 10, {
            new Node{
                "A", 7, {
                 new Node{ "w", 4 },
                 new Node{ "x", 3 },
                }
            },

            new Node{
                "B", 3, {
                 new Node{ "y", 2 },
                 new Node{ "z", 1 },
                }
            }
        }
    };

    EXPECT(areEqual(result, expected));
    freeTree(result);
    freeTree(expected);
}

STUDENT_TEST("loadGovernmentData reads a single account.") {
    auto* result   = loadGovernmentData({ { "A", "w", 1 } }, "Test Data");
    auto* expected = new Node{
        "Test Data", 1, {
            new Node{
                "A", 1, {
                    new Node{ "w", 1 }
                }
            }
        }
    };

    EXPECT(areEqual(result, expected));
    freeTree(result);
    freeTree(expected);
}

STUDENT_TEST("loadGovernmentData reads multiple accounts in one agency.") {
    auto* result = loadGovernmentData({ { "A", "w", 4 },
                                       { "A", "x", 3 },
                                       { "A", "y", 2 },
                                       { "A", "z", 1 } }, "Test Data");

    auto* expected = new Node{
        "Test Data", 10, {
            new Node{
                "A", 10, {
                 new Node{ "w", 4 },
                 new Node{ "x", 3 },
                 new Node{ "y", 2 },
                 new Node{ "z", 1 },
                 }
            }
        }
    };

    EXPECT(areEqual(result, expected));
    freeTree(result);
    freeTree(expected);
}

STUDENT_TEST("loadGovernmentData reads multiple accounts in multiple agencies.") {
    auto* result = loadGovernmentData({ { "A", "w", 4 },
                                       { "B", "w", 5 },
                                       { "A", "x", 3 },
                                       { "A", "y", 2 },
                                       { "B", "x", 2 },
                                       { "A", "z", 1 } }, "Test Data");

    auto* expected = new Node{
        "Test Data", 17, {
            new Node{
                "A", 10, {
                 new Node{ "w", 4 },
                 new Node{ "x", 3 },
                 new Node{ "y", 2 },
                 new Node{ "z", 1 },
                 }
            }, new Node {
                "B", 7, {
                 new Node{ "w", 5 },
                 new Node{ "x", 2 },
                 }
            }
        }
    };

    EXPECT(areEqual(result, expected));
    freeTree(result);
    freeTree(expected);
}

STUDENT_TEST("loadGovernmentData discards accounts with zero or negative weight.") {
    auto* result = loadGovernmentData({ { "A", "w", 4 },
                                       { "B", "w", 5 },
                                       { "A", "x", 3 },
                                       { "A", "a", 0 },
                                       { "A", "y", 2 },
                                       { "B", "x", 2 },
                                       { "B", "y", -1 },
                                       { "C", "a", 0 },
                                       { "C", "b", -1 },
                                       { "A", "z", 1 } }, "Test Data");

    auto* expected = new Node{
        "Test Data", 17, {
            new Node{
                "A", 10, {
                 new Node{ "w", 4 },
                 new Node{ "x", 3 },
                 new Node{ "y", 2 },
                 new Node{ "z", 1 },
                 }
            }, new Node {
                "B", 7, {
                 new Node{ "w", 5 },
                 new Node{ "x", 2 },
                 }
            }
        }
    };

    EXPECT(areEqual(result, expected));
    freeTree(result);
    freeTree(expected);
}

STUDENT_TEST("loadGovernmentData sorts entries by weight.") {
    auto* result = loadGovernmentData({
                                       { "B", "x", 2 },
                                       { "A", "x", 3 },
                                       { "A", "a", 0 },
                                       { "A", "y", 2 },
                                       { "B", "y", -1 },
                                       { "C", "a", 0 },
                                       { "C", "b", -1 },
                                       { "A", "w", 4 },
                                       { "B", "w", 5 },
                                       { "A", "z", 1 } }, "Test Data");

    auto* expected = new Node{
        "Test Data", 17, {
            new Node{
                "A", 10, {
                 new Node{ "w", 4 },
                 new Node{ "x", 3 },
                 new Node{ "y", 2 },
                 new Node{ "z", 1 },
                 }
            }, new Node {
                "B", 7, {
                 new Node{ "w", 5 },
                 new Node{ "x", 2 },
                 }
            }
        }
    };

    EXPECT(areEqual(result, expected));
    freeTree(result);
    freeTree(expected);
}

STUDENT_TEST("loadGovernmentData reports errors if no valid data is read.") {
    Vector<DataRow> rows = {
        { "B", "x", -2 },
        { "A", "x", -3 },
        { "A", "a", 0 },
        { "A", "y", -2 },
        { "B", "y", -1 },
        { "C", "a", 0 },
        { "C", "b", -1 },
        { "A", "w", -4 },
        { "B", "w", -5 },
        { "A", "z", -1 }
    };

    EXPECT_ERROR(loadGovernmentData(rows, "Test Data"));
}

#include <fstream>
#include <cmath>
STUDENT_TEST("loadGovernmentData passes basic checks on the real data file.") {
    ifstream input("res/FederalSpending2023.csv", ios::binary);
    if (!input) SHOW_ERROR("Internal error: Can't open federal spending file.");

    Node* data = loadGovernmentData(input, "Federal Spending 2023");
    EXPECT_NOT_EQUAL(data, nullptr);

    /* Confirm name matches. */
    EXPECT_EQUAL(data->name, "Federal Spending 2023");

    /* Confirm total weight. We round to the nearest integer here
     * to avoid rounding errors arising from inaccuracies resulting
     * from the order in which doubles are added - take CS107
     * for details!
     */
    EXPECT_EQUAL(round(data->weight), round(8904247541021.72));

    /* Confirm right number of children. */
    EXPECT_EQUAL(data->children.size(), 106);

    /* Spot-check the children. */
    EXPECT_NOT_EQUAL(data->children[0], nullptr);
    EXPECT_EQUAL(data->children[0]->name, "Department of Health and Human Services");
    EXPECT_EQUAL(round(data->children[0]->weight), round(2423434619295.49));

    EXPECT_NOT_EQUAL(data->children[39], nullptr);
    EXPECT_EQUAL(data->children[39]->name, "The Judicial Branch");
    EXPECT_EQUAL(round(data->children[39]->weight), round(613614690.93));

    EXPECT_NOT_EQUAL(data->children[46], nullptr);
    EXPECT_EQUAL(data->children[46]->name, "District of Columbia Courts");
    EXPECT_EQUAL(round(data->children[46]->weight), round(311707875.20));
    EXPECT_EQUAL(data->children[46]->children.size(), 3);
    EXPECT_NOT_EQUAL(data->children[46]->children[0], nullptr);
    EXPECT_EQUAL(data->children[46]->children[0]->name, "Salaries and Expenses, Federal Payment to the District of Columbia Courts");
    EXPECT_EQUAL(round(data->children[46]->children[0]->weight), round(270679505.30));

    freeTree(data);
}

STUDENT_TEST("loadGovernmentData stress test.") {
    Vector<DataRow> rows;

    const int kSize = 250; // Number of agencies and accounts per agency
    Node* reference = new Node{ "Stress Test", 0.0 };

    for (int agencyID = 0; agencyID < kSize; agencyID++) {
        Node* agency = new Node{ to_string(agencyID), 0.0 };
        for (int account = 0; account < kSize; account++) {
            /* Ensures all weights are in reverse-sorted order. Add one to each
             * to ensure that we don't have zero weights.
             */
            double weight = agencyID * kSize + account + 1;
            rows.add({ to_string(agencyID), to_string(account), weight });

            reference->weight += weight;
            agency->weight    += weight;
            agency->children  += new Node{ to_string(account), weight };
        }

        /* Added everything in reverse, so flip things around. */
        reverse(agency->children.begin(), agency->children.end());
        reference->children += agency;
    }

    /* Our reference tree is reversed, so flip everything around. */
    reverse(reference->children.begin(), reference->children.end());

    /* Run loadGovernmentData on our rows. */
    auto* result = loadGovernmentData(rows, "Stress Test");
    EXPECT(areEqual(result, reference));

    freeTree(result);
    freeTree(reference);
}
