#include "lexer.h"
#include <ctype.h>

typedef bool __getUntil (const char c);
bool to_strings (const char c) { return c != '"'; }
bool to_numbers (const char c) { return isdigit(c) || c == '.'; }

void lexer_getToken (__getUntil until, struct ustr* token, const char* content, size_t* pos)
{
    do {
        ustr_pushBack(token, content[*pos]);
        *pos += 1;
    } while (until(content[*pos]));


    if (until == to_strings)
        ustr_erase(token, 0, 0);

    // XXX: Check number definition.
    if (until == to_numbers)
        *pos -= 1;
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
    for (size_t i = 0; i < bytes; ++i) {
        while (isspace(content[i]))
            i++;

        // XXX: Make a cell.
        if (content[i] == '|') {
            ustr_clear(token);
            i++;
        }

        enum tokenType ttype;
        if (content[i] == '"') {
            lexer_getToken(to_strings, token, content, &i);
            ttype = TOKEN_TYPE_STRING;
        }

        // XXX: use at instead of [i + 1];
        if (isdigit(content[i]) || (content[i] == '-' && isdigit(content[i + 1]))) {
            lexer_getToken(to_numbers, token, content, &i);
            ttype = TOKEN_TYPE_NUMBER;
        }

        if (token->size) {
            printf("token: %s \t %d\n", token->data, ttype);
            ustr_clear(token);
        }
    }

    fclose(table);
    free(content);
}
