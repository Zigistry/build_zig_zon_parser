/*
    Copyright (2025) Rohan Vashisht
    Repository: https://github.com/zigistry/build_zig_zon_parser
    Check the LICENSE file for more information
*/

#ifndef BZZ_H
#define BZZ_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TOKENIZING_ERROR -1
#define TOKENIZING_SUCCESSFULL 0
#define INTEGER_STARTS_WITH_0_ERROR -2

#define PARSING_ERROR -1
#define PARSING_SUCCESSFUL 0

#ifndef BZZ_DONT_LOG
#define LOG_ERROR(fmt, ...) fprintf(stderr, "\033[1;31m" fmt "\033[0m", ##__VA_ARGS__)
#define LOG_WARNING(fmt, ...) fprintf(stderr, "\033[33m" fmt "\033[0m", ##__VA_ARGS__)
#else
#define LOG_ERROR(fmt, ...) \
    do {                    \
    } while (0)
#define LOG_WARNING(fmt, ...) \
    do {                      \
    } while (0)
#endif

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
    char* name;
    char* url;
    char* hash;
    bool lazy;
    char* path;
} Dependency;

typedef struct {
    char* name;
    unsigned long long fingerprint;
    char* version;
    char* minimum_zig_version;
    Dependency* dependencies;
    int dependency_count;
    char* paths;
} build_zig_zon_parsed_data;

int parse(const Token* const tokens, build_zig_zon_parsed_data* result);
int tokenize(const char* build_zig_zon_raw_data, Token* tokens, const size_t max_tokens);
void print_build_zig_zon_parsed_data(build_zig_zon_parsed_data data);
void free_tokens(Token* tokens);
void free_build_zig_zon_parsed_data(build_zig_zon_parsed_data* data);

#ifdef __cplusplus
}
#endif

#endif /* BZZ_H */
