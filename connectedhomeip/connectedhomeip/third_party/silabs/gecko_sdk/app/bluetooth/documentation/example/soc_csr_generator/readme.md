# SoC - Certificate Signing Request Generator

This is a certificate generator application used to generate the device elliptical curve (EC) key pair, the signing request for the device certificate, and other related data. The generated data can be read out by the Central Authority for signing. The signed certificate is then stored on the device and can be used by other applications like Certificate-Based Authentication and Pairing.

## Getting started

To get started with Silicon Labs Bluetooth and Simplicity Studio, see [QSG169: Bluetooth SDK v3.x Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg169-bluetooth-sdk-v3x-quick-start-guide.pdf).

This example is based on device certificates. To learn about device certificates, refer to [AN1268: Authenticating Silicon Labs Devices Using Device Certificates](https://www.silabs.com/documents/public/application-notes/an1268-efr32-secure-identity.pdf).

Some examples (like **Bluetooth - SoC Certification Based Authentication and Pairing**) require certificates stored on the device. To let those example run properly, the following data must be stored, preferably in the secure vault of the device:
* An EC key pair (private and public key)
* Device certificate, including the device's public key, signed by a Central Authority (CA)
* CA certificate, that is a self-signed certificate that holds the CA's public key

![](readme_img1.png)

To flash these components to the device, this firmware must be used together with some Python scripts that can be run on a host computer:
* `create_authority_certificate.py`: generates the root certificate. Also creates a header file with the root certificate to be stored on the device.
* `production_line_tool.py` (PLT): reads the certificate signing request from the device and signs it with the CA private key, then flashes it to the device.

The scripts can be found in the following folder:
*{SDK_folder}/app/bluetooth/script/certificate_authorities*

![](readme_img2.png)

## Generating the Device Certificate

To generate the device certificate and get it signed, follow this process:

1. Factory-reset your device to make sure that no EC keys and certificates are stored on it. This can be done with Simplicity Commander using the **Recover Bricked Device** option in the GUI or with the following CLI command: `commander device recover`.

2. Flash an **Internal Storage Bootloader** to your device. (Must be generated and built as a separate project.)

3. Open the slcp file of the **Bluetooth - SoC CSR Generator** project.

4. On the Overview tab, under Project Details, open the three-dots-menu, and click **Configuration**.

5. Modify the certificate data.

6. Build and flash the project to your device. This will automatically generate the EC key pair and the Certificate Signing Request on startup.

7. Create a CA certificate if you do not have one yet. Run the `create_authority_certificate.py` python script on your computer. Certificate data can be provided as parameters, see the parameter list with `create_authority_certificate.py -h`. *Note: you may need to install some python packages (cryptography, jinja2) to get this script run.*

8. The CA certificate can now be found under the `central_authority` folder.

9. Check the Jlink serial number of your debug adapter either with Simplicity Studio or with Simplicity Commander.

10. Run the `production_line_tool.py` python script on your computer with the following parameters: 
`python production_line_tool.py -p ble --serial <serialnumber>`. This will read out the signing request, sign the device certificate and flash the signed certificate on the device. *Note: the 'serial' parameter is not required if only one device is connected to your PC.*

11. Now the EC key pair and the signed certificate are stored on your device. You can flash a new application to the device, this will not remove the keys and the certificate.

12. To also flash the CA certificate (root certificate), you must copy the generated `sl_bt_cbap_root_cert.h` file (found under *{SDK_folder}/app/bluetooth/script/certificate_authorities/central_authority*) into your new application project under the */config* folder.

## Troubleshooting

Before programming the radio board mounted on the mainboard, make sure the power supply switch is in the AEM position (right side) as shown below.

![Radio board power supply switch](readme_img0.png)

## Resources

[Bluetooth Documentation](https://docs.silabs.com/bluetooth/latest/)

[UG103.14: Bluetooth LE Fundamentals](https://www.silabs.com/documents/public/user-guides/ug103-14-fundamentals-ble.pdf)

[QSG169: Bluetooth SDK v3.x Quick Start Guide](https://www.silabs.com/documents/public/quick-start-guides/qsg169-bluetooth-sdk-v3x-quick-start-guide.pdf)

[UG434: Silicon Labs Bluetooth ® C Application Developer's Guide for SDK v3.x](https://www.silabs.com/documents/public/user-guides/ug434-bluetooth-c-soc-dev-guide-sdk-v3x.pdf)

[AN1268: Authenticating Silicon Labs Devices Using Device Certificates](https://www.silabs.com/documents/public/application-notes/an1268-efr32-secure-identity.pdf)

[Bluetooth Training](https://www.silabs.com/support/training/bluetooth)

## Report Bugs & Get Support

You are always encouraged and welcome to report any issues you found to us via [Silicon Labs Community](https://www.silabs.com/community).