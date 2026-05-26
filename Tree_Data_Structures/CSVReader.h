#pragma once

#include "vector.h"
#include "map.h"
#include <string>
#include <istream>

/* Type representing data read from a CSV file containing a header row. Access is
 * provided as csv[row][column], where column can be specified either by an integer
 * or as one of the column headers.
 */
class CSVReader {
public:
    /* Parsing routines. */
    static CSVReader parse(std::istream& source);
    static CSVReader parseFile(const std::string& filename);

    /* Basic accessors. */
    int numRows() const;   // Doesn't include header
    int numCols() const;

    /* Header information. */
    Vector<std::string> headers() const;

    /* Accessor proxy class. */
    class RowRef {
    public:
        std::string operator[] (int col) const;
        std::string operator[] (const std::string& colHeader) const;

    private:
        RowRef(const CSVReader* parent, int row);
        const CSVReader* mParent;
        int mRow;

        friend class CSVReader;
    };

    RowRef operator[] (int col) const;

private:
    /* The data. It's internally represented as a 2D grid of strings, along with
     * auxiliary column header data.
     *
     * The grid itself is represented in row-major order, and does not include the
     * column headers.
     */
    Vector<std::string> mData;
    int                 mRows;

    /* Column headers are encoded as a map from headers to indices, since the
     * primary operation we'll be supporting is mapping from a name to a column.
     */
    Map<std::string, int> mColumnHeaders;
};
