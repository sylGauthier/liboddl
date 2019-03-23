#include <string.h>
#include <stdlib.h>

#include "string_utils.h"

char* str_copy(char* str) {
    unsigned int strLen;
    char* ret;

    strLen = strlen(str);
    if (!(ret = malloc(strLen*sizeof(char)))) {
        return NULL;
    }
    memcpy(ret, str, strLen);
    return ret;
}
