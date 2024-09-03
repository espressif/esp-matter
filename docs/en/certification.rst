3. Matter Certification
=======================

The Matter Certification denotes compliance to a Connectivity Standards Alliance (CSA) specification for the product and allow the use of Certified Product logos and listing of the product on the Alliance website for verification.

You need to `become a member <https://csa-iot.org/become-member/>`__ of CSA and request a Vendor ID code from CSA Certification before you apply for a Matter Certification. Then you need to choose an `authorized test provider <https://csa-iot.org/certification/testing-providers/>`__ (must be validated for Matter testing) and submit your product for testing. Here are some tips for the Matter Certification Test.

3.1 Introduction to Test Harness (TH)
-------------------------------------

Test Harness on RaspberryPi is used for Matter Certification Test. You can fetch the TH RaspberryPi image from `here <https://groups.csa-iot.org/wg/matter-csg/document/27406>`__ and install the image to a micro SD card with the `Raspberry Pi Imager <https://www.raspberrypi.com/software/>`__.

Test cases can be verified with TH by 4 methods including UI-Automated, UI-SemiAutomated, UI-Manual, and Verification Steps Document. A website UI is used for the first three methods. You can follow the instructions in `TH User Guide <https://groups.csa-iot.org/wg/matter-csg/document/24838>`__ to use the website UI. For the last method, you should use the chip-tool in path ``~/apps`` of the TH and execute the commands in the `Verification Steps Document <https://groups.csa-iot.org/wg/matter-csg/document/26925>`__ step by step.

3.2 Matter Factory Partition Binary
-----------------------------------

Matter factory partition binary files contains the commissionable information (discriminator, salt, iteration count, and spake2+ verifier) and device attestation information (Certification Declaration (CD), Product Attestation Intermediate (PAI) certificate, Device Attestation Certificate (DAC), and DAC private key), device instance information(vendor ID, vendor name, product ID, product name, etc.), and device information (fixed label, supported locales, etc.). These informations are used to identify the product and ensure the security of commissioning.

3.2.1 Certification Declaration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A Certification Declaration (CD) is a cryptographic document that allows a Matter device to assert its protocol compliance. It can be generated with following steps. We need to generate the CD which matches the vendor id and product id in DAC and the ones in basic information cluster.

A test CD signed by the test CD signing keys in `connectedhomeip <https://github.com/espressif/connectedhomeip/tree/master/credentials/test/certification-declaration>`__ SDK repository is required for Matter Certification Test, so the ``certification_type`` of it is 1 (provisional). The CD in official products passing the Matter Certification Test is issued by CSA and the ``certification_type`` is 2 (official).

- Generate the Test CD file

::

    cd path/to/esp_matter/connnectedhomeip/connnectedhomeip
    out/host/chip-cert gen-cd --format-version 1 --vendor-id 0x131B --product-id 0x1234 \
                              --device-type-id 0x010c --certificate-id CSA00000SWC00000-01 \
                              --security-level 0 --security-info 0 --version-number 1 \
                              --certification-type 1 \
                              --key credentials/test/certification-declaration/Chip-Test-CD-Signing-Key.pem \
                              --cert credentials/test/certification-declaration/Chip-Test-CD-Signing-Cert.pem \
                              --out path/to/test_CD_file

.. note::

    - The option ``--certification-type`` must be 1 for the Matter Certification Test.
    - The options ``--vendor_id`` (vendor_id) should be the Vendor ID (VID) that the vendor receives from CSA, and ``--product_id`` (product_id) could be the Product ID (PID) choosed by the vendor. They should be the same as the attributes' value in basic information cluster.
    - If the product uses the DACs and PAI certifications provided by a trusted third-party certification authority, the VID and PID in DAC are different from the ones in basic information cluster. Then the ``--dac-origin-vendor-id`` and ``--dac-origin-product-id`` options should be added in the command generating the test CD file.

3.2.2 Certificates and Keys
~~~~~~~~~~~~~~~~~~~~~~~~~~~

For Matter Certification Test, vendors should generate their own test Product Attestation Authority (PAA) certificate, Product Attestation Intermediate (PAI) certificate, and Device Attestation Certificate (DAC), but not use the default test PAA certificate in `connectedhomeip <https://github.com/espressif/connectedhomeip/tree/master/credentials/test/attestation>`__ SDK repository. So you need to generate a PAA certificate, and use it to sign and attest PAI certificates which will be used to sign and attest the DACs. The PAI certificate, DAC, and DAC's private key should be stored in the product you submit to test.

Here are the steps to generate the certificates and keys using `chip-cert`_ and `esp-matter-mfg-tool`_.

3.2.2.1 Generating PAA Certificate
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Vendor scoped PAA certificate is suggested for the Matter Certificate Test. It can be generated with the help of blow mentioned steps.

Generate the vendor scoped PAA certificate and key, please make sure to change the ``--subject-vid`` (vendor_id) option base on the one that is being used.

::

    cd path/to/connnectedhomeip/out/host/
    ./chip-cert gen-att-cert --type a --subject-cn "Example PAA CN" --subject-vid 0x131B \
                             --valid-from "2021-06-28 14:23:43" --lifetime 4294967295 \
                             --out-key /path/to/PAA_key \
                             --out /path/to/PAA_certificate

3.2.2.2 Generating Factory Partition Binary Files
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

After getting the PAA certificate and key, the factory partition binary files with PAI certificate, DAC, and DAC keys can be generated using esp-matter-mfg-tool.

- Install the requirements and export the dependent tools path if not done already

::

    cd path/to/esp_matter
    python3 -m pip install -r requirements.txt
    export PATH=$PATH:$PWD/connectedhomeip/connectedhomeip/out/host

- Generate factory partition binary files

::

    esp-matter-mfg-tool -n <count> -cn Espressif --paa -c /path/to/PAA_certificate -k /path/to/PAA_key \
                  -cd /path/to/CD_file -v 0x131B --vendor-name Espressif -p 0x1234 \
                  --product-name Test-light --hw-ver 1 --hw-ver-str v1.0

.. note::

    For more information about the arguments, you can use ``esp-matter-mfg-tool --help``

The option ``-n`` (count) is the number of generated binaries. In the above command, esp-matter-mfg-tool will generate PAI certificate and key and then use them to generate ``count`` different DACs and keys. It will use the generated certificates and keys to generate ``count`` factory partition binaries with different DACs, discriminators, and setup pincodes. Flash the factory binary to the device's NVS partition. Then the device will send the vendor's PAI certificate and DAC to the commissioner during commissioning.

3.2.2.3 Using Vendor's PAA in Test Harness(TH)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- Manual Tests (Verified by UI-Manual and Verification Steps Document)

The option ``--paa-trust-store-path`` should be added when using chip-tool to pair the device for manual tests.

.. only:: esp32 or esp32s3 or esp32c3 or esp32c2 or esp32c6

   ::

      cd path/to/connnectedhomeip/out/host/
      ./chip-tool pairing ble-wifi 0x7283 <ssid> <passphrase> <setup-pin-code> <discriminator> --paa-trust-store-path <paa-certificate-path>

.. only:: esp32c6

    or

.. only:: esp32h2 or esp32c6

   ::

     cd path/to/connnectedhomeip/out/host/
     ./chip-tool pairing ble-thread 0x7283 hex:<thread-dataset> <setup-pin-code> <discriminator> --paa-trust-store-path <paa-certificate-path>

.. note::

    - ``pincode`` and ``discriminator`` are in the /out/<vid>-<pid>/<UUID>/<uuid>-onb_codes.csv.
    - PAA certificate should be converted to DER format using ``chip-cert`` and stored in ``paa-certificate-path``.

- Automated Tests (Verified by UI-Automated and UI-SemiAutomated)

Here are the steps to upload the PAA certificate and use it for automated tests:

In Test Harness, you should modify the project configuration to use the vendor's PAA for the DUT that requires a PAA certificate to perform a pairing operation. The flag ``chip_tool_use_paa_certs`` in the ``dut_config`` should be set to ``true`` to configure the Test Harness to use the PAA certificates.

::

    "dut_config": {
        "discriminator": "3840",
        "setup_code": "20202021",
        "pairing_mode": "onnetwork",
        "chip_tool_timeout": null,
        "chip_tool_use_paa_certs": true
    }

Make sure  to copy your PAA certificates in DER format to the default path ``/var/paa-root-certs/`` on the Raspberry-Pi.

::

    sudo cp /path/to/PAA_certificate.der /var/paa-root-certs/

Run automated chip-tool tests and verify that the pairing commands are using the ``--paa-trust-store-path`` option.

3.2.3 Menuconfig Options
~~~~~~~~~~~~~~~~~~~~~~~~

Please consult the `factory data providers <./developing.html#factory-data-providers>`__ and adjust the menucofig options accordingly for the certification test.

3.3 Matter OTA Image Generation
-------------------------------

If the product supports OTA Requestor features of Matter, the test cases of OTA Software Update should be tested. So you need to provide the image for OTA test and also the way to downgrade.

Here are two ways to generate the OTA image.

3.3.1 Using menuconfig option
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Enable ``Generate Matter OTA image`` in ``→ Component config → CHIP Device Layer → Matter OTA Image``, set ``Device Vendor Id`` and ``Device Product Id`` in ``→ Component config → CHIP Device Layer → Device Identification Options``, and edit the ``PROJECT_VER`` and the ``PROJECT_VER_NUMBER`` in the project's CMakelists. Build the example and the OTA image will be generated in the build path with the app binary file.

.. note::

   The ``PROJECT_VER_NUMBER`` must always be incremental. It must be higher than the version number of firmware to be updated.

3.3.2 Using ota_image_tool script
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We should also edit the ``PROJECT_VER`` and the ``PROJECT_VER_NUMBER`` in the project's CMakelists when using the script to generate the OTA image.

- Build the example and generate the OTA image

::

    cd path/to/example
    idf.py build
    cd path/to/esp_matter/connectedhomeip/connectedhomeip/src/app
    ./ota_image_tool.py create -v <vendor-id> -p <product-id> -vn 2 -vs v1.1 -da sha256 \
                               /path/to/original_app_bin /path/to/out_ota_bin

.. note::

    The ``-vn`` (version-number) and ``-vs`` (version-string) should match the values in the project's CMakelists.

3.4 PICS files
--------------

The PICS files define the Matter features for the product. The authorized test provider will determine the test cases to be tested in Matter Certification Test according to the PICS files submitted.

The `PICS Tool <https://picstool.csa-iot.org/>`__ website is the tool to open, modify, validate, and save the XML PICS files. The `reference XML PICS template files <https://groups.csa-iot.org/wg/matter-csg/document/26122>`__ include all the reference PICS files and each of the XML files defines the features of one or several clusters on the products.

A `PICS-generator tool <https://github.com/espressif/connectedhomeip/tree/master/src/tools/PICS-generator>`__ is provided to generate the PICS files with the reference PICS XML template files. The tools will read the supported clusters, attributes, commands, and event from a paired device and generate PICS files for that device. Note that the Base XML file will not be generated with this tool. You still need to modify it in the ``PICS TOOL``.

Open the reference PICS files that include all the clusters of the product, and select the features supported by the product. Clicking the button ``Validate All``, the PICS Tool will validate all the XML files and generate a list of test cases to be tested in Matter Certification Test.

3.5 Route Information Option (RIO) notes
----------------------------------------

For Wi-Fi products using LwIP, TC-SC-4.9 should be tested in order to verify that the product can receive Router Advertisement (RA) message with RIO and add route table that indicates whether the prefix can be reached by way of the router. It can be tested with a Thread Border Router (BR) which sends RA message periodically and a Thread End Device that is used to verify the Wi-Fi product can reach the Thread network via Thread BR. Some Wi-Fi Routers might have the issue that they cannot forward RA message sent by the Thread BR, so please use a Wi-Fi Router that can forward RA message when you are testing TC-SC-4.9.

Here are the steps to set up the Thread BR and Thread End Device. You should prepare 2 Radio Co-Processors (RCP) to set up the `ot-br-posix <https://github.com/openthread/ot-br-posix>`__ and `ot-cli-posix <https://github.com/openthread/openthread/tree/main/examples/apps/cli>`__. The `RCP on ESP32-H2 <https://github.com/espressif/esp-idf/tree/master/examples/openthread/ot_rcp>`__ is suggested to be used here. And you can also use other platforms (such as nrf52840, efr32, etc.) as the RCPs.

3.5.1 Setup Thread BR
~~~~~~~~~~~~~~~~~~~~~

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

Please record the dataset you get with the last command, it will be used by otcli-posix to join the BR’s network in the next step.

3.5.2 Setup Thread End Device
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

3.6 FW/SDK configuration notes
------------------------------

- ``Enable OTA Requestor`` in ``→ Component config → CHIP Core → System Options``

  The option to enable OTA requestor. This option should be enabled if the OTA requestor feature is selected in PICS files.

- ``Enable Extended discovery Support`` in ``→ Component config → CHIP Device Layer → General Options``

  This option should be enabled if the PICS option ``MCORE.DD.EXTENDED_DISCOVERY`` is selected.

- ``Enable Device type in commissionable node discovery`` in ``→ Component config → CHIP Device Layer → General Options``

  This option should be enabled if the PICS option ``MCORE.SC.EXTENDED_DISCOVERY`` is selected.

- ``LOG_DEFAULT_LEVEL`` in ``→ Component config → Log output``

  It is suggested to set log level to ``No output`` for passing the test cases of OnOff, LevelControl, and ColorControl clusters. Here is `related issue <https://github.com/CHIP-Specifications/chip-test-plans/issues/2332>`__.

3.7 Appendix FAQs
-----------------

Here are some issues that you might meet in Matter Certification Test and quick solutions for them.

- ``TC-CNET-3.11``

  No response on step 7 is expected (`Related issue <https://github.com/CHIP-Specifications/chip-test-plans/issues/1947>`__).

  All the NetworkCommissioning commands are fail-safe required. If the commands fail with a ``FAILSAFE_REQUIRED`` status code. You need to send ``arm-fail-safe`` command and then send the NetworkCommissioning commands.

.. _`esp-matter-mfg-tool`: https://github.com/espressif/esp-matter-tools/tree/main/mfg_tool
.. _`chip-cert`: https://github.com/espressif/connectedhomeip/tree/master/src/tools/chip-cert/README.md
