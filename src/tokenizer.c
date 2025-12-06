/*
    Copyright (2025) Rohan Vashisht
    Repository: https://github.com/zigistry/build_zig_zon_parser
    Check the LICENSE file for more information
*/

#include "../include/bzz.h"

/**
 * Basic build.zig.zon tokenizer.
 * @param build_zig_zon_raw_data is the raw build.zig.zon data.
 *
 * @param tokens is pre allocated memory. tokens are null terminated by this function.
 *
 * @return TOKENIZING_ERROR or TOKENIZING_SUCCESSFULL.
 */
int tokenize(const char* build_zig_zon_raw_data, Token* tokens, const size_t max_tokens)
{
    int i = 0, count = 0;
    // I am doing - 1 to add a null terminator and avoid buffer overflow
    while (build_zig_zon_raw_data[i] && count < max_tokens - 1) {
        if (isspace(build_zig_zon_raw_data[i])) {
            i++;
            continue;
        } else if (build_zig_zon_raw_data[i] == '/') {
            i++;
            if (build_zig_zon_raw_data[i] && build_zig_zon_raw_data[i] == '/') {
                // this is a comment
                while (build_zig_zon_raw_data[i] && build_zig_zon_raw_data[i] != '\n') {
                    i++;
                }
            } else {
                LOG_ERROR("Found a '/' not supported in build.zig.zon.");
                return TOKENIZING_ERROR;
            }
        } else if (build_zig_zon_raw_data[i] == '.') {
            tokens[count++] = (Token) { DOT, "." };
        } else if (build_zig_zon_raw_data[i] == '{') {
            tokens[count++] = (Token) { L_BRACE, "{" };
        } else if (build_zig_zon_raw_data[i] == '}') {
            tokens[count++] = (Token) { R_BRACE, "}" };
        } else if (build_zig_zon_raw_data[i] == '=') {
            tokens[count++] = (Token) { EQUALS, "=" };
        } else if (build_zig_zon_raw_data[i] == '"') {
            i++; // skip opening quote
            int capacity = 101;
            char* any_string = malloc(sizeof(char) * capacity);
            int string_pos = 0;
            // I am doing - 1 to add a null terminator, same here
            while (build_zig_zon_raw_data[i]) {
                if (build_zig_zon_raw_data[i] == '\\' && build_zig_zon_raw_data[i + 1]) {
                    any_string[string_pos++] = build_zig_zon_raw_data[i++];
                    any_string[string_pos++] = build_zig_zon_raw_data[i++];
                    if (capacity == string_pos + 1) {
                        capacity = capacity + 100;
                        any_string = realloc(any_string, sizeof(char) * capacity);
                    }
                } else if (build_zig_zon_raw_data[i] == '"') {
                    i++;
                    break;
                } else {
                    any_string[string_pos++] = build_zig_zon_raw_data[i++];
                }
            }
            any_string[string_pos] = '\0';
            tokens[count++] = (Token) { STRING, any_string };
            continue;
        } else if (build_zig_zon_raw_data[i] == ',') {
            tokens[count++] = (Token) { COMMA, "," };
        } else if (isalpha(build_zig_zon_raw_data[i]) || build_zig_zon_raw_data[i] == '_') {
            int capacity = 101;
            char* identifier = malloc(sizeof(char) * capacity);
            int identifier_position = 0;
            while (isalnum(build_zig_zon_raw_data[i]) || build_zig_zon_raw_data[i] == '_') {
                identifier[identifier_position++] = build_zig_zon_raw_data[i++];
                if (capacity == identifier_position + 1) {
                    capacity = capacity + 100;
                    identifier = realloc(identifier, sizeof(char) * capacity);
                }
            }
            identifier[identifier_position] = '\0';
            tokens[count++] = (Token) { IDENTIFIER, identifier };
            continue;
        } else if (build_zig_zon_raw_data[i] == '0') {
            int capacity = 101;
            int pos = 0;
            char* my_integer = malloc(sizeof(char) * capacity);

            my_integer[0] = '0';
            i++;
            pos++;

            if (build_zig_zon_raw_data[i]) {
                my_integer[pos] = build_zig_zon_raw_data[i];
                if (build_zig_zon_raw_data[i] == 'b') {
                    my_integer[1] = 'b';
                    do {
                        i++;
                        pos++;
                        // -1 for \0 and -1 for the next character hence doing -2.
                        if (pos == capacity - 2) {
                            capacity += 100;
                            my_integer = realloc(my_integer, sizeof(char) * capacity);
                        }
                        my_integer[pos] = build_zig_zon_raw_data[i];
                    } while (build_zig_zon_raw_data[i] && (build_zig_zon_raw_data[i] == '0' || build_zig_zon_raw_data[i] == '1' || build_zig_zon_raw_data[i] == '_'));
                    tokens[count++] = (Token) { BINARY, my_integer };
                } else if (build_zig_zon_raw_data[i] == 'o') {
                    my_integer[1] = 'o';
                    do {
                        i++;
                        pos++;
                        // -1 for \0 and -1 for the next character hence doing -2.
                        if (pos == capacity - 2) {
                            capacity += 100;
                            my_integer = realloc(my_integer, sizeof(char) * capacity);
                        }
                        my_integer[pos] = build_zig_zon_raw_data[i];
                    } while (build_zig_zon_raw_data[i] && (build_zig_zon_raw_data[i] >= '0' && build_zig_zon_raw_data[i] <= '8'));
                    tokens[count++] = (Token) { OCTAL, my_integer };
                } else if (build_zig_zon_raw_data[i] == 'x') {
                    my_integer[1] = 'x';
                    do {
                        i++;
                        pos++;
                        // -1 for \0 and -1 for the next character hence doing -2.
                        if (pos == capacity - 2) {
                            capacity += 100;
                            my_integer = realloc(my_integer, sizeof(char) * capacity);
                        }
                        my_integer[pos] = build_zig_zon_raw_data[i];
                    } while (build_zig_zon_raw_data[i] && ((build_zig_zon_raw_data[i] >= '0' && build_zig_zon_raw_data[i] <= '9') || (build_zig_zon_raw_data[i] >= 'A' && build_zig_zon_raw_data[i] <= 'F') || (build_zig_zon_raw_data[i] >= 'a' && build_zig_zon_raw_data[i] <= 'f')));
                    my_integer[pos] = '\0';
                    tokens[count++] = (Token) { HEXADECIMAL, my_integer };
                } else {
                    return TOKENIZING_ERROR;
                }
            }
        } else if (build_zig_zon_raw_data[i] >= '1' && build_zig_zon_raw_data[i] <= '9') {
            int capacity = 101;
            int pos = 0;
            char* my_integer = malloc(sizeof(char) * capacity);
            my_integer[pos] = build_zig_zon_raw_data[i];
            do {
                i++;
                pos++;
                if (pos == capacity - 2) {
                    capacity += 100;
                    my_integer = realloc(my_integer, sizeof(char) * capacity);
                }
                my_integer[pos] = build_zig_zon_raw_data[i];
            } while (build_zig_zon_raw_data[i] >= '0' && build_zig_zon_raw_data[i] <= '9');
            tokens[count++] = (Token) { INTEGER, my_integer };
        }
        i++;
    }
    tokens[count] = (Token) { 0 };
    return TOKENIZING_SUCCESSFULL;
}

/**
 * Frees the tokes that were allocated.
 *
 * @param tokens List of tokens that were passed for tokenization
 *
 * @return Nothing
 */
void free_tokens(Token* tokens)
{
    for (int i = 0; tokens[i].value; i++) {
        if (tokens[i].type == STRING || tokens[i].type == IDENTIFIER || tokens[i].type == INTEGER) {
            free(tokens[i].value);
        }
    }
}
