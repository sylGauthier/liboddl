#ifndef PARSE_LIT_H
#define PARSE_LIT_H

#include "tokens.h"
#include <liboddl/liboddl.h>

void invalid_token(enum ODDLTokens token);
int find_global_name(struct ODDLDoc* doc, const char* name);

int parse_global_name(struct ODDLDoc* doc, struct ODDLStructure* cur);
int parse_local_name(struct ODDLDoc* doc, struct ODDLStructure* cur);

#endif
