#ifndef TOKENS_H
#define TOKENS_H

#include <stdio.h>

extern FILE* yyin;
extern char* strVal;
extern long long int intVal;
extern double dblVal;

enum ODDLTokens {
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
    BOOL_LIT,
    DEC_LIT,
    HEX_LIT,
    OCT_LIT,
    BIN_LIT,
    CHAR_LIT,
    STRING_LIT,

    ESC_CHAR,
    IDENT,
    UNKNOWN,
};

int yylex();

#endif
