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
