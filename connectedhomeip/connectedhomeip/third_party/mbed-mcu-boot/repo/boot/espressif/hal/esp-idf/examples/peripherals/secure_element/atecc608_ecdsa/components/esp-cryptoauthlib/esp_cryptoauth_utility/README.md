# ESP_CRYPTOAUTH_UTILITY
# Description
 The python utility helps to configure and provision `ATECC608` chip on `ESP32-WROOM-32SE` module.The [ESP32-WROOM-32SE](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32se_datasheet_en.pdf) module has Microchip's [ATECC608A](https://www.microchip.com/wwwproducts/en/ATECC608A) integrated on the module. The latest ESP32-WROOM-32SE modules have the Microchip's [ATECC608B](https://www.microchip.com/en-us/products/security-ics/trust-platform/trust-and-go) integrated on the module.
    There are currently three types of ATECC608 which are [Trust & Go](https://www.microchip.com/wwwproducts/en/ATECC608A-TNGTLS), [TrustFlex](https://www.microchip.com/wwwproducts/en/ATECC608A-TFLXTLS) and [TrustCustom](https://www.microchip.com/wwwproducts/en/ATECC608A). `Trust & Go` and `TrustFlex` chips are preconfigured by the manufacturer (Microchip) so we only need to generate manifest file for those chips. `TrustCustom` type of chips are not configured, so for `TrustCustom` type of chips need to be first configured and then provisioned with a newly  generated device certificate and key pair. The script automatically detects which type of ATECC608 chip is integrated with `ESP32-WROOM-32SE` so it will proceed to next required steps on its own.

# Hardware Required
It requires [ESP32-WROOM-32SE](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32se_datasheet_en.pdf) which has Microchip's [ATECC608A](https://www.microchip.com/wwwproducts/en/ATECC608A) (Secure Element) integrated on the module.

An ESP32 to which ATECC608 is connected with I2C interface can also be used by setting the I2C pin configurations (see below option about providing I2C pin cfg).

> Note: It is recommended to change directory to `esp_cryptoauth_utility` to execute all following commands if not already done.

## Install python dependancies
To use the utility some python depencancies must be installed with following command(current directory should be `esp_cryptoauth_utility` for executing the command).

    pip install -r requirements.txt

## Step 1:- Generate Signer Certificate
Signer cert and key pair:
* In case of `TrustCustom` chips ,these certificate and key pair are used to sign the device cert which is going to be generted.

* In case of `Trust & Go` and `TrustFlex` devices the device certs are already signed by microchip signer cert, and the signer cert and key pair generated in this step are used to sign the manifest file.

By default the utility uses the `sample_signer_cert.pem` located in the `sample_certs` folder.if you want to keep using default certificats, then directly proceed to next step(Step 2).

Create a signer key and signer cert by executing following commands sequentially. The second command will ask some details about certificate such as `ORG, CN` which are needed to be filled by the user.

 `Important`: The signer cert `CN`_(Common Name)_ must end with `FFFF` as it is required by the `cert2certdef.py` (file by microchip) to create its definition properly. for e.g valid CN = `Sample Signer FFFF`( This is compulsory only in case of `TrustCustom` type of chips and not for the other two).

    openssl ecparam -out signerkey.pem -name prime256v1 -genkey

    openssl req -new -x509 -key signerkey.pem -out signercert.pem -days 365

## Step 2:- Provision the module/Generate manifest file

*   The tool will automatically detect the type of ATECC608 chip connected to ESP module and perform its intended task which are as follows.

    * For `TrustCustom` type of ATECC608 chip first configure ATECC608 chip with its default configuration options.The tool will create a device cert by generating a private key on slot 0 of the module, passing the CSR to host, sign the CSR with signer cert generated in step above. To set validity of device cert please refer [device_cert_validity](README.md#set-validity-of-device-cert-for-trustcustom). save the device cert on the ATECC chip as well as on the host machine as `device_cert.pem`,it also saves the cert definitions in `output_files` folder for future use.

    * For `Trust & Go` and `TrustFlex` type of ATECC608 devices this script will generate the manifest file with the name of chip serial number.The manifest file will be signed with the signer cert generated above. The generated manifest file should be registered with the cloud to register the device certificate.

The command is as follows:

```
python secure_cert_mfg.py --signer-cert signercert.pem --signer-cert-private-key signerkey.pem --port /UART/COM/PORT
```
> Note: The names `signercert.pem` and `signerkey.pem` denote the name of the signer cert and key files respectively, you can replace them with `relative/path/to/you/signer/cert` and `key` respectively. The `UART/COM/PORT` represents the host machine COM port to which your ESP32-WROOM-32SE is connected.Please refer [check serial port](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/establish-serial-connection.html#check-port-on-windows) for obtaining the serial port connected to ESP.

If you do not provide `signer-cert` and `signer_key` in above command, `sample_signer_cert.pem` stored at `sample_certs` will be used.

---
### Provide I2C pin configuration (for modules other than ESP32-WROOM32-SE)
The I2C pins of the ESP32 to which ATECC608 chip is connected can be provided as a parameter to the python script.
The command is as follows:
```
python secure_cert_mfg.py --i2c-sda-pin /* SDA pin no */ --i2c-scl-pin /* SCL pin no */ /* + other options */
```
When no pin configurations are provided to the script, by default SDA=16, SCL=17 will be used which is the I2C configuration of ESP32-WROOM-32SE.

### Find type of ATECC608 chip connected to ESP32-WROOM32-SE.

The command is as follows:
```
python secure_cert_mfg.py --port /serial/port --type
```
It will print the type of ATECC608 chip connected to ESP32-WROOM-32SE on console.

### Set validity of device cert for TrustCustom
The validity (in years) of device certificate generated for `TrustCustom` chips from the time of generation of cert can be set with `--valid-for-years` option. Please refer the following command:
```
python secure_cert_mfg.py --port /serial/port --valid-for-years /Years
```

>Note: If `--valid-for-years` is not provided then default value for validity of certiticates will be used, which is 40 years.


