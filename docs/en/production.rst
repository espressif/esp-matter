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

Your manufacturing line needs to ensure that these unique manufacturing partition images are
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

Alternatively, `ESP RainMaker OTA`_ service can also be used to upgrade the firmware
on the devices remotely. As opposed to the Matter OTA, ESP RainMaker OTA allows you
the flexibility of delivering the OTA upgrades incrementally or to groups of devices.


4.3 Manufacturing
-----------------

4.3.1 Mass Manufacturing Utility
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For commissioning a device into the Matter Fabric, the device requires the following information:

-   **Device Attestation Certificate (DAC) and Certification Declaration (CD)**: verified by commissioner to determine whether a device is a Matter certified product or not.
-   **Discriminator**: advertised during commissioning to easily distinguish between advertising devices.
-   **Spake2+ parameters**: work as a proof of possession.

These details are generally programmed in the manufacturing partition that is unique
per device. ESP-Matter provides a utility (esp-matter-mfg-tool) to create these partition images
on a per-device basis for mass manufacturing purposes.

When using the utility, by default, the above details will be included in the generated manufacturing partition image. The utility also has a provision to include additional details in the same image by using CSV files.

Details about using the mass manufacturing utility can be found here: `esp-matter-mfg-tool`_

4.3.2 Pre-Provisioned Modules
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

{IDF_TARGET_NAME} modules can be pre-flashed with the manufacturing partition images
during module manufacturing itself and then be shipped to you.

This saves you the overhead of securely generating, encrypting and then
programming the partition into the device at your end.

Please contact your Espressif contact person for more information.

4.3.3 The esp-matter-mfg-tool Example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In Espressif Matter Prep-provisioning modules, the DAC key pair, DAC and PAI certificates are pre-flashed by default.

This section gives some examples on how to generate factory partition binary which includes :
    | **Device unique data** (Discriminator, Verifier, Serial Number, etc)
    | **Manufacturing information** (Vendor name, Product name, Hardware version, etc)

.. note::

    The items listed in the examples are all mandatory, some common manufacturing information could be removed if they are hard coded in the firmware.

This is the example to generate factory images after pre-provisioning:

- **Generate generic factory image**

   ::

      esp-matter-mfg-tool -cd ~/test_cert/CD/Chip-CD-131B-1000.der -v 0x131B --vendor-name ESP -p 0x1000 --product-name light --hw-ver 1 --hw-ver-str v1.0 --mfg-date 2022-10-25 --passcode 19861989 --discriminator 601 --serial-num esp32c_dev3

- **Generate multiple generic factory images**

   ::

      esp-matter-mfg-tool -n 10 -cd ~/test_cert/CD/Chip-CD-131B-1000.der -v 0x131B --vendor-name ESP -p 0x1000 --product-name light --hw-ver 1 --hw-ver-str v1.0 --mfg-date 2022-10-25

- **Generate factory image with rotating device unique identify**

   ::

      esp-matter-mfg-tool -cd ~/test_cert/CD/Chip-CD-131B-1000.der -v 0x131B --vendor-name ESP -p 0x1000 --product-name light --hw-ver 1 --hw-ver-str v1.0 --mfg-date 2022-10-25 --passcode 19861989 --discriminator 601 --serial-num esp32c_dev3 --enable-rotating-device-id --rd-id-uid c0398f4980b07c9460f71c5421e1a3c5

- **Generate multiple factory images with csv and mcsv**

   ::

      esp-matter-mfg-tool -cd ~/test_cert/CD/Chip-CD-131B-1000.der -v 0x131B --vendor-name ESP -p 0x1000 --product-name light --hw-ver 1 --hw-ver-str v1.0 --enable-rotating-device-id --mfg-date 2022-10-25 --csv mfg.csv --mcsv mfg_m.csv

- **The example of csv and mcsv file**
- CSV:
    | serial-num,data,string
    | rd-id-uid,data,hex2bin
    | discriminator,data,u32

- MCSV:
    | serial-num,rd-id-uid,discriminator
    | esp32c_dev3,c0398f4980b07c9460f71c5421e1a3c5,1234
    | esp32c_dev4,c0398f4980b07c9460f71c5421e1a3c6,1235
    | esp32c_dev5,c0398f4980b07c9460f71c5421e1a3c7,1236
    | esp32c_dev6,c0398f4980b07c9460f71c5421e1a3c8,1237
    | esp32c_dev7,c0398f4980b07c9460f71c5421e1a3c9,1238


.. _`esp-matter-mfg-tool`: https://github.com/espressif/esp-matter-tools/tree/main/mfg_tool
.. _`ESP RainMaker OTA`: https://rainmaker.espressif.com/docs/ota.html
