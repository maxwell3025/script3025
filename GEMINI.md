# General Guidelines
When editing code, do not go too long without doing building to see which errors
persist.

For types, follow the principle of being broad in what you accept, and strict in
what you return.

When adding files, make sure to update `CMakeLists.txt`

When testing, always configure first, since CMake might need up update the
makefile.

## Code Style
- Add const whenever possible.
- Use `[[nodiscard]]`` whenever it makes sense to.
- For functions local to `.cpp` files, use `static`.
- For functions defined in `.hpp` files, use `inline`.
- Append an underscore to any member variables.
- For unused parameters in functions, use unnamed parameters.

# Building And Running
Since you are not able to use `cd`, use the following commands for testing.

**Configure**
```bash
cmake -S . -B build
```

**Build**
```bash
cmake --build build -- -Bj 16
```

**Test**
```bash
ctest -VV --test-dir build
```

# Formatting
Make sure to frequently format the code with 
```bash
./format
```

