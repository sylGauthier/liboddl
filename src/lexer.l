%{
#include "tokens.h"
#include "string_utils.h"

char* strVal; /*TODO: Make string copy static, strVal should be a static array to avoid mem leaks on parse error */
uint64_t intVal;
unsigned int curLine = 1;
double dblVal;
%}

digit           [0-9]
declit          {digit}(_?{digit})*
hexdigit        [a-fA-F0-9]
octdigit        [0-7]
bindigit        [0-1]
letter          [a-zA-Z]
ident           {letter}({letter}|{digit})*

%x IN_COMMENT
%%
<INITIAL>{
"/*"              BEGIN(IN_COMMENT);
}
<IN_COMMENT>{
"*/"      BEGIN(INITIAL);
[^*\n]+   /* eat comment in chunks */
"*"       /* eat the lone star */
\n        yylineno++;
}

"{"                  { return OBRACE;       }
"}"                  { return CBRACE;       }
"["                  { return OBRACKET;     }
"]"                  { return CBRACKET;     }
"("                  { return OPAR;         }
")"                  { return CPAR;         }
","                  { return COMMA;        }
"%"                  { return PERCENT;      }
"$"                  { return DOLLAR;       }
"+"                  { return PLUS;         }
"-"                  { return MINUS;        }
"="                  { return EQUAL;        }
"."                  { return DOT;          }
"bool"|"b"           { return BOOL;         }
"int8"|"i8"          { return INT8;         }
"int16"|"i16"        { return INT16;        }
"int32"|"i32"        { return INT32;        }
"int64"|"i64"        { return INT64;        }
"unsigned_int8"|"u8"            { return UINT8;        }
"unsigned_int16"|"u16"          { return UINT16;       }
"unsigned_int32"|"u32"          { return UINT32;       }
"unsigned_int64"|"u64"          { return UINT64;       }
"half"|"float16"|"h"|"f16"      { return FLOAT16;      }
"float"|"float32"|"f"|"f32"     { return FLOAT32;      }
"double"|"float64"|"d"|"f64"    { return FLOAT64;      }
"string"|"s"         { return STRING;       }
"ref"|"r"            { return REF;          }

"null"               { return NULL_LIT;     }
"true"               { intVal = 1;
                       return BOOL;         }
"false"              { intVal = 0;
                       return BOOL;         }

{declit}?\.{declit}((e|E)(\+|\-)?{declit})? { dblVal = strtod(yytext, NULL);
                                              return FLOAT_LIT;    }
{declit}\.                                  { dblVal = strtod(yytext, NULL);
                                              return FLOAT_LIT;    }
{declit}                                    { intVal = strtol(yytext, NULL, 10);
                                              return DEC_LIT;      }
("0x"|"0X"){hexdigit}(_?{hexdigit})*        { intVal = strtol(yytext+2, NULL, 16);
                                              return HEX_LIT;      }
("0o"|"0O"){octdigit}(_?{octdigit})*        { intVal = strtol(yytext+2, NULL, 8);
                                              return OCT_LIT;      }
("0b"|"0B"){bindigit}(_?{bindigit})*        { intVal = strtol(yytext+2, NULL, 2);
                                              return BIN_LIT;      }
'.*'                                        { strVal = str_copy(yytext);
                                              return CHAR_LIT;      }
\"(?:[^"\\]|\\.)*\"                         { strVal = str_lit_copy(yytext);
                                              return STRING_LIT;      }

(\%|\$){ident}(\%{ident})+                  { strVal = str_copy(yytext);
                                              return REF_LIT;       }
{ident}              { strVal = str_copy(yytext);
                       return IDENT;      }
[ \t\r]            /* skip whitespace */
"\n"                   curLine++;
\/\/.*$                curLine++;
.                    { printf("Unknown character [%c]\n",yytext[0]);
                       return UNKNOWN;    }
%%

int yywrap(void){return 1;}
