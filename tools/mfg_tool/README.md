# Manufacturing Partition Generator Utility

## Dependencies
* [CHIP Certificate Tool](https://github.com/project-chip/connectedhomeip/tree/master/src/tools/chip-cert),
chip-cert provides command line interface (CLI) utility used for generating and manipulating CHIP certificates and CHIP private keys.

* [SPAKE2P Parameters Tool](https://github.com/project-chip/connectedhomeip/tree/master/src/tools/spake2p),
spake2p tool provides command line interface (CLI) utility used for generating spake parameters (PIN code and verifier) for device manufacturing provisioning.

* [chip-tool](https://github.com/project-chip/connectedhomeip/tree/master/examples/chip-tool),
chip-tool supports generating onboarding QR code payload and manual pairing code.

* [Mass Manufacturing Utility](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/mass_mfg.html#manufacturing-utility),
mfg_gen.py to create factory NVS partition images.

### Install python dependencies
```
cd path/to/esp-matter/tools/mfg_tool
python3 -m pip install -r requirements.txt
```

### [Build and setup tools in Matter SDK](https://github.com/project-chip/connectedhomeip/blob/master/docs/guides/BUILDING.md#build-for-the-host-os-linux-or-macos)

#### Build chip-cert and spake2p
```
cd path/to/esp-matter/connectedhomeip/connectedhomeip
source scripts/activate.sh
gn gen out/host
ninja -C out/host
```
Above commands will generate spake2p and chip-cert at `esp-matter/connectedhomeip/connectedhomeip/out/host`.

#### Add the tools path to $PATH
```
export PATH="$PATH:path/to/esp-matter/connectedhomeip/connectedhomeip/out/host"
```

## Configure your app
Open the project configuration menu using - 
```
cd <your_app>
idf.py menuconfig
```
In the configuration menu, set the following additional configuration to use custom factory partition and different values for Data and Device Info Providers.

1. Enable **`ESP32 Factory Data Provider`** [Component config → CHIP Device Layer → Commissioning options → Use ESP32 Factory Data Provider]
   > Enable config option [`CONFIG_ENABLE_ESP32_FACTORY_DATA_PROVIDER`](https://github.com/project-chip/connectedhomeip/blob/master/config/esp32/components/chip/Kconfig#L645) to use ESP32 specific implementation of CommissionableDataProvider and DeviceAttestationCredentialsProvider.
2. Enable **`ESP32 Device Instance Info Provider`** [Component config → CHIP Device Layer → Commissioning options → Use ESP32 Device Instance Info Provider]
   > Enable config option [`ENABLE_ESP32_DEVICE_INSTANCE_INFO_PROVIDER`](https://github.com/project-chip/connectedhomeip/blob/master/config/esp32/components/chip/Kconfig#L655) to get device instance info from factory partition.
3. Enable **`Attestation - Factory`** [ Component config → ESP Matter → DAC Provider options → Attestation - Factory]
   > Enable config option `CONFIG_FACTORY_PARTITION_DAC_PROVIDER` to use DAC certificates from the factory partition during Attestation.
4. Set **`chip-factory namespace partition label`** [Component config → CHIP Device Layer → Matter Manufacturing Options → chip-factory namespace partition label]
   > Set config option [`CHIP_FACTORY_NAMESPACE_PARTITION_LABEL`](https://github.com/project-chip/connectedhomeip/blob/master/config/esp32/components/chip/Kconfig#L856) to choose the label of the partition to store key-values in the "chip-factory" namespace. The dafault chosen partition label is `nvs`.

### mfg_gen.py
`mfg_gen.py` is present at path `$IDF_PATH/tools/mass_mfg/mfg_gen.py`

## Output files and directory structure
```
out
└── fff1_8000
    ├── 11fe2c53-9a38-445c-b58f-2ff0554cd981
    │   ├── 11fe2c53-9a38-445c-b58f-2ff0554cd981-onb_codes.csv
    │   ├── 11fe2c53-9a38-445c-b58f-2ff0554cd981-partition.bin
    │   ├── 11fe2c53-9a38-445c-b58f-2ff0554cd981-qrcode.png
    │   └── internal
    │       ├── DAC_cert.der
    │       ├── DAC_cert.pem
    │       ├── DAC_key.pem
    │       ├── DAC_private_key.bin
    │       ├── DAC_public_key.bin
    │       ├── PAI_cert.der
    │       └── partition.csv
    ├── 14874525-30b5-4c66-a00e-30e4af5dfb20
    │   ├── 14874525-30b5-4c66-a00e-30e4af5dfb20-onb_codes.csv
    │   ├── 14874525-30b5-4c66-a00e-30e4af5dfb20-partition.bin
    │   ├── 14874525-30b5-4c66-a00e-30e4af5dfb20-qrcode.png
    │   └── internal
    │       ├── DAC_cert.der
    │       ├── DAC_cert.pem
    │       ├── DAC_key.pem
    │       ├── DAC_private_key.bin
    │       ├── DAC_public_key.bin
    │       ├── PAI_cert.der
    │       └── partition.csv
    └── staging
        ├── config.csv
        ├── master.csv
        ├── pai_cert.der
        └── pin_disc.csv
```

Tool generates following output files:
- Partition Binary : `<uuid>-partition.bin`
- Onboarding codes : `<uuid>-onb_codes.csv`
- QR Code image    : `<uuid>-qrcode.png`

Other intermediate files are stored in `internal/` directory:
- Partition CSV    : `partition.csv`
- PAI Certificate  : `PAI_cert.der`
- DAC Certificates : `DAC_cert.der`, `DAC_cert.pem`
- DAC Private Key  : `DAC_private_key.bin`
- DAC Public Key   : `DAC_public_key.bin`

Above files are stored at `out/<vid_pid>/<UUID>`. Each device is identified with an unique UUID.

Common intermediate files are stored at `out/<vid_pid>/staging`.

## Usage examples
`mfg_tool.py -h` lists the mandatory as well as optional arguments.

Below commands uses the test PAI signing certificate and key, test certificate declaration present in Matter SDK, Vendor ID: 0xFFF2, and Product ID: 0x8001.

### Generate a factory partition
```
./mfg_tool.py -cn "My bulb" -v 0xFFF2 -p 0x8001 --pai \
   -k path/to/esp-matter/connectedhomeip/connectedhomeip/credentials/test/attestation/Chip-Test-PAI-FFF2-8001-Key.pem \
   -c path/to/esp-matter/connectedhomeip/connectedhomeip/credentials/test/attestation/Chip-Test-PAI-FFF2-8001-Cert.pem \
   -cd path/to/esp-matter/connectedhomeip/connectedhomeip/credentials/test/certification-declaration/Chip-Test-CD-FFF2-8001.der
```

### Generate 5 factory partitions [Optional argument : `-n`]
```
./mfg_tool.py -n 5 -cn "My bulb" -v 0xFFF2 -p 0x8001 --pai \
   -k path/to/esp-matter/connectedhomeip/connectedhomeip/credentials/test/attestation/Chip-Test-PAI-FFF2-8001-Key.pem \
   -c path/to/esp-matter/connectedhomeip/connectedhomeip/credentials/test/attestation/Chip-Test-PAI-FFF2-8001-Cert.pem \
   -cd path/to/esp-matter/connectedhomeip/connectedhomeip/credentials/test/certification-declaration/Chip-Test-CD-FFF2-8001.der
```

### Generate factory partition using existing DAC certificate and private key [Optional arguments : `--dac-cert` and `--dac-key`]
```
./mfg_tool.py -cn "My Bulb" -v 0xFFF2 -p 0x8001 --pai \
    -c path/to/esp-matter/connectedhomeip/connectedhomeip/credentials/test/attestation/Chip-Test-PAI-FFF2-8001-Cert.pem \
    -cd path/to/esp-matter/connectedhomeip/connectedhomeip/credentials/test/certification-declaration/Chip-Test-CD-FFF2-8001.der \
    --dac-key DAC_key.pem --dac-cert DAC_cert.pem
```

### Generate factory partitions using existing Passcode, Discriminator, and rotating device ID [Optional arguments : `--passcode`, `--discriminator`, and `--rd-id-uid`]
```
./mfg_tool.py -cn "My bulb" -v 0xFFF2 -p 0x8001 --pai \
    -k path/to/esp-matter/connectedhomeip/connectedhomeip/credentials/test/attestation/Chip-Test-PAI-FFF2-8001-Key.pem \
    -c path/to/esp-matter/connectedhomeip/connectedhomeip/credentials/test/attestation/Chip-Test-PAI-FFF2-8001-Cert.pem \
    -cd path/to/esp-matter/connectedhomeip/connectedhomeip/credentials/test/certification-declaration/Chip-Test-CD-FFF2-8001.der \
    --passcode 20202021 --discriminator 3840  --enable-rotating-device-id --rd-id-uid d2f351f57bb9387445a5f92a601d1c14
```

* NOTE: Script generates only one factory partition if **DAC or Discriminator or Passcode or Rotating-Device-ID** is specified.

### Generate factory partitions with extra NVS key-values specified using csv and mcsv file [Optional arguments : `--csv` and `--mcsv`]
```
./mfg_tool.py -cn "My bulb" -v 0xFFF2 -p 0x8001 --pai \
   -k path/to/esp-matter/connectedhomeip/connectedhomeip/credentials/test/attestation/Chip-Test-PAI-FFF2-8001-Key.pem \
   -c path/to/esp-matter/connectedhomeip/connectedhomeip/credentials/test/attestation/Chip-Test-PAI-FFF2-8001-Cert.pem \
   -cd path/to/esp-matter/connectedhomeip/connectedhomeip/credentials/test/certification-declaration/Chip-Test-CD-FFF2-8001.der \
   --csv extra_nvs_key_config.csv --mcsv extra_nvs_key_value.csv
```
Above command will generate `n` number of partitions. Where `n` is the rows in the mcsv file.
Output binary contains all the chip specific key/value and key/values specified using `--csv` and `--mcsv` option.

### Generate factory partitions without device attestation certificates and keys
```
./mfg_tool.py -v 0xFFF2 -p 0x8001 \
              -cd path/to/esp-matter/connectedhomeip/connectedhomeip/credentials/test/certification-declaration/Chip-Test-CD-FFF2-8001.der
```

* NOTE: These factory partitions are only for firmwares with other ways to get the certificates and sign message with the private key.

## Flashing the manufacturing binary
Please note that `mfg_tool.py` only generates manufacturing binary images which need to be flashed onto device using `esptool.py`.


* Flashing a binary image to the device
```
esptool.py -p <serial_port> write_flash <address> path/to/<uuid>-partition.bin
```

* NOTE: First flash your app firmware and then followed by the custom partition binary on the device. Please flash the manufacturing binary at the corresponding address of the configured factory partition set by [`CHIP_FACTORY_NAMESPACE_PARTITION_LABEL`](https://github.com/project-chip/connectedhomeip/blob/master/config/esp32/components/chip/Kconfig#L856) which by default is `nvs`.

## Commissioning the device
You can commission the device by using either - 
1. The QR code for Matter commissioners is generated at `out/<vid_pid>/<uuid>/<uuid>-qrcode.png`. If QR code is not visible, paste the below link into the browser replacing `<qr_code>` with the **QR code string** (eg. `MT:Y.K9042C00KA0648G00` - this is also the default test QR code) and scan the QR code.
```
https://project-chip.github.io/connectedhomeip/qrcode.html?data=<qr_code>
```

2. Refer the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html#commissioning-and-control) for other methods using onboarding payload found at `out/<vid_pid>/<uuid>/<uuid>-onb_codes.csv`. This contains the `QR Code String, Manual Pairing Code, Passcode and Discriminator`.

## Encrypting NVS partition

Below are the steps for encrypting the application and factory partition but before proceeding further please READ THE DOCS FIRST. Documentation References:

- [Flash and NVS encryption](https://github.com/project-chip/connectedhomeip/blob/master/docs/guides/esp32/flash_nvs_encryption.md#flash-and-nvs-encryption)

Provide `-e` option along with other options to generate the encrypted NVS partition binary.

It will generate additional partition binary (`<uuid>-keys-partition.bin`) containing the key for decrypting encrypted partition.

- Flash the partition binary containing factory data, as NVS encryption works differently, please flash is without `--encrypt` option
```
esptool.py -p (PORT) write_flash (FACTORY_PARTITION_ADDR) path/to/factory_partition.bin
```

- Flash the partition binary containing encryption keys, these SHALL be flashed with `--encrypt` option
```
esptool.py -p (PORT) write_flash --encrypt (NVS_KEYS_PARTITION_ADDR) path/to/nvs_key_partition.bin
```
