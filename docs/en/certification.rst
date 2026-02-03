Matter Certification
====================

The Matter Certification denotes compliance to a Connectivity Standards Alliance (CSA) specification for the product and allows the use of Certified Product logos and listing of the product on the Alliance website for verification.

You need to `become a member <https://csa-iot.org/become-member/>`__ of CSA and request a Vendor ID code from CSA Certification before you apply for a Matter Certification. Then you need to choose an `authorized test provider <https://csa-iot.org/certification/testing-providers/>`__ (must be validated for Matter testing) and submit your product for testing. Below are the steps for the Matter Certification.

1 Firmware Development
----------------------

1.1 Choose an esp-matter branch
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Use the branch corresponding to the Matter version you plan to certify for firmware development.

.. list-table:: esp-matter Branches Mapping
   :widths: 30 70
   :header-rows: 1

   * - Matter Version
     - esp-matter Branch
   * - Matter 1.1
     - `release/v1.1 <https://github.com/espressif/esp-matter/tree/release/v1.1>`__
   * - Matter 1.2
     - `release/v1.2 <https://github.com/espressif/esp-matter/tree/release/v1.2>`__
   * - Matter 1.3
     - `release/v1.3 <https://github.com/espressif/esp-matter/tree/release/v1.3>`__
   * - Matter 1.4
     - `release/v1.4 <https://github.com/espressif/esp-matter/tree/release/v1.4>`__
   * - Matter 1.4.2
     - `release/v1.4.2 <https://github.com/espressif/esp-matter/tree/release/v1.4.2>`__
   * - Matter 1.5
     - `release/v1.5 <https://github.com/espressif/esp-matter/tree/release/v1.5>`__

1.2 Product Attestation Authority (PAA) Generation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For Matter certification testing, vendors should generate the test Product Attestation Authority (PAA) certificate, Product Attestation Intermediate (PAI), and Device Attestation Certificate (DAC), rather than using the default test PAA certificate from the connectedhomeip SDK repository. Therefore, you need to generate a PAA certificate and use it to sign the PAI certificate, which is then used to sign the DAC. The PAI certificate, DAC certificate, and DAC private key should be stored in the product under test.
The `chip-cert <https://github.com/project-chip/connectedhomeip/blob/master/src/tools/chip-cert/README.md>`__ tool which is built in `esp-matter setup <https://github.com/espressif/esp-matter/blob/main/docs/en/developing.rst#2-esp-matter-setup>`__ process can be used to generate the PAA certificate and Certification Declaration (CD).

::

    cd $ESP_MATTER_PATH/connectedhomeip/connectedhomeip/out/host/
    ./chip-cert gen-att-cert --type a --subject-cn "Example PAA CN" --subject-vid 0x131B \
                                --valid-from "2021-06-28 14:23:43" --lifetime 4294967295 \
                                --out-key /path/to/PAA_key \
                                --out /path/to/PAA_certificate

1.3 Certification Declaration (CD) Generation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Certification Declaration (CD) is a cryptographic document that proves a Matter device complies with protocol standards. For Matter certification testing, users should issue a test CD based on the test CD signing keys in the connectedhomeip SDK repository. This test CD should match the Vendor ID and Product ID in the test DAC, as well as the information in the BasicInformation cluster. During certification testing, the CD's --certification-type should be set to 1 (provisional), while the official CD issued by CSA after passing certification will have --certification-type set to 2 (official).

::

    cd $ESP_MATTER_PATH/connectedhomeip/connectedhomeip/out/host/
    ./chip-cert gen-cd --format-version 1 --vendor-id 0x131B --product-id 0x1234 \
                                --device-type-id 0x010c --certificate-id CSA00000SWC00000-01 \
                                --security-level 0 --security-info 0 --version-number 1 \
                                --certification-type 1 \
                                --key $ESP_MATTER_PATH/connectedhomeip/connectedhomeip/credentials/test/certification-declaration/Chip-Test-CD-Signing-Key.pem \
                                --cert $ESP_MATTER_PATH/connectedhomeip/connectedhomeip/credentials/test/certification-declaration/Chip-Test-CD-Signing-Cert.pem \
                                --out path/to/test_CD_file

1.4 Factory Bin File Generation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Based on the test PAA and CD generated above, users can use `esp-matter-mfg-tool <https://github.com/espressif/esp-matter-tools?tab=readme-ov-file>`__ to generate the corresponding factory partition bin file. This file contains commissioning information (discriminator, salt, iteration count, and SPAKE2+ verifier), device attestation information (Certification Declaration (CD), Product Attestation Intermediate (PAI) certificate, Device Attestation Certificate (DAC), and DAC private key), device instance information (vendor ID, vendor name, product ID, product name, etc.), and device information (fixed label, supported locales, etc.). This information is used to identify the product and ensure secure commissioning.

In the reference command below, the -n option specifies the number of factory bin files to generate. esp-matter-mfg-tool will first generate the PAI certificate and key, then use them to generate the specified number of different DAC certificates and corresponding keys. The tool will use the generated certificates and keys to create multiple factory partition binary files with different DACs, discriminators, and setup pincodes.

::

    python3 -m pip install esp-matter-mfg-tool
    esp-matter-mfg-tool -n <count> -cn Espressif --paa -c /path/to/PAA_certificate -k /path/to/PAA_key \
                        -cd /path/to/CD_file -v 0x131B --vendor-name Espressif -p 0x1234 \
                        --product-name Test-light --hw-ver 1 --hw-ver-str v1.0

1.5 Firmware Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~

When generating test product firmware, the following configurations can be set in menuconfig to enable accessing the information from the factory bin file generated above:

::

    CONFIG_ENABLE_ESP32_FACTORY_DATA_PROVIDER=y
    CONFIG_ENABLE_ESP32_DEVICE_INSTANCE_INFO_PROVIDER=y
    CONFIG_ENABLE_ESP32_DEVICE_INFO_PROVIDER=y
    CONFIG_FACTORY_PARTITION_DAC_PROVIDER=y
    CONFIG_FACTORY_COMMISSIONABLE_DATA_PROVIDER=y
    CONFIG_FACTORY_DEVICE_INSTANCE_INFO_PROVIDER=y

1.6 OTA File Generation
~~~~~~~~~~~~~~~~~~~~~~~

If the test device supports standard Matter OTA, you need to prepare OTA files for certification testing. (Note: You can add OTA `rollback <https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-reference/system/ota.html#app-rollback>`__ functionality to facilitate certification testing)

In esp-matter projects, change the firmware version number by modifying the PROJECT_VER and PROJECT_VER_NUMBER values in your application's CMakeLists.txt file. When generating OTA files, you need to use firmware with a higher version number than the one currently running on the device.

Use `ota_image_tool.py <https://github.com/project-chip/connectedhomeip/blob/master/src/app/ota_image_tool.py>`__ to generate Matter OTA files, where the ``-vn`` parameter is the software version number, and the ``-vs`` parameter is the software version string:

::

    cd $ESP_MATTER_PATH/connectedhomeip/connectedhomeip/src/app
    ./ota_image_tool.py create -v <vendor-id> -p <product-id> -vn 2 -vs 1.1 -da sha256 \
                            /path/to/original_app_bin /path/to/out_ota_bin

1.7 Other Notes
~~~~~~~~~~~~~~~

For products that support the Identify cluster, you need to set the identify type according to the specific product form:

.. list-table:: Presentation Enumeration Values
   :widths: 15 20 55 10
   :header-rows: 1

   * - Value
     - Name
     - Summary
     - Conformance
   * - 0x00
     - None
     - No presentation.
     - M
   * - 0x01
     - LightOutput
     - Light output of a lighting product.
     - M
   * - 0x02
     - VisibleIndicator
     - Typically a small LED.
     - M
   * - 0x03
     - AudibleBeep
     - Audible beep sound.
     - M
   * - 0x04
     - Display
     - Presentation will be visible on display screen.
     - M
   * - 0x05
     - Actuator
     - Presentation via actuator (e.g., window blind operation or relay).
     - M

Implement the corresponding identify command and trigger effect command handlers:

::

    esp_err_t app_identification_cb(identification::callback_type_t type, uint16_t endpoint_id, uint8_t effect_id, uint8_t effect_variant, void *priv_data)
    {
        esp_err_t err = ESP_OK;
        ESP_LOGI(TAG, "Identification type: %d, effect_id: %d", type, effect_id);
        switch (type) {
            case esp_matter::identification::START:
                break;
            case esp_matter::identification::STOP:
                break;
            case esp_matter::identification::EFFECT:
                switch (effect_id) {
                    case (int)Identify::EffectIdentifierEnum::kBlink:
                        break;
                    case (int)Identify::EffectIdentifierEnum::kBreathe:
                        break;
                    case (int)Identify::EffectIdentifierEnum::kOkay:
                        break;
                    case (int)Identify::EffectIdentifierEnum::kChannelChange:
                        break;
                    case (int)Identify::EffectIdentifierEnum::kFinishEffect:
                        break;
                    case (int)Identify::EffectIdentifierEnum::kStopEffect:
                        break;
                    default:
                        break;
                }
                break;
            default:
                err = ESP_FAIL;
                break;
        }
        return err;
    }

2 Preparation of Files Required for Certification Testing
-----------------------------------------------------------

After preparing the device under test, users need to prepare the following files to submit to the testing organization along with the device.

2.1 PICS File
~~~~~~~~~~~~~~

The PICS file is used to define all Matter features of the product, including endpoints, device types, clusters, attributes, commands, etc. The testing organization will determine the test cases to execute during certification testing based on the PICS file submitted by the user.

The `PICS Tool <https://picstool.csa-iot.org/>`__ website is a tool for opening, modifying, validating, and saving XML format PICS files. `Pics_xml_template_files <https://groups.csa-iot.org/wg/matter-csg/document/40537>`__ contains all PICS file templates, with each XML file defining the feature characteristics of one or more clusters.

The `PICS-generator tool <https://github.com/project-chip/connectedhomeip/tree/master/src/tools/PICS-generator>`__ can generate PICS files based on Pics_xml_template_files. This tool reads the clusters, attributes, commands, features, and events supported by the device under test and outputs a PICS file. Note: This tool cannot automatically generate the Base.xml file, which needs to be opened with PICS TOOL and manually configured. Reference commands are as follows:

::

    # WiFi Device:
    python3 PICSGenerator.py --pics-template path/to/XML_template --pics-output out_folder --commissioning-method ble-wifi \
                            --wifi-ssid ssid-name --wifi-passphrase password --passcode setup-pin-code --discriminator discriminator \
                            --paa-trust-store-path path/to/paa --dm-xml path/to/data_model
    # Thread Device:
    python3 PICSGenerator.py --pics-template path/to/XML_template --pics-output out_folder --commissioning-method ble-thread \
                            --thread-dataset-hex thread_dataset --passcode setup-pin-code --discriminator discriminator \
                            --paa-trust-store-path path/to/paa --dm-xml path/to/data_model

2.2 Test PAA Certificate
~~~~~~~~~~~~~~~~~~~~~~~~~~

Use the PAA file generated above.

2.3 OTA File
~~~~~~~~~~~~~

Use the OTA file generated above.

2.4 QR Code for Device Under Test
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Use the QR code for the device under test.

2.5 Operating Instructions for Device Under Test
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Provide operating instructions for powering the device, performing factory reset, OTA updates, rollback, and other operations.

3 Testing Environment Setup and Testing Methods
-------------------------------------------------

After preparing the device under test, users can perform self-testing using the `Test Harness tool <https://github.com/project-chip/certification-tool>`__.

3.1 Different Matter versions use different `Test Harness tool versions <https://community.csa-iot.org/page/matter-th>`__ for certification.
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table:: TH Version Mapping
   :widths: 20 35 20
   :header-rows: 1

   * - Matter Version
     - TH Version
     - SDK Commit
   * - Matter 1.2
     - `TH-Fall2023 <https://drive.google.com/file/d/1WTjhc7xbYt18RvpABU3_r47uqOLd7NN1/view?usp=drive_link>`__
     - 19771ed
   * - Matter 1.3
     - v2.10+spring2024
     - 11f94c3
   * - Matter 1.4
     - v2.11+fall2024
     - f2e5de7
   * - Matter 1.4.1
     - v2.12+spring2025
     - 91eab26
   * - Matter 1.4.2
     - v2.13+summer2025
     - 1b2b3fd
   * - Matter 1.5
     - v2.14+fall2025
     - ca9d111

3.2 Test Harness Setup
~~~~~~~~~~~~~~~~~~~~~~~

After selecting the Test Harness version according to the table above, refer to the `Matter_TH_User_Guide.pdf <https://groups.csa-iot.org/wg/members-all/document/37522>`__ document for each Matter version setup, for different test harness versions, the Matter_TH_User_Guide.pdf file may be different.

4 Submitting Certification Application Online
---------------------------------------------

After the device under test passes all tests from the testing organization, users can fill out the application on the CSA certification application `website <https://zigbeecertifiedproducts.knack.com/zigbee-certified#home/>`__ . The following explanation uses Add New Product Application as an example.

4.1 Information to Fill In
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. list-table:: Application Parameters Description
   :widths: 30 70
   :header-rows: 1

   * - Parameter Name
     - Description
   * - Your Company
     - Manufacturer name, usually just select from the dropdown list
   * - Product Name
     - Product name, defined by the manufacturer
   * - Product Short Description
     - Custom short product description
   * - Product Long Description
     - Custom long product description
   * - Product Link URL
     - Optional, product description link
   * - Product Image
     - Optional, product image
   * - Date Product can be Displayed
     - Date when the product will be publicly displayed on the CSA website
   * - Product Type
     - Product certification type, select end device or platform certification. Device manufacturers generally select End Device
   * - Completed optional TIS/TRP testing
     - Optional, default is No
   * - Application Type
     - Application protocol type to be certified, select Matter
   * - Application Type Version
     - Matter protocol version, select Matter 1.1, Matter 1.2, etc. as needed
   * - Compliant Platform
     - Optional, if applying for certification based on Espressif's certified platform, select ESP32-C-Series or ESP32-H-Series from the dropdown list
   * - Firmware Version
     - Firmware version number, consistent with software version string in BasicInformation Cluster
   * - Hardware Version
     - Hardware version number, consistent with hardware version string in BasicInformation Cluster
   * - SKU
     - Optional, manufacturer-defined, usually the product model
   * - Product ID (PID)
     - Product ID, consistent with Product ID in BasicInformation Cluster
   * - Vendor ID (VID)
     - Vendor ID, the ID assigned to the manufacturer by CSA
   * - Transport Used
     - Check the transport protocols supported by the product. For example, for a WiFi product that supports BLE commissioning, check Wi-Fi and Bluetooth
   * - Use 3rd DAC
     - Optional, if using a third-party DAC, select Yes and fill in dac_origin_vendor_id and dac_origin_product_id
   * - dac_origin_vendor_id
     - If using a third-party DAC, fill in the DAC's Vendor ID
   * - dac_origin_product_id
     - If using a third-party DAC, fill in the DAC's Product ID
   * - authorized_paa_list
     - Optional, default is empty
   * - SoftwareVersion Attribute (integer)
     - Software version number, consistent with software version in BasicInformation Cluster
   * - Technical Category
     - Technical category, select Matter
   * - Primary Technical Sub Category
     - Primary technical subcategory (device type). If the certified device includes multiple device types, select the primary device type
   * - Technical Sub Categories
     - Technical subcategories (device types), select all supported device types
   * - Parent Functional Categories
     - Parent functional category. For end products, select a category that matches the actual functionality
   * - Functional Sub Categories
     - Functional subcategories. For end products, select one or more categories that match the actual product functionality
   * - Submit for processing
     - Select submit or save the filled record
   * - Declaration of Conformance Document
     - Doc document
   * - PICS Document
     - PICS file in zip format
   * - Supplemental PICS Document
     - Optional, supplemental PICS file, default is not filled
   * - External Certificate
     - Can be left blank for Matter certification
   * - Transport Attestations
     - Transport attestation documents consistent with the transport protocols checked in Transport Used. Can be declared based on Espressif certificates or certificates derived from Espressif certificates
   * - Security Attestation
     - Security attestation document

4.2 `Declaration of Conformance <https://groups.csa-iot.org/wg/members-all/document/126>`__ (Doc) Instructions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Based on the Doc template provided by Espressif, fill in the relevant content. After the manufacturer signs this document, send it to the certification testing organization. After the testing organization signs it, the manufacturer uploads the signed Doc.

4.3 Transport Attestations (`Bluetooth <https://groups.csa-iot.org/wg/members-all/document/40252>`__, `Wi-Fi <https://groups.csa-iot.org/wg/members-all/document/27435>`__, `Thread <https://groups.csa-iot.org/wg/members-all/document/27434>`__, `Ethernet <https://groups.csa-iot.org/wg/members-all/document/27436>`__) Instructions:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Fill in based on the Transport Attestation template provided by Espressif (version description). Different chip models may have different certificate numbers, which should be verified before filling in. After completion, sign and upload yourself.

4.4 Security Attestation Instructions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Fill in based on the Security Attestation template provided by Espressif. After completion, sign and upload yourself.

4.5 Get the AID and test report
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

After completing and submitting the application, you will receive an Application ID (AID). Provide this ID to the certification testing organization, and the testing organization will upload the product test report to the CSA certification body.

4.6 Other steps
~~~~~~~~~~~~~~~~

After completing the application and payment, the CSA certification team will review the application and test report submitted by the manufacturer. If there are issues, the manufacturer will be notified by email. If there are no issues, the certification is passed, and the official CD file will be issued.

5 Filling in Certified Product Information on DCL Website
----------------------------------------------------------

After the product passes certification, manufacturers also need to add Model, Model Version, and Compliance on the `Distributed Compliance Ledger <https://webui.dcl.csa-iot.org/>`__ (DCL). Refer to the documentation: `HowTo - Writing to the DCL.pdf <https://groups.csa-iot.org/wg/members-all/document/27881>`__ .

6 Other notes for some certification test cases
------------------------------------------------

6.1  Route Information Option (RIO) notes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For Wi-Fi products using LwIP, TC-SC-4.9 should be tested in order to verify that the product can receive Router Advertisement (RA) message with RIO and add route table that indicates whether the prefix can be reached by way of the router. It can be tested with a Thread Border Router (BR) which sends RA message periodically and a Thread End Device that is used to verify the Wi-Fi product can reach the Thread network via Thread BR. Some Wi-Fi Routers might have the issue that they cannot forward RA message sent by the Thread BR, so please use a Wi-Fi Router that can forward RA message when you are testing TC-SC-4.9.

Here are the steps to set up the Thread BR and Thread End Device. You should prepare 2 Radio Co-Processors (RCP) to set up the `ot-br-posix <https://github.com/openthread/ot-br-posix>`__ and `ot-cli-posix <https://github.com/openthread/openthread/tree/main/examples/apps/cli>`__. The `RCP on ESP32-H2 <https://github.com/espressif/esp-idf/tree/master/examples/openthread/ot_rcp>`__ is suggested to be used here. And you can also use other platforms (such as nrf52840, efr32, etc.) as the RCPs.

6.1.1 Setup Thread BR
^^^^^^^^^^^^^^^^^^^^^^

The otbr-posix can be run on RaspberryPi or Ubuntu machine. Connecting an RCP to the host, the port ``RCP_PORT1`` for it will be ``/dev/ttyUSBX`` or ``/dev/ttyACMX``.

- Build the otbr-posix on the host

::

    git clone https://github.com/openthread/ot-br-posix
    cd ot-br-posix
    ./script/bootstrap
    ./script/setup

Then the otbr-posix will be built and a service named otbr-agent will be created on the host. You can disable the service and start the otbr-posix manually.

::

    sudo systemctl disable otbr-agent.service
    sudo ./build/otbr/src/agent/otbr-agent -I wpan0 -B eth0 -v spinel+hdlc+uart://{RCP_PORT1}

In the above commands:

- ``wpan0`` is the infra network interface. The network interface named wpan0 will be created on the host as the thread network interface.

- ``eth0`` is the backbone network interface, which is always the ethernet or wifi network interface on the host, please ensure that the backbone network interface is connected to the AP which the Wi-Fi product is also connected to.

- ``RCP_PORT1`` is the port of RCP for Thread BR.

The otbr-posix is running on the host now. Open another terminal, start console for otbr-posix, form Thread network, and get dataset.

::

    sudo ot-ctl
    > ifconfig up
    > thread start
    > dataset active -x

Please record the dataset you get with the last command, it will be used by ot-cli-posix to join the BR's network in the next step.

6.1.2 Setup Thread End Device
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

We use the Posix Thread Command-Line Interface (CLI) as the Thread End Device. Connect another RCP to the host and get the port `RCP_PORT2` for it.

- Build the otcli on the host

::

    git clone --recursive https://github.com/openthread/openthread.git
    cd openthread/
    ./script/bootstrap
    ./bootstrap
    ./script/cmake-build posix
    ./build/posix/src/posix/ot-cli 'spinel+hdlc+uart:///dev/{RCP_PORT2}?uart-baudrate=115200' -v

The console for the ot-cli will be started. Connect the ot-cli to the otbr’s Thread network with the dataset you got in the above step.

::

    > dataset set active <PROVIDE THE DATASET OF THE BR THAT YOU NEED TO JOIN>
    > dataset commit active
    > ifconfig up
    > thread start
    > srp client autostart enable

In the console of ot-cli, discover the product IP address.

::

    > dns service 177AC531F48BE736-0000000000000190 _matter._tcp.default.service.arpa.
    DNS service resolution response for 177AC531F48BE736-0000000000000190 for service _matter._tcp.default.service.arpa.
    Port:5540, Priority:0, Weight:0, TTL:6913
    Host:72FF282E7739731F.default.service.arpa.
    HostAddress:fd11:66:0:0:22ae:27fe:13ac:54df TTL:6915
    TXT:[SII=35303030, SAI=333030, T=30] TTL:6913

.. note::

   ``177AC531F48BE736-0000000000000190`` can be get with command ``avahi-browse -rt _matter._tcp``. ``177AC531F48BE736`` is the compressed Fabric ID and ``0000000000000190`` is the node ID.

Ping the IP address of the Wi-Fi device.

::

    > ping fd11:66:0:0:22ae:27fe:13ac:54df
    16 bytes from fd11:66:0:0:22ae:27fe:13ac:54df : icmp_seq=2 hlim=64 time=14ms
    1 packets transmitted, 1 packets received. Packet loss = 0.0%. Round-trip min/avg/max = 14/14.0/14 ms.
    Done

The ping command should be successful.

6.2 TC-CNET-3.11
~~~~~~~~~~~~~~~~~

No response on step 7 is expected (`Related issue <https://github.com/CHIP-Specifications/chip-test-plans/issues/1947>`__).

All the NetworkCommissioning commands are fail-safe required. If the commands fail with a ``FAILSAFE_REQUIRED`` status code. You need to send ``arm-fail-safe`` command and then send the NetworkCommissioning commands.

6.3 TC-RR-1.1
~~~~~~~~~~~~~~

For more application endpoints with group cluster, need more nvs size to store group table, so if the ``TC-RR-1.1`` failed, can try to increase the nvs size. (`Related issue <https://github.com/project-chip/connectedhomeip/issues/32481>`__)

Please note that the minimum NVS size required is 48 KB (0xC000) when using a single endpoint with a group cluster.


7 FW/SDK configuration notes
-----------------------------

- ``Enable OTA Requestor`` in ``→ Component config → CHIP Core → System Options``

  The option to enable OTA requestor. This option should be enabled if the OTA requestor feature is selected in PICS files.

- ``Enable Extended discovery Support`` in ``→ Component config → CHIP Device Layer → General Options``

  This option should be enabled if the PICS option ``MCORE.DD.EXTENDED_DISCOVERY`` is selected.

- ``Enable Device type in commissionable node discovery`` in ``→ Component config → CHIP Device Layer → General Options``

  This option should be enabled if the PICS option ``MCORE.SC.EXTENDED_DISCOVERY`` is selected.

- ``LOG_DEFAULT_LEVEL`` in ``→ Component config → Log output``

  It is suggested to set log level to ``No output`` for passing the test cases of OnOff, LevelControl, and ColorControl clusters. Here is `related issue <https://github.com/CHIP-Specifications/chip-test-plans/issues/2332>`__.
