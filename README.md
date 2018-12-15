## Juno

Welcome to compile-time data structures library `juno`.

The library is in the early development stage, which means that the API is not stable and is **very** likely (or even, overdue) to change. However, thanks to the early design choices (extensive use of compile-time programming techniques) the actual runtime is "stable" in a sense that it is unlikely to cause crashes or other *runtime* issues in the user code. This is because almost all code is executed by the compiler during the compilation (i.e. template type transformations and `constexpr`) rather than in the runtime. Nevertheless, users are encouraged to read Overload articles (*linked below*) which explain the programming techniques used as well as envisioned use cases, as to avoid surprises.

Due to immaturity of this library, users are expected to copy the content of `juno` subdirectory and use it "as is" in their own projects, rather than declare dependency on the library (and risk breakage of their own code when the API of this library changes in the future versions). The library is "include only" and has no dependencies other than a modern C++17 compliant compiler with the C++ standard library.

Available primitives include:

- `juno::set` a set of types, on top of which following other primitives are built. Implementation details of this primitive are explained in Overload 146, *link below*
- `juno::map` a map of selector types to arbitrary types. Please see Overload 147 for details, *link below*
- `juno::val` a map of selector types to arbitrary data. Please see Overload 148 for details, *link below*

#### Note

The set of primitives listed above is likely to change in the future releases, e.g. they *might* get merged into a single primitive (or they might *not*). Discussion on this (or similar) ideas is welcome in the "issues" section of this project.

## Releases

The meta-programming concepts explored in this library are documented in the series of articles in ACCU Overload journal. Releases and branches correspond to individual articles, as listed below:

https://accu.org/index.php/journals/2531 "Set of types", branch/release `overload_146`

https://accu.org/index.php/journals/2562 "Map of types", branch/release `overload_147`

https://accu.org/index.php/journals/2587 "Map of values", branch/release `overload_148`

There is also earlier `original` branch and release, which differs in design and implements slightly more functionality (e.g. `tagset`)

## Feedback

Users (actual and potential) are welcome to report bugs or send ideas at https://github.com/Bronek/juno/issues
