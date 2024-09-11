# Light with RainMaker-WiFi-Provisioning

This example creates a Color Temperature Light device using the ESP Matter data model and is integrated with ESP-RainMaker and wifi_provisioning.

See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) for more information about building and flashing the firmware.

## 1. Prerequisites

- ESP32-S3-DevKitM / ESP32-C3-DevKitM
- chip-tool (For Matter commissioning)
- Android RainMaker APP of version 3.4.1 or iOS RainMaker APP of version 3.2.0 (For RainMaker Provisioning)
- [ESP-IDF](https://github.com/espressif/esp-idf) on tag v5.2.2
- [ESP RainMaker](https://github.com/espressif/esp-rainmaker/tree/fd781295) on commit fd78129500aa1a8b2eee9e0dcc0720d7b14cd3dc
- [ESP Secure Cert Manager](https://github.com/espressif/esp_secure_cert_mgr)

## 2. Additional Environment Setup

### 2.1 Claiming device certificates

This example will use [host driven claiming](https://rainmaker.espressif.com/docs/claiming/#host-driven-claiming) via the RainMaker CLI.

Make sure your device is connected to the host machine, login into the CLI and execute this:
```
$ cd $RMAKER_PATH/cli
$ ./rainmaker.py claim --matter <port>
```
The CLI will fetch the device certificates and flash them into the secure cert partition of your device. The certificates will be used for both the Matter device attestation verification and RainMaker MQTT connection.

### 2.2 Generating the factory nvs binary

The factory nvs (fctry partition) needs to be generated using the mfg_tool of esp-matter. It is released on pypi as [esp-matter-mfg-tool](https://pypi.org/project/esp-matter-mfg-tool) and can be installed by running `pip install esp-matter-mfg-tool`

```
$ export ESP_SECURE_CERT_PATH=/path/to/esp_secure_cert_mgr
$ esp-matter-mfg-tool -v 0x131B -p 0x2 -cd $RMAKER_PATH/examples/matter/mfg/cd_131B_0002.der --csv $RMAKER_PATH/examples/matter/mfg/keys.csv --mcsv $RMAKER_PATH/examples/matter/mfg/master.csv
```
This not only generates the factory nvs binary required for matter, but also embeds the RainMaker MQTT Host url into it via the master.csv file. Optionally, you can embed the MQTT host into the firmware itself by using idf.py menuconfig -> ESP RainMaker Config -> ESP_RMAKER_READ_MQTT_HOST_FROM_CONFIG and then skipping the --csv and --mcsv options to mfg_tool

The factory binary generated above should be flashed onto the fctry partition (default : 0x3e0000 for ESP32-S3 and ESP32-C3. Do check your partition table for exact address).

```
$ esptool.py write_flash 0x3e0000 out/131b_2/<node-id>/<node-id>-partition.bin
```

### 2.3 Build the example

Once the environment and required files are set up, we can now proceed to build and flash the example

```
$ idf.py set-target <target>
$ idf.py build
$ idf.py flash monitor
```

## 3. Post Commissioning Setup

See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html#commissioning-and-control) for the information about Matter commissioning with chip-tool. Note that you need to specific the PAA path to the `paa_cert` under the directory of this example when you use the chip-tool to commisioning the device.

```
$ ./chip-tool pairing ble-wifi <node-id> <ssid> <passcode> <setup-pincode> <discriminator> --paa-trust-store-path <PAA-path>
```

**Note**: The `setup-picode` and `discriminator` is generated with the factory partition binary. You can find it in the `out/131b_2/<node-id>/<node-id>-onb_codes.csv`.

## 4. External platform

This example uses [external platform](../common/secondary_ble_adv/).

In the external platform, `SetSecondaryXX()` APIs are added in the `BLEManagerImpl` class for setting up the secondary BLE advertisement and services. After you call these APIs of `BLEManagerImpl`, there will be an additional BLE advertisement and corresponding service for wifi_provisioning after you initialize the Matter stack.

There are also some WiFi stack initialization changes to avoid duplicated Wi-Fi stack initialization of wifi_provisioning and Matter stack.

## 5. RainMaker Provisioning

After you flash the example to the DevKit board, the console will print the QR code for RainMaker Provisioning. After you scan the QR code with the RainMaker Phone App, the RainMaker provisioning will be done and the device will be added to your RainMaker home.

## 6. Manufacturing Considerations

This step is only suggested for Privately deployed Production and not required for test set up.

### 6.1 RainMaker MQTT Host

Find your private deployment's mqtt hostname (if applicable) by sending a GET request at `https://<Rainmaker-API-endpoint>/mqtt_host`. You should replace the mqtt host in master.csv (As described in the section above) with this to generate the factory nvs binary.

### 6.2 Matter VID/PID

For production devices which may have a different matter vid and pid, please set the values of DEVICE_VENDOR_ID and DEVICE_PRODUCT_ID by using `idf.py menuconfig` -> `Component config` -> `CHIP Device Layer` -> `Device Identification Options`. These same should also be used in the `mfg_tool`.

**Note**: The CD used by `mfg_tool` should also be changed when VID/PID is changed. Please refer to [this](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/certification.html#certification-declaration) about how to generate a test CD file.

### 6.3 Matter DAC

For public RainMaker, some test DACs are provided via claiming. For private deployments, test DACs can be generated using `mfg_tool`.

```
export ESP_SECURE_CERT_PATH=/path/to/esp_secure_cert_mgr
esp-matter-mfg-tool -v <vendor-id> -p <product-id> --pai -k <pai-key> -c <pai-cert> -cd <cert-dclrn> --csv /path/to/keys.csv --mcsv /path/to/master.csv
```

Samples of keys.csv and master.csv can be found in $RMAKER_PATH/examples/matter/mfg/.

For testing, you can use the test VID, PID, PAI and CD as shown below.
```
$ esp-matter-mfg-tool --dac-in-secure-cert -v 0xFFF2 -p 0x8001 --pai -k $ESP_MATTER_PATH/connectedhomeip/connectedhomeip/credentials/test/attestation/Chip-Test-PAI-FFF2-8001-Key.pem -c $ESP_MATTER_PATH/connectedhomeip/connectedhomeip/credentials/test/attestation/Chip-Test-PAI-FFF2-8001-Cert.pem -cd $ESP_MATTER_PATH/connectedhomeip/connectedhomeip/credentials/test/certification-declaration/Chip-Test-CD-FFF2-8001.der --csv $RMAKER_PATH/examples/matter/mfg/keys.csv --mcsv $RMAKER_PATH/examples/matter/mfg/master.csv
```

Note the path where the files are generated after running the above command since it will be required later.

### 6.4 Configure your app

Open the project configuration menu using
```
idf.py menuconfig
```

In the configuration menu, set the following additional configuration to use custom factory partition and different values for Data and Device Info Providers.

1. Enable ESP32 Factory Data Provider [Component config → CHIP Device Layer → Commissioning options → Use ESP32 Factory Data Provider]

    Enable config option CONFIG_ENABLE_ESP32_FACTORY_DATA_PROVIDER to use ESP32 specific implementation of CommissionableDataProvider and DeviceAttestationCredentialsProvider.

2. Enable ESP32 Device Instance Info Provider [Component config → CHIP Device Layer → Commissioning options → Use ESP32 Device Instance Info Provider]

    Enable config option ENABLE_ESP32_DEVICE_INSTANCE_INFO_PROVIDER to get device instance info from factory partition.

3. Enable Attestation - Secure Cert [ Component config → ESP Matter → DAC Provider options → Attestation - Secure Cert]

    Enable config option CONFIG_FACTORY_PARTITION_DAC_PROVIDER to use DAC certificates from the secure_cert partition during Attestation.

4. Set chip-factory namespace partition label [Component config → CHIP Device Layer → Matter Manufacturing Options → chip-factory namespace partition label]

    Set config option CHIP_FACTORY_NAMESPACE_PARTITION_LABEL to choose the label of the partition to store key-values in the "chip-factory" namespace. The default chosen partition label is nvs, change it to fctry.


Connect your esp32 device to your computer. Enter the below command to flash certificates and factory partition

```
$ esptool.py write_flash 0xd000 /out/<vendor-id>_<product-id>/<node-id>/<node-id>_esp_secure_cert.bin 0x3e0000 ./out/<vendor-id>_<product-id>/<node-id>/<node-id>-partition.bin
```

The csv file generate at `/out/<vendor-id>_<product-id>/cn_dacs-<date>-<time>.csv` should be registered to your private RainMaker deployment (if applicable) using the steps mentioned here.

> **In production use case, the DACs will be pre-provisioned in the modules and a csv file will be provided by the Espressif factory directly. Optionally, even the fctry partitions can be pre programmed. If not, use the mfg_tool to generate these nvs binaries**
