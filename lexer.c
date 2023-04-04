#include "lexer.h"

typedef bool __getUntil (char);
bool _tilString (char c) { return c != '"'; }
bool _tilNumber (char c) { return isdigit(c) || c == '.'; }
bool _tilCoords (char c) { return isdigit(c) || islower(c); }

void _getToken (__getUntil, const char*, struct ustr*, size_t*, enum tokenType*);

void lexer_read (FILE* table)
{
    fseek(table, 0, SEEK_END);
    size_t numbytes = ftell(table);
    fseek(table, 0, SEEK_SET);

    char* content = (char*) malloc(numbytes);
    fread(content, numbytes, 1, table);
    content[numbytes - 1] = '\0';
    fclose(table);

    struct ustr* token = ustr_make();
    parser_init();

    for (size_t pos = 0; pos < numbytes; ++pos) {
        while (isspace(content[pos]) && content[pos] != '\n')
            pos++;

        if (content[pos] == '|')
            parser_newCell();
        if (content[pos] == '\n')
            parser_setNewRow();

        bool tokenops[] = {
            content[pos] == '@',                                      /* Coordinate to another cell. */
            content[pos] == '"',                                      /* String literal. */
            isdigit(content[pos]),                                    /* Positive number. */
            content[pos] == '-' && isdigit(ustr_at(token, pos + 1))   /* Negative number. */
        };

        enum tokenType thistype;
        if (tokenops[0]) {
            thistype = TOKEN_TYPE_COORDS;
            _getToken(_tilCoords, content, token, &pos, NULL);
        }
        else if (tokenops[1]) {
            thistype = TOKEN_TYPE_STRING;
            _getToken(_tilString, content, token, &pos, NULL);
        }
        else if (tokenops[2] || tokenops[3]) {
            thistype = TOKEN_TYPE_NUMBER;
            _getToken(_tilNumber, content, token, &pos, &thistype);
        }

        if (token->size) {
            ustr_clear(token);
        }
    }

    ustr_kill(token);
    free(content);
}

void _getToken (__getUntil til, const char* content, struct ustr* token, size_t* pos, enum tokenType* type)
{
    unsigned char npoints = 0;
    do {
        if (content[*pos] == '.')
            npoints++;
        ustr_pushBack(token, content[*pos]);
        *pos += 1;
    } while (til(content[*pos]));

    /* Removes the first double-quote of the token to get
     * only the string content itself. */
    if (til == _tilString) {
        ustr_erase(token, 0, 0);
        return;
    }
    if (til == _tilNumber && npoints >= 2) {
        ustr_clear(token);
        ustr_append(token, "number_defintion!");
        *type = TOKEN_TYPE_ERROR;
    }

    *pos -= 1;
}
