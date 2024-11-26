def setup_directories() {
    sh '''
    mkdir -p ${PACKAGE_PATH}/Firmware/Evaluation
    mkdir -p ${PACKAGE_PATH}/Script
    mkdir -p ${PACKAGE_PATH}/Tools
    '''
}

def setup_environment() {
    sh '''
    cd ${IDF_PATH}
    . ./export.sh

    cd ${ESP_MATTER_PATH}
    git config --global --add safe.directory ${REPOS_PATH}/esp-matter
    git fetch origin main --depth 1
    git checkout FETCH_HEAD
    git clean -fd
    git submodule update --init --depth 1

    cd ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip
    rm -rf .environment/
    ./scripts/checkout_submodules.py --platform esp32 linux --shallow

    cd ${ESP_MATTER_PATH}
    ./install.sh
    . ./export.sh

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
    
    printf "\n\n" >> ${REPOS_PATH}/build_details.txt
    '''
}

def firmware_build() {
    sh '''
    printf "\n\n" >> ${REPOS_PATH}/build_details.txt
    echo "firmware_type: ${FIRMWARE_TYPE}" >> ${REPOS_PATH}/build_details.txt
 
    cd ${IDF_PATH}
    . ./export.sh

    cd ${ESP_MATTER_PATH}
    . ./export.sh

    cd ${ESP_MATTER_PATH}/examples/${product}
    SDKCONFIG_FILE=${ESP_MATTER_PATH}/examples/${product}/sdkconfig.defaults

    rm -rf build sdkconfig sdkconfig.old managed_components dependencies.lock

    echo "CONFIG_ENABLE_ESP32_FACTORY_DATA_PROVIDER=y" >> sdkconfig.defaults
    echo "CONFIG_ENABLE_ESP32_DEVICE_INSTANCE_INFO_PROVIDER=y" >> sdkconfig.defaults
    echo "CONFIG_SEC_CERT_DAC_PROVIDER=y" >> sdkconfig.defaults
    echo "CONFIG_ESP_SECURE_CERT_DS_PERIPHERAL=n" >> sdkconfig.defaults
    echo "CONFIG_ENABLE_OTA_REQUESTOR=y" >> sdkconfig.defaults
    echo "CONFIG_ESP_COREDUMP_ENABLE_TO_UART=y" >> sdkconfig.defaults
    echo "CONFIG_FACTORY_DEVICE_INSTANCE_INFO_PROVIDER=y" >> sdkconfig.defaults

    idf.py set-target ${chip}
    if [ "${FIRMWARE_TYPE}" = "OTA" ]; then
       MIN_PROJECT_VER=10
       MAX_PROJECT_VER=100
       OTA_PROJECT_VER=$((RANDOM % (MAX_PROJECT_VER - MIN_PROJECT_VER + 1) + MIN_PROJECT_VER))
       OTA_PROJECT_VER_STRING="${OTA_PROJECT_VER}.0"
       echo "OTA Project Version Number: ${OTA_PROJECT_VER}" >> ${REPOS_PATH}/build_details.txt
       echo "OTA Project Version String: ${OTA_PROJECT_VER_STRING}" >> ${REPOS_PATH}/build_details.txt
       idf.py -DCLI_PROJECT_VER=${OTA_PROJECT_VER_STRING} -DCLI_PROJECT_VER_NUMBER=${OTA_PROJECT_VER} build
    else
       idf.py build
    fi

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
    SCRIPTS_PATH=${PACKAGE_PATH}/Script/manufacturing_scripts
    mkdir -p ${SCRIPTS_PATH}

    mkdir -p ${SCRIPTS_PATH}/esp-matter/connectedhomeip/connectedhomeip/credentials
    cp -r ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/credentials/test ${SCRIPTS_PATH}/esp-matter/connectedhomeip/connectedhomeip/credentials
    cp -r ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/credentials/production ${SCRIPTS_PATH}/esp-matter/connectedhomeip/connectedhomeip/credentials
    cp -r ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/credentials/development ${SCRIPTS_PATH}/esp-matter/connectedhomeip/connectedhomeip/credentials

    '''
}

def tools_artifacts_create() {
    sh '''
    mkdir -p ${PACKAGE_PATH}/Tools/chip-tool
    cp ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/out/host/chip-tool ${PACKAGE_PATH}/Tools/chip-tool

    mkdir -p ${PACKAGE_PATH}/Tools/chip-cert
    cp ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/out/host/chip-cert ${PACKAGE_PATH}/Tools/chip-cert

    mkdir -p ${PACKAGE_PATH}/Tools/chip-ota-provider-app
    cp ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/out/debug/chip-ota-provider-app ${PACKAGE_PATH}/Tools/chip-ota-provider-app

    TOOL_PATH=${PACKAGE_PATH}/Tools/chip-ota-provider-app

    mkdir -p ${TOOL_PATH}/esp-matter/connectedhomeip/connectedhomeip/src/app
    cp -r ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/src/app/ota_image_tool.py ${TOOL_PATH}/esp-matter/connectedhomeip/connectedhomeip/src/app

    mkdir -p ${TOOL_PATH}/esp-matter/connectedhomeip/connectedhomeip/src/controller
    cp -r ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/src/controller/python ${TOOL_PATH}/esp-matter/connectedhomeip/connectedhomeip/src/controller

    mkdir -p ${PACKAGE_PATH}/Tools/certs
    cp -r ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/credentials/test/attestation/Chip-Test-PAA-NoVID-Cert.der ${PACKAGE_PATH}/Tools/certs

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
