set(SRC_DIRS_LIST       "."
                        "${MATTER_SDK_PATH}/examples/all-clusters-app/all-clusters-common/src"
   )

set(INCLUDE_DIRS_LIST   "${MATTER_SDK_PATH}/examples/all-clusters-app/all-clusters-common/include" )

set(exclude_srcs_list   "${MATTER_SDK_PATH}/examples/all-clusters-app/all-clusters-common/src/binding-handler.cpp"
                        "${MATTER_SDK_PATH}/examples/all-clusters-app/all-clusters-common/src/bridged-actions-stub.cpp"
                        "${MATTER_SDK_PATH}/examples/all-clusters-app/all-clusters-common/src/fan-stub.cpp"
                        "${MATTER_SDK_PATH}/examples/all-clusters-app/all-clusters-common/src/operational-state-delegate-impl.cpp"
                        "${MATTER_SDK_PATH}/examples/all-clusters-app/all-clusters-common/src/operational-state-delegates.cpp"
                        "${MATTER_SDK_PATH}/examples/all-clusters-app/all-clusters-common/src/resource-monitoring-instances.cpp"
                        "${MATTER_SDK_PATH}/examples/all-clusters-app/all-clusters-common/src/smco-stub.cpp"
                        "${MATTER_SDK_PATH}/examples/all-clusters-app/all-clusters-common/src/static-supported-modes-manager.cpp"
   )

idf_component_register(SRC_DIRS          ${SRC_DIRS_LIST}
                      EXCLUDE_SRCS       ${exclude_srcs_list}
                      INCLUDE_DIRS    ${INCLUDE_DIRS_LIST}
                      PRIV_INCLUDE_DIRS  ".")

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 17)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
