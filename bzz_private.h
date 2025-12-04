#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOKENIZING_ERROR -1
#define TOKENIZING_SUCCESSFULL 0
#define INTEGER_STARTS_WITH_0_ERROR -2

#define PARSING_ERROR -1
#define PARSING_SUCCESSFUL 0

// Limiting any string in buid.zig.zon to be 400

typedef enum {
    DOT,
    L_BRACE,
    R_BRACE,
    EQUALS,
    STRING,
    COMMA,
    IDENTIFIER,
    BINARY,
    OCTAL,
    HEXADECIMAL,
    INTEGER,
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

int tokenize(const char* build_zig_zon_raw_data, Token* tokens, const size_t max_tokens);
void free_tokens(Token* tokens);
