#include "parser.h"

static struct Cell* __cells;
static unsigned* __bgnsrows;

void parser_init ()
{
    __cells = (struct Cell*) malloc(0);
    __bgnsrows = (unsigned*) malloc(4);

    /* The first row position starts when zero
     * cells has been created. */
    __bgnsrows[0] = 0;
}
