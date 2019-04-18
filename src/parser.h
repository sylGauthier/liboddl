#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdint.h>

#define TYPE_SIZE(t) (((t) == TYPE_BOOL || (t) == TYPE_UINT8 || (t) == TYPE_INT8 || (t) == TYPE_TYPE)*1 + \
                      ((t) == TYPE_UINT16 || (t) == TYPE_INT16 || (t) == TYPE_FLOAT16)*2 + \
                      ((t) == TYPE_UINT32 || (t) == TYPE_INT32 || (t) == TYPE_FLOAT32)*4 + \
                      ((t) == TYPE_UINT64 || (t) == TYPE_INT64 || (t) == TYPE_FLOAT64)*8 + \
                      ((t) == TYPE_STRING)*sizeof(char*) + \
                      ((t) == TYPE_REF)*sizeof(struct ODDLRef))

enum ODDLDataType {
    TYPE_BOOL = 1,
    TYPE_INT8,
    TYPE_INT16,
    TYPE_INT32,
    TYPE_INT64,
    TYPE_UINT8,
    TYPE_UINT16,
    TYPE_UINT32,
    TYPE_UINT64,
    TYPE_FLOAT16,
    TYPE_FLOAT32,
    TYPE_FLOAT64,
    TYPE_STRING,
    TYPE_REF,
    TYPE_TYPE
};

extern char* typeName[TYPE_TYPE+1];

struct ODDLStructure;
struct ODDLRef;

struct ODDLName {
    int globalNameIdx;
    char* str;
};

struct ODDLStructure {
    struct ODDLName name;
    struct ODDLStructure* parent;

    /* Data structure components */
    enum ODDLDataType type;
	unsigned int vecSize;
    unsigned int nbVec;
    void* dataList;

    /* Meta structure components */
    char* identifier;
    unsigned int nbProperties;
    unsigned int nbStructures;
    struct ODDLProperty* properties;
    struct ODDLStructure** structures;
};

struct ODDLRef {
    char* refStr;
    struct ODDLStructure* ref;
};

struct ODDLProperty {
    char* identifier;
    uint64_t llint;
    double dbl;
    char* str;
    struct ODDLRef ref;
    enum ODDLDataType type;
};

struct ODDLGlobalName {
    char* str;
    struct ODDLStructure* ref;
};

struct ODDLParser {
    struct ODDLStructure* root;

    unsigned int nbGlobalNames;
    struct ODDLGlobalName* globalNames;
};

int oddl_parse(struct ODDLParser* parser, FILE* file);
void oddl_free(struct ODDLParser* parser);

#endif
