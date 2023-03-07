cmake_minimum_required ( VERSION 3.13.0 )
project ( "corePKCS11 unit test"
          VERSION 3.0.1
          LANGUAGES C )

# Allow the project to be organized into folders.
set_property( GLOBAL PROPERTY USE_FOLDERS ON )

# Use C90.
set( CMAKE_C_STANDARD 90 )
set( CMAKE_C_STANDARD_REQUIRED ON )

# Do not allow in-source build.
if( ${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR} )
    message( FATAL_ERROR "In-source build is not allowed. Please build in a separate directory, such as ${PROJECT_SOURCE_DIR}/build." )
endif()

# Set global path variables.
get_filename_component(__MODULE_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
set(MODULE_ROOT_DIR ${__MODULE_ROOT_DIR} CACHE INTERNAL "corePKCS11 repository root.")

# Configure options to always show in CMake GUI.
option( BUILD_CLONE_SUBMODULES
        "Set this to ON to automatically clone any required Git submodules. When OFF, submodules must be manually cloned."
        OFF )

# Set output directories.
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )

# ===================================== Coverity Analysis Configuration =================================================

# Include filepaths for source and include.
include( ${MODULE_ROOT_DIR}/pkcsFilePaths.cmake )

# Target for Coverity analysis that builds the library.
add_library( coverity_analysis
             ${PKCS_SOURCES} )

# Build corePKCS11 library target without custom config dependency.
target_compile_definitions( coverity_analysis PUBLIC -DMBEDTLS_CONFIG_FILE="aws_mbedtls_config.h" )

# corePKCS11 public include path.
target_include_directories( coverity_analysis PUBLIC ${PKCS_INCLUDE_PUBLIC_DIRS} ${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls/include ${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls_utils ${MODULE_ROOT_DIR}/test/unit-test/config )

#  ====================================  Test Configuration ========================================

# Define a CMock resource path.
set( CMOCK_DIR ${MODULE_ROOT_DIR}/test/unit-test/CMock CACHE INTERNAL "CMock library source directory." )

# Include CMock build configuration.
include( unit-test/cmock_build.cmake )

# Check if the CMock source directory exists, and if not present, clone the submodule
# if BUILD_CLONE_SUBMODULES configuration is enabled.
if( NOT EXISTS ${CMOCK_DIR}/src )
    # Attempt to clone CMock.
    if( ${BUILD_CLONE_SUBMODULES} )
        clone_cmock()
    else()
        message( FATAL_ERROR "The required submodule CMock does not exist. Either clone it manually, or set BUILD_CLONE_SUBMODULES to 1 to automatically clone it during build." )
    endif()
endif()

# Add unit test and coverage configuration.

# Use CTest utility for managing test runs. This has to be added BEFORE
# defining test targets with add_test()
enable_testing()

# We do this to swap out the default mbed TLS config for one geared towards the
# unit tests.
add_compile_definitions( MBEDTLS_CONFIG_FILE="aws_mbedtls_config.h" )


# Add build targets for CMock and Unit, required for unit testing.
add_cmock_targets()

# Add function to enable CMock based tests and coverage.
include( ${MODULE_ROOT_DIR}/tools/cmock/create_test.cmake )

# Include build configuration for unit tests.
if(UNIT_TESTS)
add_subdirectory( unit-test )
endif()

# Include build configuration for system tests.
if(SYSTEM_TESTS)
add_subdirectory( system-test )
endif()


#  ==================================== Coverage Analysis configuration ========================================


# Add a target for running coverage on tests.
add_custom_target( coverage
    COMMAND ${CMAKE_COMMAND} -P ${MODULE_ROOT_DIR}/tools/cmock/coverage.cmake
    DEPENDS cmock unity core_pkcs11_mbedtls_utest core_pkcs11_utest core_pki_utils_utest
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
