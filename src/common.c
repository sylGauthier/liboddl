#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "common.h"

struct ODDLStructure* oddl_new_structure() {
    struct ODDLStructure* st;

    if (!(st = malloc(sizeof(struct ODDLStructure)))) {
        fprintf(stderr, "Error allocating memory for new structure\n");
        return NULL;
    }
    st->name.globalNameIdx = -1;
    st->name.str = NULL;
    st->parent = NULL;

    st->type = 0;
    st->vecSize = 0;
    st->nbVec = 0;
    st->dataList = NULL;

    st->identifier = NULL;
    st->nbProperties = 0;
    st->nbStructures = 0;
    st->properties = NULL;
    st->structures = 0;
    return st;
}

void invalid_token(enum ODDLTokens token) {
    fprintf(stderr, "Error: line %d: invalid token: %d\n", curLine, token);
}

int find_global_name(struct ODDLDoc* doc, const char* name) {
    int i;

    for (i = 0; i < doc->nbGlobalNames; i++) {
        if (!strcmp(doc->globalNames[i].str, name))
            return i;
    }
    return -1;
}

int parse_global_name(struct ODDLDoc* doc, struct ODDLStructure* ref) {
    struct ODDLGlobalName* tmp;
    char* name;
    enum ODDLTokens curToken;

    curToken = yylex();
    if (curToken != IDENT) {
        invalid_token(curToken);
        return -1;
    }
    name = strVal;
    if (find_global_name(doc, name) > 0) {
        fprintf(stderr, "Error: redefinition of gobal name '%s'\n", name);
        return -1;
    }
    if (!(tmp = realloc(doc->globalNames, (doc->nbGlobalNames+1)*sizeof(*doc->globalNames)))) {
        fprintf(stderr, "Error reallocating memory for global names\n");
        return -1;
    }
    doc->globalNames = tmp;
    doc->globalNames[doc->nbGlobalNames].str = name;
    doc->globalNames[doc->nbGlobalNames].ref = ref;
    ref->name.str = name;
    ref->name.globalNameIdx = doc->nbGlobalNames;
    return doc->nbGlobalNames++;
}

int parse_local_name(struct ODDLDoc* doc, struct ODDLStructure* ref) {
    enum ODDLTokens curToken;

    curToken = yylex();
    if (curToken != IDENT) {
        invalid_token(curToken);
        return -1;
    }
    ref->name.str = strVal;
    ref->name.globalNameIdx = -1;
    return 0;
}

int parse_int(void* in, unsigned size) {
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

int parse_uint(void* in, unsigned size) {
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

int parse_bool(void* in, unsigned size) {
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

int parse_float(void* in, unsigned size) {
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

int parse_string(void* in, unsigned size) {
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

int parse_ref(void* in, unsigned size) {
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

int parse_type(void* in, unsigned size) {
    fprintf(stderr, "Ref parsing not implemented\n");
    return 1;
}
