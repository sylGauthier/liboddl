#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "data_structure.h"
#include "common.h"

static int parse_int(void* in, unsigned size) {
    enum ODDLTokens curToken;
    int sign = 1;

    curToken = yylex();
    if (curToken == PLUS) {
        sign = 1;
        curToken = yylex();
    } else if (curToken == MINUS) {
        sign = -1;
        curToken = yylex();
    }
    if (curToken == DEC_LIT
            || curToken == HEX_LIT
            || curToken == OCT_LIT
            || curToken == BIN_LIT) {
        switch (size) {
            case 1:
                *((int8_t*)in) = sign*intVal;
                break;
            case 2:
                *((int16_t*)in) = sign*intVal;
                break;
            case 4:
                *((int32_t*)in) = sign*intVal;
                break;
            case 8:
                *((int64_t*)in) = sign*intVal;
                break;
            default:
                fprintf(stderr, "Unhandled int size\n");
                return 0;
        }
        return 1;
    }
    invalid_token(curToken);
    return 0;
}

static int parse_uint(void* in, unsigned size) {
    enum ODDLTokens curToken;

    curToken = yylex();
    if (curToken == PLUS) {
        curToken = yylex();
    }
    if (curToken == DEC_LIT
            || curToken == HEX_LIT
            || curToken == OCT_LIT
            || curToken == BIN_LIT) {
        switch (size) {
            case 1:
                *((uint8_t*)in) = intVal;
                break;
            case 2:
                *((uint16_t*)in) = intVal;
                break;
            case 4:
                *((uint32_t*)in) = intVal;
                break;
            case 8:
                *((uint64_t*)in) = intVal;
                break;
            default:
                fprintf(stderr, "Error: line %d: Unhandled uint size(%d)\n", curLine, size);
                return 0;
        }
        return 1;
    }
    invalid_token(curToken);
    return 0;
}

static int parse_bool(void* in, unsigned size) {
    enum ODDLTokens curToken;
    char* boolean = in;

    curToken = yylex();
    switch (curToken) {
        case BOOL_LIT:
            *boolean = intVal;
            return 1;
        default:
            invalid_token(curToken);
            return 0;
    }
    return 0;
}

static int parse_float(void* in, unsigned size) {
    enum ODDLTokens curToken;
    float* fl = in;
    float sign = 1.0;
    uint32_t bin;

    curToken = yylex();
    if (curToken == MINUS) {
        sign = -1.0;
        curToken = yylex();
    } else if (curToken == PLUS) {
        curToken = yylex();
    }
    switch (curToken) {
        case HEX_LIT:
        case OCT_LIT:
        case BIN_LIT:
            bin = intVal;
            memcpy(fl, &bin, sizeof(float));
            *fl *= sign;
            return 1;
        case FLOAT_LIT:
            *fl = sign * dblVal;
            return 1;
        case DEC_LIT:
            *fl = sign * intVal;
            return 1;
        default:
            invalid_token(curToken);
            return 0;
    }
    return 1;
}

static int parse_string(void* in, unsigned size) {
    enum ODDLTokens curToken;
    char** str = in;

    *str = NULL;
    curToken = yylex();
    if (curToken != STRING_LIT) {
        invalid_token(STRING_LIT);
        return 0;
    }
    *str = strVal;

    return 1;
}

static int parse_ref(void* in, unsigned size) {
    enum ODDLTokens curToken;
    struct ODDLRef* ref = in;
    char prefix = 0;

    curToken = yylex();

    switch (curToken) {
        case NULL_LIT:
            ref->ref = NULL;
            ref->refStr = NULL;
            return 1;
        case DOLLAR:
            prefix = '$';
            break;
        case PERCENT:
            prefix = '%';
            break;
        case REF_LIT:
            ref->refStr = strVal;
            return 1;
        default:
            invalid_token(curToken);
            return 0;
    }

    curToken = yylex();
    if (curToken != IDENT) {
        invalid_token(curToken);
        return 0;
    }
    if (!(ref->refStr = malloc(strlen(strVal)+2))) {
        return 0;
    }
    *(ref->refStr) = prefix;
    strcpy(ref->refStr+1, strVal);
    free(strVal);

    return 1;
}

static int parse_type(void* in, unsigned size) {
    fprintf(stderr, "Ref parsing not implemented\n");
    return 1;
}

static void* parse_list(enum ODDLDataType type, unsigned int vecSize, void* list, unsigned int* nbVec) {
    void* ret = NULL;
    unsigned int i;
    int (*parse_lit)(void* dest, unsigned size);
    enum ODDLTokens curToken;

    if (vecSize) {
        if (list) {
            ret = list;
        } else {
            return NULL;
        }
    }
    switch (type) {
        case TYPE_BOOL:
            parse_lit = parse_bool;
            break;
        case TYPE_INT8:
        case TYPE_INT16:
        case TYPE_INT32:
        case TYPE_INT64:
            parse_lit = parse_int;
            break;
        case TYPE_UINT8:
        case TYPE_UINT16:
        case TYPE_UINT32:
        case TYPE_UINT64:
            parse_lit = parse_uint;
            break;
        case TYPE_FLOAT16:
        case TYPE_FLOAT32:
        case TYPE_FLOAT64:
            parse_lit = parse_float;
            break;
        case TYPE_STRING:
            parse_lit = parse_string;
            break;
        case TYPE_REF:
            parse_lit = parse_ref;
            break;
        case TYPE_TYPE:
            parse_lit = parse_type;
            break;
        default:
            fprintf(stderr, "Error: No rule to parse type\n");
            return NULL;
    }
    for (i = 0;;i++) {
        if (!list) {
            void* tmp;
            if (!(tmp = realloc(ret, (i+1)*TYPE_SIZE(type)))) {
                fprintf(stderr, "Error reallocating memory for data list\n");
                free(ret);
                return NULL;
            }
            ret = tmp;
        }
        if (!(parse_lit(((char*)ret) + i*TYPE_SIZE(type), TYPE_SIZE(type)))) {
            fprintf(stderr, "Failed parsing literal\n");
            if (!list)
                free(ret);
            return NULL;
        }
        curToken = yylex();
        if (curToken == CBRACE) {
            if (vecSize > 0 && (i+1) != vecSize) {
                fprintf(stderr, "Error: data vector too short (%d instead of %d)\n", i+1, vecSize);
                if (!list)
                    free(ret);
                return NULL;
            }
            *nbVec = i+1;
            return ret;
        } else if (curToken != COMMA) {
            invalid_token(curToken);
            if (!list)
                free(ret);
            return NULL;
        }
        if (vecSize > 0 && (i+1) > vecSize) {
            fprintf(stderr, "Error: data vector too long\n");
            if (!list)
                free(ret);
            return NULL;
        }
    }
}

static int parse_data_list(struct ODDLDoc* doc,
                           struct ODDLStructure* dataStruct) {
    enum ODDLTokens curToken;

    dataStruct->dataList = NULL;
    if (dataStruct->vecSize > 0) {
        int i = 1;
        while (1) {
            void* tmp;

            if (!(tmp = realloc(dataStruct->dataList, i*dataStruct->vecSize*TYPE_SIZE(dataStruct->type)))) {
                fprintf(stderr, "Error reallocating memory for data array list\n");
                free(dataStruct->dataList);
                return 0;
            }
            dataStruct->dataList = tmp;
            curToken = yylex();
            if (curToken != OBRACE) {
                invalid_token(curToken);
                free(dataStruct->dataList);
                return 0;
            }
            if (!parse_list(dataStruct->type, dataStruct->vecSize,
                            (char*)dataStruct->dataList + (i-1)*dataStruct->vecSize*TYPE_SIZE(dataStruct->type),
                            &dataStruct->nbVec)) {
                fprintf(stderr, "Error parsing data array list\n");
                free(dataStruct->dataList);
                return 0;
            }
            curToken = yylex();
            switch (curToken) {
                case CBRACE:
                    dataStruct->nbVec = i;
                    return 1;
                case COMMA:
                    break;
                default:
                    invalid_token(curToken);
                    free(dataStruct->dataList);
                    return 0;
            }
            i++;
        }
    } else {
        if (!(dataStruct->dataList = parse_list(dataStruct->type, 0, NULL, &dataStruct->nbVec))) {
            fprintf(stderr, "Error parsing data list\n");
            return 0;
        }
        dataStruct->vecSize = 1;
    }
    return 1;
}

int parse_data_structure(struct ODDLDoc* doc,
                         struct ODDLStructure* st,
                         enum ODDLTokens typeToken) {
    enum ODDLTokens curToken;

    st->type = typeToken - TYPE_OFFSET;
    curToken = yylex();
    if (curToken == OBRACKET) {
        if (!parse_uint(&st->vecSize, sizeof(st->vecSize))) {
            return -1;
        }
        if ((curToken = yylex()) != CBRACKET) {
            invalid_token(curToken);
            return -1;
        }
        curToken = yylex();
    }
    if (curToken == PERCENT) {
        parse_local_name(doc, st);
        curToken = yylex();
    } else if (curToken == DOLLAR) {
        parse_global_name(doc, st);
        curToken = yylex();
    }
    if (curToken != OBRACE) {
        invalid_token(curToken);
        return -1;
    }
    if (!parse_data_list(doc, st)) {
        return -1;
    }
    return 1;
}
