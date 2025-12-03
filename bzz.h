#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define TOKENIZING_ERROR -1
#define TOKENIZING_SUCCESSFULL 0

#define PARSING_ERROR -1
#define PARSING_SUCCESSFUL 0


// Limiting any string in buid.zig.zon to be 400
#define MAX_STRING_LENGTH 400

typedef enum {
    DOT,
    L_BRACE,
    R_BRACE,
    EQUALS,
    STRING,
    COMMA,
    IDENTIFIER,
} types;

typedef struct {
    types type;
    char* value;
} Token;

typedef struct {
    char* url;
    char* hash;
    _Bool lazy;
    char* path;
} Dependency;

typedef struct {
    char* name;
    uint64_t* fingerprint;
    char* version;
    char* minimum_zig_version;
    Dependency* dependencies;
    char* paths[];
} build_zig_zon_parsed_data;
