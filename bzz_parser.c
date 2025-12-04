#include "./bzz_private.h"
#include <stdio.h>

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

const char* const TEST = "// comment\n .{"
                         "    .something = 10,"
                         "    .name = .capy,"
                         "    .fingerprint = 0x4724968847bbbb92,"
                         "    .version = \"0.4.1\","
                         "    .minimum_zig_version = \"0.14.1\","
                         "    .dependencies = .{"
                         "        .@\"zig-objc\" = .{"
                         "            .url = "
                         "\"https://github.com/mitchellh/zig-objc/archive/"
                         "362d12f4d91dfde84668e0befc5a8ca76659965a.zip\","
                         "            .hash = "
                         "\"12206038da3a8d42de25babfadaa3b8fb01c223850a1f1ce309034172d150df61a8c\","
                         "            .lazy = true,"
                         "        },"
                         "        .macos_sdk = .{"
                         "            .url = "
                         "\"https://github.com/mitchellh/zig-build-macos-sdk/archive/"
                         "a4ea24f105902111633c6ae9f888b676ac5e36df.tar.gz\","
                         "            .hash = "
                         "\"12209cc9ee372456eda52b71cf9ae77dcc707fa42c9f9d68996b5bf7495b53229c2e\","
                         "            .lazy = true,"
                         "        },"
                         "        .zigimg = .{"
                         "            .url = "
                         "\"git+https://github.com/zigimg/"
                         "zigimg#74caab5edd7c5f1d2f7d87e5717435ce0f0affa1\","
                         "            .hash = "
                         "\"zigimg-0.1.0-8_eo2nWlEgCddu8EGLOM_RkYshx3sC8tWv-yYA4-htS6\","
                         "        },"
                         "    },"
                         "    .paths = .{"
                         "},"
                         "}";

int main() {
    const char* build_zig_zon_raw_data = TEST;
    Token tokens[700];
    const size_t max_tokens = 700;

    if (tokenize(build_zig_zon_raw_data, tokens, max_tokens) == TOKENIZING_ERROR) {
        printf("Tokenizing error.\n");
    } else {
        for (int i = 0; tokens[i].value; i++) {
            printf("%s", tokens[i].value);
        }
    }

    free_tokens(tokens);
}
