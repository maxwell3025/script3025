# script3025

# Language Details
*Note that this is incomplete; I am assuming that you are looking at the tests and can infer based
on typical typed lambda calculus notation*

`replace` keyword
```
(replace T P a b e l): P b
```
where
`T: Type`
`P: Pi (t: T).Type`
`a: T`
`b: T`
`e: a = b`
`l: P a`


# Dependencies
To build this repository, [CMake](https://cmake.org/) is required.

For coverage, [gcovr](https://github.com/gcovr/gcovr) is required.

# Build
Create a directory for the build output.
```bash
mkdir build
cd build
```

Configure the project (see the [Configuration](#configuration) section for more options).
```bash
cmake ..
```

Build the project.
```bash
cmake --build .
```

Run tests.
```bash
ctest -V
```

# Setup For Contributors
In order to enable Ci/CD hooks, please run
```bash
git config --local core.hooksPath .githooks/
```

# Configuration
## Options
### `CMAKE_BUILD_TYPE`
Specify the build type.

Options: `Debug`, `Release`, `RelWithDebInfo`, `MinSizeRel`

See the [official CMake documentation](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html)
for more info.

### `SPDLOG_ACTIVE_LEVEL`
Because CMake can only do value literal macros, `SPDLOG_ACTIVE_LEVEL` should be configured based on
the definitions in spdlog (shown below).
The definitions below are from version [1.15.2 on GitHub](
https://github.com/gabime/spdlog/blob/48bcf39a661a13be22666ac64db8a7f886f2637e/include/spdlog/common.h#L231-L237)
```c++
#define SPDLOG_LEVEL_TRACE 0
#define SPDLOG_LEVEL_DEBUG 1
#define SPDLOG_LEVEL_INFO 2
#define SPDLOG_LEVEL_WARN 3
#define SPDLOG_LEVEL_ERROR 4
#define SPDLOG_LEVEL_CRITICAL 5
#define SPDLOG_LEVEL_OFF 6
```
For example, we can use
```bash
cmake -DSPDLOG_ACTIVE_LEVEL=0 <insert other options here> ..
```

## Examples
### Debug
```bash
cmake -DSPDLOG_ACTIVE_LEVEL=0 -DCMAKE_BUILD_TYPE=Debug ..
```

### Release
```bash
cmake -DSPDLOG_ACTIVE_LEVEL=6 -DCMAKE_BUILD_TYPE=Release ..
```

### Compile
```bash
env MAKE_TERMERR=kitty MAKE_TERMOUT=kitty make -j 16 2>&1 | less -R
```

### Test
```bash
GTEST_COLOR=1 ctest -V | less -R
```
