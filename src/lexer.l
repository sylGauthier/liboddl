%{
#include "tokens.h"
#include "string_utils.h"

char* strVal;
long long int intVal;
double dblVal;
%}

digit           [0-9]
hexdigit        [a-fA-F0-9]
octdigit        [0-7]
bindigit        [0-1]
letter          [a-zA-Z]

%%
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
"uint8"|"u8"         { return UINT8;        }
"uint16"|"u16"       { return UINT16;       }
"uint32"|"u32"       { return UINT32;       }
"uint64"|"u64"       { return UINT64;       }
"float16"|"h"|"f16"  { return FLOAT16;      }
"float32"|"f"|"f32"  { return FLOAT32;      }
"float64"|"d"|"f64"  { return FLOAT64;      }
"string"|"s"         { return STRING;       }
"ref"|"r"            { return REF;          }
"type"|"t"           { return TYPE;         }

"null"               { return NULL_LIT;     }
"true"               { intVal = 1;
                       return BOOL;         }
"false"              { intVal = 0;
                       return BOOL;         }

{digit}(_?{digit})*                         { intVal = strtol(yytext, NULL, 10);
                                              return DEC_LIT;      }
("0x"|"0X"){hexdigit}(_?{hexdigit})*        { intVal = strtol(yytext+2, NULL, 16);
                                              return HEX_LIT;      }
("0o"|"0O"){octdigit}(_?{octdigit})*        { intVal = strtol(yytext+2, NULL, 8);
                                              return OCT_LIT;      }
("0b"|"0B"){bindigit}(_?{bindigit})*        { intVal = strtol(yytext+2, NULL, 2);
                                              return BIN_LIT;      }
'.*'                                        { strVal = str_copy(yytext);
                                              return CHAR_LIT;      }
\".*\"                                      { strVal = str_copy(yytext);
                                              return STRING_LIT;      }

{letter}({letter}|{digit})* {
                       strVal = str_copy(yytext);
                       return IDENT;      }
[ \t\n\r]            /* skip whitespace */
.                    { printf("Unknown character [%c]\n",yytext[0]);
                       return UNKNOWN;    }
%%

int yywrap(void){return 1;}
