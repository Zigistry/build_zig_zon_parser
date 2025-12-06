#include "../include/bzz.h"

/**
 * Parses tokens to get meaningful information
 * out of tokenized build.zig.zon.
 * @param tokens expects null terminated list of tokens.
 *
 * @param result The resulting pointer to a build_zig_zon_parsed_data
 * struct. The data would be written to this.
 * 
 * @return PARSING_ERROR or PARSING_SUCCESSFUL.
 *
 * TODO: Implement paths parsing.
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
