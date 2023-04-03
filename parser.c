#include "parser.h"

static struct Cell* __cells;
static unsigned* __pos1cols;
static size_t __numbercells;

void _doCell (struct Cell*);
void _setError (struct Cell*, const char*);

void parser_init ()
{
    __cells = (struct Cell*) malloc(0);
    __pos1cols = (unsigned*) malloc(0);
    __numbercells = 0;
}

void parser_newCell (unsigned numcells)
{
    /* When a cell is empty it is gonna be set
     * as an error cell. */
    if (!__cells[__numbercells - 1].numtokens && __numbercells) {
        _setError(&__cells[__numbercells - 1], "0_token!");
    }
    
    __cells = (struct Cell*) realloc(
        __cells, ++__numbercells * sizeof(struct Cell)
    );

    struct Cell newc = {0};
    newc.content = (struct Token*) malloc(0);
    newc.data = (char*) malloc(0);
    newc.numtokens = 0;
    newc.type = TOKEN_TYPE_UNKNOWN;
    __cells[__numbercells - 1] = newc;
}

void parser_newRow ()
{
    /* If there's empty lines at the beginnig of the file
     * that would be interpreted as a new row, but it ain't,
     * so just skip them. */
    if (!__numbercells)
        return;
    
    static unsigned numrows = 0;
    __pos1cols = (unsigned*) realloc(
        __pos1cols, ++numrows * sizeof(unsigned)
    );
    __pos1cols[numrows - 1] = (unsigned) __numbercells;
}

void parser_newToken (const char* data, const enum tokenType type)
{
    if (!__numbercells) {
        fprintf(stderr, "Trying to push a new token, however any cell has been created.\n");
        fprintf(stderr, "Token: '%s'.\n", data);
        exit(EXIT_FAILURE);
    }

    struct Cell* currcell = &__cells[__numbercells - 1];
    /* The cell is defined as its first token since that
     * token defines what the cell is gonna perform. */
    if (!currcell->numtokens) {
        currcell->data = (char*) realloc(currcell->data, strlen(data));
        strcpy(currcell->data, data);
        currcell->type = type;
    }

    struct Token newt = {0};
    newt.data = (char*) malloc(strlen(data));
    newt.type = type;
    strcpy(newt.data, data);

    size_t ntokens = currcell->numtokens;
    currcell->content = (struct Token*) realloc(currcell->content, ++ntokens * sizeof(struct Token));
    currcell->content[ntokens - 1] = newt;
    currcell->numtokens = ntokens;
}

void parser_parse ()
{    
    unsigned currow = 0;
    for (size_t i = 0; i < __numbercells; i++) {
        if (__pos1cols[currow] == i) {
            putchar(10);
            currow++;
        }
        _doCell(&__cells[i]);
    }
    free(__cells);
    free(__pos1cols);
    putchar(10);
}

void _doCell (struct Cell* cell)
{
    printf("(%s) | ", cell->data);
    for (unsigned i = 0; i < cell->numtokens; i++)
        free(cell->content[i].data);
    free(cell->content);
}

void _setError (struct Cell* cell, const char* errmsg)
{
    cell->data = (char*) realloc(cell->data, strlen(errmsg));
    strcpy(cell->data, errmsg);
    cell->type = TOKEN_TYPE_ERROR;
}
