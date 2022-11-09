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


## Flashing the manufacturing binary
Please note that `mfg_tool.py` only generates manufacturing binary images which need to be flashed onto device using `esptool.py`.

`esptool.py` is present at path `$IDF_PATH/components/esptool_py/esptool/esptool.py`

* Flashing a binary image to the device
```
esptool.py -p <serial_port> write_flash <address> path/to/<uuid>-partition.bin
```
