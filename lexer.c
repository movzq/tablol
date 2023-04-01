#include "lexer.h"
#include <ctype.h>

typedef bool __getUntil (const char c);
bool until_strings (const char c) { return c != '"'; }
bool until_numbers (const char c) { return isdigit(c) || c == '.'; }

void lexer_getToken (__getUntil until, struct ustr* token, const char* content, size_t* pos, enum tokenType *type)
{
    unsigned char npoints = 0;
    do {
        if ( content[*pos] == '.' )
            npoints++;

        ustr_pushBack(token, content[*pos]);
        *pos += 1;
    } while (until(content[*pos]));


    if (until == until_strings)
        ustr_erase(token, 0, 0);

    if (until == until_numbers && npoints >= 2) {
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

    parser_init();
    struct ustr* token = ustr_make();
    for (size_t i = 0; i < bytes; ++i) {
        while (isspace(content[i]))
            i++;

        // XXX: Make a cell.
        if (content[i] == '|') {
            ustr_clear(token);
            i++;
        }

        enum tokenType ttype;
        bool isstring = content[i] == '"';
        bool isposnum = isdigit(content[i]);
        bool isnegnum = content[i] == '-' && isdigit(ustr_at(token, i + 1));

        if (isstring) {
            ttype = TOKEN_TYPE_STRING;
            lexer_getToken(until_strings, token, content, &i, NULL);
        }
        else if (isposnum || isnegnum) {
            ttype = TOKEN_TYPE_NUMBER;
            lexer_getToken(until_numbers, token, content, &i, &ttype);
        }

        if (token->size) {
            printf("token: %s \t %d\n", token->data, ttype);
            ustr_clear(token);
        }
    }

    ustr_kill(token);
    fclose(table);
    free(content);
}
