#include <ctype.h>
#include <stdio.h>
#include <string.h>

#define PARSING_ERROR -0x01
#define PARSING_SUCCESSFUL 0x00

#define MAX_STRING_LENGTH 500

const char* const TEST = "// comment\n .{"
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

int main()
{
    int i = 0, count = 0;
    Token tokens[700];
    // I am doing - 1 to add a null terminator and avoid buffer overflow
    while (TEST[i] && count < 700 - 1) {
        if (TEST[i] == ' ') {
        } else if (TEST[i] == '/') {
            i++;
            if (TEST[i] && TEST[i] == '/') {
                // this is a comment
                while (TEST[i] != '\n') {
                    i++;
                }
            } else {
                return -1;
            }
        } else if (TEST[i] == '.') {
            tokens[count++] = (Token) { DOT, "." };
        } else if (TEST[i] == '{') {
            tokens[count++] = (Token) { L_BRACE, "{" };
        } else if (TEST[i] == '}') {
            tokens[count++] = (Token) { R_BRACE, "}" };
        } else if (TEST[i] == '=') {
            tokens[count++] = (Token) { EQUALS, "=" };
        } else if (TEST[i] == '"') {
            i++; // skip opening quote
            char any_string[MAX_STRING_LENGTH];
            int string_pos = 0;
            // I am doing - 1 to add a null terminator, same here
            while (TEST[i] && string_pos < MAX_STRING_LENGTH - 1) {
                if (TEST[i] == '\\' && TEST[i + 1]) {
                    any_string[string_pos++] = TEST[i++];
                    any_string[string_pos++] = TEST[i++];
                } else if (TEST[i] == '"') {
                    i++;
                    break;
                } else {
                    any_string[string_pos++] = TEST[i++];
                }
            }
            any_string[string_pos] = '\0';
            tokens[count++] = (Token) { STRING, strdup(any_string) };
            continue;
        } else if (TEST[i] == ',') {
            tokens[count++] = (Token) { COMMA, "," };
        } else if (isalnum(TEST[i]) || TEST[i] == '_') {
            char identifier[MAX_STRING_LENGTH];
            int identifier_position = 0;
            while (isalnum(TEST[i]) || TEST[i] == '_') {
                identifier[identifier_position++] = TEST[i++];
            }
            identifier[identifier_position] = '\0';
            tokens[count++] = (Token) { IDENTIFIER, strdup(identifier) };
            continue;
        }
        i++;
    }
    tokens[count] = (Token) { 0 }; // Null terminated

    // parse only minimum version
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
