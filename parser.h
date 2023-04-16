#ifndef _PARSER_H
#define _PARSER_H
#include "ustring/ustring.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

enum tablolTokenType {
    TABLOL_STRING_TYPE  = '"',
    TABLOL_NUMBER_TYPE  = '0',
    TABLOL_ADD_SY_TYPE  = '+',
    TABLOL_SUB_SY_TYPE  = '-',
    TABLOL_DIV_SY_TYPE  = '/',
    TABLOL_MUL_SY_TYPE  = '*',
    TABLOL_LPA_SY_TYPE  = '(',
    TABLOL_RPA_SY_TYPE  = ')',
    TABLOL_EQU_SY_TYPE  = '=',
    TABLOL_COORDS_TYPE  = '@',
    TABLOL_UNKNOWN_TYPE = '?',
    TABLOL_ERROR_TYPE   = '!'
};

struct tablolToken {
    char* data;
    enum tablolTokenType type;
};

struct tablolCell {
    struct tablolToken* list;
    size_t listsize;
    char* data;
    enum tablolTokenType type;
};

void parser_init ();
void parser_newCell ();
void parser_newRow ();
void parser_newToken (const char*, const enum tablolTokenType);
void parser_execute ();

#endif
