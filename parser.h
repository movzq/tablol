#ifndef _PARSER_H
#define _PARSER_H
#include "ustring/ustring.h"
#include <stdio.h>
#include <ctype.h>

enum tokenType {
    TOKEN_TYPE_ERROR   = 0,
    TOKEN_TYPE_COORDS  = 1,
    TOKEN_TYPE_STRING  = 2,
    TOKEN_TYPE_NUMBER  = 3,
    TOKEN_TYPE_UNKNOWN = 4
};

struct Token {
    char* data;
    enum tokenType type;
};

struct Cell {
    struct Token* list;
    char* data;
    enum tokenType type;
};

void parser_init ();
void parser_newCell ();
void parser_setNewRow ();

#endif
