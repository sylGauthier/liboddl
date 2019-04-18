#ifndef PARSE_LIT_H
#define PARSE_LIT_H

#include "tokens.h"
#include "parser.h"

struct ODDLStructure* new_structure();

void invalid_token(enum ODDLTokens token);
int find_global_name(struct ODDLParser* parser, const char* name);

int parse_int(void* in, unsigned size);
int parse_uint(void* in, unsigned size);
int parse_bool(void* in, unsigned size);
int parse_float(void* in, unsigned size);
int parse_string(void* in, unsigned size);
int parse_ref(void* in, unsigned size);
int parse_type(void* in, unsigned size);

int parse_global_name(struct ODDLParser* parser, struct ODDLStructure* cur);
int parse_local_name(struct ODDLParser* parser, struct ODDLStructure* cur);

#endif
