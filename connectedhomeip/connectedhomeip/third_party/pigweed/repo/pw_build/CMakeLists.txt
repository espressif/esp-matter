# Copyright 2020 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.

# IMPORTANT: The compilation flags in this file must be kept in sync with
#            the GN flags //pw_build/BUILD.gn.

include("$ENV{PW_ROOT}/pw_build/cc_blob_library.cmake")
include("$ENV{PW_ROOT}/pw_build/pigweed.cmake")

# Target that specifies the standard Pigweed build options.
pw_add_library_generic(pw_build INTERFACE
  PUBLIC_COMPILE_OPTIONS
    -g
    # Force the compiler use colorized output. This is required for Ninja.
    $<$<CXX_COMPILER_ID:Clang>:-fcolor-diagnostics>
    $<$<CXX_COMPILER_ID:GNU>:-fdiagnostics-color=always>
  PUBLIC_DEPS
    pw_build.reduced_size
    pw_build.cpp17
)
if(ZEPHYR_PIGWEED_MODULE_DIR)
  target_link_libraries(pw_build INTERFACE zephyr_interface)
endif()

# Declare top-level targets for tests.
add_custom_target(pw_tests.default)
add_custom_target(pw_run_tests.default)

add_custom_target(pw_tests DEPENDS pw_tests.default)
add_custom_target(pw_run_tests DEPENDS pw_run_tests.default)

# Define the standard Pigweed compile options.
pw_add_library_generic(pw_build.reduced_size INTERFACE
  PUBLIC_COMPILE_OPTIONS
    "-fno-common"
    "-fno-exceptions"
    "-ffunction-sections"
    "-fdata-sections"
    $<$<COMPILE_LANGUAGE:CXX>:-fno-rtti>
)

# Define the standard Pigweed compile options.
#
# The pw_build.warnings library is used by upstream Pigweed targets to add
# compiler warnings to the build.
#
# Toolchains may override these warnings by setting pw_build_WARNINGS:
#
#   set(pw_build_WARNINGS my_warnings CACHE STRING "" FORCE)
#
set(pw_build_WARNINGS pw_build.strict_warnings
    CACHE STRING "Warnings libraries to use for Pigweed upstream code")

pw_add_library_generic(pw_build.warnings INTERFACE
  PUBLIC_DEPS
    ${pw_build_WARNINGS}
)

# TODO(hepler): These Zephyr exceptions should be made by overriding
#     pw_build_WARNINGS.
if(ZEPHYR_PIGWEED_MODULE_DIR)
  # -Wtype-limits is incompatible with Kconfig at times, disable it for Zephyr
  # builds.
  set(strict_warnings_cond "-Wno-type-limits")
else()
  # Only include these flags if we're not building with Zephyr.
  set(strict_warnings_cond "-Wundef")
endif()

pw_add_library_generic(pw_build.strict_warnings INTERFACE
  PUBLIC_COMPILE_OPTIONS
    "-Wall"
    "-Wextra"
    "-Wimplicit-fallthrough"
    ${strict_warnings_cond}
    "-Wpointer-arith"

    # Make all warnings errors, except for the exemptions below.
    "-Werror"
    "-Wno-error=cpp"  # preprocessor #warning statement
    "-Wno-error=deprecated-declarations"  # [[deprecated]] attribute

    $<$<COMPILE_LANGUAGE:CXX>:-Wnon-virtual-dtor>
)

if(NOT ZEPHYR_PIGWEED_MODULE_DIR)
  # Only include these flags if we're not building with Zephyr.
  set(extra_strict_warnings_cond "-Wredundant-decls")
endif()

pw_add_library_generic(pw_build.extra_strict_warnings INTERFACE
  PUBLIC_COMPILE_OPTIONS
    "-Wshadow"
    ${extra_strict_warnings_cond}
    $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>
)

pw_add_library_generic(pw_build.pedantic_warnings INTERFACE
  PUBLIC_COMPILE_OPTIONS
    # Enable -Wpedantic, but disable a few warnings.
    "-Wpedantic"

    # Allow designated initializers, which were added in C++20 but widely
    # supported prior and permitted by the Google style guide.
    "-Wno-c++20-designator"

    # Allow empty ... arguments in macros, which are permitted in C++20 but
    # widely supported prior.
    "-Wno-gnu-zero-variadic-macro-arguments"
)

pw_add_library_generic(pw_build.cpp17 INTERFACE
  PUBLIC_COMPILE_OPTIONS
    $<$<COMPILE_LANGUAGE:CXX>:-std=c++17>
    # Allow uses of the register keyword, which may appear in C headers.
    $<$<COMPILE_LANGUAGE:CXX>:-Wno-register>
)

# Create an empty C++ source file and library for general use.
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/empty_file.cc" "")
add_library(pw_build.empty OBJECT "${CMAKE_CURRENT_BINARY_DIR}/empty_file.cc" "")

pw_add_test(pw_build.cc_blob_library_test
  SOURCES
    cc_blob_library_test.cc
  PRIVATE_DEPS
    pw_build.test_blob
  GROUPS
    modules
    pw_build
)

pw_cc_blob_library(pw_build.test_blob
  HEADER
    pw_build/test_blob.h
  NAMESPACE
    test::ns
  BLOB
    SYMBOL_NAME kFirstBlob0123
    PATH test_blob_0123.bin
    ALIGNAS 512
  BLOB
    SYMBOL_NAME kSecondBlob0123
    PATH test_blob_0123.bin
)
