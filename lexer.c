#include "lexer.h"

/* The tokens such as strings, numbers and coordinates
 * requires more than one character (numbers can be the execption)
 * so it is necessary have conditions to get the whole token. */
typedef bool __getTil (const char);
bool _tilNumber (const char);
bool _tilString (const char);
bool _tilCoords (const char);

enum tablolTokenType _isThisType (const char, const char);
void _getWholeToken (__getTil, const char*, struct ustr*, size_t*, enum tablolTokenType*);

void lexer_read (FILE* table)
{
    fseek(table, 0, SEEK_END);
    size_t bytes = ftell(table);
    fseek(table, 0, SEEK_SET);

    /* Reading the whole file. */
    char* data = (char*) malloc(bytes);
    fread(data, bytes, 1, table);
    data[bytes - 1] = '\0';
    fclose(table);

    struct ustr* token = ustr_make();
    parser_init();

    for (size_t pos = 0; pos < bytes; pos++) {
        while (isspace(data[pos]) && data[pos] != '\n')
            pos++;

        const char thisc = data[pos];
        if (thisc == '|')
            parser_newCell();
        if (thisc == '\n')
            parser_newRow();

        enum tablolTokenType thistype = _isThisType(thisc, data[pos + 1]);

        if (thistype == TABLOL_STRING_TYPE)
            _getWholeToken(_tilString, data, token, &pos, &thistype);
        if (thistype == TABLOL_NUMBER_TYPE)
            _getWholeToken(_tilNumber, data, token, &pos, &thistype);
        if (thistype == TABLOL_COORDS_TYPE)
            _getWholeToken(_tilCoords, data, token, &pos, &thistype);

        if (token->size >= 1) {
            parser_newToken(token->data, thistype);
            ustr_clear(token);
        }
    }

    ustr_kill(token);
    free(data);
    parser_execute();
}

bool _tilNumber (const char c)
{
    return isdigit(c) || c == '.';
}

bool _tilString (const char c)
{
    return c != '"';
}

bool _tilCoords (const char c)
{
    return isupper(c) || isdigit(c);
}

enum tablolTokenType _isThisType (const char thisc, const char nextc)
{
    if (thisc == '"')
        return TABLOL_STRING_TYPE;
    if (isdigit(thisc))
        return TABLOL_NUMBER_TYPE;
    if (thisc == '-' && isdigit(nextc))
        return TABLOL_NUMBER_TYPE;
    if (thisc == '@')
        return TABLOL_COORDS_TYPE;

    switch (thisc) {
        case TABLOL_ADD_SY_TYPE: case TABLOL_SUB_SY_TYPE:
        case TABLOL_MUL_SY_TYPE: case TABLOL_DIV_SY_TYPE:
        case TABLOL_RPA_SY_TYPE: case TABLOL_LPA_SY_TYPE:
        case TABLOL_EQU_SY_TYPE: {
            char* token = (char*) malloc(2);
            snprintf(token, 2, "%c", thisc);
            parser_newToken(token, thisc);

            free(token);
            return thisc;
        }
    }

    return TABLOL_UNKNOWN_TYPE;
}

void _getWholeToken (__getTil til, const char* data, struct ustr* token, size_t* pos, enum tablolTokenType* type)
{
    unsigned short npoints = 0;
    do {
        if (data[*pos] == '.')
            npoints++;

        ustr_pushBack(token, data[*pos]);
        *pos += 1;
    } while (til(data[*pos]));

    if (til == _tilString) {
        ustr_erase(token, 0, 0);
        return;
    }
    if (til == _tilNumber && npoints >= 2) {
        ustr_clear(token);
        ustr_append(token, "number_defintion!");
        *type = TABLOL_ERROR_TYPE;
    }

    *pos -= 1;
}
