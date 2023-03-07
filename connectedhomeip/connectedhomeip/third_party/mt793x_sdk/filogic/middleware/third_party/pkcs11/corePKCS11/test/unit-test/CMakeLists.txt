project( "pkcs11_wrapper" )
#Include filepaths for source and include.
include( ${MODULE_ROOT_DIR}/pkcsFilePaths.cmake )

# =============================  PKCS #11 wrapper unit tests  ===========================
set(project_name "core_pkcs11")
set(utest_name "${project_name}_utest")
set(utest_source "${project_name}_utest.c")

# =====================  Create your mock here  (edit)  ========================

# list of files to preprocess
# pkcs11.h is pre-processed to generate it's function prototypes.
list(APPEND preprocessed_mock_list
        "${MODULE_ROOT_DIR}/source/dependency/3rdparty/pkcs11/pkcs11.h"
    )

set(preprocess_commands
        "-I ${MODULE_ROOT_DIR}/source/dependency/3rdparty/pkcs11/ -D'CK_PTR=*' -D'NULL_PTR=0' \
        -D'CK_DEFINE_FUNCTION( returnType, name )=returnType name'\
        -D'CK_DECLARE_FUNCTION( returnType, name )=returnType name'\
        -D'CK_DECLARE_FUNCTION_POINTER( returnType, name )=returnType( CK_PTR name )'\
        -D'CK_CALLBACK_FUNCTION( returnType, name )=returnType( CK_PTR name )'"
    )

# list the files to mock here
list(APPEND mock_list
        "${MODULE_ROOT_DIR}/source/dependency/3rdparty/pkcs11/pkcs11.h"
        "${MODULE_ROOT_DIR}/test/unit-test/config/mock_osal.h"
    )

# list the directories your mocks need
list(APPEND mock_include_list
        config
        "${PKCS_INCLUDE_PUBLIC_DIRS}"
        "${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls/include/mbedtls"
        "${MODULE_ROOT_DIR}/test/unit-test/config"
    )

#list the definitions of your mocks to control what to be included
list(APPEND mock_define_list
    ""
    )

# ================= Create the library under test here (edit) ==================

# list the files you would like to test here
list(APPEND real_source_files
            "${MODULE_ROOT_DIR}/source/core_pkcs11.c"
            "${MODULE_ROOT_DIR}/test/shared-config/logging_stack.h"
            "${MODULE_ROOT_DIR}/test/shared-config/logging_levels.h"
        )

# list the directories the module under test includes
list(APPEND real_include_directories
    "config"
    ${MODULE_ROOT_DIR}/source/include
    ${MODULE_ROOT_DIR}/source/dependency/3rdparty/pkcs11
    )

# =====================  Create UnitTest Code here (edit)  =====================

# list the directories your test needs to include
list(APPEND test_include_directories
            "config"
            "${CMAKE_CURRENT_BINARY_DIR}/pkcs_wrapper/mocks"
            ${PKCS_INCLUDE_PUBLIC_DIRS}
            ${PKCS_INCLUDE_PRIVATE_DIRS}
        )

# =============================  (end edit)  ===================================

set(mock_name "${project_name}_mock")
set(real_name "${project_name}_real")
set(pre_mock_name  "pre_${mock_name}")

set(mock_dir "wrapper_mocks")
create_mock_list("${mock_name}"
                "${mock_list}"
                "${MODULE_ROOT_DIR}/tools/cmock/pkcs_wrapper/project.yml"
                "${mock_include_list}"
                "${mock_define_list}"
                "${mock_dir}"
        )

separate_arguments(unix_flags UNIX_COMMAND "${preprocess_commands}")

preprocess_mock_list(${mock_name}
            ${preprocessed_mock_list}
            "${unix_flags}"
        )

add_dependencies(${mock_name}
                ${pre_mock_name}
    )

create_real_library(${real_name}
                    "${real_source_files}"
                    "${real_include_directories}"
                    "${mock_name}"
        )

list(APPEND utest_link_list
            -l${mock_name}
            lib${real_name}.a
        )

list(APPEND utest_dep_list
            ${real_name}
        )

create_test(${utest_name}
            ${utest_source}
            "${utest_link_list}"
            "${utest_dep_list}"
            "${test_include_directories}"
            "${MODULE_ROOT_DIR}/tools/cmock/pkcs_wrapper/project.yml"
            "${mock_dir}"
        )


# =============================  PKCS #11 mbedtls unit tests  ===========================
project( "pkcs11_mbedtls" )
set(project_name "core_pkcs11_mbedtls")
set(utest_name "${project_name}_utest")
set(utest_source "${project_name}_utest.c")
# =====================  Create your mock here  (edit)  ========================

# clear the original variables
set( mock_list "" )
set( mock_include_list "" )
set( mock_define_list "" )
set( real_source_files "" )
set( real_include_directories "" )
set( test_include_directories "" )
set( utest_link_list "" )
set( utest_dep_list "" )
set( mock_dir "mbedtls_mocks" )

# list the files to mock here
list(APPEND mock_list
            "${MODULE_ROOT_DIR}/test/unit-test/config/mock_osal.h"
            "${MODULE_ROOT_DIR}/source/include/core_pkcs11_pal.h"
            "${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls/include/mbedtls/ctr_drbg.h"
            "${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls/include/mbedtls/sha256.h"
            "${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls/include/mbedtls/base64.h"
            "${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls/include/mbedtls/bignum.h"
            "${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls/include/mbedtls/entropy.h"
            "${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls/include/mbedtls/md.h"
            "${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls/include/mbedtls/rsa.h"
            "${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls/include/mbedtls/ecp.h"
            "${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls/include/mbedtls/pk_internal.h"
            "${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls/include/mbedtls/ecdsa.h"
            "${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls_utils/mbedtls_error.h"
            "${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls/include/mbedtls/pk.h"
            "${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls/include/mbedtls/x509_crt.h"
            "${MODULE_ROOT_DIR}/source/include/core_pki_utils.h"
    )

# list the directories your mocks need
list(APPEND mock_include_list
            "${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls/include/mbedtls"
            "${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls_utils"
            "${MODULE_ROOT_DIR}/test/unit-test/config"
            "${MODULE_ROOT_DIR}/source/include/"
    )

#list the definitions of your mocks to control what to be included
list(APPEND mock_define_list
    ""
    )

# ================= Create the library under test here (edit) ==================

# list the files you would like to test here
list(APPEND real_source_files
            ${MODULE_ROOT_DIR}/source/portable/mbedtls/core_pkcs11_mbedtls.c
            ${MODULE_ROOT_DIR}/test/unit-test/config/core_pkcs11_config.h
    )

# list the directories the module under test includes
list(APPEND real_include_directories
            "${MODULE_ROOT_DIR}/source/dependency/3rdparty/pkcs11"
            "${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls/include"
            "${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls_utils"
            "${MODULE_ROOT_DIR}/test/unit-test/config"
            "${MODULE_ROOT_DIR}/source/include/"
            "${CMAKE_CURRENT_BINARY_DIR}/${mock_dir}"
    )
# =====================  Create UnitTest Code here (edit)  =====================

# list the directories your test needs to include
list(APPEND test_include_directories
            "${MODULE_ROOT_DIR}/source/dependency/3rdparty/pkcs11"
            "${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls/include/mbedtls"
            "${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls_utils"
            "${MODULE_ROOT_DIR}/test/unit-test/config"
            "${MODULE_ROOT_DIR}/source/include/"
            "${CMAKE_CURRENT_BINARY_DIR}/${mock_dir}"
    )
# =============================  (end edit)  ===================================

set(mock_name "${project_name}_mock")
set(real_name "${project_name}_real")

create_mock_list("${mock_name}"
            "${mock_list}"
            "${MODULE_ROOT_DIR}/tools/cmock/pkcs_mbedtls/project.yml"
            "${mock_include_list}"
            "${mock_define_list}"
            "${mock_dir}"
    )

# PKCS #11 header files must always come before the mbedTLS header files
target_include_directories(${mock_name} BEFORE PUBLIC
            "${MODULE_ROOT_DIR}/source/dependency/3rdparty/pkcs11"
    )

create_real_library(${real_name}
            "${real_source_files}"
            "${real_include_directories}"
            "${mock_name}"
    )

list(APPEND utest_link_list
            -l${mock_name}
            lib${real_name}.a
    )

list(APPEND utest_dep_list
            ${real_name}
    )

create_test(${utest_name}
            "${utest_source}"
            "${utest_link_list}"
            "${utest_dep_list}"
            "${test_include_directories}"
            "${MODULE_ROOT_DIR}/tools/cmock/pkcs_mbedtls/project.yml"
            "${mock_dir}"
    )

# =============================  PKCS #11 utils unit tests  ===========================
project( "pkcs11_utils" )
set(project_name "core_pki_utils")
set(utest_name "${project_name}_utest")
set(utest_source "${project_name}_utest.c")
# =====================  Create your mock here  (edit)  ========================

# clear the original variables
set( mock_list "" )
set( mock_include_list "" )
set( mock_define_list "" )
set( real_source_files "" )
set( real_include_directories "" )
set( test_include_directories "" )
set( utest_link_list "" )
set( utest_dep_list "" )
set(mock_dir "wrapper_mocks")

# list the files to mock here
# Can't have an empty mock list.
list(APPEND mock_list
    "${MODULE_ROOT_DIR}/source/dependency/3rdparty/mbedtls_utils/mbedtls_error.h"
)

# list the directories your mocks need
list(APPEND mock_include_list
         config
    )

#list the definitions of your mocks to control what to be included
list(APPEND mock_define_list
    ""
    )

# ================= Create the library under test here (edit) ==================

# list the files you would like to test here
list(APPEND real_source_files
            ${MODULE_ROOT_DIR}/source/core_pki_utils.c
    )

# list the directories the module under test includes
list(APPEND real_include_directories
            config
            "${MODULE_ROOT_DIR}/source/include/"
            "${CMAKE_CURRENT_BINARY_DIR}/${mock_dir}"
    )
# =====================  Create UnitTest Code here (edit)  =====================

# list the directories your test needs to include
list(APPEND test_include_directories
            config
            "${MODULE_ROOT_DIR}/source/include/"
            "${CMAKE_CURRENT_BINARY_DIR}/${mock_dir}"
    )
# =============================  (end edit)  ===================================

set(mock_name "${project_name}_mock")
set(real_name "${project_name}_real")

create_mock_list("${mock_name}"
            "${mock_list}"
            "${MODULE_ROOT_DIR}/tools/cmock/pkcs_wrapper/project.yml"
            "${mock_include_list}"
            "${mock_define_list}"
            "${mock_dir}"
    )

create_real_library(${real_name}
            "${real_source_files}"
            "${real_include_directories}"
            "${mock_name}"
    )

list(APPEND utest_link_list
            -l${mock_name}
            lib${real_name}.a
    )

list(APPEND utest_dep_list
            ${real_name}
    )

create_test(${utest_name}
            "${utest_source}"
            "${utest_link_list}"
            "${utest_dep_list}"
            "${test_include_directories}"
            "${MODULE_ROOT_DIR}/tools/cmock/pkcs_wrapper/project.yml"
            "${mock_dir}"
    )
