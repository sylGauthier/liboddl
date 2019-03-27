#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

enum ODDLDataType {
    BOOL,
    INT8,
    INT16,
    INT32,
    INT64,
    UINT8,
    UINT16,
    UINT32,
    UINT64,
    FLOAT16,
    FLOAT32,
    FLOAT64,
    STRING,
    REF,
    TYPE,
};

enum ODDLStructType {
    DATA_STRUCTURE,
    META_STRUCTURE
};

struct ODDLDataList {
    enum ODDLDataType type;
    unsigned int size;
    void* data;
};

struct ODDLDataStructure {
    unsigned int name;
    enum ODDLDataType type;
    unsigned int size;
    struct ODDLDataList* dataList;
};

struct ODDLMetaStructure {
    unsigned int identifier;
    unsigned int name;
    unsigned int nbProperties;
    unsigned int nbStructures;
    struct ODDLProperty* properties;
    struct ODDLStructure* structures;
};

union ODDLObject {
    struct ODDLMetaStructure;
    struct ODDLDataStructure;
};

struct ODDLStructure {
    enum ODDLStructType type;
    union ODDLObject structure;
};

struct ODDLRef {
    enum ODDLStructType type;
    void* ref;
};

struct ODDLIdentifier {
    char* name;
    void* object;
};

struct ODDLParser {
    struct ODDLStructure* root;

    unsigned int nbIdentifiers;
    struct ODDLIdentifier* identifiers;
};

struct ODDLParser* oddl_parse(FILE* file);

#endif
