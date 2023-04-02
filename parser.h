#ifndef _PARSER_H
#define _PARSER_H
#include "ustring/ustring.h"
#include <stdio.h>

enum tokenType {
    TOKEN_TYPE_ERROR   = 0,
    TOKEN_TYPE_UNKNOWN = 1,
    TOKEN_TYPE_STRING  = 2,
    TOKEN_TYPE_NUMBER  = 3
};

struct Token {
    char* data;
    enum tokenType type;
};

struct Cell {
    struct Token* content;
    char* data;
    size_t numtokens;
    enum tokenType type;
};

void parser_init ();
void parser_newCell (unsigned);
void parser_newToken (const char*, const enum tokenType);
void parser_parse ();

#endif
