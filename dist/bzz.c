#include "bzz.h"

/**
 * Prints build_zig_zon_parsed_data struct.
 * @param data build_zig_zon_parsed_data struct.
 *
 * @return Nothing
 */
void print_build_zig_zon_parsed_data(const build_zig_zon_parsed_data data)
{
    printf("name: %s\n", data.name);
    printf("minimum_zig_version: %s\n", data.minimum_zig_version);
    printf("version: %s\n", data.version);
    printf("fingerprint: %llu\n", data.fingerprint);

    for (int i = 0; i < data.dependency_count; i++) {
        printf("Name: %s\n", data.dependencies[i].name);
        printf("    > Hash: %s\n", data.dependencies[i].hash);
        printf("    > Lazy: %d\n", data.dependencies[i].lazy);
        printf("    > Url: %s\n", data.dependencies[i].url);
        printf("    > Path: %s\n", data.dependencies[i].path);
    }
}

/**
 * Parses tokens to get meaningful information
 * out of tokenized build.zig.zon.
 * @param tokens expects null terminated list of tokens.
 *
 * @param result The resulting pointer to a build_zig_zon_parsed_data
 * struct. The data would be written to this.
 *
 * @return PARSING_ERROR or PARSING_SUCCESSFUL.
 */
int parse(const Token* const tokens, build_zig_zon_parsed_data* result)
{
    int depth = 0;
    for (int i = 0; tokens[i].value; i++) {
        if (tokens[i].type == L_BRACE) {
            depth++;
        } else if (tokens[i].type == R_BRACE) {
            depth--;
        } else if (depth == 1 && tokens[i].type == IDENTIFIER) {
            if (strcmp(tokens[i].value, "minimum_zig_version") == 0) {
                i++;
                if (!tokens[i].value || tokens[i].type != EQUALS) {
                    LOG_ERROR("Expected '=' after \"minimum_zig_version\".'\n");
                    return PARSING_ERROR;
                }
                i++;
                if (!tokens[i].value || tokens[i].type != STRING) {
                    LOG_ERROR("\"minimum_zig_version\" is of type string. Found: %s\n", tokens[i].value);
                    return PARSING_ERROR;
                } else {
                    result->minimum_zig_version = tokens[i].value;
                }
            }
            // Problem I was facing here is that in older build.zig.zon
            // we had name as a string, now, we have it as an identifier.
            // I will implement this in such a way that it can handle
            // both the ways.
            else if (strcmp(tokens[i].value, "name") == 0) {
                i++;
                if (!tokens[i].value || tokens[i].type != EQUALS) {
                    LOG_ERROR("Expected '=' after \"name\".'\n");

                    return PARSING_ERROR;
                }
                i++;
                if (tokens[i].value && tokens[i].type == DOT) {
                    // if its identifier, because zig 0.12.0 had strings
                    // as name, now we have enum literal, both should be
                    // compatible.
                    i++;
                }
                // Hence, can be a string or an identifier.
                if (!tokens[i].value || (tokens[i].type != STRING && tokens[i].type != IDENTIFIER)) {
                    LOG_ERROR("Expected String or an Identifier as name, found: %s'\n", tokens[i].value);
                    return PARSING_ERROR;
                } else {
                    result->name = tokens[i].value;
                }
            } else if (strcmp(tokens[i].value, "version") == 0) {
                i++;
                if (tokens[i].value == NULL || tokens[i].type != EQUALS) {
                    LOG_ERROR("Expected '=' after \"version\".'\n");
                    return PARSING_ERROR;
                }
                i++;
                if (tokens[i].value == NULL || tokens[i].type != STRING) {
                    LOG_ERROR("Expected String as version, found: %s'\n", tokens[i].value);
                    return PARSING_ERROR;
                } else {
                    result->version = tokens[i].value;
                }
            } else if (strcmp(tokens[i].value, "fingerprint") == 0) {
                i++;
                if (!tokens[i].value || tokens[i].type != EQUALS) {
                    LOG_ERROR("Expected '=' after \"fingerprint\".'\n");
                    return PARSING_ERROR;
                }
                i++;
                if (!tokens[i].value || tokens[i].type != HEXADECIMAL) {
                    LOG_ERROR("Expected Hexadecimal as fingerprint, found: %s'\n", tokens[i].value);
                    return PARSING_ERROR;
                } else {
                    result->fingerprint = strtoull(tokens[i].value, NULL, 16);
                }
            } else if (strcmp(tokens[i].value, "dependencies") == 0) {
                i++;
                if (!tokens[i].value || tokens[i].type != EQUALS) {
                    LOG_ERROR("Expected '=' after \"dependencies\".\n");
                    return PARSING_ERROR;
                }
                i++;
                if (!tokens[i].value || tokens[i].type != DOT) {
                    LOG_ERROR("Expected '.' after \"dependencies = \". before '{'\n");
                    return PARSING_ERROR;
                }
                i++;
                if (!tokens[i].value || tokens[i].type != L_BRACE) {
                    LOG_ERROR("Expected '{' after \"dependencies = .\" found: %s\n", tokens[i].value);
                    return PARSING_ERROR;
                } else {
                    depth++;
                }
                i++;
                // from here we will put a loop to loop the dependencies
                result->dependency_count = 0;
                int dependency_capacity = 20;
                result->dependencies = malloc(sizeof(Dependency) * dependency_capacity);
                while (tokens[i].value && depth > 1) {
                    if (tokens[i].value && tokens[i].type == R_BRACE) {
                        depth--;
                        i++;
                        if (tokens[i].value && tokens[i].type == COMMA) {
                            // ignore the comma
                            i++;
                        }
                        break;
                    }
                    if (result->dependency_count == dependency_capacity - 2) {
                        dependency_capacity += 20;
                        result->dependencies = realloc(result->dependencies, sizeof(Dependency) * dependency_capacity);
                    }
                    if (tokens[i].type != DOT) {
                        LOG_ERROR("Expected '.' inside \"dependencies = {...}\". found: %s\n", tokens[i].value);
                        return PARSING_ERROR;
                    }
                    i++;
                    // I had ignored the @ symbol
                    if (!tokens[i].value || (tokens[i].type != STRING && tokens[i].type != IDENTIFIER)) {
                        LOG_ERROR("Expected String or Identifier after '.' inside \"dependencies = {...}\". found: %s\n", tokens[i].value);
                        return PARSING_ERROR;
                    } else {
                        result->dependencies[result->dependency_count].name = tokens[i].value;
                    }
                    i++;
                    if (!tokens[i].value || tokens[i].type != EQUALS) {
                        LOG_ERROR("Expected '=' after key inside \"dependencies = {...}\". found: %s\n", tokens[i].value);
                        return PARSING_ERROR;
                    }
                    i++;
                    if (!tokens[i].value || tokens[i].type != DOT) {
                        LOG_ERROR("Expected '.' after '=' inside \"dependencies = {...}\". found: %s\n", tokens[i].value);
                        return PARSING_ERROR;
                    }
                    i++;
                    if (!tokens[i].value || tokens[i].type != L_BRACE) {
                        LOG_ERROR("Expected '{' after '.' inside \"dependencies = {...}\". found: %s\n", tokens[i].value);
                        return PARSING_ERROR;
                    } else {
                        depth++;
                    }
                    i++;
                    int count_2 = 0;
                    while (count_2 < 5 && tokens[i].value) {
                        char* key_name;
                        if (tokens[i].type != DOT) {
                            LOG_ERROR("Expected '.' inside body of a dependency inside \"dependencies = {...}\". found: %s\n", tokens[i].value);
                            return PARSING_ERROR;
                        }
                        i++;
                        if (!tokens[i].value || (tokens[i].type != STRING && tokens[i].type != IDENTIFIER)) {
                            LOG_ERROR("Expected String or Identifier inside body of a dependency after '.' inside \"dependencies = {...}\". found: %s\n", tokens[i].value);
                            return PARSING_ERROR;
                        } else {
                            key_name = tokens[i].value;
                        }
                        i++;
                        if (!tokens[i].value || tokens[i].type != EQUALS) {
                            LOG_ERROR("Expected '=' inside body of a dependency after key inside \"dependencies = {...}\". found: %s\n", tokens[i].value);
                            return PARSING_ERROR;
                        }
                        i++;
                        if (!tokens[i].value || (tokens[i].type != STRING && tokens[i].type != IDENTIFIER)) {
                            LOG_ERROR("Expected String or Identifier inside body of a dependency after key inside \"dependencies = {...}\". found: %s\n", tokens[i].value);
                            return PARSING_ERROR;
                        } else {
                            if (strcmp(key_name, "url") == 0) {
                                result->dependencies[result->dependency_count].url = tokens[i].value;
                            } else if (strcmp(key_name, "hash") == 0) {
                                result->dependencies[result->dependency_count].hash = tokens[i].value;
                            } else if (strcmp(key_name, "path") == 0) {
                                result->dependencies[result->dependency_count].path = tokens[i].value;
                            } else if (strcmp(key_name, "lazy") == 0) {
                                result->dependencies[result->dependency_count].lazy = strcmp(tokens[i].value, "true") == 0 ? 1 : 0;
                            } else {
                                LOG_WARNING("Found unknown parameter inside a dependency inside \"dependencies={...}\". found: %s.\n", tokens[i].value);
                            }
                        }
                        i++;
                        if (tokens[i].value && tokens[i].type == COMMA) {
                            // ignore the comma
                            i++;
                        }
                        if (tokens[i].value && tokens[i].type == R_BRACE) {
                            depth--;
                            i++;
                            if (tokens[i].value && tokens[i].type == COMMA) {
                                // ignore the comma
                                i++;
                            }
                            break;
                        }
                        count_2++;
                    }
                    if (tokens[i].value && tokens[i].type == COMMA) {
                        // ignore the comma
                        i++;
                    }
                    result->dependency_count++;
                }
            }
        }
    }

    return PARSING_SUCCESSFUL;
}

/**
 * Prints build_zig_zon_parsed_data struct.
 * @param data build_zig_zon_parsed_data struct.
 *
 * @return Nothing
 */
void free_build_zig_zon_parsed_data(build_zig_zon_parsed_data* data)
{
    if (!data)
        return;
    if (data->dependencies) {
        free(data->dependencies);
        data->dependencies = NULL;
    }
    data->dependency_count = 0;
}

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
