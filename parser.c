#include "parser.h"
#include <math.h>

static struct Cell* __table;
static size_t* __newrow_at;
static size_t __numcells;
static size_t __numrows;

void _performCell (struct Cell*);
void _setCellAsError (const char*, struct Cell*);
void _cloneCell (struct Cell*);
struct Cell* _getCellByCoords (struct Cell*);
void _checkNoEmptyCell ();

void parser_init ()
{
    __table = (struct Cell*) malloc(0);
    __numcells = 0;
    __numrows = 1;

    /* Saves the number of cell of every first cell
     * in a new row. */
    __newrow_at = (size_t*) malloc(sizeof(size_t));
    __newrow_at[0] = 0;
}

void parser_newCell ()
{
    _checkNoEmptyCell();
    __table = (struct Cell*) realloc(
        __table, ++__numcells * sizeof(struct Cell)
    );

    struct Cell newcell = {
        .list = (struct Token*) malloc(0),
        .numtokens = 0,
        .data = (char*) malloc(0),
        .type = TOKEN_TYPE_UNKNOWN
    };
    __table[__numcells - 1] = newcell;
}

void parser_setNewRow ()
{
    /* If ain't cells how can rows exist? */
    if (!__numcells)
        return;

    __newrow_at = (size_t*) realloc(
        __newrow_at, ++__numrows * sizeof(size_t)
    );

    /* Since the current row has reached its end the
     * next cell on the table is gonna be the first
     * of the new row and it has a position of '__numcells'. */
    __newrow_at[__numrows - 1] = __numcells;
}

void parser_newToken (const char* data, const enum tokenType type)
{
    /* If ain't cells how can tokens exist? */
    if (!__numcells)
        return;

    struct Cell* currcell = &__table[__numcells - 1];
    struct Token newtoken = {
        .data = (char*) malloc(strlen(data)),
        .type = type
    };
    strcpy(newtoken.data, data);

    /* The first token of every cell defines what that
     * cell's gonna do, so the information of every first
     * token is gonna be the same than the cell itself until
     * the cell is performed. */
    if (currcell->type == TOKEN_TYPE_UNKNOWN) {
        currcell->data = (char*) malloc(strlen(data));
        strcpy(currcell->data, data);
        currcell->type = type;
    }

    currcell->list = (struct Token*) realloc(
        currcell->list,
        ++currcell->numtokens * sizeof(struct Token)
    );
    currcell->list[currcell->numtokens - 1] = newtoken;
}

void parser_print ()
{
    _checkNoEmptyCell();
    unsigned currow = 0;
    for (size_t i = 0; i < __numcells; i++) {
        if (__newrow_at[currow] == i) {
            putchar(10);
            currow++;
        }
        _performCell(&__table[i]);
    }

    free(__table);
    free(__newrow_at);
    putchar(10);
}

void _performCell (struct Cell* cell)
{
    if (cell->type == TOKEN_TYPE_COORDS)
        _cloneCell(cell);

    printf("%s | ", cell->data);
    for (size_t i = 0; i < cell->numtokens; i++)
        free(cell->list[i].data);
    free(cell->list);
}

void _setCellAsError (const char* msg, struct Cell* cell)
{
    cell->data = (char*) realloc(cell->data, strlen(msg));
    strcpy(cell->data, msg);
    cell->type = TOKEN_TYPE_ERROR;
}

void _cloneCell (struct Cell* cell)
{
    /* This cell make sure ain't a cycle
     * while copying operation is being
     * performed. */
    static struct Cell* calledin = NULL;
    struct Cell* clone = _getCellByCoords(cell);

    if (cell == calledin) {
        _setCellAsError("loop_dependecy!", cell);
        return;
    }

    if (!calledin)
        calledin = cell;
    if (!clone) {
        _setCellAsError("outta_range!", cell);
        return;
    }

    if (clone->type == TOKEN_TYPE_COORDS)
        _cloneCell(clone);


    cell->type = clone->type;
    cell->data = (char*) realloc(cell->data, strlen(clone->data));
    strcpy(cell->data, clone->data);
    calledin = NULL;
}

struct Cell* _getCellByCoords (struct Cell* cell)
{
    unsigned row = 0, col = 0;
    struct ustr* rowvalue = ustr_make();

    for (size_t i = 1; i < strlen(cell->data); i++) {
        const char thisc = cell->data[i];
        if (isupper(thisc)) col += thisc - 'A';
        else ustr_pushBack(rowvalue, thisc);
    }

    if (rowvalue->size)
        row += atoi(rowvalue->data);

    free(rowvalue);
    if (row >= __numrows)
        return NULL;

    unsigned pos = __newrow_at[row] + col;
    if (pos >= __numcells)
        return NULL;
    return &__table[pos];
}

void _checkNoEmptyCell ()
{
    if (__numcells && !__table[__numcells - 1].numtokens)
        _setCellAsError("empty_cell!", &__table[__numcells - 1]);
}
