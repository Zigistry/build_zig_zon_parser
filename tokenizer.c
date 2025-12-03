#include "./bzz.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * Basic build.zig.zon tokenizer.
 * @param build_zig_zon_raw_data is the raw build.zig.zon data.
 * @param tokens is pre allocated memory. tokens are null terminated by this function.
 * @return TOKENIZING_ERROR or TOKENIZING_SUCCESSFULL.
 */
int tokenize(const char* build_zig_zon_raw_data, Token* tokens, const size_t max_tokens)
{
    int i = 0, count = 0;
    // I am doing - 1 to add a null terminator and avoid buffer overflow
    while (build_zig_zon_raw_data[i] && count < max_tokens - 1) {
        if (build_zig_zon_raw_data[i] == ' ') {
        } else if (build_zig_zon_raw_data[i] == '/') {
            i++;
            if (build_zig_zon_raw_data[i] && build_zig_zon_raw_data[i] == '/') {
                // this is a comment
                while (build_zig_zon_raw_data[i] != '\n') {
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
            char any_string[MAX_STRING_LENGTH];
            int string_pos = 0;
            // I am doing - 1 to add a null terminator, same here
            while (build_zig_zon_raw_data[i] && string_pos < MAX_STRING_LENGTH - 1) {
                if (build_zig_zon_raw_data[i] == '\\' && build_zig_zon_raw_data[i + 1]) {
                    any_string[string_pos++] = build_zig_zon_raw_data[i++];
                    any_string[string_pos++] = build_zig_zon_raw_data[i++];
                } else if (build_zig_zon_raw_data[i] == '"') {
                    i++;
                    break;
                } else {
                    any_string[string_pos++] = build_zig_zon_raw_data[i++];
                }
            }
            any_string[string_pos] = '\0';
            tokens[count++] = (Token) { STRING, strdup(any_string) };
            continue;
        } else if (build_zig_zon_raw_data[i] == ',') {
            tokens[count++] = (Token) { COMMA, "," };
        } else if (isalnum(build_zig_zon_raw_data[i]) || build_zig_zon_raw_data[i] == '_') {
            char identifier[MAX_STRING_LENGTH];
            int identifier_position = 0;
            while (isalnum(build_zig_zon_raw_data[i]) || build_zig_zon_raw_data[i] == '_') {
                identifier[identifier_position++] = build_zig_zon_raw_data[i++];
            }
            identifier[identifier_position] = '\0';
            tokens[count++] = (Token) { IDENTIFIER, strdup(identifier) };
            continue;
        }
        i++;
    }
    tokens[count] = (Token) { 0 };
    return TOKENIZING_SUCCESSFULL;
}


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

void free_tokens(Token* tokens) {
    for(int i = 0; tokens[i].value; i++) {
        if(tokens[i].type == STRING || tokens[i].type == IDENTIFIER) {
            free(tokens[i].value);
        }
    }
}

int main()
{
    const char* build_zig_zon_raw_data = TEST;
    Token tokens[700];
    const size_t max_tokens = 700;

    if(tokenize(build_zig_zon_raw_data, tokens, max_tokens) == TOKENIZING_ERROR) {
        printf("Tokenizing error.\n");
    } else {
        for(int i = 0; tokens[i].value; i++) {
            printf("%s", tokens[i].value);
        }
    }

    free_tokens(tokens);
}
