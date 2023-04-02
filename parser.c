#include "parser.h"

static struct Cell* __cells;
static unsigned* __bgnsrows;
static size_t __numbercells;

void _doCell (struct Cell*);

void parser_init ()
{
    __cells = (struct Cell*) malloc(0);
    __bgnsrows = (unsigned*) malloc(0);
    __numbercells = 0;
}

void parser_newCell (unsigned rowidx)
{
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

void parser_newToken (const char* data, const enum tokenType type)
{
    if ( !__numbercells ) {
        fprintf(stderr, "Trying to push a new token, however any cell has been created.\n");
        fprintf(stderr, "Token: '%s'.\n", data);
        exit(EXIT_FAILURE);
    }

    struct Cell* currcell = &__cells[__numbercells - 1];
    if ( !currcell->numtokens )
        currcell->type = type;

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
    for (unsigned i = 0; i < __numbercells; i++) {
        _doCell(&__cells[i]);
        free(__cells[i].data);
    }

    free(__cells);
    free(__bgnsrows);
}

void _doCell (struct Cell* cell)
{
    const enum tokenType ttype = cell->type;
    if ( ttype == TOKEN_TYPE_STRING || ttype == TOKEN_TYPE_NUMBER || ttype == TOKEN_TYPE_ERROR ) {
        char* tokendata = cell->content[0].data;
        cell->data = (char*) realloc(cell->data, strlen(tokendata));
        strcpy(cell->data, tokendata);
    }

    printf("%s |", cell->data);
    for (unsigned i = 0; i < cell->numtokens; i++)
        free(cell->content[i].data);
    free(cell->content);
}
