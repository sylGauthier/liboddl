#ifndef TOKENS_H
#define TOKENS_H

#include <stdio.h>
#include <stdint.h>

extern FILE* yyin;
extern char* strVal;
extern uint64_t intVal;
extern double dblVal;
extern unsigned int curLine;

enum ODDLTokens {
    END = 0,
    /* Symbols */
    OBRACE = 1,
    CBRACE,
    OBRACKET,
    CBRACKET,
    OPAR,
    CPAR,
    COMMA,
    PERCENT,
    DOLLAR,
    PLUS,
    MINUS,
    EQUAL,
    DOT,
    /* Types */
    TYPE_OFFSET,    /* Should always be on top of type list */
    BOOL,
    INT8,
    INT16,
    INT32,
    INT64,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    FLOAT16,
    FLOAT32,
    FLOAT64,
    STRING,
    REF,
    TYPE,
    /* Literals */
    NULL_LIT,
    BOOL_LIT,
    DEC_LIT,
    HEX_LIT,
    OCT_LIT,
    BIN_LIT,
    CHAR_LIT,
    STRING_LIT,
    FLOAT_LIT,
    REF_LIT,

    ESC_CHAR,
    IDENT,
    UNKNOWN
};

int yylex();
int yylex_destroy();

#endif
