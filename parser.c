#include "parser.h"

static struct Cell* __table;
static size_t __numcells;
static size_t* __newrow_at;

void parser_init ()
{
    __table = (struct Cell*) malloc(0);
    __numcells = 0;

    /* Saves the number of cell of every first cell
     * in a new row. */
    __newrow_at = (size_t*) malloc(sizeof(size_t));
    __newrow_at[0] = 0;
}

void parser_newCell ()
{
    // XXX: Set as null empty cells.
    puts("NEW CELL");
    __table = (struct Cell*) realloc(
        __table, ++__numcells * sizeof(struct Cell)
    );

    struct Cell newcell = {
        .list = (struct Token*) malloc(0),
        .data = (char*) malloc(0),
        .type = TOKEN_TYPE_UNKNOWN
    };
    __table[__numcells - 1] = newcell;
}

void parser_setNewRow ()
{
    /* If ain't cells how can exist rows? */
    if (!__numcells)
        return;

    puts("NEW ROW");
    static unsigned numrows = 0;
    __newrow_at = (size_t*) realloc(
        __newrow_at, ++numrows * sizeof(size_t)
    );

    /* Since the current row has reached its end the
     * next cell on the table is gonna be the first
     * of the new row. */
    __newrow_at[numrows - 1] = __numcells;
}
