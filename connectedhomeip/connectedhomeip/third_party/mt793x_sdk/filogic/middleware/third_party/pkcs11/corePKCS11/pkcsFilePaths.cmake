# This file is to add source files and include directories
# into variables so that it can be reused from different repositories
# in their Cmake based build system by including this file.
#
# Files specific to the repository such as test runner, platform tests
# are not added to the variables.

# PKCS library source files.
set( PKCS_SOURCES
     "${CMAKE_CURRENT_LIST_DIR}/source/core_pkcs11.c"
     "${CMAKE_CURRENT_LIST_DIR}/source/portable/mbedtls/core_pkcs11_mbedtls.c"
     "${CMAKE_CURRENT_LIST_DIR}/source/core_pki_utils.c"
     )

 # PKCS library Public Include directories.
 set( PKCS_INCLUDE_PUBLIC_DIRS
     "${CMAKE_CURRENT_LIST_DIR}/source/dependency/3rdparty/pkcs11"
     "${CMAKE_CURRENT_LIST_DIR}/source/include"
     )
