add_test_subdirectory (mocks)

IF(NOT CMAKE_BUILD_TYPE MATCHES Test)
  add_library(zaf_config STATIC
    ${ZAF_CONFIGDIR}/zaf_config_api.c
  )
  
  target_include_directories(zaf_config
    PUBLIC
      ${ZAF_CONFIGDIR}
      ${ZPAL_API_DIR}
      ${CONFIG_DIR}
  )

  target_compile_options(zaf_config PRIVATE -Wpedantic -Wconversion -Wextra)
ENDIF(NOT CMAKE_BUILD_TYPE MATCHES Test)
