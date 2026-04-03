# c90\_snippets

This is a repository of C90/C89/ANSI C snippets I've had to type several times before and decided to leave them somewhere handy for when I have to do so again in the future.

## Source Code

### C Code

* Strict ANSI C (C90)
* Be pedantic, avoid undefined behavior.
* No compiler or platform language extensions
* No C99 or later features
* No C++ features
* If existing code relies on C99 or later features, warn the user.
* If existing code relies on C++ features, warn the user.
* Styling:
  * Use clang-format, which is configured to use a kernel-style style. If the user doesn't have clang-format installed, ask them to install it. Do not install it for them.
  * Includes should be in four blank-line-separated blocks:
    1. C Standard library headers (e.g. `<stdio.h>`)
    2. System/Platform headers (e.g. `<unistd.h>`)
    3. "Third-party" library headers (e.g. `<glib.h>`)
    4. Project headers (e.g. `"foo.h"`)
  * Each of the include blocks should be sorted.

### Header Files

* Header files are assumed to be C, not C++. No concern needs to be given to C++ compatibility.
* Always use header guards in the form of FILENAME_H, where FILENAME is the name of the file in uppercase.

## Build System

- Simple GNU Makefile
- Source code and headers live under `src/`
- Tests live under `tests/`
  - One test binary per source file
  - Common test utilities live under `tests/test.h` and `tests/test.c`
  - Tests should be self contained and limit their dependencies to `src/` and `tests/test.h` and `tests/test.c` - no external dependencies or inter-test dependencies.
  - Tests should be written in C90 and use the same styling as the source code.
  - Tests should be written in a way that they can be run independently of each other in parallel.
  - Tests should be written to clean up after themselves, including memory - such that running the test binary with valgrind will pass.
- `make` builds the project
- `make clean` cleans the project
- `make check` runs the tests
- `make valgrind` runs the tests with valgrind
- Prefer running make commands from the project root, which defines common flags. There are recursive rules defined in the root Makefile, e.g. `make src/%` and `make tests/%` to make this work.
