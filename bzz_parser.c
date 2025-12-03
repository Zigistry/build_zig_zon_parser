#include "./bzz.h"

/**
 * parsing function that list of tokens, its length
 */

int parse(Token* tokens, build_zig_zon_parsed_data result) {
    for (int i = 0; tokens[i].value != NULL; i++) {
        // printf("TOKEN: %s\n", tokens[i].value);
        if (tokens[i].type == IDENTIFIER) {
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
                    if(tokens[i].value != NULL && tokens[i].type == COMMA) {
                        // ignore the comma
                        i++;
                    }
                    if(tokens[i].value != NULL && tokens[i].type == R_BRACE) {
                        return PARSING_SUCCESSFUL;
                    }
                }
            }
        }
    }
}
