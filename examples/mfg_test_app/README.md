## Matter Manufacturing Test Application

The Matter Manufacturing Test Application is a tool used to verify the successful pre-provisioning of a Matter module.
The purpose of this application is to ensure that the module is only running signed firmware and that all necessary
security features are enabled and configured properly.

It performs the tests listed below:

- Reads DAC and PAI certificate from the secure cert partition and dumps few details:
    - VID and PID
    - Public Key
    - Subject and Authority Key Identifier
- Signs the sample dataset with DAC private key (using DAC provider APIs) and verifies the signature using public key in DAC.
- Validates the certificate chain DAC -> PAI -> PAA. This example by default uses the Espressif's
  [PAA](main/paa_cert.der).
- Makes sure that all the security bits like flash encryption and secure boot are enabled and they are configured in
  *release mode* and corresponding efuses are burned.


### What is expected from the customers opting for Matter pre-provisioning service

If a customer opts for the Matter pre-provisioning service, they must provide a signed firmware binary of this firmware
to test the pre-provisioning of their module.

The secure boot signing key used for signing this firmware must match the key that was used to sign the bootloader
provided for pre-provisioning purposes.

#### Steps to build the firmware

- Copy the key for signing the firmware in the project directory with name `secure_boot_signing_key.pem`.

- Configure Device Vendor Id and Device Product Id using `idf.py menuconfig`,
    - Menu path: `(Top) -> Component config -> CHIP Device Layer -> Device Identification Options`
    - Config options: `CONFIG_DEVICE_VENDOR_ID` and `CONFIG_DEVICE_PRODUCT_ID`

- Build the firmware and provide the signed application binary (`build/mfg_test_app.bin`) to Espressif.

- Please use esp-idf [v5.1](https://github.com/espressif/esp-idf/tree/v5.1).
