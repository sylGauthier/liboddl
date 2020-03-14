#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "common.h"

struct ODDLStructure* oddl_new_structure(void) {
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

struct ODDLStructure* oddl_new_data_structure(enum ODDLDataType type, unsigned int nbVec, unsigned int vecSize) {
    struct ODDLStructure* st;
    size_t size;
    unsigned int i;

    if (vecSize > ((size_t)-1) / (size = TYPE_SIZE(type))
     || nbVec > ((size_t)-1) / (size *= vecSize)
     || !(st = oddl_new_structure())) {
        return NULL;
    }
    st->type = type;
    st->nbVec = nbVec;
    st->vecSize = vecSize;
    if (!(st->dataList = malloc(size * nbVec))) {
        free(st);
        return NULL;
    }
    if (st->type == TYPE_REF) {
        struct ODDLRef* refs = st->dataList;
        for (i = 0; i < st->nbVec*st->vecSize; i++) {
            refs[i].refStr = NULL;
        }
    } else if (st->type == TYPE_STRING) {
        char** strings = st->dataList;
        for (i = 0; i < st->nbVec*st->vecSize; i++) {
            strings[i] = NULL;
        }
    }
    return st;
}

int oddl_structure_add_child(struct ODDLStructure* parent, struct ODDLStructure* child) {
    struct ODDLStructure** tmp = NULL;

    if (!(tmp = realloc(parent->structures, (parent->nbStructures + 1) * sizeof(struct ODDLStructure*)))) {
        return 0;
    }
    parent->structures = tmp;
    child->parent = parent;
    parent->structures[parent->nbStructures] = child;
    parent->nbStructures++;
    return 1;
}

int oddl_structure_add_property(struct ODDLStructure* structure, const struct ODDLProperty* property) {
    struct ODDLProperty* tmp;

    if (!(tmp = realloc(structure->properties, (structure->nbProperties + 1) * sizeof(struct ODDLProperty)))) {
        return 0;
    }
    structure->properties = tmp;
    structure->properties[structure->nbProperties] = *property;
    structure->nbProperties++;
    return 1;
}

int oddl_structure_add_string_property(struct ODDLStructure* structure, const char* identifier, const char* value) {
    struct ODDLProperty prop = {0};
    if (!(prop.identifier = malloc(strlen(identifier) + 1)) || !(prop.str = malloc(strlen(value) + 1))) {
        free(prop.identifier);
        return 0;
    }
    strcpy(prop.identifier, identifier);
    strcpy(prop.str, value);
    if (oddl_structure_add_property(structure, &prop)) {
        return 1;
    }
    free(prop.identifier);
    free(prop.str);
    return 0;
}

int oddl_structure_add_double_property(struct ODDLStructure* structure, const char* identifier, double value) {
    struct ODDLProperty prop = {0};
    if (!(prop.identifier = malloc(strlen(identifier) + 1))) {
        return 0;
    }
    strcpy(prop.identifier, identifier);
    prop.dbl = value;
    prop.llint = value;
    if (oddl_structure_add_property(structure, &prop)) {
        return 1;
    }
    free(prop.identifier);
    return 0;
}

int oddl_structure_add_uint_property(struct ODDLStructure* structure, const char* identifier, uint64_t value) {
    struct ODDLProperty prop = {0};
    if (!(prop.identifier = malloc(strlen(identifier) + 1))) {
        return 0;
    }
    strcpy(prop.identifier, identifier);
    prop.llint = value;
    if (oddl_structure_add_property(structure, &prop)) {
        return 1;
    }
    free(prop.identifier);
    return 0;
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
