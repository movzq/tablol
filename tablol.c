#include "lexer.h"

void usage ()
{
    puts("u: This program needs a table file to parse.");
    exit(0);
}

int main (int argc, char** argv)
{
    FILE* table = fopen(argv[1], "r");
    if (!table)
        usage();
    lexer_read(table);
    return 0;
}
