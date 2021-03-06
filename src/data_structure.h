#ifndef DATA_STRUCTURE_H
#define DATA_STRUCTURE_H

#include <liboddl/liboddl.h>
#include "tokens.h"

int parse_data_structure(struct ODDLDoc* doc,
                         struct ODDLStructure* st,
                         enum ODDLTokens typeToken);
#endif
