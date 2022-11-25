4. Production Considerations
============================

4.1 Prerequisites
-----------------

All Matter examples use certain test or evaluation values that enables you to quickly build and test Matter. As you get ready to go to production, these must be replaced with the actual values. These values are typically a part of the manufacturing partition in your device.

4.1.1 Vendor ID and Product ID
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A **Vendor Identifier (VID)** is a 16-bit number that uniquely identifies a particular
product manufacturer or a vendor. It is allocated by the Connectivity Standards
Alliance (CSA). Please reach out to CSA for this.

A **Product Identifier (PID)** is a 16-bit number that uniquely identifies a product
of a vendor. It is assigned by the vendor (you).

A VID-PID combination uniquely identifies a Matter product.

4.1.2 Certificates
~~~~~~~~~~~~~~~~~~

A **Device Attestation Certificate (DAC)** proves the authenticity of the device
manufacturer and the certification status of the device's hardware and software.
Every Matter device must have a DAC and corresponding private key, unique to it.
The device should also have a Product Attestation Intermediate (PAI) certificate
that was used to sign and attest the DAC. The PAI certificate in turn is signed
and attested by Product Attestation Authority (PAA).
The PAA certificate is an implicitly trusted self-signed root certificate.

Please reach out to your Espressif representative for the details about how to
procure the DAC.

4.1.3 Certification Declaration (CD)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A **Certification Declaration (CD)** is a cryptographic document that allows a Matter
device to assert its protocol compliance. Once your product is certified, the CSA
creates a CD for that device. The CD should then be included in the device firmware
by the device manufacturer.

4.1.4 Setup Passcode, Discriminator and Onboarding Payload
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The unique **setup passcode** serves as the proof of possession and is also used to compute
the shared secret during commissioning. The corresponding SPAKE2+ verifier of the
passcode is installed on the device and not the actual passcode.

The **discriminator** is used to easily distinguish between devices to provide a seamless
experience during commissioning.

The onboarding payload is the **QR code** and the **manual pairing code** that assists
a commissioner (like a phone app) to allow onboarding a device into the Matter network.
The QR code and/or the manual pairing code are generally printed on the packaging of the
device.


4.1.5 Manufacturing Partition
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Espressif's SDK for Matter uses a separate manufacturing partition to store all the information
mentioned above. Because the DACs are unique to every device, the manufacturing partition
will also be unique per device. Thus by moving all the typical per device unique fields into
the manufacturing partition, the rest of the components like the bootloader, firmware image
are common across all your devices. You can refer the Manufacturing section below for creating
a large number of manufacturing partition images.

Your manufacturing line needs to ensure that these unique manufacturing parition images are
correctly written to each device and the appropriate QR code images associated with each device.
You may also opt for Espressif's pre-provisioning service that pre-provisions these unique
images before shipping the modules and provides a manifest (CSV file) along with QR code images
bundle.


4.2 Over-the-Air (OTA) Updates
------------------------------

Matter devices must support OTA firmware updates, either by using Matter-based OTA
or vendor specific means.

In case of Matter OTA, there's an *OTA provider* that
assists an *OTA requestor* to get upgraded. The SDK examples support Matter OTA
requestor role out of the box. The OTA provider could be a manufacturer specific
phone app or any Matter node that has internet connectivity.

Alternatively, `ESP RainMaker OTA <https://rainmaker.espressif.com/docs/ota.html>`__
service can also be used to upgrade the firmware on the devices remotely. As opposed to the Matter OTA, ESP RainMaker OTA allows you the flexibility of delivering the OTA upgrades incrementally or to groups of devices.


4.3 Manufacturing
-----------------

4.3.1 Mass Manufacturing Utility
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For commissioning a device into the Matter Fabric, the device requires the following information:

-   **Device Attestation Certificate (DAC) and Certification Declaration (CD)**: verified by commissioner to determine whether a device is a Matter certified product or not.
-   **Discriminator**: advertised during commissioning to easily distinguish between advertising devices.
-   **Spake2+ parameters**: work as a proof of possession.

These details are generally programmed in the manufacturing partition that is unique
per device. ESP-Matter provides a utility (mfg_tool.py) to create these partition images
on a per-device basis for mass manufacturing purposes.

When using the utility, by default, the above details will be included in the generated manufacturing partition image. The utility also has a provision to include additional details in the same image by using CSV files.

Details about using the mass manufacturing utility can be found here:
:project_file:`mfg_tool<tools/mfg_tool/README.md>`.

4.3.2 Pre-Provisioned Modules
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

ESP32 modules can be pre-flashed with the manufacturing partition images
during module manufacturing itself and then be shipped to you.

This saves you the overhead of securely generating, encrypting and then
programming the partition into the device at your end.

Please contact your Espressif contact person for more information.

4.4 Security
------------

Matter conformance requires you to protect the DACs that are uniquely programmed
on each device. To achieve this, you need to take care of the following aspects.


4.4.1 Secure Boot
~~~~~~~~~~~~~~~~~

Secure boot ensures that only trusted code runs on the device.

ESP32 supports RSA based secure boot scheme whereby the bootROM verifies
the software boot loader for authenticity using the RSA algorithm. The
verified software boot loader then checks the partition table and
verifies the active application firmware and then boots it.

Details about implementing the secure boot can be found here:
`secure_boot <https://docs.espressif.com/projects/esp-idf/en/v4.4.2/security/secure-boot.html>`__.

4.4.2 Flash Encryption
~~~~~~~~~~~~~~~~~~~~~~

Flash encryption prevents the plain-text reading of the flash contents.

ESP32 supports AES-256 based flash encryption scheme. The ESP32 flash
controller has an ability to access the flash contents encrypted with a
key and place them in the cache after decryption. It also has ability to
allow to write the data to the flash by encrypting it. Both the
read/write encryption operations happen transparently.

Details about implementing the flash encryption can be found here:
`flash_encryption <https://docs.espressif.com/projects/esp-idf/en/v4.4.2/security/flash-encryption.html>`__.

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

Details about implementing the NVS encryption can be found here:
`nvs_encryption <https://docs.espressif.com/projects/esp-idf/en/v4.4.2/api-reference/storage/nvs_flash.html#nvs-encryption>`__.
