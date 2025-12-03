#include "./bzz.h"

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
int parse(Token* tokens, build_zig_zon_parsed_data* result)
{
    int depth = 0;
    for (int i = 0; tokens[i].value != NULL; i++) {
        // printf("TOKEN: %s\n", tokens[i].value);
        if (tokens[i].type == L_BRACE)
            depth++;
        if (tokens[i].type == R_BRACE)
            depth--;

        if (depth == 1 && tokens[i].type == IDENTIFIER) {
            if (strcmp(tokens[i].value, "minimum_zig_version") == 0) {
                i++;
                if (tokens[i].value != NULL && tokens[i].type != EQUALS) {
                    return PARSING_ERROR;
                }
                i++;
                if (tokens[i].value != NULL && tokens[i].type != STRING) {
                    return PARSING_ERROR;
                } else {
                    printf("Version: %s\n", tokens[i].value);
                }
            }
            // Problem I was facing here is that in older build.zig.zon
            // we had name as a string, now, we have it as an identifier.
            // I will implement this in such a way that it can handle
            // both the ways.
            else if (strcmp(tokens[i].value, "name") == 0) {
                i++;
                if (tokens[i].value != NULL && tokens[i].type != EQUALS) {
                    return PARSING_ERROR;
                }
                i++;
                if (tokens[i].value != NULL && (tokens[i].type != STRING && tokens[i].type != IDENTIFIER)) {
                    return PARSING_ERROR;
                } else {
                    printf("name: %s\n", tokens[i].value);
                }
            } else if (strcmp(tokens[i].value, "version") == 0) {
                i++;
                if (tokens[i].value != NULL && tokens[i].type != EQUALS) {
                    return PARSING_ERROR;
                }
                i++;
                if (tokens[i].value != NULL && tokens[i].type != STRING) {
                    return PARSING_ERROR;
                } else {
                    printf("version: %s\n", tokens[i].value);
                }
            } else if (strcmp(tokens[i].value, "fingerprint") == 0) {
                i++;
                if (tokens[i].value != NULL && tokens[i].type != EQUALS) {
                    return PARSING_ERROR;
                }
                i++;
                if (tokens[i].value != NULL && tokens[i].type != STRING) {
                    return PARSING_ERROR;
                } else {
                    printf("fingerprint: %s\n", tokens[i].value);
                }
            }
        }
    }
    for (int i = 0; tokens[i].value != NULL; i++) {
        if (strcmp(tokens[i].value, "dependencies") == 0) {
            i++;
            if (tokens[i].value != NULL && tokens[i].type != EQUALS) {
                return PARSING_ERROR;
            }
            i++;
            if (tokens[i].value != NULL && tokens[i].type != DOT) {
                return PARSING_ERROR;
            } else {
                printf("DOT: %s\n", tokens[i].value);
            }
            i++;
            if (tokens[i].value != NULL && tokens[i].type != L_BRACE) {
                return PARSING_ERROR;
            } else {
                printf("L brace: %s\n", tokens[i].value);
            }
            i++;
            // from here we will put a loop to loop the dependencies
            while (1) {
                if (tokens[i].value != NULL && tokens[i].type != DOT) {
                    return PARSING_ERROR;
                } else {
                    printf("DOT: %s\n", tokens[i].value);
                }
                i++;
                printf("NOW THE THINGY: %s\n", tokens[i].value);
                // I had ignored the @ symbol
                if (tokens[i].value != NULL && (tokens[i].type != STRING && tokens[i].type != IDENTIFIER)) {
                    return PARSING_ERROR;
                } else {
                    printf("NOW THE THINGY: %s\n", tokens[i].value);
                }
                i++;
                if (tokens[i].value != NULL && tokens[i].type != EQUALS) {
                    return PARSING_ERROR;
                }
                i++;
                if (tokens[i].value != NULL && tokens[i].type != DOT) {
                    return PARSING_ERROR;
                } else {
                    printf("DOT: %s\n", tokens[i].value);
                }
                i++;
                if (tokens[i].value != NULL && tokens[i].type != L_BRACE) {
                    return PARSING_ERROR;
                } else {
                    printf("L brace: %s\n", tokens[i].value);
                }
                i++;
                int count_2 = 0;
                while (count_2 < 5) {
                    if (tokens[i].value != NULL && tokens[i].type != DOT) {
                        return PARSING_ERROR;
                    } else {
                        printf("DOT: %s\n", tokens[i].value);
                    }
                    i++;
                    if (tokens[i].value != NULL && (tokens[i].type != STRING && tokens[i].type != IDENTIFIER)) {
                        return PARSING_ERROR;
                    } else {
                        printf("NOW THE THINGY: %s\n", tokens[i].value);
                    }
                    i++;
                    if (tokens[i].value != NULL && tokens[i].type != EQUALS) {
                        return PARSING_ERROR;
                    }
                    i++;
                    if (tokens[i].value != NULL && (tokens[i].type != STRING && tokens[i].type != IDENTIFIER)) {
                        return PARSING_ERROR;
                    } else {
                        printf("NOW THE THINGY: %s\n", tokens[i].value);
                    }
                    i++;
                    if (tokens[i].value != NULL && tokens[i].type == COMMA) {
                        // ignore the comma
                        i++;
                    }
                    if (tokens[i].value != NULL && tokens[i].type == R_BRACE) {
                        return PARSING_SUCCESSFUL;
                    }
                }
            }
        }
    }

    return PARSING_SUCCESSFUL;
}
