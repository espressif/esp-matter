# Getting started

## CMake build process and examples

An introduction to the CMake build process can be found inside each example's README.md. If you are building from the
source repository rather than a release package, you need to first generate examples from templates by following
[the Open IoT SDK's top-level README.md](../README.md).

## Reusing fetched components

By default, each component is fetched by CMake's
[FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html) into a
subdirectory of `_deps` inside the build directory during configuration. If you want to
use a local copy of a particular component (to either save time and disk space or build some
modified code), you can pass to CMake during the configuration stage
`-D FETCHCONTENT_SOURCE_DIR_<uppercaseName>=<path-to-your-local-copy>`
(more details [here](https://cmake.org/cmake/help/latest/module/FetchContent.html#variable:FETCHCONTENT_SOURCE_DIR_%3CuppercaseName%3E))
where `<uppercaseName>` is the name passed to
[`FetchContent_Declare()`](https://cmake.org/cmake/help/latest/module/FetchContent.html#command:fetchcontent_declare)
with all letters converted to uppercase (e.g. `FETCHCONTENT_SOURCE_DIR_TF-M` for a module
declared as `tf-m`).

Note that CMake will not update your local component, so it is your responsibility
to make sure the code is compatible.

## Integration tests

For software prerequistes required to run the tests, please see [Prerequisites.md](Prerequisites.md).

When you generate examples as mentioned above, integration tests are also generated. The build process for tests and
examples are identical. The only difference is that an example intends to show the use of a component in a simple and
readable way, whereas an integration test exercises a component more rigorously and may not be a good teaching example.

Each test is run using htrun which loads the test image onto the platform, starts the test application and checks if
the test produces expected outputs. [CTest](https://cmake.org/cmake/help/latest/manual/ctest.1.html), which is part of
the CMake tools, invokes the htrun command with arguments specified by the `add_test()` call in each application's
`CMakeLists.txt`.

**Note**: Running of each example can also be automated with CTest.

After building a test or an example, you can run the following command for testing:

```
# If your build directory is "build"
ctest --test-dir build -V
```

## Writing your own application

Each example shows the typical way a user project can add and configure the Open IoT SDK and use its components.

## Contributing an example or a test to the SDK

To contribute an example or a test to the SDK, you need to add it as a template. See
[the documentation for templates](../templates/README.md) for details.

You do _not_ need to use templates for applications outside the SDK repository such as your own application.
