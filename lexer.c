#include "lexer.h"

void lexer_read (FILE* table)
{
    struct ustr *row = ustr_make();
    char content;
    while ( (content = fgetc(table)) != EOF )
    {
        ustr_pushBack(row, content);
    }

    printf("%s", row->data);

    ustr_kill(row);
    fclose(table);
}
