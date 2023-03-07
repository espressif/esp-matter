# Design guidelines

## Includes

Use the `module/header.h` path-spec in the include directives.

Limit the module include path to the module directory.

Say you have a module directory structure like so:

```
path/to/module
├── CMakeLists.txt
├── include
│   └── module
│       └── header.h
```

You should limit the include directories to `path/to/module/include`, and
not provide `path/to/module/include/module` as an include directory. This looks
like the following in CMake:

```
target_include_directories(my-module
    PUBLIC
        include
)
```

### #include style

We follow LLVM #include style with one exception:
- we group headers (not preserving the groups). Each group is separated by a blank line.

Details can be found at [LLVM #include style guideline](https://llvm.org/docs/CodingStandards.html#include-style).

## C linkage protection in C header files

Each C header that could be used by C++ code should have C linkage protection.

```
#ifdef __cplusplus
extern "C" {
#endif

void a_function(void);

#ifdef __cplusplus
}
#endif
```

## Short module names

Use short names instead of fully qualified names for modules (and prefixes).

"net" is very commonly used as a short name for "network stack" and "mm" is
a common short name (/acronym) for "memory manager/management".

If there's no reasonable short name, use full name. "critical section" is
a good example where using full name makes more sense.

For instance, for network stack memory manager, use "net_mm_*" instead of
"network_stack_memory_manager".

## Namespacing public functions

The public functions in a module should be prefixed with the name of the module. This prevents name collisions between modules. Static functions are generally not prefixed, but may be prefixed if doing so improves readability.

```
void fx_serial_init(...)
{
    ...
}

serial_status_t fx_serial_status(...)
{
    ...
}

static void init_direct(...)
{
    ...
}
```

## Documentation

### Arguments description

Use the long options whenever possible, when describing a utility command in our documentation.

```
$ ctest --test-dir cmake_build/ --tests-regex <test-cmake-executable-name> --verbose
```
