#include "lexer.h"
#include <ctype.h>

typedef bool __getUntil (const char);
bool _untilStrs (const char c) { return c != '"'; }
bool _untilNums (const char c) { return isdigit(c) || c == '.'; }

void lexer_getToken (__getUntil until, struct ustr* token, const char* content, size_t* pos, enum tokenType *type)
{
    unsigned char npoints = 0;
    do {
        if ( content[*pos] == '.' )
            npoints++;

        ustr_pushBack(token, content[*pos]);
        *pos += 1;
    } while (until(content[*pos]));


    if (until == _untilStrs)
        ustr_erase(token, 0, 0);

    if (until == _untilNums && npoints >= 2) {
        ustr_clear(token);
        ustr_append(token, "num_def!");
        *type = TOKEN_TYPE_ERROR;
    }
}

void lexer_read (FILE* table)
{
    fseek(table, 0, SEEK_END);
    size_t bytes = ftell(table);
    fseek(table, 0, SEEK_SET);

    char* content = (char*) malloc(bytes + 1);
    fread(content, 1, bytes, table);
    content[bytes] = '\0';

    struct ustr* token = ustr_make();
    unsigned cellpos = 0;
    parser_init();

    for (size_t i = 0; i < bytes; ++i) {
        while (isspace(content[i]))
            i++;

        if (content[i] == '|') {
            parser_newCell(cellpos++);
            ustr_clear(token);
            i++;
        }

        enum tokenType ttype;
        bool isstring = content[i] == '"';
        bool isposnum = isdigit(content[i]);
        bool isnegnum = content[i] == '-' && isdigit(ustr_at(token, i + 1));

        if (isstring) {
            ttype = TOKEN_TYPE_STRING;
            lexer_getToken(_untilStrs, token, content, &i, NULL);
        }
        else if (isposnum || isnegnum) {
            ttype = TOKEN_TYPE_NUMBER;
            lexer_getToken(_untilNums, token, content, &i, &ttype);
        }

        if (token->size) {
            parser_newToken(token->data, ttype);
            ustr_clear(token);
        }
    }

    fclose(table);
    ustr_kill(token);
    free(content);
    parser_parse();
}
