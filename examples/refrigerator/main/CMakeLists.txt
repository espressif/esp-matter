set(SRC_LIST       app_driver.cpp
                   app_main.cpp
                   ${MATTER_SDK_PATH}/examples/all-clusters-app/all-clusters-common/src/static-supported-temperature-levels.cpp
                   ${MATTER_SDK_PATH}/examples/all-clusters-app/all-clusters-common/src/tcc-mode.cpp
   )

set(INCLUDE_DIRS_LIST   "${MATTER_SDK_PATH}/examples/all-clusters-app/all-clusters-common/include")

idf_component_register(SRCS              ${SRC_LIST}
                      INCLUDE_DIRS       ${INCLUDE_DIRS_LIST}
                      PRIV_INCLUDE_DIRS  "." "${ESP_MATTER_PATH}/examples/common/utils")

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 17)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
