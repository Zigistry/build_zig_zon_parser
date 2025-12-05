#include "./bzz_private.h"
#include <stdio.h>
#include <string.h>

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
        // printf("TOKEN: %s\n", tokens[i].value);
        if (tokens[i].type == L_BRACE) {
            depth++;
            continue;
        } else if (tokens[i].type == R_BRACE) {
            depth--;
            continue;
        } else if (depth == 1 && tokens[i].type == IDENTIFIER) {
            if (strcmp(tokens[i].value, "minimum_zig_version") == 0) {
                i++;
                if (!tokens[i].value || tokens[i].type != EQUALS) {
                    return PARSING_ERROR;
                }
                i++;
                if (!tokens[i].value || tokens[i].type != STRING) {
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
                    return PARSING_ERROR;
                }
                i++;
                if (tokens[i].value && tokens[i].type == DOT) {
                    // if its identifier 
                    i++;
                }
                if (!tokens[i].value || (tokens[i].type != STRING && tokens[i].type != IDENTIFIER)) {
                    return PARSING_ERROR;
                } else {
                    result->name = tokens[i].value;
                }
            } else if (strcmp(tokens[i].value, "version") == 0) {
                i++;
                if (tokens[i].value == NULL || tokens[i].type != EQUALS) {
                    return PARSING_ERROR;
                }
                i++;
                if (tokens[i].value == NULL || tokens[i].type != STRING) {
                    return PARSING_ERROR;
                } else {
                    result->version = strdup(tokens[i].value);
                }
            } else if (strcmp(tokens[i].value, "fingerprint") == 0) {
                i++;
                if (!tokens[i].value || tokens[i].type != EQUALS) {
                    return PARSING_ERROR;
                }
                i++;
                if (!tokens[i].value || tokens[i].type != HEXADECIMAL) {
                    return PARSING_ERROR;
                } else {
                    result->fingerprint = strtoull(tokens[i].value, NULL, 16);
                }
            } else if (strcmp(tokens[i].value, "dependencies") == 0) {
                i++;
                if (!tokens[i].value || tokens[i].type != EQUALS) {
                    return PARSING_ERROR;
                }
                i++;
                if (!tokens[i].value || tokens[i].type != DOT) {
                    return PARSING_ERROR;
                }
                i++;
                if (!tokens[i].value || tokens[i].type != L_BRACE) {
                    return PARSING_ERROR;
                } else {
                    depth++;
                }
                i++;
                // from here we will put a loop to loop the dependencies
                int dependency_count = 0;
                int dependency_capacity = 20;
                result->dependencies = malloc(sizeof(Dependency)*dependency_capacity);
                while (tokens[i].value) {
                    if(dependency_count == dependency_capacity-2) {
                        dependency_capacity += 20;
                        result->dependencies = realloc(result->dependencies, sizeof(Dependency)*dependency_capacity);
                    }
                    if (tokens[i].type != DOT) {
                        return PARSING_ERROR;
                    }
                    i++;
                    // I had ignored the @ symbol
                    if (!tokens[i].value || (tokens[i].type != STRING && tokens[i].type != IDENTIFIER)) {
                        return PARSING_ERROR;
                    } else {
                        result->dependencies[dependency_count].name = tokens[i].value;
                    }
                    i++;
                    if (!tokens[i].value || tokens[i].type != EQUALS) {
                        return PARSING_ERROR;
                    }
                    i++;
                    if (!tokens[i].value || tokens[i].type != DOT) {
                        return PARSING_ERROR;
                    }
                    i++;
                    if (!tokens[i].value || tokens[i].type != L_BRACE) {
                        return PARSING_ERROR;
                    } else {
                        depth++;
                    }
                    i++;
                    int count_2 = 0;
                    while (count_2 < 5 && tokens[i].value) {
                        char* key_name;
                        if (tokens[i].type != DOT) {
                            return PARSING_ERROR;
                        }
                        i++;
                        if (!tokens[i].value || (tokens[i].type != STRING && tokens[i].type != IDENTIFIER)) {
                            return PARSING_ERROR;
                        } else {
                            key_name = tokens[i].value;
                        }
                        i++;
                        if (!tokens[i].value || tokens[i].type != EQUALS) {
                            return PARSING_ERROR;
                        }
                        i++;
                        if (!tokens[i].value || (tokens[i].type != STRING && tokens[i].type != IDENTIFIER)) {
                            return PARSING_ERROR;
                        } else {
                            if(strcmp(key_name, "url")){
                                result->dependencies[dependency_count].url = tokens[i].value;
                            } else if (strcmp(key_name, "hash")) {
                                result->dependencies[dependency_count].hash = tokens[i].value;
                            } else if (strcmp(key_name, "path")) {
                                result->dependencies[dependency_count].path = tokens[i].value;
                            } else if (strcmp(key_name, "lazy")) {
                                result->dependencies[dependency_count].lazy = strcmp(tokens[i].value, "true") ? 1 : 0;
                            }
                        }
                        i++;
                        if (tokens[i].value && tokens[i].type == COMMA) {
                            // ignore the comma
                            i++;
                        }
                        if (tokens[i].value && tokens[i].type == R_BRACE) {
                            return PARSING_SUCCESSFUL;
                        }
                    }
                    dependency_count++;
                }
            }
        }
    }

    return PARSING_SUCCESSFUL;
}

const char* const TEST = "// comment\n .{"
                         "    .something = 10,"
                         "    .name = .something,"
                         "    .fingerprint = 0x1234,"
                         "    .version = \"0.10.0\","
                         "    .minimum_zig_version = \"0.10.0\","
                         "    .dependencies = .{"
                         "        .@\"something\" = .{"
                         "            .url = "
                         "\"https://example.com/some/route\","
                         "            .hash = "
                         "\"anything random\","
                         "            .lazy = true,"
                         "        },"
                         "        .macos_sdk = .{"
                         "            .url = "
                         "\"https://example.com/something/something"
                         "some.tar.gz\","
                         "            .hash = "
                         "\"something random\","
                         "            .lazy = true,"
                         "        },"
                         "        .zigimg = .{"
                         "            .url = "
                         "\"git+https://example.com/something/"
                         "something#some_hash\","
                         "            .hash = "
                         "\"some random hash\","
                         "        },"
                         "    },"
                         "    .paths = .{"
                         "},"
                         "}";

int main()
{
    const char* data = TEST;
    printf("%s\n\n", TEST);
    Token tokens[700];
    const size_t max_tokens = 700;

    if (tokenize(data, tokens, max_tokens) != TOKENIZING_SUCCESSFULL) {
        printf("Tokenizing error.\n");
    } else {
        for (int i = 0; tokens[i].value; i++) {
            printf("%s\n", tokens[i].value);
        }
        build_zig_zon_parsed_data results;
        if (parse(tokens, &results) != PARSING_SUCCESSFUL) {
            printf("Parsing error.\n");
        }

        printf("name: %s\n", results.name);
        printf("minimum_zig_version: %s\n", results.minimum_zig_version);
        printf("version: %s\n", results.version);
        printf("fingerprint: %llu\n", results.fingerprint);
        for(int i = 0; i < 2; i++) {
            printf("%s\n", results.dependencies[i].name);
        }
    }

    free_tokens(tokens);
}
