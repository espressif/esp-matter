# Templates for examples and integration tests

Example and tests for the Open IoT SDK are maintained in the form of templates. This allows us to generate multiple
variations of the same example or test based on different real-time operating systems and/or platforms, using an
example's or test's template.

The command for converting all templates to actual examples and tests is described in the SDK's [top-level README.md](../README.md).

As an overview, the mechanism consists of the following:
* `templates/CMakeLists.txt` is the top-level, entry-point script for converting all templates to real examples and
tests
* Supported RTOSes are defined in `templates/rtoses.cmake`
* Supported platforms are defined in `templates/platforms.cmake`
* Common files that are useful to multiple examples and tests are in `templates/common/`
* Templates for examples and tests need to be added as subdirectories of `templates/examples/` and `templates/tests/`
* Inside each example or test template's subdirectory (e.g. `templates/examples/cmsis-rtos-api/`)
    - `CMakeLists.txt` loops through platforms and RTOSes (or none if using baremetal) the example/test intends to run
    on, and extracts variables for the current platform and RTOS by calling `get_target_platform_variables` (or
    `get_tfm_target_platform_variables` if using TF-M) and `get_target_os_variables`. Then it calls `configure_file` to
    convert template files and `file(COPY ...)` to copy non-template files into output destination
    - `CMakeLists.txt.in` is the template for the generated example/test's top-level `CMakeLists.txt`. Placeholder
    variables (between `@` signs) in a template will be substituted with values of CMake variables by `configure_file`
    - Any other project files, which can be templates or non-templates, are stored here

To understand how templates work in detail, take a look at existing templates in subdirectories of `examples` and
`tests`.
