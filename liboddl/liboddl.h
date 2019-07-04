#ifndef LIBODDL_H
#define LIBODDL_H

#include <stdio.h>
#include <stdint.h>

#define TYPE_SIZE(t) (((t) == TYPE_BOOL || (t) == TYPE_UINT8 || (t) == TYPE_INT8 || (t) == TYPE_TYPE)*1 + \
                      ((t) == TYPE_UINT16 || (t) == TYPE_INT16 || (t) == TYPE_FLOAT16)*2 + \
                      ((t) == TYPE_UINT32 || (t) == TYPE_INT32 || (t) == TYPE_FLOAT32)*4 + \
                      ((t) == TYPE_UINT64 || (t) == TYPE_INT64 || (t) == TYPE_FLOAT64)*8 + \
                      ((t) == TYPE_STRING)*sizeof(char*) + \
                      ((t) == TYPE_REF)*sizeof(struct ODDLRef))

/* ODDL types */
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

/* The long name of each type */
extern char* typeName[TYPE_TYPE+1];

/* Structure name */
struct ODDLName {
    int globalNameIdx;  /* If the name is global, index in the global name array, -1 otherwise */
    char* str;          /* The name string, NULL if none, gets freed in oddl_free() */
};

/* The ODDL base structure */
struct ODDLStructure {
    struct ODDLName name;           /* Structure's name */
    struct ODDLStructure* parent;   /* Structure's parent, NULL only for the root */

    /* Each structure is either a data structure containing an array of basic type
     * or a meta structure containing any number of sub structures.
     */

    /* Data structure components */
    enum ODDLDataType type; /* Type of data */
    unsigned int vecSize;   /* Size of each individual vector in number of elements */
    unsigned int nbVec;     /* Number of vectors in the array */
    void* dataList;         /* The (flat) vector array, of size nbVec * vecSize * TYPE_SIZE(type) */
                            /* Gets freed in oddl_free() */

    /* Meta structure components */
    char* identifier;                   /* The identifier, cannot be NULL, gets freed in oddl_free() */
    unsigned int nbProperties;          /* Number of properties */
    unsigned int nbStructures;          /* Number of structures */
    struct ODDLProperty* properties;    /* The array of properties, freed in oddl_free() */
    struct ODDLStructure** structures;  /* The array of sub structures, freed in oddl_free() */
};

/* The Ref structure */
struct ODDLRef {
    /* The ref string, eg. $st1%st2%st3, will be exported as-is in oddl_write()
     * so it's the user's responsibility to ensure the validity of this string when
     * setting up a structure tree to be exported. Freed in oddl_free()
     */
    char* refStr;

    struct ODDLStructure* ref; /* A pointer to the resolved ref. Set when imported, useless for export */
};

/* A property */
struct ODDLProperty {
    /* Because the type of the property should be inferred by the user, we set
     * all the types that are applicable for the literal encountered.
     */
    char* identifier;       /* Identifier, left of the = sign, freed in oddl_free() */
    uint64_t llint;         /* The long long int value, will get set for bools, all (unsigned) ints, floats */
    double dbl;             /* The double value, will get set for floats and integers */
    char* str;              /* String value, set only for string literals */
    struct ODDLRef ref;     /* Ref value, set only for ref strings and null */
    enum ODDLDataType type; /* Type value, set only for type identifiers */
};

/* A global name */
struct ODDLGlobalName {
    char* str;                  /* Name literal, freed in oddl_free() */
    struct ODDLStructure* ref;  /* A pointer to the structure bearing the name */
};

/* The top level ODDL document */
struct ODDLDoc {
    struct ODDLStructure* root;         /* The root, virtual meta structure, freed in oddl_free()*/

    unsigned int nbGlobalNames;         /* Number of global names */
    struct ODDLGlobalName* globalNames; /* All the global names, freed in oddl_free() */
};

/* Parses a file, build the tree, allocate and populate all the structures */
int oddl_parse(struct ODDLDoc* doc, FILE* file);

/* Writes an existing tree into a file, doesn't check the validity of the tree, the user is trusted */
int oddl_write(struct ODDLDoc* doc, FILE* file);

/* Frees all the memory allocated in a tree */
void oddl_free(struct ODDLDoc* doc);

/* Returns a pointer to the property bearing the name propName in the structure st, or NULL if no such property */
struct ODDLProperty* oddl_get_property(struct ODDLStructure* st, char* propName);

/* Allocates and zeroes a new ODDLStructure */
struct ODDLStructure* oddl_new_structure();
#endif
