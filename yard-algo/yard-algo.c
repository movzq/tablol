#include "yard-algo.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

enum yardTokenType _isTypeOf (const char*);
bool _samePrecedence (const enum yardTokenType, const enum yardTokenType);
bool _lowerPrecedence (const enum yardTokenType, const enum yardTokenType);
void _pushIntoQueue (struct yardAlgo*, const char*, const enum yardTokenType);
void _pushIntoStack (struct yardAlgo*, const enum yardTokenType);
void _precedenceStuff (struct yardAlgo*, const enum yardTokenType);
void _performOperations (double*, size_t*, const enum yardTokenType);
void _pushIntoOutput (double*, size_t, double);
void _closeParentheses (struct yardAlgo*);

struct yardAlgo* yard_make ()
{
    struct yardAlgo* yarda = (struct yardAlgo*) malloc(sizeof(struct yardAlgo));
    yarda->stack = (enum yardTokenType*) malloc(0);
    yarda->queue = (struct yardToken*) malloc(0);
    yarda->stacksize = 0;
    yarda->queuesize = 0;

    return yarda;
}

void yard_evaluateToken (struct yardAlgo* yarda, const char* token)
{
    assert(yarda);
    assert(token);

    enum yardTokenType thistype = _isTypeOf(token);
    if (thistype == YARD_TOKEN_TYPE_NUM)
        _pushIntoQueue(yarda, token, YARD_TOKEN_TYPE_NUM);
    else
        _precedenceStuff(yarda, thistype);
}

double yard_solve (struct yardAlgo* yarda)
{
    assert(yarda);
    size_t i;

    for (i = yarda->stacksize - 1; i != -1; i--)
        _pushIntoQueue(yarda, (char*) &yarda->stack[i], yarda->stack[i]);
    free(yarda->stack);

    double* output = (double*) malloc(0);
    size_t outsize = 0;

    for (i = 0; i < yarda->queuesize; i++) {
        if (yarda->queue[i].type == YARD_TOKEN_TYPE_NUM) { 
            double thisvalue = strtod(yarda->queue[i].data, NULL);
            _pushIntoOutput(output, ++outsize, thisvalue);
        }
        else
            _performOperations(output, &outsize, yarda->queue[i].type);

        free(yarda->queue[i].data);
    }

    double out = output[0];
    free(yarda->queue);
    free(yarda);
    free(output);
    return out;
}

enum yardTokenType _isTypeOf (const char* token)
{
    if (strlen(token) == 1) {
        switch (token[0]) {
            case '+' : return YARD_TOKEN_TYPE_ADD;
            case '-' : return YARD_TOKEN_TYPE_SUB;
            case '*' : return YARD_TOKEN_TYPE_MUL;
            case '/' : return YARD_TOKEN_TYPE_DIV;
            case '(' : return YARD_TOKEN_TYPE_LPA;
            case ')' : return YARD_TOKEN_TYPE_RPA;
        }
    }

    return YARD_TOKEN_TYPE_NUM;
}

bool _samePrecedence (const enum yardTokenType top, const enum yardTokenType new)
{
    bool same1 = (top == '+' || top == '-') && (new == '+' || new == '-');
    bool same2 = (top == '*' || top == '/') && (new == '/' || new == '*');
    return same1 || same2;
}

bool _lowerPrecedence (const enum yardTokenType top, const enum yardTokenType new)
{
    bool lower = (new == '+' || new == '-') && (top == '*' || top == '/');
    return lower;
}

void _pushIntoQueue (struct yardAlgo* yarda, const char* token, const enum yardTokenType type)
{
    yarda->queue = (struct yardToken*) realloc(
        yarda->queue,
        ++yarda->queuesize * sizeof(struct yardToken)
    );

    struct yardToken thist = {
        .data = (char*) malloc(strlen(token) + 1),
        .type = type
    };

    strcpy(thist.data, token);
    yarda->queue[yarda->queuesize - 1] = thist;
}

void _pushIntoStack (struct yardAlgo* yarda, const enum yardTokenType type)
{
    yarda->stack = (enum yardTokenType*) realloc(
        yarda->stack,
        ++yarda->stacksize * sizeof(enum yardTokenType)
    );
    yarda->stack[yarda->stacksize - 1] = type;
}

void _precedenceStuff (struct yardAlgo* yarda, const enum yardTokenType type)
{
    if (type != YARD_TOKEN_TYPE_RPA)
        _pushIntoStack(yarda, type);
    else {
        _closeParentheses(yarda);
        return;
    }

    if (yarda->stacksize == 1)
        return;

    enum yardTokenType prev = yarda->stack[yarda->stacksize - 2];
    bool precedenceissue = false;

    while (_lowerPrecedence(prev, type) || _samePrecedence(prev, type)) {
        _pushIntoQueue(yarda, (char*) &prev, prev);

        yarda->stack[--yarda->stacksize - 1] = type;
        if (yarda->stacksize >= 2)
            prev = yarda->stack[yarda->stacksize - 2];
        else
            break; 
        precedenceissue = true;
    }

    if (precedenceissue) {
        yarda->stack = (enum yardTokenType*) realloc(
            yarda->stack,
            yarda->stacksize * sizeof(double)
        );
    }
}

void _performOperations (double* output, size_t* outsize, const enum yardTokenType type)
{
    double value;
    double top = output[*outsize - 1];
    double prv = output[*outsize - 2];

    switch (type) {
        case YARD_TOKEN_TYPE_ADD: { value = prv + top; break; }
        case YARD_TOKEN_TYPE_SUB: { value = prv - top; break; }
        case YARD_TOKEN_TYPE_MUL: { value = prv * top; break; }
        case YARD_TOKEN_TYPE_DIV: { value = prv / top; break; }
    }

    *outsize -= 1;
    _pushIntoOutput(output, *outsize, value);
}

void _pushIntoOutput (double* output, size_t outsize, double value)
{
    output = (double*) realloc(
        output,
        outsize * sizeof(double)
    );
    output[outsize - 1] = value;
}

void _closeParentheses (struct yardAlgo* yarda)
{
    enum yardTokenType currtype;
    do {
        currtype = yarda->stack[yarda->stacksize - 1];
        _pushIntoQueue(yarda, (char*) &currtype, currtype);

        currtype = yarda->stack[--yarda->stacksize - 1];
    } while (currtype != YARD_TOKEN_TYPE_LPA);

    yarda->stack = (enum yardTokenType*) realloc(
        yarda->stack,
        --yarda->stacksize * sizeof(enum yardTokenType)
    );
}
