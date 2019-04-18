#include <stdio.h>

#include "../src/parser.h"
#include "test-utils.h"

#define INDENT(l) {int i; for(i = 0; i < (l); i++) printf("    ");}

void print_help(char* cmd) {
    printf("Usage: %s <filename>\n", cmd);
}

void print_data_list(void* dataList, enum ODDLDataType type, unsigned int nbVec, unsigned int vecSize) {
    void* (*print_clbk)(void* data);
    void* ptr;
    int i, j;

    switch (type) {
        case TYPE_BOOL:
            print_clbk = print_bool;
            break;
        case TYPE_INT8:
            print_clbk = print_int8;
            break;
        case TYPE_INT16:
            print_clbk = print_int16;
            break;
        case TYPE_INT32:
            print_clbk = print_int32;
            break;
        case TYPE_INT64:
            print_clbk = print_int64;
            break;
        case TYPE_UINT8:
            print_clbk = print_uint8;
            break;
        case TYPE_UINT16:
            print_clbk = print_uint16;
            break;
        case TYPE_UINT32:
            print_clbk = print_uint32;
            break;
        case TYPE_UINT64:
            print_clbk = print_uint64;
            break;
        case TYPE_FLOAT16:
            print_clbk = print_float16;
            break;
        case TYPE_FLOAT32:
            print_clbk = print_float32;
            break;
        case TYPE_FLOAT64:
            print_clbk = print_float64;
            break;
        case TYPE_STRING:
            print_clbk = print_string;
            break;
        case TYPE_REF:
            print_clbk = print_ref;
            break;
        case TYPE_TYPE:
            print_clbk = print_type;
            break;
        default:
            break;
    }
    printf("{");
    ptr = dataList;
    for (i = 0; i < nbVec; i++) {
        if (vecSize) {
            printf("{");
            for (j = 0; j < vecSize; j++) {
                ptr = print_clbk(ptr);
                if (j < vecSize-1) printf(", ");
            }
            printf("}");
        } else {
            ptr = print_clbk(ptr);
        }
        if (i < nbVec-1) printf(", ");
    }
    printf("}\n");
}

void print_name(struct ODDLStructure* st, int level) {
    if (st->name.str) {
        if (st->name.globalNameIdx >= 0)
            printf("$");
        printf("%s", st->name.str);
        printf(" ");
    }
}

void print_property(struct ODDLProperty* prop) {
    printf("%s = ", prop->identifier);
    if (prop->dbl != 0.) {
        printf("%lf", prop->dbl);
    } else if (prop->llint) {
        printf("%ld", prop->llint);
    } else if (prop->str) {
        printf("\"%s\"", prop->str);
    } else if (prop->ref.ref) {
        print_ref(&prop->ref);
    } else if (prop->type) {
        printf("%s", typeName[prop->type]);
    } else {
        printf("0");
    }
}

void print_structure(struct ODDLStructure* st, int level) {
    int i;

    if (st->nbStructures) {
        INDENT(level) print_name(st, level); printf("%s <%p> ", st->identifier, st);
        if (st->nbProperties) {
            printf("(");
            for (i = 0; i < st->nbProperties; i++) {
                print_property(st->properties+i);
                printf(",");
            }
            printf(")");
        }
        printf(" {\n");
        for (i = 0; i < st->nbStructures; i++) {
            print_structure(st->structures[i], level+1);
        }
        INDENT(level) printf("}\n");
    } else {
        INDENT(level) print_name(st, level);
        printf("%s[%d][%d]: ", typeName[st->type], st->nbVec, st->vecSize);
        print_data_list(st->dataList, st->type, st->nbVec, st->vecSize);
    }
}

void print_global_names(struct ODDLParser* doc) {
    int i;

    for ( i = 0; i < doc->nbGlobalNames; i++) {
        printf("%d: %s -> %p\n", i, doc->globalNames[i].str, (void*)doc->globalNames[i].ref);
    }
}

int main(int argc, char** argv) {
    FILE* file;
    struct ODDLParser oddlDoc;
    int success;

    if (argc < 2) {
        print_help(argv[0]);
        return 1;
    }

    if (!(file = fopen(argv[1], "r"))) {
        fprintf(stderr, "Error: can't open file %s\n", argv[1]);
        return 1;
    }

    success = oddl_parse(&oddlDoc, file);
    if (!success) {
        fprintf(stderr, "Error in parsing module\n");
        return 1;
    }

    printf("### Global names ###\n");
    print_global_names(&oddlDoc);
    printf("### Tree ###\n");
    print_structure(oddlDoc.root, 0);
    oddl_free(&oddlDoc);
    oddlDoc.root = NULL;
    oddlDoc.globalNames = NULL;
    fclose(file);
    return 0;
}
