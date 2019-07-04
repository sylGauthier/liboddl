#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdint.h>
#include <liboddl/liboddl.h>

void* print_bool(void* in) {
    char* val = in;
    if (*val) printf("true"); else printf("false");
    return val + 1;
}

void* print_int8(void* in) {
    printf("%d", *(int8_t*)in);
    return ((int8_t*)in)+1;
}

void* print_int16(void* in) {
    printf("%d", *(int16_t*)in);
    return ((int16_t*)in)+1;
}

void* print_int32(void* in) {
    printf("%d", *(int32_t*)in);
    return ((int32_t*)in)+1;
}

void* print_int64(void* in) {
    printf("%ld", *(int64_t*)in);
    return ((int64_t*)in)+1;
}

void* print_uint8(void* in) {
    printf("%d", *(uint8_t*)in);
    return ((uint8_t*)in)+1;
}

void* print_uint16(void* in) {
    printf("%d", *(uint16_t*)in);
    return ((uint16_t*)in)+1;
}

void* print_uint32(void* in) {
    printf("%d", *(uint32_t*)in);
    return ((uint32_t*)in)+1;
}

void* print_uint64(void* in) {
    printf("%ld", *(uint64_t*)in);
    return ((uint64_t*)in)+1;
}

void* print_float16(void* in) {
    printf("?half?");
    return ((char*)in)+2;
}

void* print_float32(void* in) {
    printf("%f", *(float*)in);
    return ((float*)in)+1;
}

void* print_float64(void* in) {
    printf("%f", *(double*)in);
    return ((double*)in)+1;
}

void* print_string(void* in) {
    printf("\"%s\"", *(char**) in);
    return ((char**)in)+1;
}

void* print_ref(void* in) {
    struct ODDLRef* ref = in;

    printf(ref->refStr);
    return ((char**)in)+1;
}

void* print_type(void* in) {
    printf("%s", typeName[(int)*(char*)in]);
    return ((char*)in)+1;
}

#endif
