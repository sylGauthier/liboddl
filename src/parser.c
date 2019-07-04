#include <stdlib.h>
#include <string.h>

#include <liboddl/liboddl.h>
#include "tokens.h"
#include "data_structure.h"
#include "common.h"
#include "string_utils.h"

char* typeName[TYPE_TYPE+1] = {
    "NONE",
    "bool",
    "int8",
    "int16",
    "int32",
    "int64",
    "unsigned_int8",
    "unsigned_int16",
    "unsigned_int32",
    "unsigned_int64",
    "half",
    "float",
    "double",
    "string",
    "ref",
    "type"
};


static void free_property(struct ODDLProperty* prop) {
    free(prop->identifier);
    free(prop->str);
    free(prop->ref.refStr);
}

static void free_structure(struct ODDLStructure* st) {
    int i;

    for (i = 0; i < st->nbStructures; i++) {
        free_structure(st->structures[i]);
    }
    free(st->structures);

    for (i = 0; i < st->nbProperties; i++) {
        free_property(st->properties + i);
    }
    if (st->type == TYPE_REF) {
        struct ODDLRef* refs = st->dataList;
        for (i = 0; i < st->nbVec*st->vecSize; i++) {
            free(refs[i].refStr);
        }
    } else if (st->type == TYPE_STRING) {
        char** strings = st->dataList;
        for (i = 0; i < st->nbVec*st->vecSize; i++) {
            free(strings[i]);
        }
    }
    free(st->properties);
    free(st->dataList);
    free(st->identifier);
    free(st->name.str);
    free(st);
}

static int parse_property(struct ODDLDoc* doc, struct ODDLStructure* ret) {
    enum ODDLTokens curToken;
    struct ODDLProperty newProp = {NULL, 0, 0, NULL, {NULL, NULL}, 0};
    struct ODDLProperty* tmp;

    curToken = yylex();
    if (curToken != IDENT) {
        invalid_token(curToken);
        return 0;
    }
    newProp.identifier = strVal;
    curToken = yylex();
    if (curToken != EQUAL) {
        invalid_token(curToken);
        return 0;
    }
    curToken = yylex();
    switch (curToken) {
        case BOOL_LIT :
        case DEC_LIT :
        case HEX_LIT :
        case OCT_LIT :
        case BIN_LIT :
            newProp.llint = intVal;
            newProp.dbl = intVal;
            break;
        case FLOAT_LIT :
            newProp.dbl = dblVal;
            break;
        case STRING_LIT :
            newProp.str = strVal;
            break;
        case REF_LIT :
            newProp.ref.refStr = strVal;
            break;
        case DOLLAR :
            if ((curToken = yylex()) != IDENT) {
                invalid_token(curToken);
                return 0;
            }
            newProp.ref.refStr = str_prefix('$', strVal);
            free(strVal);
            break;
        case PERCENT :
            if ((curToken = yylex()) != IDENT) {
                invalid_token(curToken);
                return 0;
            }
            newProp.ref.refStr = str_prefix('%', strVal);
            free(strVal);
            break;
        case BOOL :
            newProp.type = TYPE_BOOL;
            break;
        case INT8 :
            newProp.type = TYPE_INT8;
            break;
        case INT16 :
            newProp.type = TYPE_INT16;
            break;
        case INT32 :
            newProp.type = TYPE_INT32;
            break;
        case INT64 :
            newProp.type = TYPE_INT64;
            break;
        case UINT8 :
            newProp.type = TYPE_UINT8;
            break;
        case UINT16 :
            newProp.type = TYPE_UINT16;
            break;
        case UINT32 :
            newProp.type = TYPE_UINT32;
            break;
        case UINT64 :
            newProp.type = TYPE_UINT64;
            break;
        case FLOAT16 :
            newProp.type = TYPE_FLOAT16;
            break;
        case FLOAT32 :
            newProp.type = TYPE_FLOAT32;
            break;
        case FLOAT64 :
            newProp.type = TYPE_FLOAT64;
            break;
        case STRING :
            newProp.type = TYPE_STRING;
            break;
        case REF :
            newProp.type = TYPE_REF;
            break;
        case TYPE :
            newProp.type = TYPE_TYPE;
            break;
        default :
            invalid_token(curToken);
            return 0;
    }
    if (!(tmp = realloc(ret->properties, (ret->nbProperties+1)*sizeof(struct ODDLProperty)))) {
        free_property(&newProp);
        fprintf(stderr, "Error reallocating property array\n");
        return 0;
    }
    ret->properties = tmp;
    ret->properties[ret->nbProperties] = newProp;
    ret->nbProperties++;
    return 1;
}

static int parse_structure(struct ODDLDoc* doc, struct ODDLStructure* ret);

static int parse_meta_structure(struct ODDLDoc* doc,
                                struct ODDLStructure* ret) {
    enum ODDLTokens curToken;

    ret->identifier = strVal;
    ret->nbProperties = 0;
    ret->properties = NULL;
    curToken = yylex();
    if (curToken == PERCENT) {
        parse_local_name(doc, ret);
        curToken = yylex();
    } else if (curToken == DOLLAR) {
        parse_global_name(doc, ret);
        curToken = yylex();
    }
    if (curToken == OPAR) {
        while (1) {
            if (!(parse_property(doc, ret))) {
                fprintf(stderr, "Error parsing properties, aborting\n");
                free_structure(ret);
                return -1;
            }
            curToken = yylex();
            if (curToken == CPAR) {
                break;
            } else if (curToken == END) {
                fprintf(stderr, "Error: missing ')'\n");
                free_structure(ret);
                return -1;
            } else if (curToken != COMMA) {
                invalid_token(curToken);
                free_structure(ret);
                return -1;
            }
        }
        curToken = yylex();
    }
    if (curToken != OBRACE) {
        invalid_token(curToken);
        return -1;
    }
    ret->nbStructures = 0;
    ret->structures = NULL;
    do {
        struct ODDLStructure* tmp;
        int status;

        tmp = oddl_new_structure();
        status = parse_structure(doc, tmp);
        if (status == -2) {
            free_structure(tmp);
            break;
        } else if (status <= 0) {
            free_structure(ret);
            return -1;
        } else {
            struct ODDLStructure** new;

            if (!(new = realloc(ret->structures, (ret->nbStructures+1)*sizeof(struct ODDLStructure*)))) {
                fprintf(stderr, "Error reallocating memory for meta structure\n");
                free_structure(ret);
                return -1;
            }
            ret->structures = new;
            ret->structures[ret->nbStructures] = tmp;
            ret->nbStructures++;
        }
    } while (1);
    return 1;
}

static int parse_structure(struct ODDLDoc* doc, struct ODDLStructure* ret) {
    enum ODDLTokens curToken;

    curToken = yylex();
    switch (curToken) {
        case END:
            return 0;
        case IDENT:
            return parse_meta_structure(doc, ret);
        case BOOL:
        case INT8:
        case INT16:
        case INT32:
        case INT64:
        case UINT8:
        case UINT16:
        case UINT32:
        case UINT64:
        case FLOAT16:
        case FLOAT32:
        case FLOAT64:
        case STRING:
        case REF:
        case TYPE:
            return parse_data_structure(doc, ret, curToken);
        case CBRACE:
            return -2;
        default:
            invalid_token(curToken);
            return -1;
    }
}

static unsigned split_sub_idents(char* ref) {
    char* curs = ref;
    unsigned int nbIdents = 1;

    while (*curs) {
        if (*curs == '%') {
            *curs = '\0';
            nbIdents++;
        }
        curs += 1;
    }
    return nbIdents;
}

static char* next_sub_ident(char* cur) {
    char* ret = cur;

    while (*ret) {
        ret += 1;
    }
    return ret + 1;
}

static struct ODDLStructure* find_child(struct ODDLStructure* st, char* name) {
    unsigned i;

    for (i = 0; i < st->nbStructures; i++) {
        if (!(strcmp(name, st->structures[i]->name.str))) {
            printf("Found ref %s == %s\n", name, st->structures[i]->name.str);
            return st->structures[i];
        }
    }
    return NULL;
}

static struct ODDLStructure* resolve_ref(struct ODDLDoc* doc,
                                         struct ODDLStructure* st,
                                         struct ODDLRef* ref) {
    char* cursor = ref->refStr;
    struct ODDLStructure* ret = NULL;
    struct ODDLStructure* refRoot = NULL;
    unsigned nbIdents;

    nbIdents = split_sub_idents(cursor+1);

    if (*cursor == '$') {
        int idx;

        idx = find_global_name(doc, cursor+1);
        if (idx < 0) {
            fprintf(stderr, "Undefined global name: $%s\n", cursor+1);
            return NULL;
        }
        refRoot = doc->globalNames[idx].ref;
        cursor = next_sub_ident(cursor);
        nbIdents--;
    } else if (*cursor == '%') {
        if (!(refRoot = st->parent)) {
            return NULL;
        }
    }
    ret = refRoot;
    for (; nbIdents > 0; nbIdents--) {
        if (!(ret = find_child(ret, cursor))) {
            fprintf(stderr, "Error: could not resolve ref, unknown ident: %s\n", cursor);
            return NULL;
        }
        cursor = next_sub_ident(cursor);
    }
    return ret;
}

static int resolve_all_refs(struct ODDLDoc* doc, struct ODDLStructure* root) {
    if (root->nbProperties > 0) {
        unsigned i;

        for (i = 0; i < root->nbProperties; i++) {
            if (root->properties[i].ref.refStr) {
                root->properties[i].ref.ref = resolve_ref(doc, root, &root->properties[i].ref);
            }
        }
    }
    if (root->nbStructures > 0) {
        unsigned i;

        for (i = 0; i < root->nbStructures; i++) {
            if (!resolve_all_refs(doc, root->structures[i])) {
                return 0;
            }
        }
    } else if (root->type == TYPE_REF) {
        struct ODDLRef* refs = root->dataList;
        unsigned i;

        for (i = 0; i < root->vecSize*root->nbVec; i++) {
            if (!(refs[i].ref = resolve_ref(doc, root, refs+i))) {
                fprintf(stderr, "Error: failed to resolve a ref: %s\n", refs[i].refStr);
                return 0;
            }
        }
    }
    return 1;
}

void oddl_free(struct ODDLDoc* doc) {
    free_structure(doc->root);
    free(doc->globalNames);
}

int oddl_parse(struct ODDLDoc* ret, FILE* file) {
    struct ODDLStructure* tmpStruct;
    int parseStatus = 0;

    yyin = file;
    ret->nbGlobalNames = 0;
    ret->globalNames = NULL;
    ret->root = oddl_new_structure();

    while ((tmpStruct = oddl_new_structure()) && (parseStatus = parse_structure(ret, tmpStruct)) > 0) {
        struct ODDLStructure** tmpPtr = NULL;

        if (!(tmpPtr = realloc(ret->root->structures,
                              (ret->root->nbStructures+1)*sizeof(struct ODDLStructure*)))) {
            fprintf(stderr, "Error reallocating memory for root list\n");
            oddl_free(ret);
            return 0;
        }
        ret->root->structures = tmpPtr;
        tmpStruct->parent = ret->root;
        ret->root->structures[ret->root->nbStructures] = tmpStruct;
        ret->root->nbStructures++;
    }
    if (parseStatus < 0) {
        fprintf(stderr, "Parse error, aborting\n");
        oddl_free(ret);
        return 0;
    }
    if (!resolve_all_refs(ret, ret->root)) {
        fprintf(stderr, "Error resolving refs, aborting\n");
        oddl_free(ret);
    }
    if (tmpStruct) {
        free_structure(tmpStruct);
    }

    yylex_destroy();
    return 1;
}
