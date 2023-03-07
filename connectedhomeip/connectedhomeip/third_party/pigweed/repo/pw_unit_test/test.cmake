# Copyright 2022 The Pigweed Authors
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
include_guard(GLOBAL)

include($ENV{PW_ROOT}/pw_build/pigweed.cmake)

set(pw_unit_test_GOOGLETEST_BACKEND pw_unit_test.light CACHE STRING
    "CMake target which implements GoogleTest, by default pw_unit_test.light \
     is used. You could, for example, point this at pw_third_party.googletest \
     if using upstream GoogleTest directly on your host for GoogleMock.")

# TODO(ewout): Remove the default.
set(pw_unit_test_ADD_EXECUTABLE_FUNCTION "pw_add_test_executable" CACHE STRING
    "The name of the CMake function used to instantiate pw_unit_test \
     executables")

# TODO(ewout): Remove the default.
set(pw_unit_test_ADD_EXECUTABLE_FUNCTION_FILE
    "$ENV{PW_ROOT}/targets/host/pw_add_test_executable.cmake" CACHE STRING
    "The path to the .cmake file that defines \
     pw_unit_test_ADD_EXECUTABLE_FUNCTION.")

# TODO(ewout): Remove the default to match GN and support Windows.
set(pw_unit_test_AUTOMATIC_RUNNER "$ENV{PW_ROOT}/targets/host/run_test" CACHE
    STRING
    "Path to a test runner to automatically run unit tests after they are \
     built. \
     If set, a pw_add_test's {NAME}.run action will invoke the test runner \
     specified by this variable, passing the path to the unit test to run. If \
     set to an empty string, the {NAME}.run step will fail to build.")

set(pw_unit_test_AUTOMATIC_RUNNER_TIMEOUT_SECONDS "" CACHE STRING
    "Optional timeout to apply when running tests via the automatic runner. \
     Timeout is in seconds. Defaults to empty which means no timeout.")

set(pw_unit_test_AUTOMATIC_RUNNER_ARGS "" CACHE STRING
    "Optional arguments to forward to the automatic runner")

# pw_add_test: Declares a single unit test suite with Pigweed naming rules and
#              compiler warning options.
#
#   {NAME} depends on ${NAME}.run if pw_unit_test_AUTOMATIC_RUNNER is set, else
#          it depends on ${NAME}.bin
#   {NAME}.lib contains the provided test sources as a library target, which can
#              then be linked into a test executable.
#   {NAME}.bin is a standalone executable which contains only the test sources
#              specified in the pw_unit_test_template.
#   {NAME}.run which runs the unit test executable after building it if
#              pw_unit_test_AUTOMATIC_RUNNER is set, else it fails to build.
#
# Required Arguments:
#
#   NAME: name to use for the produced test targets specified above
#
# Optional Arguments:
#
#   SOURCES - source files for this library
#   HEADERS - header files for this library
#   PRIVATE_DEPS - private pw_target_link_targets arguments
#   PRIVATE_INCLUDES - public target_include_directories argument
#   PRIVATE_DEFINES - private target_compile_definitions arguments
#   PRIVATE_COMPILE_OPTIONS - private target_compile_options arguments
#   PRIVATE_LINK_OPTIONS - private target_link_options arguments
#
#  TODO(ewout, hepler): Deprecate the following legacy arguments
#   GROUPS - groups to which to add this test.
#
function(pw_add_test NAME)
  pw_parse_arguments(
    NUM_POSITIONAL_ARGS
      1
    MULTI_VALUE_ARGS
      SOURCES HEADERS PRIVATE_DEPS PRIVATE_INCLUDES
      PRIVATE_DEFINES PRIVATE_COMPILE_OPTIONS
      PRIVATE_LINK_OPTIONS GROUPS
  )

  _pw_check_name_is_relative_to_root("${NAME}" "$ENV{PW_ROOT}"
    REMAP_PREFIXES
      third_party pw_third_party
  )

  pw_add_test_generic(${NAME}
    SOURCES
      ${arg_SOURCES}
    HEADERS
      ${arg_HEADERS}
    PRIVATE_DEPS
      # TODO(b/232141950): Apply compilation options that affect ABI
      # globally in the CMake build instead of injecting them into libraries.
      pw_build
      ${arg_PRIVATE_DEPS}
    PRIVATE_INCLUDES
      ${arg_PRIVATE_INCLUDES}
    PRIVATE_DEFINES
      ${arg_PRIVATE_DEFINES}
    PRIVATE_COMPILE_OPTIONS_DEPS_BEFORE
      pw_build.warnings
    PRIVATE_COMPILE_OPTIONS
      ${arg_PRIVATE_COMPILE_OPTIONS}
    PRIVATE_LINK_OPTIONS
      ${arg_PRIVATE_LINK_OPTIONS}
    GROUPS
      ${arg_GROUPS}
  )
endfunction()

# pw_add_test_generic: Declares a single unit test suite.
#
#   {NAME} depends on ${NAME}.run if pw_unit_test_AUTOMATIC_RUNNER is set, else
#          it depends on ${NAME}.bin
#   {NAME}.lib contains the provided test sources as a library target, which can
#              then be linked into a test executable.
#   {NAME}.bin is a standalone executable which contains only the test sources
#              specified in the pw_unit_test_template.
#   {NAME}.run which runs the unit test executable after building it if
#              pw_unit_test_AUTOMATIC_RUNNER is set, else it fails to build.
#
# Required Arguments:
#
#   NAME: name to use for the produced test targets specified above
#
# Optional Arguments:
#
#   SOURCES - source files for this library
#   HEADERS - header files for this library
#   PRIVATE_DEPS - private pw_target_link_targets arguments
#   PRIVATE_INCLUDES - public target_include_directories argument
#   PRIVATE_DEFINES - private target_compile_definitions arguments
#   PRIVATE_COMPILE_OPTIONS_DEPS_BEFORE - private target_compile_options BEFORE
#     arguments from the specified deps's INTERFACE_COMPILE_OPTIONS. Note that
#     these deps are not pulled in as target_link_libraries. This should not be
#     exposed by the non-generic API.
#   PRIVATE_COMPILE_OPTIONS - private target_compile_options arguments
#   PRIVATE_LINK_OPTIONS - private target_link_options arguments
#
#  TODO(ewout, hepler): Deprecate the following legacy arguments
#   GROUPS - groups to which to add this test.
#
function(pw_add_test_generic NAME)
  pw_parse_arguments(
    NUM_POSITIONAL_ARGS
      1
    MULTI_VALUE_ARGS
      SOURCES HEADERS PRIVATE_DEPS PRIVATE_INCLUDES
      PRIVATE_DEFINES
      PRIVATE_COMPILE_OPTIONS_DEPS_BEFORE PRIVATE_COMPILE_OPTIONS
      PRIVATE_LINK_OPTIONS GROUPS
  )

  # Add the library target under "${NAME}.lib".
  # OBJECT libraries require at least one source file.
  if("${arg_SOURCES}" STREQUAL "")
    set(lib_type "INTERFACE")
  else()
    set(lib_type "OBJECT")
  endif()
  pw_add_library_generic("${NAME}.lib" ${lib_type}
    SOURCES
      ${arg_SOURCES}
    HEADERS
      ${arg_HEADERS}
    PRIVATE_DEPS
      pw_unit_test
      ${arg_PRIVATE_DEPS}
    PRIVATE_INCLUDES
      ${arg_PRIVATE_INCLUDES}
    PRIVATE_DEFINES
      ${arg_PRIVATE_DEFINES}
    PRIVATE_COMPILE_OPTIONS_DEPS_BEFORE
      ${PRIVATE_COMPILE_OPTIONS_DEPS_BEFORE}
    PRIVATE_COMPILE_OPTIONS
      ${arg_PRIVATE_COMPILE_OPTIONS}
    PRIVATE_LINK_OPTIONS
      ${arg_PRIVATE_LINK_OPTIONS}
  )

  # Add the executable target under "${NAME}.bin".
  if(("${pw_unit_test_ADD_EXECUTABLE_FUNCTION}" STREQUAL "") OR
     ("${pw_unit_test_ADD_EXECUTABLE_FUNCTION_FILE}" STREQUAL ""))
    pw_add_error_target("${NAME}.bin"
      MESSAGE
        "Attempted to build the ${NAME}.bin without enabling the unit "
        "test executable function via pw_unit_test_ADD_EXECUTABLE_FUNCTION "
        "and pw_unit_test_ADD_EXECUTABLE_FUNCTION_FILE. "
        "See https://pigweed.dev/pw_unit_test for more details."
    )
  else()
    include("${pw_unit_test_ADD_EXECUTABLE_FUNCTION_FILE}")
    cmake_language(CALL "${pw_unit_test_ADD_EXECUTABLE_FUNCTION}"
                   "${NAME}.bin" "${NAME}.lib")
  endif()

  # Add the ${NAME} target and optionally the run target under ${NAME}.run.
  add_custom_target("${NAME}")
  if("${pw_unit_test_AUTOMATIC_RUNNER}" STREQUAL "")
    # Test runner is not provided, only build the executable.
    add_dependencies("${NAME}" "${NAME}.bin")

    pw_add_error_target("${NAME}.run"
      MESSAGE
        "Attempted to build ${NAME}.run which is not available because "
        "pw_unit_test_AUTOMATIC_RUNNER has not been configured. "
        "See https://pigweed.dev/pw_unit_test."
    )
  else()  # pw_unit_test_AUTOMATIC_RUNNER is provided.
    # Define a target for running the test. The target creates a stamp file to
    # indicate successful test completion. This allows running tests in parallel
    # with Ninja's full dependency resolution.
    if(NOT "${pw_unit_test_AUTOMATIC_RUNNER_TIMEOUT_SECONDS}" STREQUAL "")
      set(optional_timeout_arg
          "--timeout ${pw_unit_test_AUTOMATIC_RUNNER_TIMEOUT_SECONDS}")
    endif()
    if(NOT "${pw_unit_test_AUTOMATIC_RUNNER_ARGS}" STREQUAL "")
      set(optional_runner_args "-- ${pw_unit_test_AUTOMATIC_RUNNER_ARGS}")
    endif()
    add_custom_command(
      COMMAND
        python3 -m pw_unit_test.test_runner
        --runner "${pw_unit_test_AUTOMATIC_RUNNER}"
        --test "$<TARGET_FILE:${NAME}.bin>"
        ${optional_timeout_arg}
        ${optional_runner_args}
      COMMAND
        "${CMAKE_COMMAND}" -E touch "${NAME}.stamp"
      DEPENDS
        "${NAME}.bin"
      OUTPUT
        "${NAME}.stamp"
    )
    add_custom_target("${NAME}.run" DEPENDS "${NAME}.stamp")
    add_dependencies("${NAME}" "${NAME}.run")
  endif()

  if(arg_GROUPS)
    pw_add_test_to_groups("${NAME}" ${arg_GROUPS})
  endif()
endfunction(pw_add_test_generic)

# pw_add_test_group: Defines a collection of tests or other test groups.
#
# Creates the following targets:
#
#   {NAME} depends on ${NAME}.run if pw_unit_test_AUTOMATIC_RUNNER is set, else
#          it depends on ${NAME}.bin
#   {NAME}.bundle depends on ${NAME}.bundle.run if pw_unit_test_AUTOMATIC_RUNNER
#                 is set, else it depends on ${NAME}.bundle.bin
#   {NAME}.lib depends on ${NAME}.bundle.lib.
#   {NAME}.bin depends on the provided TESTS's <test_dep>.bin targets.
#   {NAME}.run depends on the provided TESTS's <test_dep>.run targets if
#              pw_unit_test_AUTOMATIC_RUNNER is set, else it fails to build.
#   {NAME}.bundle.lib contains the provided tests bundled as a library target,
#                     which can then be linked into a test executable.
#   {NAME}.bundle.bin standalone executable which contains the bundled tests.
#   {NAME}.bundle.run runs the {NAME}.bundle.bin test bundle executable after
#                     building it if pw_unit_test_AUTOMATIC_RUNNER is set, else
#                     it fails to build.
#
# Required Arguments:
#
#   NAME - The name of the executable target to be created.
#   TESTS - pw_add_test targets and pw_add_test_group bundles to be included in
#           this test bundle
#
function(pw_add_test_group NAME)
  pw_parse_arguments(
    NUM_POSITIONAL_ARGMENTS
      1
    MULTI_VALUE_ARGS
      TESTS
    REQUIRED_ARGS
      TESTS
  )

  set(test_lib_targets "")
  set(test_bin_targets "")
  set(test_run_targets "")
  foreach(test IN LISTS arg_TESTS)
    list(APPEND test_lib_targets "${test}.lib")
    list(APPEND test_bin_targets "${test}.bin")
    list(APPEND test_run_targets "${test}.run")
  endforeach()

  # This produces ${NAME}.bundle, ${NAME}.bundle.lib, ${NAME}.bundle.bin, and
  # ${NAME}.bundle.run.
  pw_add_test("${NAME}.bundle"
    PRIVATE_DEPS
      ${test_lib_targets}
  )

  # Produce ${NAME}.lib.
  pw_add_library_generic("${NAME}.lib" INTERFACE
    PUBLIC_DEPS
      ${NAME}.bundle.lib
  )

  # Produce ${NAME}.bin.
  add_custom_target("${NAME}.bin")
  add_dependencies("${NAME}.bin" ${test_bin_targets})

  # Produce ${NAME} and ${NAME}.run.
  add_custom_target("${NAME}")
  if("${pw_unit_test_AUTOMATIC_RUNNER}" STREQUAL "")
    # Test runner is not provided, only build the executable.
    add_dependencies("${NAME}" "${NAME}.bin")

    pw_add_error_target("${NAME}.run"
      MESSAGE
        "Attempted to build ${NAME}.run which is not available because "
        "pw_unit_test_AUTOMATIC_RUNNER has not been configured. "
        "See https://pigweed.dev/pw_unit_test."
    )
  else()  # pw_unit_test_AUTOMATIC_RUNNER is provided, build and run the test.
    add_custom_target("${NAME}.run")
    add_dependencies("${NAME}.run" ${test_run_targets})

    add_dependencies("${NAME}" "${NAME}.run")
  endif()
endfunction(pw_add_test_group)

# Adds a test target to the specified test groups. Test groups can be built with
# the pw_tests_GROUP_NAME target or executed with the pw_run_tests_GROUP_NAME
# target.
function(pw_add_test_to_groups TEST_NAME)
  foreach(group IN LISTS ARGN)
    if(NOT TARGET "pw_tests.${group}")
      add_custom_target("pw_tests.${group}")
      add_custom_target("pw_run_tests.${group}")
    endif()

    add_dependencies("pw_tests.${group}" "${TEST_NAME}.bin")
    add_dependencies("pw_run_tests.${group}" "${TEST_NAME}.run")
  endforeach()
endfunction(pw_add_test_to_groups)
