# High performance build.zig.zon parser in C

This uses no regex.h, just uses loops.

## How to use?

1) You can use the [bzz.h](./dist/bzz.h) and [bzz.c](./dist/bzz.c) from the dist folder.
2) You can pass the -DBZZ_DONT_LOG flag to not print any errors just return them.
3) 