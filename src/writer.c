#include <liboddl/liboddl.h>

#include "write_utils.h"

#define INDENT(file, lvl) { int i; for (i = 0; i < (lvl); i++) fprintf((file), "    "); }

static void print_property(struct ODDLProperty* prop, FILE* file) {
    if (prop->dbl != 0.) {
        fprintf(file, "%f", prop->dbl);
    } else if (prop->llint) {
        fprintf(file, "%ld", prop->llint);
    } else if (prop->str) {
        fprintf(file, "\"%s\"", prop->str);
    } else if (prop->ref.ref) {
        print_ref(file, &prop->ref);
    } else if (prop->type) {
        fprintf(file, "%s", typeName[prop->type]);
    } else {
        fprintf(file, "0");
    }
}

static int write_data_structure(struct ODDLStructure* st, FILE* file, unsigned level) {
    void* (*print_clbk)(FILE* f, void* data);
    void* ptr;
    unsigned i, j;

    switch (st->type) {
        case TYPE_BOOL:     print_clbk = print_bool; break;
        case TYPE_INT8:     print_clbk = print_int8; break;
        case TYPE_INT16:    print_clbk = print_int16; break;
        case TYPE_INT32:    print_clbk = print_int32; break;
        case TYPE_INT64:    print_clbk = print_int64; break;
        case TYPE_UINT8:    print_clbk = print_uint8; break;
        case TYPE_UINT16:   print_clbk = print_uint16; break;
        case TYPE_UINT32:   print_clbk = print_uint32; break;
        case TYPE_UINT64:   print_clbk = print_uint64; break;
        case TYPE_FLOAT16:  print_clbk = print_float16; break;
        case TYPE_FLOAT32:  print_clbk = print_float32; break;
        case TYPE_FLOAT64:  print_clbk = print_float64; break;
        case TYPE_STRING:   print_clbk = print_string; break;
        case TYPE_REF:      print_clbk = print_ref; break;
        case TYPE_TYPE:     print_clbk = print_type; break;
        default:            return 0;
    }
    INDENT(file, level) fprintf(file, "%s", typeName[st->type]);
    if (st->vecSize >= 2) fprintf(file, "[%d] ", st->vecSize);
    else fprintf(file, " ");
    if (st->name.str) fprintf(file, "%c%s ", st->name.globalNameIdx >= 0 ? '$' : '%', st->name.str);
    fprintf(file, "{");
    ptr = st->dataList;
    for (i = 0; i < st->nbVec; i++) {
        if (st->vecSize >= 2) {
            fprintf(file, "{");
            for (j = 0; j < st->vecSize; j++) {
                ptr = print_clbk(file, ptr);
                if (j < st->vecSize-1) fprintf(file, ", ");
            }
            fprintf(file, "}");
        } else {
            ptr = print_clbk(file, ptr);
        }
        if (i < st->nbVec-1) fprintf(file, ", ");
    }
    fprintf(file, "}\n");
    return 1;
}

static int write_meta_structure(struct ODDLStructure* st, FILE* file, unsigned level) {
    unsigned i;
    
    INDENT(file, level) fprintf(file, "%s ", st->identifier);
    if (st->name.str) fprintf(file, "%c%s ", st->name.globalNameIdx >= 0 ? '$' : '%', st->name.str);
    if (st->nbProperties) {
        fprintf(file, "(");
        for (i = 0; i < st->nbProperties; i++) {
            fprintf(file, "%s = ", st->properties[i].identifier);
            print_property(st->properties + i, file);
            if (i < st->nbProperties - 1) fprintf(file, ", ");
        }
        fprintf(file, ") ");
    }
    fprintf(file, "{\n");
    for (i = 0; i < st->nbStructures; i++) {
        if (st->structures[i]->dataList)
            write_data_structure(st->structures[i], file, level + 1);
        else
            write_meta_structure(st->structures[i], file, level + 1);
        if (i < st->nbStructures - 1)
            fprintf(file, "\n");
    }
    INDENT(file, level) fprintf(file, "}\n");
    return 1;
}

int oddl_write(struct ODDLDoc* doc, FILE* file) {
    unsigned i;

    for (i = 0; i < doc->root->nbStructures; i++) {
        if (doc->root->structures[i]->dataList) {
            write_data_structure(doc->root->structures[i], file, 0);
        } else {
            write_meta_structure(doc->root->structures[i], file, 0);
        }
        if (i < doc->root->nbStructures - 1)
            fprintf(file, "\n");
    }
    return 1;
}
