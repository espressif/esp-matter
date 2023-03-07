project ("pkcs11 system test")
cmake_minimum_required (VERSION 3.2.0)

# Include PKCS #11 library's source and header path variables.
include("${CMAKE_SOURCE_DIR}/../pkcsFilePaths.cmake")

# ====================  Define your project name (edit) ========================
set(project_name "pkcs11_system")

# ================= Create the library under test here (edit) ==================

# list the files you would like to test here
set(real_source_files "${PKCS_SOURCES}")
set(corePKCS11_location "${CMAKE_SOURCE_DIR}/../")
set(CORE_PKCS11_3RDPARTY_LOCATION "${corePKCS11_location}/source/dependency/3rdparty")
set(test_config "test-config")
set(LOGGING_INCLUDE_DIRS ${corePKCS11_location}/test/shared-config)
file(GLOB MBEDTLS_FILES CONFIGURE_DEPENDS "${CORE_PKCS11_3RDPARTY_LOCATION}/mbedtls/library/*.c")
file(GLOB MBEDTLS_FILES_INCLUDES CONFIGURE_DEPENDS "${CORE_PKCS11_3RDPARTY_LOCATION}/mbedtls/include/mbedtls/*.h")
file(GLOB test_config_sources CONFIGURE_DEPENDS "${test_config}/*.h")
list(APPEND real_source_files
            ${MBEDTLS_FILES}
            ${test_config_sources}
            "${corePKCS11_location}/source/portable/posix/core_pkcs11_pal.c"
            "${CORE_PKCS11_3RDPARTY_LOCATION}/mbedtls_utils/mbedtls_utils.c"
            "${CORE_PKCS11_3RDPARTY_LOCATION}/mbedtls_utils/mbedtls_error.c"
            )

# list the directories the module under test includes
list(APPEND real_include_directories
            .
            ${test_config}
            ${PKCS_INCLUDE_PUBLIC_DIRS}
            ${LOGGING_INCLUDE_DIRS}
            "${CORE_PKCS11_3RDPARTY_LOCATION}/pkcs11"
            "${CORE_PKCS11_3RDPARTY_LOCATION}/mbedtls/include"
            "${CORE_PKCS11_3RDPARTY_LOCATION}/mbedtls_utils"
        )

# =====================  Create UnitTest Code here (edit)  =====================

# list the directories your test needs to include
list(APPEND test_include_directories
            .
            ${PKCS_INCLUDE_PUBLIC_DIRS}
            ${LOGGING_INCLUDE_DIRS}
            "${CORE_PKCS11_3RDPARTY_LOCATION}/pkcs11"
            "${CORE_PKCS11_3RDPARTY_LOCATION}/mbedtls/include"
        )

# =============================  (end edit)  ===================================
set(real_name "${project_name}_real")

create_real_library(${real_name}
                    "${real_source_files}"
                    "${real_include_directories}"
                    # Empty mock name as create_real_library needs the 4th argument.
                    ""
        )
target_compile_definitions(
    ${real_name}
    PUBLIC
        -DMBEDTLS_CONFIG_FILE="mbedtls_config.h"
)

list(APPEND stest_link_list
            lib${real_name}.a
        )

list(APPEND stest_dep_list
            ${real_name}
        )

set(stest_name "${project_name}_test")
set(stest_source "system-tests/${project_name}_test.c")
create_test(${stest_name}
            ${stest_source}
            "${stest_link_list}"
            "${stest_dep_list}"
            "${test_include_directories}"
            "${MODULE_ROOT_DIR}/tools/cmock/pkcs_wrapper/project.yml" # unused param
            "${mock_dir}" # unused param
        )

set_source_files_properties(
    "${corePKCS11_location}/source/portable/mbedtls/core_pkcs11_mbedtls.c"
    PROPERTIES COMPILE_FLAGS
    "-Wno-pedantic -Wno-error"
)

set_source_files_properties(
    "${CORE_PKCS11_3RDPARTY_LOCATION}/mbedtls_utils/mbedtls_utils.c"
    PROPERTIES COMPILE_FLAGS
    "-Wno-pedantic -Wno-error"
)

set_source_files_properties(
    "${CORE_PKCS11_3RDPARTY_LOCATION}/mbedtls_utils/mbedtls_error.c"
    PROPERTIES COMPILE_FLAGS
    "-Wno-pedantic -Wno-error"
)

set_source_files_properties(
    "${CMAKE_SOURCE_DIR}/integration-test/pkcs11/system-tests/pkcs11_system_test.c"
    PROPERTIES COMPILE_FLAGS
    "-Wno-error=clobbered"
)

set_source_files_properties(
    ${MBEDTLS_FILES}
    PROPERTIES COMPILE_FLAGS
    "-Wno-pedantic -Wno-error"
)

set_source_files_properties(
    ${MBEDTLS_FILES_INCLUDES}
    PROPERTIES COMPILE_FLAGS
    "-Wno-pedantic"
)
