# votess-benchmark

To initialize the repository after a fresh commit, run:

```bash
init.sh
```

After doing so, review the `Makefile` to verify that each sub program will be
compiled with the correct compiler. After doing so, run the following command:

```bash
make clean && make && make
```

For some reason you have to make twice (I haven't bothered to figure out why).

And to run the benchmarks, use the command:

```bash
run.sh
```

To configure parameters for the benchmark, all variables are configured in
compile time in `include/params.hpp`.
