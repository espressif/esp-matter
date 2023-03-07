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

include($ENV{PW_ROOT}/pw_build/pigweed.cmake)

# Declares a protocol buffers library. This function creates a library for each
# supported protocol buffer implementation:
#
#   ${NAME}.pwpb - pw_protobuf generated code
#   ${NAME}.nanopb - Nanopb generated code (requires Nanopb)
#
# This function also creates libraries for generating pw_rpc code:
#
#   ${NAME}.pwpb_rpc - generates pw_protobuf pw_rpc code
#   ${NAME}.nanopb_rpc - generates Nanopb pw_rpc code
#   ${NAME}.raw_rpc - generates raw pw_rpc (no protobuf library) code
#
# Args:
#
#   NAME - the base name of the libraries to create
#   SOURCES - .proto source files
#   DEPS - dependencies on other pw_proto_library targets
#   PREFIX - prefix add to the proto files
#   STRIP_PREFIX - prefix to remove from the proto files
#   INPUTS - files to include along with the .proto files (such as Nanopb
#       .options files)
#
function(pw_proto_library NAME)
  pw_parse_arguments(
    NUM_POSITIONAL_ARGS
      1
    ONE_VALUE_ARGS
      STRIP_PREFIX
      PREFIX
    MULTI_VALUE_ARGS
      SOURCES
      INPUTS
      DEPS
    REQUIRED_ARGS
      SOURCES
  )

  set(out_dir "${CMAKE_CURRENT_BINARY_DIR}/${NAME}")

  # Use INTERFACE libraries to track the proto include paths that are passed to
  # protoc.
  set(include_deps "${arg_DEPS}")
  list(TRANSFORM include_deps APPEND ._includes)

  pw_add_library_generic("${NAME}._includes" INTERFACE
    PUBLIC_INCLUDES
      "${out_dir}/sources"
    PUBLIC_DEPS
      ${include_deps}
  )

  # Generate a file with all include paths needed by protoc. Use the include
  # directory paths and replace ; with \n.
  set(include_file "${out_dir}/include_paths.txt")
  file(GENERATE OUTPUT "${include_file}"
     CONTENT
       "$<JOIN:$<TARGET_PROPERTY:${NAME}._includes,INTERFACE_INCLUDE_DIRECTORIES>,\n>")

  if("${arg_STRIP_PREFIX}" STREQUAL "")
    set(arg_STRIP_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}")
  else()
    get_filename_component(arg_STRIP_PREFIX "${arg_STRIP_PREFIX}" ABSOLUTE)
  endif()

  foreach(path IN LISTS arg_SOURCES arg_INPUTS)
    get_filename_component(abspath "${path}" ABSOLUTE)
    list(APPEND files_to_mirror "${abspath}")
  endforeach()

  # Mirror the sources to the output directory with the specified prefix.
  _pw_rebase_paths(
      sources "${out_dir}/sources/${arg_PREFIX}" "${arg_STRIP_PREFIX}"
      "${arg_SOURCES}" "")
  _pw_rebase_paths(
      inputs "${out_dir}/sources/${arg_PREFIX}" "${arg_STRIP_PREFIX}"
      "${arg_INPUTS}" "")

  add_custom_command(
    COMMAND
      python3
      "$ENV{PW_ROOT}/pw_build/py/pw_build/mirror_tree.py"
      --source-root "${arg_STRIP_PREFIX}"
      --directory "${out_dir}/sources/${arg_PREFIX}"
      ${files_to_mirror}
    DEPENDS
      "$ENV{PW_ROOT}/pw_build/py/pw_build/mirror_tree.py"
      ${files_to_mirror}
    OUTPUT
      ${sources} ${inputs}
  )
  add_custom_target("${NAME}._sources" DEPENDS ${sources} ${inputs})

  set(sources_deps "${arg_DEPS}")
  list(TRANSFORM sources_deps APPEND ._sources)

  if(sources_deps)
    add_dependencies("${NAME}._sources" ${sources_deps})
  endif()

  # Create a protobuf target for each supported protobuf library.
  _pw_pwpb_library("${NAME}"
    SOURCES
      ${sources}
    INPUTS
      ${inputs}
    DEPS
      ${arg_DEPS}
    INCLUDE_FILE
      "${include_file}"
    OUT_DIR
      "${out_dir}"
  )
  _pw_pwpb_rpc_library("${NAME}"
    SOURCES
      ${sources}
    INPUTS
      ${inputs}
    DEPS
      ${arg_DEPS}
    INCLUDE_FILE
      "${include_file}"
    OUT_DIR
      "${out_dir}"
  )
  _pw_raw_rpc_library("${NAME}"
    SOURCES
      ${sources}
    INPUTS
      ${inputs}
    DEPS
      ${arg_DEPS}
    INCLUDE_FILE
      "${include_file}"
    OUT_DIR
      "${out_dir}"
  )
  _pw_nanopb_library("${NAME}"
    SOURCES
      ${sources}
    INPUTS
      ${inputs}
    DEPS
      ${arg_DEPS}
    INCLUDE_FILE
      "${include_file}"
    OUT_DIR
      "${out_dir}"
  )
  _pw_nanopb_rpc_library("${NAME}"
    SOURCES
      ${sources}
    INPUTS
      ${inputs}
    DEPS
      ${arg_DEPS}
    INCLUDE_FILE
      "${include_file}"
    OUT_DIR
      "${out_dir}"
  )
endfunction(pw_proto_library)

function(_pw_rebase_paths VAR OUT_DIR ROOT FILES EXTENSIONS)
  foreach(file IN LISTS FILES)
    get_filename_component(file "${file}" ABSOLUTE)
    file(RELATIVE_PATH file "${ROOT}" "${file}")

    if ("${EXTENSIONS}" STREQUAL "")
      list(APPEND mirrored_files "${OUT_DIR}/${file}")
    else()
      foreach(ext IN LISTS EXTENSIONS)
        get_filename_component(dir "${file}" DIRECTORY)
        get_filename_component(name "${file}" NAME_WE)
        list(APPEND mirrored_files "${OUT_DIR}/${dir}/${name}${ext}")
      endforeach()
    endif()
  endforeach()

  set("${VAR}" "${mirrored_files}" PARENT_SCOPE)
endfunction(_pw_rebase_paths)

# Internal function that invokes protoc through generate_protos.py.
function(_pw_generate_protos TARGET LANGUAGE)
  pw_parse_arguments(
    NUM_POSITIONAL_ARGS
      2
    ONE_VALUE_ARGS
      PLUGIN
      INCLUDE_FILE
      OUT_DIR
    MULTI_VALUE_ARGS
      OUTPUT_EXTS
      SOURCES
      INPUTS
      DEPENDS
  )

  # Determine the names of the compiled output files.
  _pw_rebase_paths(outputs
      "${arg_OUT_DIR}/${LANGUAGE}" "${arg_OUT_DIR}/sources" "${arg_SOURCES}"
      "${arg_OUTPUT_EXTS}")

  # Export the output files to the caller's scope so it can use them if needed.
  set(generated_outputs "${outputs}" PARENT_SCOPE)

  if("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Windows")
      get_filename_component(dir "${source_file}" DIRECTORY)
      get_filename_component(name "${source_file}" NAME_WE)
      set(arg_PLUGIN "${dir}/${name}.bat")
  endif()

  set(script "$ENV{PW_ROOT}/pw_protobuf_compiler/py/pw_protobuf_compiler/generate_protos.py")
  add_custom_command(
    COMMAND
      python3
      "${script}"
      --language "${LANGUAGE}"
      --plugin-path "${arg_PLUGIN}"
      --include-file "${arg_INCLUDE_FILE}"
      --compile-dir "${arg_OUT_DIR}/sources"
      --out-dir "${arg_OUT_DIR}/${LANGUAGE}"
      --sources ${arg_SOURCES}
    DEPENDS
      ${script}
      ${arg_SOURCES}
      ${arg_INPUTS}
      ${arg_DEPENDS}
    OUTPUT
      ${outputs}
  )
  add_custom_target("${TARGET}._generate.${LANGUAGE}" DEPENDS ${outputs})
  add_dependencies("${TARGET}._generate.${LANGUAGE}" "${TARGET}._sources")
endfunction(_pw_generate_protos)

# Internal function that creates a pwpb proto library.
function(_pw_pwpb_library NAME)
  pw_parse_arguments(
    NUM_POSITIONAL_ARGS
      1
    ONE_VALUE_ARGS
      INCLUDE_FILE
      OUT_DIR
    MULTI_VALUE_ARGS
      SOURCES
      INPUTS
      DEPS
  )

  list(TRANSFORM arg_DEPS APPEND .pwpb)

  _pw_generate_protos("${NAME}" pwpb
    PLUGIN
      "$ENV{PW_ROOT}/pw_protobuf/py/pw_protobuf/plugin.py"
    OUTPUT_EXTS
      ".pwpb.h"
    INCLUDE_FILE
      "${arg_INCLUDE_FILE}"
    OUT_DIR
      "${arg_OUT_DIR}"
    SOURCES
      ${arg_SOURCES}
    INPUTS
      ${arg_INPUTS}
    DEPENDS
      ${arg_DEPS}
  )

  # Create the library with the generated source files.
  pw_add_library_generic("${NAME}.pwpb" INTERFACE
    PUBLIC_INCLUDES
      "${arg_OUT_DIR}/pwpb"
    PUBLIC_DEPS
      pw_build
      pw_protobuf
      pw_span
      pw_string.string
      ${arg_DEPS}
  )
  add_dependencies("${NAME}.pwpb" "${NAME}._generate.pwpb")
endfunction(_pw_pwpb_library)

# Internal function that creates a pwpb_rpc library.
function(_pw_pwpb_rpc_library NAME)
  pw_parse_arguments(
    NUM_POSITIONAL_ARGS
      1
    ONE_VALUE_ARGS
      INCLUDE_FILE
      OUT_DIR
    MULTI_VALUE_ARGS
      SOURCES
      INPUTS
      DEPS
  )

  # Determine the names of the output files.
  list(TRANSFORM arg_DEPS APPEND .pwpb_rpc)

  _pw_generate_protos("${NAME}" pwpb_rpc
    PLUGIN
      "$ENV{PW_ROOT}/pw_rpc/py/pw_rpc/plugin_pwpb.py"
    OUTPUT_EXTS
      ".rpc.pwpb.h"
    INCLUDE_FILE
      "${arg_INCLUDE_FILE}"
    OUT_DIR
      "${arg_OUT_DIR}"
    SOURCES
      ${arg_SOURCES}
    INPUTS
      ${arg_INPUTS}
    DEPENDS
      ${arg_DEPS}
      "$ENV{PW_ROOT}/pw_rpc/py/pw_rpc/plugin.py"
  )

  # Create the library with the generated source files.
  pw_add_library_generic("${NAME}.pwpb_rpc" INTERFACE
    PUBLIC_INCLUDES
      "${arg_OUT_DIR}/pwpb_rpc"
    PUBLIC_DEPS
      "${NAME}.pwpb"
      pw_build
      pw_rpc.pwpb.client_api
      pw_rpc.pwpb.server_api
      pw_rpc.server
      ${arg_DEPS}
  )
  add_dependencies("${NAME}.pwpb_rpc" "${NAME}._generate.pwpb_rpc")
endfunction(_pw_pwpb_rpc_library)

# Internal function that creates a raw_rpc proto library.
function(_pw_raw_rpc_library NAME)
  pw_parse_arguments(
    NUM_POSITIONAL_ARGS
      1
    ONE_VALUE_ARGS
      INCLUDE_FILE
      OUT_DIR
    MULTI_VALUE_ARGS
      SOURCES
      INPUTS
      DEPS
  )

  list(TRANSFORM arg_DEPS APPEND .raw_rpc)

  _pw_generate_protos("${NAME}" raw_rpc
    PLUGIN
      "$ENV{PW_ROOT}/pw_rpc/py/pw_rpc/plugin_raw.py"
    OUTPUT_EXTS
      ".raw_rpc.pb.h"
    INCLUDE_FILE
      "${arg_INCLUDE_FILE}"
    OUT_DIR
      "${arg_OUT_DIR}"
    SOURCES
      ${arg_SOURCES}
    INPUTS
      ${arg_INPUTS}
    DEPENDS
      ${arg_DEPS}
      "$ENV{PW_ROOT}/pw_rpc/py/pw_rpc/plugin.py"
  )

  # Create the library with the generated source files.
  pw_add_library_generic("${NAME}.raw_rpc" INTERFACE
    PUBLIC_INCLUDES
      "${arg_OUT_DIR}/raw_rpc"
    PUBLIC_DEPS
      pw_build
      pw_rpc.raw.server_api
      pw_rpc.raw.client_api
      pw_rpc.server
      ${arg_DEPS}
  )
  add_dependencies("${NAME}.raw_rpc" "${NAME}._generate.raw_rpc")
endfunction(_pw_raw_rpc_library)

# Internal function that creates a nanopb proto library.
function(_pw_nanopb_library NAME)
  pw_parse_arguments(
    NUM_POSITIONAL_ARGS
      1
    ONE_VALUE_ARGS
      INCLUDE_FILE
      OUT_DIR
    MULTI_VALUE_ARGS
      SOURCES
      INPUTS
      DEPS
  )

  list(TRANSFORM arg_DEPS APPEND .nanopb)

  if("${dir_pw_third_party_nanopb}" STREQUAL "")
    add_custom_target("${NAME}._generate.nanopb")  # Nothing to do
    pw_add_error_target("${NAME}.nanopb"
      MESSAGE
        "Attempting to use pw_proto_library, but dir_pw_third_party_nanopb is "
        "not set. Set dir_pw_third_party_nanopb to the path to the Nanopb "
        "repository."
    )
  else()
    # When compiling with the Nanopb plugin, the nanopb.proto file is already
    # compiled internally, so skip recompiling it with protoc.
    if("${arg_SOURCES}" MATCHES "nanopb\\.proto")
      add_custom_target("${NAME}._generate.nanopb")  # Nothing to do
      pw_add_library_generic("${NAME}.nanopb" INTERFACE
        PUBLIC_DEPS
          pw_build
          pw_third_party.nanopb
          ${arg_DEPS}
      )
    else()
      _pw_generate_protos("${NAME}" nanopb
        PLUGIN
          "${dir_pw_third_party_nanopb}/generator/protoc-gen-nanopb"
        OUTPUT_EXTS
          ".pb.h"
          ".pb.c"
        INCLUDE_FILE
          "${arg_INCLUDE_FILE}"
        OUT_DIR
          "${arg_OUT_DIR}"
        SOURCES
          ${arg_SOURCES}
        INPUTS
          ${arg_INPUTS}
        DEPENDS
          ${arg_DEPS}
      )

      # Create the library with the generated source files.
      pw_add_library_generic("${NAME}.nanopb" STATIC
        SOURCES
          ${generated_outputs}
        PUBLIC_INCLUDES
          "${arg_OUT_DIR}/nanopb"
        PUBLIC_DEPS
          pw_build
          pw_third_party.nanopb
          ${arg_DEPS}
      )
    endif()

    add_dependencies("${NAME}.nanopb" "${NAME}._generate.nanopb")

    # Ensure that nanopb_pb2.py is generated to avoid race conditions.
    add_dependencies("${NAME}._generate.nanopb"
        pw_third_party.nanopb.generate_proto
    )
  endif()
endfunction(_pw_nanopb_library)

# Internal function that creates a nanopb_rpc library.
function(_pw_nanopb_rpc_library NAME)
  pw_parse_arguments(
    NUM_POSITIONAL_ARGS
      1
    ONE_VALUE_ARGS
      INCLUDE_FILE
      OUT_DIR
    MULTI_VALUE_ARGS
      SOURCES
      INPUTS
      DEPS
  )

  # Determine the names of the output files.
  list(TRANSFORM arg_DEPS APPEND .nanopb_rpc)

  _pw_generate_protos("${NAME}" nanopb_rpc
    PLUGIN
      "$ENV{PW_ROOT}/pw_rpc/py/pw_rpc/plugin_nanopb.py"
    OUTPUT_EXTS
      ".rpc.pb.h"
    INCLUDE_FILE
      "${arg_INCLUDE_FILE}"
    OUT_DIR
      "${arg_OUT_DIR}"
    SOURCES
      ${arg_SOURCES}
    INPUTS
      ${arg_INPUTS}
    DEPENDS
      ${arg_DEPS}
      "$ENV{PW_ROOT}/pw_rpc/py/pw_rpc/plugin.py"
  )

  # Create the library with the generated source files.
  pw_add_library_generic("${NAME}.nanopb_rpc" INTERFACE
    PUBLIC_INCLUDES
      "${arg_OUT_DIR}/nanopb_rpc"
    PUBLIC_DEPS
      "${NAME}.nanopb"
      pw_build
      pw_rpc.nanopb.client_api
      pw_rpc.nanopb.server_api
      pw_rpc.server
      ${arg_DEPS}
  )
  add_dependencies("${NAME}.nanopb_rpc" "${NAME}._generate.nanopb_rpc")
endfunction(_pw_nanopb_rpc_library)
