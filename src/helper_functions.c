/*
    Copyright (2025) Rohan Vashisht
    Repository: https://github.com/zigistry/build_zig_zon_parser
    Check the LICENSE file for more information
*/

#include "../include/bzz.h"

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
