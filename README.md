# High performance Zig build.zig.zon parser

This uses no regex.h, just uses loops.

## How to use?

- You can pass the -DBZZ_DONT_LOG flag to not print any errors just return them if any.

## Performance:

```
➜  build_zig_zon_parser git:(main) ✗ hyperfine ./a.out --warmup 5 -N
Benchmark 1: ./a.out
  Time (mean ± σ):       2.1 ms ±   0.4 ms    [User: 0.8 ms, System: 0.7 ms]
  Range (min … max):     1.9 ms …  16.9 ms    1479 runs
```
on M2 Air Mac OS
