def setup_directories_firmware() {
    sh '''
    echo "repos path: ${REPOS_PATH}"
    ls -l ${REPOS_PATH} 
    cd ${REPOS_PATH}
    cd ..
    ls -l
    mkdir -p ${PACKAGE_PATH}
    mkdir -p ${PACKAGE_PATH}/Firmware
    mkdir -p ${PACKAGE_PATH}/Firmware/Evaluation
    '''
}

def setup_directories_script() {
    sh '''
    cd ${REPOS_PATH}
    mkdir -p ${PACKAGE_PATH}
    mkdir -p ${PACKAGE_PATH}/Script
    '''
}

def setup_directories_tools() {
    sh '''
    cd ${REPOS_PATH}
    mkdir -p ${PACKAGE_PATH}
    mkdir -p ${PACKAGE_PATH}/Tools
    '''
}

def setup_environment() {
    sh '''
    cd ${REPOS_PATH}/esp-idf
    . ./export.sh
    cd ..

    cd ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip
    scripts/examples/gn_build_example.sh examples/ota-provider-app/linux out/debug chip_config_network_layer_ble=false

    IDF_BRANCH=`cd ${REPOS_PATH}/esp-idf && git rev-parse --abbrev-ref HEAD`
    IDF_COMMIT_ID=`cd ${REPOS_PATH}/esp-idf && git rev-parse --verify HEAD`
    ESP_MATTER_BRANCH=`cd ${REPOS_PATH}/esp-matter && git rev-parse --abbrev-ref HEAD`
    ESP_MATTER_COMMIT_ID=`cd ${REPOS_PATH}/esp-matter && git rev-parse --verify HEAD`
 
    echo "esp-idf: ${IDF_BRANCH}: ${IDF_COMMIT_ID}" >> ${REPOS_PATH}/build_details.txt
    echo "esp-matter: ${ESP_MATTER_BRANCH}: ${ESP_MATTER_COMMIT_ID}" >> ${REPOS_PATH}/build_details.txt    
    printf "\n\n" >> ${REPOS_PATH}/build_details.txt

    echo "product: ${product}" >> ${REPOS_PATH}/build_details.txt
    echo "chip: ${chip}" >> ${REPOS_PATH}/build_details.txt
    echo "flash_size: 4MB" >> ${REPOS_PATH}/build_details.txt
    echo "secure_boot: enabled" >> ${REPOS_PATH}/build_details.txt
    
    printf "\n\n" >> ${REPOS_PATH}/build_details.txt
    '''
}

def firmware_build() {
    sh '''
    printf "\n\n" >> ${REPOS_PATH}/build_details.txt
    echo "firmware_type: ${FIRMWARE_TYPE}" >> ${REPOS_PATH}/build_details.txt
 
    echo "product :${product} "
    cd ${ESP_MATTER_PATH}/examples/${product}
    SDKCONFIG_FILE=${ESP_MATTER_PATH}/examples/${product}/sdkconfig.defaults
   
    . ${IDF_PATH}/export.sh
    . ${ESP_MATTER_PATH}/export.sh;

    idf.py fullclean

    config_option1="CONFIG_ENABLE_ESP32_FACTORY_DATA_PROVIDER=y"
    config_option2="CONFIG_ENABLE_ESP32_DEVICE_INSTANCE_INFO_PROVIDER=y"
    config_option3="CONFIG_SEC_CERT_DAC_PROVIDER=y"
    config_option4="CONFIG_ESP_SECURE_CERT_DS_PERIPHERAL=n"
    config_option5="CONFIG_ENABLE_OTA_REQUESTOR=y"
    config_option6="CONFIG_ESP_COREDUMP_ENABLE_TO_UART=y"
    config_option7="CONFIG_FACTORY_DEVICE_INSTANCE_INFO_PROVIDER=y"

    if [ -e "sdkconfig.defaults.${chip}" ]; then
        if ! grep -q "^${config_option1%%=*}=" "sdkconfig.defaults.${chip}"; then
            echo "${config_option1}" >> "sdkconfig.defaults.${chip}"
        fi

        if ! grep -q "^${config_option2%%=*}=" "sdkconfig.defaults.${chip}"; then
            echo "${config_option2}" >> "sdkconfig.defaults.${chip}"
        fi

        if ! grep -q "^${config_option3%%=*}=" "sdkconfig.defaults.${chip}"; then
            echo "${config_option3}" >> "sdkconfig.defaults.${chip}"
        fi

        if [ "${chip}" != "esp32h2" ] && ! grep -q "^${config_option4%%=*}=" "sdkconfig.defaults.${chip}"; then
            echo "${config_option4}" >> "sdkconfig.defaults.${chip}"
        fi

        if ! grep -q "${config_option5}" "sdkconfig.defaults.${chip}"; then
                echo "${config_option5}" >> "sdkconfig.defaults.${chip}"
        fi

        echo "${config_option6}" >> "sdkconfig.defaults.${chip}"

        if ! grep -q "${config_option7}" "sdkconfig.defaults.${chip}"; then
                echo "${config_option7}" >> "sdkconfig.defaults.${chip}"
        fi

    else
        if ! grep -q "^${config_option1%%=*}=" sdkconfig.defaults; then
            echo "${config_option1}" >> "sdkconfig.defaults"
        fi

        if ! grep -q "^${config_option2%%=*}=" sdkconfig.defaults; then
            echo "${config_option2}" >> "sdkconfig.defaults"
        fi

        if ! grep -q "^${config_option3%%=*}=" sdkconfig.defaults; then
            echo "${config_option3}" >> "sdkconfig.defaults"
        fi

        if ! grep -q "^${config_option4%%=*}=" sdkconfig.defaults; then
            echo "${config_option4}" >> "sdkconfig.defaults"
        fi

        if ! grep -q "${config_option5}" "sdkconfig.defaults"; then
            echo "${config_option5}" >> "sdkconfig.defaults"
        fi

        echo "${config_option6}" >> "sdkconfig.defaults"

        if ! grep -q "${config_option7}" "sdkconfig.defaults"; then
            echo "${config_option7}" >> "sdkconfig.defaults"
        fi
    fi

    idf.py set-target ${chip}

    cat sdkconfig
    idf.py build
    '''
}

def firmware_build_save() {
    sh '''
    PACKAGE_FIRMWARE_PATH=${PACKAGE_PATH}/Firmware/${FIRMWARE_TYPE}
    cd ${ESP_MATTER_PATH}/examples/${product}

    mkdir -p ${PACKAGE_FIRMWARE_PATH}/build
    mkdir -p ${PACKAGE_FIRMWARE_PATH}/build/bootloader
    mkdir -p ${PACKAGE_FIRMWARE_PATH}/build/partition_table

    cp build/${product}.bin ${PACKAGE_FIRMWARE_PATH}/build/${product}.bin
    cp build/bootloader/bootloader.bin ${PACKAGE_FIRMWARE_PATH}/build/bootloader/bootloader.bin
    cp build/partition_table/partition-table.bin ${PACKAGE_FIRMWARE_PATH}/build/partition_table/partition-table.bin
    cp build/ota_data_initial.bin ${PACKAGE_FIRMWARE_PATH}/build/ota_data_initial.bin

    cp build/${product}.elf ${PACKAGE_FIRMWARE_PATH}/build/${product}.elf
    cp build/${product}.map ${PACKAGE_FIRMWARE_PATH}/build/${product}.map

    # Helper files
    echo -n "${FILE_PREFIX}" >> ${PACKAGE_FIRMWARE_PATH}/file_prefix_ota_0.txt
    echo -n "${FILE_PREFIX}" >> ${PACKAGE_FIRMWARE_PATH}/file_prefix_bootloader.txt

    # json
    cp build/project_description.json ${PACKAGE_FIRMWARE_PATH}/build/project_description.json
    cp build/flasher_args.json ${PACKAGE_FIRMWARE_PATH}/build/flasher_args.json

    '''
}

def script_artifacts_create() {
    sh '''
    PACKAGE_SCRIPT_PATH=${PACKAGE_PATH}/Script
    SCRIPTS_DIRECTORY_NAME=manufacturing_scripts
    SCRIPTS_PATH=${PACKAGE_SCRIPT_PATH}/${SCRIPTS_DIRECTORY_NAME}
    mkdir -p ${SCRIPTS_PATH}

    # esp-idf
    mkdir -p ${SCRIPTS_PATH}/esp-idf
    mkdir -p ${SCRIPTS_PATH}/esp-idf/components
    mkdir -p ${SCRIPTS_PATH}/esp-idf/components/nvs_flash
    cp -r ${IDF_PATH}/components/nvs_flash/nvs_partition_generator ${SCRIPTS_PATH}/esp-idf/components/nvs_flash/

    mkdir -p ${SCRIPTS_PATH}/esp-idf/tools
    cp -r ${IDF_PATH}/tools/mass_mfg ${SCRIPTS_PATH}/esp-idf/tools/

    # esp-matter
    mkdir -p ${SCRIPTS_PATH}/esp-matter
    mkdir -p ${SCRIPTS_PATH}/esp-matter/tools
    cp -r ${ESP_MATTER_PATH}/tools/mfg_tool ${SCRIPTS_PATH}/esp-matter/tools/

    mkdir -p ${SCRIPTS_PATH}/esp-matter/connectedhomeip
    mkdir -p ${SCRIPTS_PATH}/esp-matter/connectedhomeip/connectedhomeip
    mkdir -p ${SCRIPTS_PATH}/esp-matter/connectedhomeip/connectedhomeip/scripts
    mkdir -p ${SCRIPTS_PATH}/esp-matter/connectedhomeip/connectedhomeip/scripts/tools
    cp -r ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/scripts/tools/spake2p ${SCRIPTS_PATH}/esp-matter/connectedhomeip/connectedhomeip/scripts/tools

    mkdir -p ${SCRIPTS_PATH}/esp-matter/connectedhomeip/connectedhomeip/src
    mkdir -p ${SCRIPTS_PATH}/esp-matter/connectedhomeip/connectedhomeip/src/setup_payload
    cp -r ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/src/setup_payload/python ${SCRIPTS_PATH}/esp-matter/connectedhomeip/connectedhomeip/src/setup_payload

    mkdir -p ${SCRIPTS_PATH}/esp-matter/connectedhomeip/connectedhomeip/credentials
    cp -r ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/credentials/test ${SCRIPTS_PATH}/esp-matter/connectedhomeip/connectedhomeip/credentials
    cp -r ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/credentials/production ${SCRIPTS_PATH}/esp-matter/connectedhomeip/connectedhomeip/credentials
    cp -r ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/credentials/development ${SCRIPTS_PATH}/esp-matter/connectedhomeip/connectedhomeip/credentials

    # script_description
    mkdir -p ${SCRIPTS_PATH}/script_description
    cp ${SCRIPTS_PATH}/esp-matter/tools/mfg_tool/requirements.txt ${SCRIPTS_PATH}/script_description/
    cp ${SCRIPTS_PATH}/esp-matter/tools/mfg_tool/README.md ${SCRIPTS_PATH}/script_description/
    '''
}

def tools_artifacts_create() {
    sh '''
    PACKAGE_TOOLS_PATH=${PACKAGE_PATH}/Tools
    TOOL_DIRECTORY_NAME=chip-tool
    TOOL_PATH=${PACKAGE_TOOLS_PATH}/${TOOL_DIRECTORY_NAME}
    mkdir -p ${TOOL_PATH}

    cp ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/out/host/chip-tool ${TOOL_PATH}/

    TOOL_DIRECTORY_NAME=chip-cert
    TOOL_PATH=${PACKAGE_TOOLS_PATH}/${TOOL_DIRECTORY_NAME}
    mkdir -p ${TOOL_PATH}

    cp ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/out/host/chip-cert ${TOOL_PATH}/

    TOOL_DIRECTORY_NAME=chip-ota-provider-app
    TOOL_PATH=${PACKAGE_TOOLS_PATH}/${TOOL_DIRECTORY_NAME}
    mkdir -p ${TOOL_PATH}

    cp ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/out/debug/chip-ota-provider-app ${TOOL_PATH}/

    # esp-matter
    mkdir -p ${TOOL_PATH}/esp-matter/connectedhomeip/connectedhomeip/src/app
    cp -r ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/src/app/ota_image_tool.py ${TOOL_PATH}/esp-matter/connectedhomeip/connectedhomeip/src/app

    mkdir -p ${TOOL_PATH}/esp-matter/connectedhomeip/connectedhomeip/src/controller
    cp -r ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/src/controller/python ${TOOL_PATH}/esp-matter/connectedhomeip/connectedhomeip/src/controller
    '''
}

def artifacts_save() {
    sh '''
    cd ${REPOS_PATH}
    cp build_details.txt ${PACKAGE_NAME}/

    mkdir ${WORKSPACE}/artifacts
    cp build_details.txt ${WORKSPACE}/artifacts/
    tar -zcvf ${WORKSPACE}/artifacts/${PACKAGE_NAME}.tar.gz ${PACKAGE_NAME}
    '''
}

return this
