# MbedTLS Library

The folder provides build scripts and configuration recipes for building
the MbedTLS library. The source code needs to be downloaded by the user,
preferably using Git submodules. For gn build,
set `dir_pw_third_party_mbedtls` to point to the path of the source code.
For applications using MbedTLS, add `$dir_pw_third_party/mbedtls` to the
dependency list. The config header can be set using gn variable
`pw_third_party_mbedtls_CONFIG_HEADER`.
