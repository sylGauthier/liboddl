#include <stdio.h>

#include "../src/tokens.h"

int main() {

    while (1) {
        enum ODDLTokens ret;

        ret = yylex();
        switch (ret) {
            case END:
                printf("EOF\n");
                return 0;
            case OBRACE:
                printf("OBRACE\n");
                break;
            case CBRACE:
                printf("CBRACE\n");
                break;
            case OBRACKET:
                printf("OBRACKET\n");
                break;
            case CBRACKET:
                printf("CBRACKET\n");
                break;
            case OPAR:
                printf("OPAR\n");
                break;
            case CPAR:
                printf("CPAR\n");
                break;
            case COMMA:
                printf("COMMA\n");
                break;
            case PERCENT:
                printf("PERCENT\n");
                break;
            case DOLLAR:
                printf("DOLLAR\n");
                break;
            case PLUS:
                printf("PLUS\n");
                break;
            case MINUS:
                printf("MINUS\n");
                break;
            case EQUAL:
                printf("EQUAL\n");
                break;
            case DOT:
                printf("DOT\n");
                break;
            case BOOL:
                printf("BOOL\n");
                break;
            case INT8:
                printf("INT8\n");
                break;
            case INT16:
                printf("INT16\n");
                break;
            case INT32:
                printf("INT32\n");
                break;
            case INT64:
                printf("INT64\n");
                break;
            case UINT8:
                printf("UINT8\n");
                break;
            case UINT16:
                printf("UINT16\n");
                break;
            case UINT32:
                printf("UINT32\n");
                break;
            case UINT64:
                printf("UINT64\n");
                break;
            case FLOAT16:
                printf("FLOAT16\n");
                break;
            case FLOAT32:
                printf("FLOAT32\n");
                break;
            case FLOAT64:
                printf("FLOAT64\n");
                break;
            case STRING:
                printf("STRING\n");
                break;
            case REF:
                printf("REF\n");
                break;
            case TYPE:
                printf("TYPE\n");
                break;
            case BOOL_LIT:
                printf("BOOL_LIT\n");
                break;
            case DEC_LIT:
                printf("DEC_LIT\n");
                break;
            case HEX_LIT:
                printf("HEX_LIT\n");
                break;
            case OCT_LIT:
                printf("OCT_LIT\n");
                break;
            case BIN_LIT:
                printf("BIN_LIT\n");
                break;
            case CHAR_LIT:
                printf("CHAR_LIT\n");
                break;
            case STRING_LIT:
                printf("STRING_LIT\n");
                break;
            case FLOAT_LIT:
                printf("FLOAT_LIT\n");
                break;
            case REF_LIT:
                printf("REF_LIT\n");
                break;
            case ESC_CHAR:
                printf("ESC_CHAR\n");
                break;
            case IDENT:
                printf("IDENT\n");
                break;
            case UNKNOWN:
                printf("UNKNOWN\n");
                break;
            default:
                printf("Etoooo...\n");
                return 1;
        }
    }
    return 0;
}
