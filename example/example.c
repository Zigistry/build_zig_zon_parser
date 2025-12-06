#include "../include/bzz.h"

static const char build_zig_zon_string[] =

    ".{\n"
    "    .something = 10,\n"
    "    .name = .something,\n"
    "    .fingerprint = 0x1234123412341234,\n"
    "    .version = \"0.10.0\",\n"
    "    .minimum_zig_version = \"0.10.0\",\n"
    "    .dependencies = .{\n"
    "        .something = .{\n"
    "            .url = \"https://example.com/some/route\",\n"
    "            .hash = \"anything random\",\n"
    "            .lazy = true,\n"
    "        },\n"
    "        .macos_sdk = .{\n"
    "            .url = \"https://example.com/something/somethingsome.tar.gz\",\n"
    "            .hash = \"something random\",\n"
    "            .lazy = true,\n"
    "            .something = \"someting that is not expected here\","
    "        },\n"
    "        .zigimg = .{\n"
    "            .url = \"git+https://example.com/something/something#some_hash\",\n"
    "            .hash = \"some random hash\",\n"
    "        },\n"
    "    },\n"
    "    .paths = .{},\n"
    "}\n";




int main()
{
    Token tokens[700];
    if(tokenize(build_zig_zon_string, tokens, 700) == TOKENIZING_SUCCESSFULL) {
        build_zig_zon_parsed_data results;
        if(parse(tokens, &results) == PARSING_SUCCESSFUL) {
            print_build_zig_zon_parsed_data(results);
            free_build_zig_zon_parsed_data(&results);
        }
        free_tokens(tokens);
    }
}