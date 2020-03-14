#include <string.h>

#include <liboddl/liboddl.h>

struct ODDLProperty* oddl_get_property(const struct ODDLStructure* st, const char* propName) {
    int i;

    if (!(st->properties)) {
        return NULL;
    }

    for (i = 0; i < st->nbProperties; i++) {
        if (!(strcmp(st->properties[i].identifier, propName))) {
            return st->properties + i;
        }
    }
    return NULL;
}
