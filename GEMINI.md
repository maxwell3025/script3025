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
- Use `[[nodiscard]]` whenever it makes sense to.
- For functions local to `.cpp` files, use `static` (unless they are `static` member functions).
- For functions defined in `.hpp` files, use `inline` (except for templated and class member functions).
- Append an underscore to any member variables.
- For unused parameters in functions, use unnamed parameters.
- Getters should be named after the variable (e.g., `variable_name()`). There should be two versions: a `const` version returning a `const` reference, and a non-`const` version returning a non-`const` reference.

## Lazy-loaded Loggers
Loggers are implemented as lazy-loaded static variables. Do not change their implementation.

**Implementation Rules:**
- **Standalone Functions:** For loggers within standalone functions (e.g., `tokenize`, `parse`), declare the logger as a `static std::shared_ptr<spdlog::logger>` variable directly within the function. Initialize it using a lambda expression that configures the logger (name, level, pattern).
- **Class Methods:** For loggers associated with a class, declare a `static std::shared_ptr<spdlog::logger>` member variable within the class. Provide a `static` member function (e.g., `get_logger()`) that returns this shared pointer. The initialization should occur within the definition of this static member function, using a lambda expression for configuration.

**Naming Convention:**
- Logger names should follow the pattern `ModuleName::FunctionName` for standalone functions (e.g., `script3025::tokenize`) or `ClassName::MethodName` for class methods (e.g., `script3025::Expression`).

**Configuration:**
- Set the logger level to `spdlog::level::warn` by default.
- Set the pattern to `"%^[%l] [tid=%t] [%T.%F] [%s:%#] %v%$"`.

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

