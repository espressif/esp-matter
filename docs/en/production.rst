4. Production Considerations
============================

4.1 Over-the-air Updates (OTA)
------------------------------

ESP-IDF has a component for OTA from any URL. More information and
details about implementing can be found here:
`esp_https_ota <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/esp_https_ota.html#esp-https-ota>`__.

todo. add about DCL.

4.2 Getting your own Vendor ID and Product ID
---------------------------------------------

todo. Info about DAC, pai, paa too.

4.3 Manufacturing
-----------------

4.3.1 Mass Manufacturing Utility
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For commissioning a device into the Matter Fabric, the device requires the following information:

-   **Device Attestation Certificate (DAC) and Certification Declaration (CD)**: verified by commissioner to determine whether a device is a Matter certified product or not.
-   **Discriminator**: advertised during commissioning to easily distinguish between advertising devices.
-   **Spake2+ parameters**: work as a proof of possession.

These details are generally programmed in the factory NVS partitions that are unique
per device. ESP-Matter provides a utility (mfg_tool.py) to create instances of the factory NVS partition images on a per-device basis for mass manufacturing purposes.

When using the utility, by default, the above details will be included in the generated NVS partition image. The utility also has provision to include additional details in the same NVS image by using the config and value CSV files.

Details about using the ESP Matter mass manufacturing utility can be found here:
:project_file:`mfg_tool<tools/mfg_tool/README.md>`.

4.3.2 Pre-Provisioned Modules
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ESP32 modules can be pre-flashed with the factory NVS partition during
manufacturing itself and then be shipped to you.

This saves you the overhead of securely generating, encrypting and then
programming the NVS partition into the device at your end.
Pre-provisioning is an optional service which Espressif provides.

Details about pre-provisioned modules can be found here:
`docs <https://glab.espressif.cn/alexa/esp-va-sdk/wikis/home>`__.

Please contact your Espressif contact person for more information.

4.4 Security
------------

4.4.1 Secure Boot
~~~~~~~~~~~~~~~~~

Secure boot ensures that only trusted code runs on the device.

ESP32 supports RSA based secure boot scheme whereby the bootROM verifies
the software boot loader for authenticity using the RSA algorithm. The
verified software boot loader then checks the partition table and
verifies the active application firmware and then boots it.

More information about how secure boot works on ESP32 can be found here:
`docs <https://glab.espressif.cn/alexa/esp-va-sdk/wikis/home>`__.

Details about implementing the secure boot can be found here:
`secure_boot <https://docs.espressif.com/projects/esp-idf/en/latest/security/secure-boot.html>`__.

4.4.2 Flash Encryption
~~~~~~~~~~~~~~~~~~~~~~

Flash encryption prevents the plain-text reading of the flash contents.

ESP32 supports AES-256 based flash encryption scheme. The ESP32 flash
controller has an ability to access the flash contents encrypted with a
key and place them in the cache after decryption. It also has ability to
allow to write the data to the flash by encrypting it. Both the
read/write encryption operations happen transparently.

More information about how flash encryption works on ESP32 can be found
here: `docs <https://glab.espressif.cn/alexa/esp-va-sdk/wikis/home>`__.

Details about implementing the flash encryption can be found here:
`flash_encryption <https://docs.espressif.com/projects/esp-idf/en/latest/security/flash-encryption.html>`__.

4.4.3 NVS Encryption
~~~~~~~~~~~~~~~~~~~~

For the manufacturing data that needs to be stored on the device in the
NVS format, ESP-IDF provides the NVS image creation utility which allows
the encryption of NVS partition on the host using a randomly generated
(per device unique) or pre-generated (common for a batch) NVS encryption
key.

A separate flash partition is used for storing the NVS encryption keys.
This flash partition is then encrypted using flash encryption. So, flash
encryption becomes a mandatory feature to secure the NVS encryption
keys.

More information about how NVS encryption works on ESP32 can be found
here: `docs <https://glab.espressif.cn/alexa/esp-va-sdk/wikis/home>`__.

Details about implementing the NVS encryption can be found here:
`nvs_encryption <https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/storage/nvs_flash.html#nvs-encryption>`__.
