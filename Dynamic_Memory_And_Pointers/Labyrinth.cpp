#include "Labyrinth.h"
using namespace std;

/* helper function that matches character with the corresponding direction.*/
MazeCell* matchDir(MazeCell* start, char nextDir) {
    if (nextDir == 'N') {
        return start->north;
    } else if (nextDir == 'S') {
        return start->south;
    } else if (nextDir == 'E') {
        return start->east;
    } else {
        return start->west;
    }
}

/* main recursive implementation here */
bool canEscape(MazeCell* start, const string& moves,
               bool spellBook, bool wand, bool potion) {
    /* See what's inside the current maze cell and record the observation.
     */
    Item content = start->whatsHere;
    if (content == Item::SPELLBOOK) {
        spellBook = true;
    } else if (content == Item::WAND) {
        wand = true;
    } else if (content == Item::POTION) {
        potion = true;
    }

    /* Base case 1: if there is no more step left, we don't need to keep
     * proceeding anymore. Stop and check if we've found all of the items
     * needed.
     */
    if (moves == "") {
        if (spellBook && wand && potion) return true;
        return false;
    }

    /* Base case 2: there is at least one step left that we need to check.
     * If the step is not permitted in the current MazeCell, immediately
     * identify that the path is illegal and call stop.
     */
    char nextDir = moves[0];
    MazeCell* next = matchDir(start, nextDir);
    if (next == nullptr) return false;

    /* Recursive case: current step is permitted so we proceed one step forward.
     */
    return canEscape(next, moves.substr(1), spellBook, wand, potion);
}

/* wrapper function */
bool isPathToFreedom(MazeCell* start, const string& moves) {
    return canEscape(start, moves, false, false, false);
}


/* * * * * * Unit tests curated by Sunny Zhao * * * * * */
#include "GUI/SimpleTest.h"
#include "Demos/MazeGenerator.h"
















/* * * * * Unit tests curated by Sunny Zhao * * * * */

/* Utility function to free all memory allocated for a maze. */
void deleteMaze(const Grid<MazeCell*>& maze) {
    for (auto* elem: maze) {
        delete elem;
    }
    /* Good question to ponder: why don't we write 'delete maze;'
     * rather than what's shown above?
     */
}

STUDENT_TEST("Checks paths in the sample maze.") {
    auto maze = toMaze({"* *-W *",
                        "| |   |",
                        "*-* * *",
                        "  | | |",
                        "S *-*-*",
                        "|   | |",
                        "*-*-* P"});

    /* These paths are the ones in the handout. They all work. */
    EXPECT(isPathToFreedom(maze[2][2], "ESNWWNNEWSSESWWN"));
    EXPECT(isPathToFreedom(maze[2][2], "SWWNSEENWNNEWSSEES"));
    EXPECT(isPathToFreedom(maze[2][2], "WNNEWSSESWWNSEENES"));

    /* These paths don't work, since they don't pick up all items. */
    EXPECT(!isPathToFreedom(maze[2][2], "ESNW"));
    EXPECT(!isPathToFreedom(maze[2][2], "SWWN"));
    EXPECT(!isPathToFreedom(maze[2][2], "WNNE"));

    /* These paths don't work, since they aren't legal paths. */
    EXPECT(!isPathToFreedom(maze[2][2], "WW"));
    EXPECT(!isPathToFreedom(maze[2][2], "NN"));
    EXPECT(!isPathToFreedom(maze[2][2], "EE"));
    EXPECT(!isPathToFreedom(maze[2][2], "SS"));

    deleteMaze(maze);
}

STUDENT_TEST("Can't walk through walls.") {
    auto maze = toMaze({"* S *",
                        "     ",
                        "W * P",
                        "     ",
                        "* * *"});

    EXPECT(!isPathToFreedom(maze[1][1], "WNEES"));
    EXPECT(!isPathToFreedom(maze[1][1], "NWSEE"));
    EXPECT(!isPathToFreedom(maze[1][1], "ENWWS"));
    EXPECT(!isPathToFreedom(maze[1][1], "SWNNEES"));

    deleteMaze(maze);
}

STUDENT_TEST("Works when starting on an item.") {
    auto maze = toMaze({"P-S-W"});

    EXPECT(isPathToFreedom(maze[0][0], "EE"));
    EXPECT(isPathToFreedom(maze[0][1], "WEE"));
    EXPECT(isPathToFreedom(maze[0][2], "WW"));

    deleteMaze(maze);
}

/* Printer for items. */
ostream& operator<< (ostream& out, Item item) {
    if (item == Item::NOTHING) {
        return out << "Item::NOTHING";
    } else if (item == Item::WAND) {
        return out << "Item::WAND";
    } else if (item == Item::POTION) {
        return out << "Item::POTION";
    } else if (item == Item::SPELLBOOK) {
        return out << "Item::SPELLBOOK";
    } else {
        return out << "<unknown item type>";
    }
}
