#include "./bzz.h"

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
