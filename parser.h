#ifndef _PARSER_H
#define _PARSER_H
#include "ustring/ustring.h"
#include <stdio.h>

enum tokenType {
    TOKEN_TYPE_UNKNOWN,
    TOKEN_TYPE_ERROR,
    TOKEN_TYPE_STRING,
    TOKEN_TYPE_NUMBER
};

struct Cell {
    char* data;
    enum tokenType type;
};

void parser_init ();

#endif
