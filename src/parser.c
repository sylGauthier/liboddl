#include <stdlib.h>

#include "parser.h"
#include "tokens.h"

static unsigned int find_name(struct ODDLParser* parser, char* name) {
    unsigned int i;

    for (i = 1; i < parser->nbIdentifiers; i++) {
        if (strcmp(name, parser->identifiers[i]))
            return i;
    }
    return 0;
}

static unsigned int parse_name(struct ODDLParser* parser) {
    enum ODDLTokens token;
    struct ODDLIdentifier* tmp;

    token = yylex();
    if (token != IDENT) {
        invalid_token(token);
        return 0;
    }
    if (find_name(strVal)) {
        fprintf(stderr, "Parse error: multiple definitions of %s\n", strVal);
        return 0;
    }
    if (!(tmp = realloc(parser->identifiers, (parser->nbIdentifiers+1)*sizeof(struct ODDLIdentifier)))) {
        fprintf(stderr, "Error reallocating memory for identifiers\n");
        return 0;
    }
    parser->identifiers[parser->nbIdentifiers].name = strVal;
    parser->identifiers[parser->nbIdentifiers].object = NULL;
    return parser->nbIdentifiers++;
}

static struct ODDLDataStructure* parse_data_structure(struct ODDLParser* parser,
                                                      struct ODDLDataStructure* dataStruct,
                                                      enum ODDLTokens typeToken) {
    enum ODDLTokens curToken;
    struct ODDLDataStructure* ret;

    if (!(ret = malloc(sizeof(*ret)))) {
        return NULL;
    }
    ret->name = 0;
    ret->size = 1;
    curToken = yylex();
    switch (curToken) {
        case OBRACKET :
        case DOLLAR :
        case PERCENT :
            if (!(ret->name = parse_name(parser))) {
                free(ret);
                return NULL;
            }
            curToken = yylex();
            if (curToken != OBRACE) {
                invalid_token(curToken);
                free(ret);
                return NULL;
            }
        case OBRACE :
    }
}

static int parse_structure(struct ODDLParser* parser) {
    enum ODDLTokens curToken;

    curToken = yylex();
    switch (curToken) {
        case IDENT:
            return parse_meta_structure(parser);
        case BOOL :
        case INT8 :
        case INT16 :
        case INT32 :
        case INT64 :
        case UINT8 :
        case UINT16 :
        case UINT32 :
        case UINT64 :
        case FLOAT16 :
        case FLOAT32 :
        case FLOAT64 :
        case STRING :
        case REF :
        case TYPE :
            return parse_data_structure(parser, curToken);
        default :
            invalid_token(curToken);
            return 0;
    }
}

struct ODDLParser* oddl_parse(FILE* file) {
    struct ODDLParser* ret = NULL;

    yyin = file;
    if ((ret = malloc(sizeof(*ret)))) {
        ret->nbIdentifiers = 1;
        ret->identifiers = NULL;
        if ((ret->root = malloc(sizeof(struct ODDLStructure)))) {
            ret->root->type = META_STRUCTURE;
            while (parse_structure(ret, file));
        }
    }

    return ret;
}
