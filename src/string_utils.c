#include <string.h>
#include <stdlib.h>

#include "string_utils.h"

char* str_copy(char* str) {
    unsigned int strSize;
    char* ret;

    strSize = strlen(str) + 1;
    if (!(ret = malloc(strSize*sizeof(char)))) {
        return NULL;
    }
    memcpy(ret, str, strSize);
    return ret;
}

char* str_lit_copy(char* str) {
    unsigned strSize;
    char* ret;

    strSize = strlen(str) - 1;
    if (!(ret = malloc(strSize*sizeof(char)))) {
        return NULL;
    }
    memcpy(ret, str+1, strSize-1);
    ret[strSize-1] = 0;
    return ret;
}

char* str_prefix(char prefix, char* str) {
    unsigned strSize;
    char* ret;

    strSize = strlen(str) + 2;
    if (!(ret = malloc(strSize*sizeof(char)))) {
        return NULL;
    }
    ret[0] = prefix;
    memcpy(ret+1, str, strSize-1);
    return ret;
}
