#ifndef WRITE_UTILS_H
#define WRITE_UTILS_H

#include <stdint.h>
#include <liboddl/liboddl.h>

void* print_bool(FILE* f, void* in) {
    char* val = in;
    if (*val) fprintf(f, "true"); else fprintf(f, "false");
    return val + 1;
}

void* print_int8(FILE* f, void* in) {
    fprintf(f, "%d", *(int8_t*)in);
    return ((int8_t*)in)+1;
}

void* print_int16(FILE* f, void* in) {
    fprintf(f, "%d", *(int16_t*)in);
    return ((int16_t*)in)+1;
}

void* print_int32(FILE* f, void* in) {
    fprintf(f, "%d", *(int32_t*)in);
    return ((int32_t*)in)+1;
}

void* print_int64(FILE* f, void* in) {
    fprintf(f, "%ld", *(int64_t*)in);
    return ((int64_t*)in)+1;
}

void* print_uint8(FILE* f, void* in) {
    fprintf(f, "%d", *(uint8_t*)in);
    return ((uint8_t*)in)+1;
}

void* print_uint16(FILE* f, void* in) {
    fprintf(f, "%d", *(uint16_t*)in);
    return ((uint16_t*)in)+1;
}

void* print_uint32(FILE* f, void* in) {
    fprintf(f, "%d", *(uint32_t*)in);
    return ((uint32_t*)in)+1;
}

void* print_uint64(FILE* f, void* in) {
    fprintf(f, "%ld", *(uint64_t*)in);
    return ((uint64_t*)in)+1;
}

void* print_float16(FILE* f, void* in) {
    fprintf(f, "?half?");
    return ((char*)in)+2;
}

void* print_float32(FILE* f, void* in) {
    fprintf(f, "%f", *(float*)in);
    return ((float*)in)+1;
}

void* print_float64(FILE* f, void* in) {
    fprintf(f, "%f", *(double*)in);
    return ((double*)in)+1;
}

void* print_string(FILE* f, void* in) {
    fprintf(f, "\"%s\"", *(char**) in);
    return ((char**)in)+1;
}

void* print_ref(FILE* f, void* in) {
    struct ODDLRef* ref = in;

    if (ref->refStr) {
        fprintf(f, "%s", ref->refStr);
    } else {
        fprintf(f, "null");
    }
    return ((struct ODDLRef*)in)+1;
}

void* print_type(FILE* f, void* in) {
    fprintf(f, "%s", typeName[(int)*(char*)in]);
    return ((char*)in)+1;
}

#endif
