# Controller

This example creates a Matter OTA Provider using the ESP Matter data model.


See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) for more information about building and flashing the firmware.

## 1. Additional Environment Setup

No additional setup is required.

## 2. OTA Provider Example

To test this OTA Provider example, you need to upload your ota candidate to the DCL(Distributed Compliance Ledger) [TestNet](https://testnet.iotledger.io/models). This candidate should include an OTA image URL which can be used for downloading the OTA image.

For offical products, the ota candidate should be uploaded to the DCL [MainNet](https://dcl.iotledger.io/models).

The Matter OTA instruction can be found in [docs](https://github.com/project-chip/connectedhomeip/blob/master/docs/guides/esp32/ota.md).
