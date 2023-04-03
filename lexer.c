#include "lexer.h"
#include <ctype.h>

/* Functions to get the token depending on an specific
 * condition. */
typedef bool __getUntil (const char);
bool _untilStrs (const char c) { return c != '"'; }
bool _untilNums (const char c) { return isdigit(c) || c == '.'; }
void _getToken (__getUntil, struct ustr*, const char*, size_t*, enum tokenType *);

void lexer_read (FILE* table)
{
    fseek(table, 0, SEEK_END);
    size_t bytes = ftell(table);
    fseek(table, 0, SEEK_SET);

    char* content = (char*) malloc(bytes + 1);
    fread(content, 1, bytes, table);
    content[bytes] = '\0';

    struct ustr* token = ustr_make();
    parser_init();

    for (size_t i = 0; i < bytes; ++i) {
        while (isspace(content[i]) && content[i] != '\n')
            i++;

        /* New lines marks when a new row is gonna be created and every '|' character
         * defines a new cell creation. */
        if (content[i] == '\n') parser_newRow();
        if (content[i] == '|') parser_newCell();

        enum tokenType ttype;
        bool isstring = content[i] == '"';
        bool isposnum = isdigit(content[i]);
        bool isnegnum = content[i] == '-' && isdigit(ustr_at(token, i + 1));

        if (isstring) {
            ttype = TOKEN_TYPE_STRING;
            _getToken(_untilStrs, token, content, &i, NULL);
        }
        else if (isposnum || isnegnum) {
            ttype = TOKEN_TYPE_NUMBER;
            _getToken(_untilNums, token, content, &i, &ttype);
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

void _getToken (__getUntil until, struct ustr* token, const char* content, size_t* pos, enum tokenType *type)
{
    unsigned char npoints = 0;
    do {
        if ( content[*pos] == '.' )
            npoints++;
        ustr_pushBack(token, content[*pos]);
        *pos += 1;
    } while (until(content[*pos]));

    /* When an string token is being created the first quote is added since
     * that is what defines the token, however the last one is not since it
     * defines the end, thus the first quote is removed to have the content
     * of the string itself. */
    if (until == _untilStrs)
        ustr_erase(token, 0, 0);

    if (until == _untilNums) {
        if (npoints >= 2) {
            ustr_clear(token);
            ustr_append(token, "num_def!");
            *type = TOKEN_TYPE_ERROR;
        }
        *pos -= 1;
    }
}
