#include "parser.h"
#include "yard-algo/yard-algo.h"
#include <string.h>

static struct tablolCell* __table;
static size_t* __newrow_made_at;
static size_t __numbercells;
static size_t __numberrows;

void _chechNoEmptyCell ();
void _setErrorCell (struct tablolCell*, const char*);
void _performCell (struct tablolCell*);
void _cloneCell (struct tablolCell*);
void* _getCellByCoords (struct tablolCell*, const char*, bool);
void _doArithmetic (struct tablolCell*);

void parser_init ()
{
    __table = (struct tablolCell*) malloc(0);
    __numbercells = 0;
    __numberrows = 1;

    __newrow_made_at = (size_t*) malloc(sizeof(size_t));
    __newrow_made_at[0] = 0;
}

void parser_newCell ()
{
    _chechNoEmptyCell();
    __table = (struct tablolCell*) realloc(
        __table,
        ++__numbercells * sizeof(struct tablolCell)
    );

    struct tablolCell newcell = {
        .list = (struct tablolToken*) malloc(0),
        .listsize = 0,
        .data = (char*) malloc(0),
        .type = TABLOL_UNKNOWN_TYPE
    };
    __table[__numbercells - 1] = newcell;
}

void parser_newRow ()
{
    if (!__numbercells)
        return;

    __newrow_made_at = (size_t*) realloc(
        __newrow_made_at,
        ++__numberrows * sizeof(size_t)
    );
    __newrow_made_at[__numberrows - 1] = __numbercells;
}

void parser_newToken (const char* data, const enum tablolTokenType type)
{
    if (!__numbercells)
        return;

    struct tablolCell* currcell = &__table[__numbercells - 1];
    struct tablolToken newtoken = {
        .data = (char*) malloc(strlen(data) + 1),
        .type = type
    };
    strcpy(newtoken.data, data);

    /* The first token of every cell defines what that cell is gonna
     * perform, even if the token is an error. */
    if (currcell->type == TABLOL_UNKNOWN_TYPE) {
        currcell->data = (char*) realloc(currcell->data, strlen(data) + 1);
        strcpy(currcell->data, data);

        currcell->type = type;
    }

    currcell->list = (struct tablolToken*) realloc(
        currcell->list,
        ++currcell->listsize * sizeof(struct tablolToken)
    );
    currcell->list[currcell->listsize - 1] = newtoken;
}

void parser_execute ()
{
    _chechNoEmptyCell();
    size_t currcellidx = 1;

    for (size_t pos = 0; pos < __numbercells; pos++) {
        if (__newrow_made_at[currcellidx] == pos) {
            putchar(10);
            currcellidx++;
        }
        _performCell(&__table[pos]);
    }

    free(__table);
    free(__newrow_made_at);
    putchar(10);
}

void _chechNoEmptyCell ()
{
    if (!__numbercells)
        return;
    if (!__table[__numbercells - 1].listsize)
        _setErrorCell(&__table[__numbercells - 1], "empty_cell!");
}

void _setErrorCell (struct tablolCell* cell, const char* msg)
{
    cell->type = TABLOL_ERROR_TYPE;
    cell->data = (char*) realloc(cell->data, strlen(msg) + 1);
    strcpy(cell->data, msg);
}

void _performCell (struct tablolCell* cell)
{
    if (cell->type == TABLOL_COORDS_TYPE)
        _cloneCell(cell);
    if (cell->type == TABLOL_EQU_SY_TYPE)
        _doArithmetic(cell);

    printf("%s | ", cell->data);
    for (size_t i = 0; i < cell->listsize; i++)
        free(cell->list[i].data);
    free(cell->list);
}

void _cloneCell (struct tablolCell* cell)
{
    static struct tablolCell* beginsat = NULL;
    struct tablolCell* copy = _getCellByCoords(cell, cell->data, false);

    if (cell == beginsat) {
        _setErrorCell(cell, "loop_dependecy!");
        return;
    }

    if (!beginsat)
        beginsat = cell;
    if (copy->type == TABLOL_ERROR_TYPE)
        return;

    if (copy->type == TABLOL_COORDS_TYPE)
        _cloneCell(copy);

    cell->type = copy->type;
    cell->data = (char*) realloc(cell->data, strlen(copy->data) + 1);
    strcpy(cell->data, copy->data);
    beginsat = NULL;
}

void* _getCellByCoords (struct tablolCell* cell, const char* coord, bool onlydata)
{
    unsigned row = 0, col = 0;
    struct ustr* rowis = ustr_make();

    for (size_t i = 1; i < strlen(coord); i++) {
        const char thisc = coord[i];
        if (isdigit(thisc))
            ustr_pushBack(rowis, thisc);
        else
            col += thisc - 'A';
    }

    if (rowis->size)
        row = atoi(rowis->data);
    ustr_kill(rowis);

    unsigned position = __newrow_made_at[row] + col;
    if (position >= __numbercells || row >= __numberrows) {
        _setErrorCell(cell, "outta_range!");
        return NULL;
    }

    if (onlydata)
        return __table[position].data;
    return &__table[position];
}

void _doArithmetic (struct tablolCell* cell)
{
    if (cell->listsize <= 3) {
        _setErrorCell(cell, "missing_args!");
        return;
    }

    struct yardAlgo* yard = yard_make();
    for (size_t i = 1; i < cell->listsize; ++i) {
        if (cell->list[i].type == TABLOL_COORDS_TYPE) {
            char* number = (char*) _getCellByCoords(cell, cell->list[i].data, true);
            if (cell->type == TABLOL_ERROR_TYPE)
                return;
            yard_evaluateToken (yard, number);
        }
        else
            yard_evaluateToken (yard, cell->list[i].data);
    }

    cell->data = (char*) realloc(cell->data, 10);
    snprintf(cell->data, 10, "%f", yard_solve(yard));
    cell->type = TABLOL_NUMBER_TYPE;
}
