idf_component_register(SRCS "rfid_nfc_reader.cpp"
                       INCLUDE_DIRS ".")

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
