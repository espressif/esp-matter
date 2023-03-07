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
include_guard(GLOBAL)

cmake_minimum_required(VERSION 3.19)

# The PW_ROOT environment variable should be set in bootstrap. If it is not set,
# set it to the root of the Pigweed repository.
if("$ENV{PW_ROOT}" STREQUAL "")
  get_filename_component(pw_root "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
  message("The PW_ROOT environment variable is not set; "
          "using ${pw_root} within CMake")
  set(ENV{PW_ROOT} "${pw_root}")
endif()

# TOOD(ewout, hepler): Remove this legacy include once all users pull in
# pw_unit_test/test.cmake for test functions and variables instead of relying
# on them to be provided by pw_build/pigweed.cmake.
include("$ENV{PW_ROOT}/pw_unit_test/test.cmake")

# Wrapper around cmake_parse_arguments that fails with an error if any arguments
# remained unparsed or a required argument was not provided.
#
# All parsed arguments are prefixed with "arg_". This helper can only be used
# by functions, not macros.
#
# Required Arguments:
#
#   NUM_POSITIONAL_ARGS - PARSE_ARGV <N> arguments for
#                              cmake_parse_arguments
#
# Optional Args:
#
#   OPTION_ARGS - <option> arguments for cmake_parse_arguments
#   ONE_VALUE_ARGS - <one_value_keywords> arguments for cmake_parse_arguments
#   MULTI_VALUE_ARGS - <multi_value_keywords> arguments for
#                           cmake_parse_arguments
#   REQUIRED_ARGS - required arguments which must be set, these may any
#                        argument type (<option>, <one_value_keywords>, and/or
#                        <multi_value_keywords>)
#
macro(pw_parse_arguments)
  # First parse the arguments to this macro.
  cmake_parse_arguments(
    pw_parse_arg "" "NUM_POSITIONAL_ARGS"
    "OPTION_ARGS;ONE_VALUE_ARGS;MULTI_VALUE_ARGS;REQUIRED_ARGS"
    ${ARGN}
  )
  pw_require_args("pw_parse_arguments" "pw_parse_arg_" NUM_POSITIONAL_ARGS)
  if(NOT "${pw_parse_arg_UNPARSED_ARGUMENTS}" STREQUAL "")
    message(FATAL_ERROR "Unexpected arguments to pw_parse_arguments: "
            "${pw_parse_arg_UNPARSED_ARGUMENTS}")
  endif()

  # Now that we have the macro's arguments, process the caller's arguments.
  pw_parse_arguments_strict("${CMAKE_CURRENT_FUNCTION}"
    "${pw_parse_arg_NUM_POSITIONAL_ARGS}"
    "${pw_parse_arg_OPTION_ARGS}"
    "${pw_parse_arg_ONE_VALUE_ARGS}"
    "${pw_parse_arg_MULTI_VALUE_ARGS}"
  )
  pw_require_args("${CMAKE_CURRENT_FUNCTION}" "arg_"
                  ${pw_parse_arg_REQUIRED_ARGS})
endmacro()

# TODO(ewout, hepler): Deprecate this function in favor of pw_parse_arguments.
# Wrapper around cmake_parse_arguments that fails with an error if any arguments
# remained unparsed.
macro(pw_parse_arguments_strict function start_arg options one multi)
  cmake_parse_arguments(PARSE_ARGV
      "${start_arg}" arg "${options}" "${one}" "${multi}"
  )
  if(NOT "${arg_UNPARSED_ARGUMENTS}" STREQUAL "")
    set(_all_args ${options} ${one} ${multi})
    message(FATAL_ERROR
        "Unexpected arguments to ${function}: ${arg_UNPARSED_ARGUMENTS}\n"
        "Valid arguments: ${_all_args}"
    )
  endif()
endmacro()

# Checks that one or more variables are set. This is used to check that
# arguments were provided to a function. Fails with FATAL_ERROR if
# ${ARG_PREFIX}${name} is empty. The FUNCTION_NAME is used in the error message.
# If FUNCTION_NAME is "", it is set to CMAKE_CURRENT_FUNCTION.
#
# Usage:
#
#   pw_require_args(FUNCTION_NAME ARG_PREFIX ARG_NAME [ARG_NAME ...])
#
# Examples:
#
#   # Checks that arg_FOO is non-empty, using the current function name.
#   pw_require_args("" arg_ FOO)
#
#   # Checks that FOO and BAR are non-empty, using function name "do_the_thing".
#   pw_require_args(do_the_thing "" FOO BAR)
#
macro(pw_require_args FUNCTION_NAME ARG_PREFIX)
  if("${FUNCTION_NAME}" STREQUAL "")
    set(_pw_require_args_FUNCTION_NAME "${CMAKE_CURRENT_FUNCTION}")
  else()
    set(_pw_require_args_FUNCTION_NAME "${FUNCTION_NAME}")
  endif()

  foreach(name IN ITEMS ${ARGN})
    if("${${ARG_PREFIX}${name}}" STREQUAL "")
      message(FATAL_ERROR "A value must be provided for ${name} in "
          "${_pw_require_args_FUNCTION_NAME}.")
    endif()
  endforeach()
endmacro()

# pw_target_link_targets: CMake target only form of target_link_libraries.
#
# Helper wrapper around target_link_libraries which only supports CMake targets
# and detects when the target does not exist.
#
# NOTE: Generator expressions are not supported.
#
# Due to the processing order of list files, the list of targets has to be
# checked at the end of the root CMake list file. Instead of requiring all
# list files to be modified, a DEFER CALL is used.
#
# Required Args:
#
#   <name> - The library target to add the TARGET link dependencies to.
#
# Optional Args:
#
#   INTERFACE - interface target_link_libraries arguments which are all TARGETs.
#   PUBLIC - public target_link_libraries arguments which are all TARGETs.
#   PRIVATE - private target_link_libraries arguments which are all TARGETs.
function(pw_target_link_targets NAME)
  set(types INTERFACE PUBLIC PRIVATE )
  pw_parse_arguments(
    NUM_POSITIONAL_ARGS
      1
    MULTI_VALUE_ARGS
      ${types}
  )

  if(NOT TARGET "${NAME}")
    message(FATAL_ERROR "\"${NAME}\" must be a TARGET library")
  endif()

  foreach(type IN LISTS types)
    foreach(library IN LISTS arg_${type})
      target_link_libraries(${NAME} ${type} ${library})
      if(NOT TARGET ${library})
        # It's possible the target has not yet been defined due to the ordering
        # of add_subdirectory. Ergo defer the call until the end of the
        # configuration phase.

        # cmake_language(DEFER ...) evaluates arguments at the time the deferred
        # call is executed, ergo wrap it in a cmake_language(EVAL CODE ...) to
        # evaluate the arguments now. The arguments are wrapped in brackets to
        # avoid re-evaluation at the deferred call.
        cmake_language(EVAL CODE
          "cmake_language(DEFER DIRECTORY ${CMAKE_SOURCE_DIR} CALL
                          _pw_target_link_targets_deferred_check
                          [[${NAME}]] [[${type}]] ${library})"
        )
      endif()
    endforeach()
  endforeach()
endfunction()

# Runs any deferred library checks for pw_target_link_targets.
#
# Required Args:
#
#   <name> - The name of the library target to add the link dependencies to.
#   <type> - The type of the library (INTERFACE, PUBLIC, PRIVATE).
#   <library> - The library to check to assert it's a TARGET.
function(_pw_target_link_targets_deferred_check NAME TYPE LIBRARY)
  if(NOT TARGET ${LIBRARY})
      message(FATAL_ERROR
        "${NAME}'s ${TYPE} dep \"${LIBRARY}\" is not a target.")
  endif()
endfunction()

# Sets the provided variable to the multi_value_keywords from pw_add_library.
macro(_pw_add_library_multi_value_args variable)
  set("${variable}" SOURCES HEADERS
                    PUBLIC_DEPS PRIVATE_DEPS
                    PUBLIC_INCLUDES PRIVATE_INCLUDES
                    PUBLIC_DEFINES PRIVATE_DEFINES
                    PUBLIC_COMPILE_OPTIONS PRIVATE_COMPILE_OPTIONS
                    PUBLIC_LINK_OPTIONS PRIVATE_LINK_OPTIONS "${ARGN}")
endmacro()

# pw_add_library_generic: Creates a CMake library target.
#
# Required Args:
#
#   <name> - The name of the library target to be created.
#   <type> - The library type which must be INTERFACE, OBJECT, STATIC, or
#            SHARED.
#
# Optional Args:
#
#   SOURCES - source files for this library
#   HEADERS - header files for this library
#   PUBLIC_DEPS - public pw_target_link_targets arguments
#   PRIVATE_DEPS - private pw_target_link_targets arguments
#   PUBLIC_INCLUDES - public target_include_directories argument
#   PRIVATE_INCLUDES - public target_include_directories argument
#   PUBLIC_DEFINES - public target_compile_definitions arguments
#   PRIVATE_DEFINES - private target_compile_definitions arguments
#   PUBLIC_COMPILE_OPTIONS - public target_compile_options arguments
#   PRIVATE_COMPILE_OPTIONS - private target_compile_options arguments
#   PRIVATE_COMPILE_OPTIONS_DEPS_BEFORE - private target_compile_options BEFORE
#     arguments from the specified deps's INTERFACE_COMPILE_OPTIONS. Note that
#     these deps are not pulled in as target_link_libraries. This should not be
#     exposed by the non-generic API.
#   PUBLIC_LINK_OPTIONS - public target_link_options arguments
#   PRIVATE_LINK_OPTIONS - private target_link_options arguments
function(pw_add_library_generic NAME TYPE)
  set(supported_library_types INTERFACE OBJECT STATIC SHARED)
  if(NOT "${TYPE}" IN_LIST supported_library_types)
    message(FATAL_ERROR "\"${TYPE}\" is not a valid library type for ${NAME}. "
          "Must be INTERFACE, OBJECT, STATIC, or SHARED.")
  endif()

  _pw_add_library_multi_value_args(multi_value_args)
  pw_parse_arguments(
    NUM_POSITIONAL_ARGS
      2
    MULTI_VALUE_ARGS
      ${multi_value_args}
      PRIVATE_COMPILE_OPTIONS_DEPS_BEFORE
  )

  # CMake 3.22 does not have a notion of target_headers yet, so in the mean
  # time we ask for headers to be specified for consistency with GN & Bazel and
  # to improve the IDE experience. However, we do want to ensure all the headers
  # which are otherwise ignored by CMake are present.
  #
  # See https://gitlab.kitware.com/cmake/cmake/-/issues/22468 for adding support
  # to CMake to associate headers with targets properly for CMake 3.23.
  foreach(header IN ITEMS ${arg_HEADERS})
    get_filename_component(header "${header}" ABSOLUTE)
    if(NOT EXISTS ${header})
      message(FATAL_ERROR "Header not found: \"${header}\"")
    endif()
  endforeach()

  # In order to more easily create the various types of libraries, two hidden
  # targets are created: NAME._config and NAME._public_config which loosely
  # mirror the GN configs although we also carry target link dependencies
  # through these.

  # Add the NAME._config target_link_libraries dependency with the
  # PRIVATE_INCLUDES, PRIVATE_DEFINES, PRIVATE_COMPILE_OPTIONS,
  # PRIVATE_LINK_OPTIONS, and PRIVATE_DEPS.
  add_library("${NAME}._config" INTERFACE EXCLUDE_FROM_ALL)
  target_include_directories("${NAME}._config"
    INTERFACE
      ${arg_PRIVATE_INCLUDES}
  )
  target_compile_definitions("${NAME}._config"
    INTERFACE
      ${arg_PRIVATE_DEFINES}
  )
  target_compile_options("${NAME}._config"
    INTERFACE
      ${arg_PRIVATE_COMPILE_OPTIONS}
  )
  target_link_options("${NAME}._config"
    INTERFACE
      ${arg_PRIVATE_LINK_OPTIONS}
  )
  pw_target_link_targets("${NAME}._config"
    INTERFACE
      ${arg_PRIVATE_DEPS}
  )

  # Add the NAME._public_config target_link_libraries dependency with the
  # PUBLIC_INCLUDES, PUBLIC_DEFINES, PUBLIC_COMPILE_OPTIONS,
  # PUBLIC_LINK_OPTIONS, and PUBLIC_DEPS.
  add_library("${NAME}._public_config" INTERFACE EXCLUDE_FROM_ALL)
  target_include_directories("${NAME}._public_config"
    INTERFACE
      ${arg_PUBLIC_INCLUDES}
  )
  target_compile_definitions("${NAME}._public_config"
    INTERFACE
      ${arg_PUBLIC_DEFINES}
  )
  target_compile_options("${NAME}._public_config"
    INTERFACE
      ${arg_PUBLIC_COMPILE_OPTIONS}
  )
  target_link_options("${NAME}._public_config"
    INTERFACE
      ${arg_PUBLIC_LINK_OPTIONS}
  )
  pw_target_link_targets("${NAME}._public_config"
    INTERFACE
      ${arg_PUBLIC_DEPS}
  )

  # Instantiate the library depending on the type using the NAME._config and
  # NAME._public_config libraries we just created.
  if("${TYPE}" STREQUAL "INTERFACE")
    if(NOT "${arg_SOURCES}" STREQUAL "")
      message(
        SEND_ERROR "${NAME} cannot have sources as it's an INTERFACE library")
    endif(NOT "${arg_SOURCES}" STREQUAL "")

    add_library("${NAME}" INTERFACE EXCLUDE_FROM_ALL)
    target_sources("${NAME}" PRIVATE ${arg_HEADERS})
    pw_target_link_targets("${NAME}"
      INTERFACE
        "${NAME}._public_config"
    )
  elseif(("${TYPE}" STREQUAL "STATIC") OR ("${TYPE}" STREQUAL "SHARED"))
    if("${arg_SOURCES}" STREQUAL "")
      message(
        SEND_ERROR "${NAME} must have SOURCES as it's not an INTERFACE library")
    endif("${arg_SOURCES}" STREQUAL "")

    add_library("${NAME}" "${TYPE}" EXCLUDE_FROM_ALL)
    target_sources("${NAME}" PRIVATE ${arg_HEADERS} ${arg_SOURCES})
    pw_target_link_targets("${NAME}"
      PUBLIC
        "${NAME}._public_config"
      PRIVATE
        "${NAME}._config"
    )
    foreach(compile_option_dep IN LISTS arg_PRIVATE_COMPILE_OPTIONS_DEPS_BEFORE)
      # This will fail at build time if the target does not exist.
      target_compile_options("${NAME}" BEFORE PRIVATE
          $<TARGET_PROPERTY:${compile_option_dep},INTERFACE_COMPILE_OPTIONS>
      )
    endforeach()
  elseif("${TYPE}" STREQUAL "OBJECT")
    if("${arg_SOURCES}" STREQUAL "")
      message(
        SEND_ERROR "${NAME} must have SOURCES as it's not an INTERFACE library")
    endif("${arg_SOURCES}" STREQUAL "")

    # In order to support OBJECT libraries while maintaining transitive
    # linking dependencies, the library has to be split up into two where the
    # outer interface library forwards not only the internal object library
    # but also its TARGET_OBJECTS.
    add_library("${NAME}._object" OBJECT EXCLUDE_FROM_ALL)
    target_sources("${NAME}._object" PRIVATE ${arg_SOURCES})
    pw_target_link_targets("${NAME}._object"
      PRIVATE
        "${NAME}._public_config"
        "${NAME}._config"
    )
    foreach(compile_option_dep IN LISTS arg_PRIVATE_COMPILE_OPTIONS_DEPS_BEFORE)
      # This will fail at build time if the target does not exist.
      target_compile_options("${NAME}._object" BEFORE PRIVATE
          $<TARGET_PROPERTY:${compile_option_dep},INTERFACE_COMPILE_OPTIONS>
      )
    endforeach()

    add_library("${NAME}" INTERFACE EXCLUDE_FROM_ALL)
    target_sources("${NAME}" PRIVATE ${arg_HEADERS})
    pw_target_link_targets("${NAME}"
      INTERFACE
        "${NAME}._public_config"
        "${NAME}._object"
    )
    target_link_libraries("${NAME}"
      INTERFACE
        $<TARGET_OBJECTS:${NAME}._object>
    )
  else()
    message(FATAL_ERROR "Unsupported libary type: ${TYPE}")
  endif()
endfunction(pw_add_library_generic)

# Checks that the library's name is prefixed by the relative path with dot
# separators instead of forward slashes. Ignores paths not under the root
# directory.
#
# Optional Args:
#
#   REMAP_PREFIXES - support remapping a prefix for checks
#
function(_pw_check_name_is_relative_to_root NAME ROOT)
  pw_parse_arguments(
    NUM_POSITIONAL_ARGS
      2
    MULTI_VALUE_ARGS
      REMAP_PREFIXES
  )

  file(RELATIVE_PATH rel_path "${ROOT}" "${CMAKE_CURRENT_SOURCE_DIR}")
  if("${rel_path}" MATCHES "^\\.\\.")
    return()  # Ignore paths not under ROOT
  endif()

  list(LENGTH arg_REMAP_PREFIXES remap_arg_count)
  if("${remap_arg_count}" EQUAL 2)
    list(GET arg_REMAP_PREFIXES 0 from_prefix)
    list(GET arg_REMAP_PREFIXES 1 to_prefix)
    string(REGEX REPLACE "^${from_prefix}" "${to_prefix}" rel_path "${rel_path}")
  elseif(NOT "${remap_arg_count}" EQUAL 0)
    message(FATAL_ERROR
        "If REMAP_PREFIXES is specified, exactly two arguments must be given.")
  endif()

  if(NOT "${rel_path}" MATCHES "^\\.\\..*")
    string(REPLACE "/" "." dot_rel_path "${rel_path}")
    if(NOT "${NAME}" MATCHES "^${dot_rel_path}(\\.[^\\.]+)?(\\.facade)?$")
      message(FATAL_ERROR
          "Module libraries under ${ROOT} must match the module name or be in "
          "the form 'PATH_TO.THE_TARGET.NAME'. The library '${NAME}' does not "
          "match. Expected ${dot_rel_path}.LIBRARY_NAME"
      )
    endif()
  endif()
endfunction(_pw_check_name_is_relative_to_root)

# Creates a pw module library.
#
# Required Args:
#
#   <name> - The name of the library target to be created.
#   <type> - The library type which must be INTERFACE, OBJECT, STATIC or SHARED.
#
# Optional Args:
#
#   SOURCES - source files for this library
#   HEADERS - header files for this library
#   PUBLIC_DEPS - public pw_target_link_targets arguments
#   PRIVATE_DEPS - private pw_target_link_targets arguments
#   PUBLIC_INCLUDES - public target_include_directories argument
#   PRIVATE_INCLUDES - public target_include_directories argument
#   PUBLIC_DEFINES - public target_compile_definitions arguments
#   PRIVATE_DEFINES - private target_compile_definitions arguments
#   PUBLIC_COMPILE_OPTIONS - public target_compile_options arguments
#   PRIVATE_COMPILE_OPTIONS - private target_compile_options arguments
#   PUBLIC_LINK_OPTIONS - public target_link_options arguments
#   PRIVATE_LINK_OPTIONS - private target_link_options arguments
#
function(pw_add_library NAME TYPE)
  _pw_add_library_multi_value_args(pw_add_library_generic_multi_value_args)
  pw_parse_arguments(
    NUM_POSITIONAL_ARGS
      2
    MULTI_VALUE_ARGS
      ${pw_add_library_generic_multi_value_args}
  )

  _pw_check_name_is_relative_to_root("${NAME}" "$ENV{PW_ROOT}"
    REMAP_PREFIXES
      third_party pw_third_party
  )

  pw_add_library_generic(${NAME} ${TYPE}
    SOURCES
      ${arg_SOURCES}
    HEADERS
      ${arg_HEADERS}
    PUBLIC_DEPS
      # TODO(b/232141950): Apply compilation options that affect ABI
      # globally in the CMake build instead of injecting them into libraries.
      pw_build
      ${arg_PUBLIC_DEPS}
    PRIVATE_DEPS
      ${arg_PRIVATE_DEPS}
    PUBLIC_INCLUDES
      ${arg_PUBLIC_INCLUDES}
    PRIVATE_INCLUDES
      ${arg_PRIVATE_INCLUDES}
    PUBLIC_DEFINES
      ${arg_PUBLIC_DEFINES}
    PRIVATE_DEFINES
      ${arg_PRIVATE_DEFINES}
    PUBLIC_COMPILE_OPTIONS
      ${arg_PUBLIC_COMPILE_OPTIONS}
    PRIVATE_COMPILE_OPTIONS_DEPS_BEFORE
      pw_build.warnings
    PRIVATE_COMPILE_OPTIONS
      ${arg_PRIVATE_COMPILE_OPTIONS}
    PUBLIC_LINK_OPTIONS
      ${arg_PUBLIC_LINK_OPTIONS}
    PRIVATE_LINK_OPTIONS
      ${arg_PRIVATE_LINK_OPTIONS}
  )
endfunction(pw_add_library)

# Declares a module as a facade.
#
# Facades are declared as two libraries to avoid circular dependencies.
# Libraries that use the facade depend on a library named for the module. The
# module that implements the facade depends on a library named
# MODULE_NAME.facade.
#
# pw_add_facade accepts the same arguments as pw_add_library.
# It also accepts the following argument:
#
#  BACKEND - The name of the facade's backend variable.
function(pw_add_facade NAME TYPE)
  _pw_add_library_multi_value_args(multi_value_args)
  pw_parse_arguments(
    NUM_POSITIONAL_ARGS
      2
    ONE_VALUE_ARGS
      BACKEND
    MULTI_VALUE_ARGS
      ${multi_value_args}
  )

  _pw_check_name_is_relative_to_root("${NAME}" "$ENV{PW_ROOT}"
    REMAP_PREFIXES
      third_party pw_third_party
  )

  pw_add_facade_generic("${NAME}" "${TYPE}"
    BACKEND
      ${arg_BACKEND}
    SOURCES
      ${arg_SOURCES}
    HEADERS
      ${arg_HEADERS}
    PUBLIC_DEPS
      # TODO(b/232141950): Apply compilation options that affect ABI
      # globally in the CMake build instead of injecting them into libraries.
      pw_build
      ${arg_PUBLIC_DEPS}
    PRIVATE_DEPS
      ${arg_PRIVATE_DEPS}
    PUBLIC_INCLUDES
      ${arg_PUBLIC_INCLUDES}
    PRIVATE_INCLUDES
      ${arg_PRIVATE_INCLUDES}
    PUBLIC_DEFINES
      ${arg_PUBLIC_DEFINES}
    PRIVATE_DEFINES
      ${arg_PRIVATE_DEFINES}
    PUBLIC_COMPILE_OPTIONS
      ${arg_PUBLIC_COMPILE_OPTIONS}
    PRIVATE_COMPILE_OPTIONS_DEPS_BEFORE
      pw_build.warnings
    PRIVATE_COMPILE_OPTIONS
      ${arg_PRIVATE_COMPILE_OPTIONS}
    PUBLIC_LINK_OPTIONS
      ${arg_PUBLIC_LINK_OPTIONS}
    PRIVATE_LINK_OPTIONS
      ${arg_PRIVATE_LINK_OPTIONS}
  )
endfunction(pw_add_facade)

# pw_add_facade_generic: Creates a CMake facade library target.
#
# Facades are declared as two libraries to avoid circular dependencies.
# Libraries that use the facade depend on the <name> of this target. The
# libraries that implement this facade have to depend on an internal library
# named <name>.facade.
#
# Required Args:
#
#   <name> - The name for the public facade target (<name>) for all users and
#            the suffixed facade target for backend implementers (<name.facade).
#   <type> - The library type which must be INTERFACE, OBJECT, STATIC, or
#            SHARED.
#   BACKEND - The name of the facade's backend variable.
#
# Optional Args:
#
#   SOURCES - source files for this library
#   HEADERS - header files for this library
#   PUBLIC_DEPS - public pw_target_link_targets arguments
#   PRIVATE_DEPS - private pw_target_link_targets arguments
#   PUBLIC_INCLUDES - public target_include_directories argument
#   PRIVATE_INCLUDES - public target_include_directories argument
#   PUBLIC_DEFINES - public target_compile_definitions arguments
#   PRIVATE_DEFINES - private target_compile_definitions arguments
#   PUBLIC_COMPILE_OPTIONS - public target_compile_options arguments
#   PRIVATE_COMPILE_OPTIONS_DEPS_BEFORE - private target_compile_options BEFORE
#     arguments from the specified deps's INTERFACE_COMPILE_OPTIONS. Note that
#     these deps are not pulled in as target_link_libraries. This should not be
#     exposed by the non-generic API.
#   PRIVATE_COMPILE_OPTIONS - private target_compile_options arguments
#   PUBLIC_LINK_OPTIONS - public target_link_options arguments
#   PRIVATE_LINK_OPTIONS - private target_link_options arguments
function(pw_add_facade_generic NAME TYPE)
  set(supported_library_types INTERFACE OBJECT STATIC SHARED)
  if(NOT "${TYPE}" IN_LIST supported_library_types)
    message(FATAL_ERROR "\"${TYPE}\" is not a valid library type for ${NAME}. "
          "Must be INTERFACE, OBJECT, STATIC, or SHARED.")
  endif()

  _pw_add_library_multi_value_args(multi_value_args)
  pw_parse_arguments(
    NUM_POSITIONAL_ARGS
      2
    ONE_VALUE_ARGS
      BACKEND
    MULTI_VALUE_ARGS
      ${multi_value_args}
      PRIVATE_COMPILE_OPTIONS_DEPS_BEFORE
    REQUIRED_ARGS
      BACKEND
  )

  if(NOT DEFINED "${arg_BACKEND}")
    message(FATAL_ERROR "${NAME}'s backend variable ${arg_BACKEND} has not "
            "been defined, you may be missing a pw_add_backend_variable or "
            "the *.cmake import to that file.")
  endif()
  string(REGEX MATCH ".+_BACKEND" backend_ends_in_backend "${arg_BACKEND}")
  if(NOT backend_ends_in_backend)
    message(FATAL_ERROR "The ${NAME} pw_add_generic_facade's BACKEND argument "
            "(${arg_BACKEND}) must end in _BACKEND (${name_ends_in_backend})")
  endif()

  set(backend_target "${${arg_BACKEND}}")
  if ("${backend_target}" STREQUAL "")
    # If no backend is set, a script that displays an error message is used
    # instead. If the facade is used in the build, it fails with this error.
    pw_add_error_target("${NAME}.NO_BACKEND_SET"
      MESSAGE
        "Attempted to build the ${NAME} facade with no backend set. "
        "Configure the ${NAME} backend using pw_set_backend or remove all "
        "dependencies on it. See https://pigweed.dev/pw_build."
    )

    set(backend_target "${NAME}.NO_BACKEND_SET")
  endif()

  # Define the facade library, which is used by the backend to avoid circular
  # dependencies.
  pw_add_library_generic("${NAME}.facade" INTERFACE
    HEADERS
      ${arg_HEADERS}
    PUBLIC_INCLUDES
      ${arg_PUBLIC_INCLUDES}
    PUBLIC_DEPS
      ${arg_PUBLIC_DEPS}
    PUBLIC_DEFINES
      ${arg_PUBLIC_DEFINES}
    PUBLIC_COMPILE_OPTIONS
      ${arg_PUBLIC_COMPILE_OPTIONS}
    PUBLIC_LINK_OPTIONS
      ${arg_PUBLIC_LINK_OPTIONS}
  )

  # Define the public-facing library for this facade, which depends on the
  # header files and public interface aspects from the .facade target and
  # exposes the dependency on the backend along with the private library
  # target components.
  pw_add_library_generic("${NAME}" "${TYPE}"
    PUBLIC_DEPS
      "${NAME}.facade"
      "${backend_target}"
    SOURCES
      ${arg_SOURCES}
    PRIVATE_INCLUDES
      ${arg_PRIVATE_INCLUDES}
    PRIVATE_DEPS
      ${arg_PRIVATE_DEPS}
    PRIVATE_DEFINES
      ${arg_PRIVATE_DEFINES}
    PRIVATE_COMPILE_OPTIONS_DEPS_BEFORE
      ${arg_PRIVATE_COMPILE_OPTIONS_DEPS_BEFORE}
    PRIVATE_COMPILE_OPTIONS
      ${arg_PRIVATE_COMPILE_OPTIONS}
    PRIVATE_LINK_OPTIONS
      ${arg_PRIVATE_LINK_OPTIONS}
  )
endfunction(pw_add_facade_generic)

# Declare a facade's backend variables which can be overriden later by using
# pw_set_backend.
#
# Required Arguments:
#   NAME - Name of the facade's backend variable.
#
# Optional Arguments:
#   DEFAULT_BACKEND - Optional default backend selection for the facade.
#
function(pw_add_backend_variable NAME)
  pw_parse_arguments(
    NUM_POSITIONAL_ARGS
      1
    ONE_VALUE_ARGS
      DEFAULT_BACKEND
  )

  string(REGEX MATCH ".+_BACKEND" name_ends_in_backend "${NAME}")
  if(NOT name_ends_in_backend)
    message(FATAL_ERROR "The ${NAME} pw_add_backend_variable's NAME argument "
            "must end in _BACKEND")
  endif()

  set("${NAME}" "${arg_DEFAULT_BACKEND}" CACHE STRING
      "${NAME} backend variable for a facade")
endfunction()

# Sets which backend to use for the given facade's backend variable.
function(pw_set_backend NAME BACKEND)
  # TODO(ewout, hepler): Deprecate this temporarily support which permits the
  # direct facade name directly, instead of the facade's backend variable name.
  # Also update this to later assert the variable is DEFINED to catch typos.
  string(REGEX MATCH ".+_BACKEND" name_ends_in_backend "${NAME}")
  if(NOT name_ends_in_backend)
    set(NAME "${NAME}_BACKEND")
  endif()
  if(NOT DEFINED "${NAME}")
    message(WARNING "${NAME} was not defined when pw_set_backend was invoked, "
            "you may be missing a pw_add_backend_variable or the *.cmake "
            "import to that file.")
  endif()

  set("${NAME}" "${BACKEND}" CACHE STRING "backend variable for a facade" FORCE)
endfunction(pw_set_backend)

# Zephyr specific wrapper for pw_set_backend, selects the default zephyr backend based on a Kconfig while
# still allowing the application to set the backend if they choose to
function(pw_set_zephyr_backend_ifdef COND FACADE BACKEND)
  get_property(result CACHE "${FACADE}_BACKEND" PROPERTY TYPE)
  if(${${COND}})
    if("${result}" STREQUAL "")
      pw_set_backend("${FACADE}" "${BACKEND}")
    endif()
  endif()
endfunction()

# Set up the default pw_build_DEFAULT_MODULE_CONFIG.
set("pw_build_DEFAULT_MODULE_CONFIG" pw_build.empty CACHE STRING
    "Default implementation for all Pigweed module configurations.")

# Declares a module configuration variable for module libraries to depend on.
# Configs should be set to libraries which can be used to provide defines
# directly or though included header files.
#
# The configs can be selected either through the pw_set_module_config function
# to set the pw_build_DEFAULT_MODULE_CONFIG used by default for all Pigweed
# modules or by selecting a specific one for the given NAME'd configuration.
#
# Args:
#
#   NAME: name to use for the target which can be depended on for the config.
function(pw_add_module_config NAME)
  pw_parse_arguments(NUM_POSITIONAL_ARGS 1)

  # Declare the module configuration variable for this module.
  set("${NAME}" "${pw_build_DEFAULT_MODULE_CONFIG}"
      CACHE STRING "Module configuration for ${NAME}")
endfunction(pw_add_module_config)

# Sets which config library to use for the given module.
#
# This can be used to set a specific module configuration or the default
# module configuration used for all Pigweed modules:
#
#   pw_set_module_config(pw_build_DEFAULT_MODULE_CONFIG my_config)
#   pw_set_module_config(pw_foo_CONFIG my_foo_config)
function(pw_set_module_config NAME LIBRARY)
  pw_parse_arguments(NUM_POSITIONAL_ARGS 2)

  # Update the module configuration variable.
  set("${NAME}" "${LIBRARY}" CACHE STRING "Config for ${NAME}" FORCE)
endfunction(pw_set_module_config)

# Adds compiler options to all targets built by CMake. Flags may be added any
# time after this function is defined. The effect is global; all targets added
# before or after a pw_add_global_compile_options call will be built with the
# flags, regardless of where the files are located.
#
# pw_add_global_compile_options takes one optional named argument:
#
#   LANGUAGES: Which languages (ASM, C, CXX) to apply the options to. Flags
#       apply to all languages by default.
#
# All other arguments are interpreted as compiler options.
function(pw_add_global_compile_options)
  cmake_parse_arguments(PARSE_ARGV 0 args "" "" "LANGUAGES")

  set(supported_build_languages ASM C CXX)

  if(NOT args_LANGUAGES)
    set(args_LANGUAGES ${supported_build_languages})
  endif()

  # Check the selected language.
  foreach(lang IN LISTS args_LANGUAGES)
    if(NOT "${lang}" IN_LIST supported_build_languages)
      message(FATAL_ERROR "'${lang}' is not a supported language. "
              "Supported languages: ${supported_build_languages}")
    endif()
  endforeach()

  # Enumerate which flags variables to set.
  foreach(lang IN LISTS args_LANGUAGES)
    list(APPEND cmake_flags_variables "CMAKE_${lang}_FLAGS")
  endforeach()

  # Set each flag for each specified flags variable.
  foreach(variable IN LISTS cmake_flags_variables)
    foreach(flag IN LISTS args_UNPARSED_ARGUMENTS)
      set(${variable} "${${variable}} ${flag}" CACHE INTERNAL "" FORCE)
    endforeach()
  endforeach()
endfunction(pw_add_global_compile_options)

# pw_add_error_target: Creates a CMake target which fails to build and prints a
#                      message
#
# This function prints a message and causes a build failure only if you attempt
# to build the target. This is useful when FATAL_ERROR messages cannot be used
# to catch problems during the CMake configuration phase.
#
# Args:
#
#   NAME: name to use for the target
#   MESSAGE: The message to print, prefixed with "ERROR: ". The message may be
#            composed of multiple pieces by passing multiple strings.
#
function(pw_add_error_target NAME)
  pw_parse_arguments(
    NUM_POSITIONAL_ARGS
      1
    MULTI_VALUE_ARGS
      MESSAGE
  )

  # In case the message is comprised of multiple strings, stitch them together.
  set(message "ERROR: ")
  foreach(line IN LISTS arg_MESSAGE)
    string(APPEND message "${line}")
  endforeach()

  add_custom_target("${NAME}._error_message"
    COMMAND
      "${CMAKE_COMMAND}" -E echo "${message}"
    COMMAND
      "${CMAKE_COMMAND}" -E false
  )

  # A static library is provided, in case this rule nominally provides a
  # compiled output, e.g. to enable $<TARGET_FILE:"${NAME}">.
  pw_add_library_generic("${NAME}" STATIC
    SOURCES
      $<TARGET_PROPERTY:pw_build.empty,SOURCES>
  )
  add_dependencies("${NAME}" "${NAME}._error_message")
endfunction(pw_add_error_target)
