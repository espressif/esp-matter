{
  "featureLevel": 62,
  "creator": "zap",
  "keyValuePairs": [
    {
      "key": "commandDiscovery",
      "value": "1"
    },
    {
      "key": "defaultResponsePolicy",
      "value": "always"
    },
    {
      "key": "manufacturerCodes",
      "value": "0x1002"
    }
  ],
  "package": [
    {
      "pathRelativity": "relativeToZap",
      "path": "../../zcl-builtin/silabs/zcl.json",
      "version": "ZCL Test Data",
      "type": "zcl-properties"
    },
    {
      "pathRelativity": "relativeToZap",
      "path": "../gen-template/zigbee/gen-templates.json",
      "version": "test-v1",
      "type": "gen-templates-json"
    }
  ],
  "endpointTypes": [
    {
      "name": "Saved endpoint type",
      "deviceTypeName": null,
      "deviceTypeCode": null,
      "deviceTypeProfileId": null,
      "clusters": [
        {
          "name": "Basic",
          "code": 0,
          "mfgCode": null,
          "define": "BASIC_CLUSTER",
          "side": "client",
          "enabled": 0,
          "commands": [
            {
              "name": "ResetToFactoryDefaults",
              "code": 0,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": [
            {
              "name": "cluster revision",
              "code": 65533,
              "mfgCode": null,
              "side": "client",
              "included": null,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x0001",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Basic",
          "code": 0,
          "mfgCode": null,
          "define": "BASIC_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [],
          "attributes": [
            {
              "name": "ZCL version",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": null,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x03",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "power source",
              "code": 7,
              "mfgCode": null,
              "side": "server",
              "included": null,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "cluster revision",
              "code": 65533,
              "mfgCode": null,
              "side": "server",
              "included": null,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x0001",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Power Configuration",
          "code": 1,
          "mfgCode": null,
          "define": "POWER_CONFIG_CLUSTER",
          "side": "client",
          "enabled": 1,
          "commands": [],
          "attributes": []
        },
        {
          "name": "Power Configuration",
          "code": 1,
          "mfgCode": null,
          "define": "POWER_CONFIG_CLUSTER",
          "side": "server",
          "enabled": 0,
          "commands": [],
          "attributes": [
            {
              "name": "mains voltage",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": null,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "mains frequency",
              "code": 1,
              "mfgCode": null,
              "side": "server",
              "included": null,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "mains alarm mask",
              "code": 16,
              "mfgCode": null,
              "side": "server",
              "included": null,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "mains voltage min threshold",
              "code": 17,
              "mfgCode": null,
              "side": "server",
              "included": null,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x0000",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "mains voltage max threshold",
              "code": 18,
              "mfgCode": null,
              "side": "server",
              "included": null,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0xFFFF",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Device Temperature Configuration",
          "code": 2,
          "mfgCode": null,
          "define": "DEVICE_TEMP_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [],
          "attributes": []
        },
        {
          "name": "Identify",
          "code": 3,
          "mfgCode": null,
          "define": "IDENTIFY_CLUSTER",
          "side": "client",
          "enabled": 1,
          "commands": [],
          "attributes": []
        },
        {
          "name": "Groups",
          "code": 4,
          "mfgCode": null,
          "define": "GROUPS_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [],
          "attributes": []
        },
        {
          "name": "Information",
          "code": 2304,
          "mfgCode": null,
          "define": "INFORMATION_CLUSTER",
          "side": "client",
          "enabled": 1,
          "commands": [
            {
              "name": "RequestInformation",
              "code": 0,
              "mfgCode": null,
              "source": "client",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "PushInformationResponse",
              "code": 1,
              "mfgCode": null,
              "source": "client",
              "incoming": 0,
              "outgoing": 1
            }
          ],
          "attributes": [
            {
              "name": "cluster revision",
              "code": 65533,
              "mfgCode": null,
              "side": "client",
              "included": null,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x0001",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Information",
          "code": 2304,
          "mfgCode": null,
          "define": "INFORMATION_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [
            {
              "name": "RequestInformationResponse",
              "code": 0,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "PushInformation",
              "code": 1,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
              "outgoing": 1
            }
          ],
          "attributes": [
            {
              "name": "node description",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": null,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "delivery enable",
              "code": 1,
              "mfgCode": null,
              "side": "server",
              "included": null,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "enable secure configuration",
              "code": 3,
              "mfgCode": null,
              "side": "server",
              "included": null,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "cluster revision",
              "code": 65533,
              "mfgCode": null,
              "side": "server",
              "included": null,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x0001",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Billing",
          "code": 2562,
          "mfgCode": null,
          "define": "BILLING_CLUSTER",
          "side": "client",
          "enabled": 1,
          "commands": [],
          "attributes": [
            {
              "name": "cluster revision",
              "code": 65533,
              "mfgCode": null,
              "side": "client",
              "included": null,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x0001",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Billing",
          "code": 2562,
          "mfgCode": null,
          "define": "BILLING_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [
            {
              "name": "CheckBillStatus",
              "code": 0,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "SendBillRecord",
              "code": 1,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
              "outgoing": 1
            }
          ],
          "attributes": [
            {
              "name": "user id",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": null,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "service id",
              "code": 16,
              "mfgCode": null,
              "side": "server",
              "included": null,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "service provider id",
              "code": 17,
              "mfgCode": null,
              "side": "server",
              "included": null,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "session interval",
              "code": 18,
              "mfgCode": null,
              "side": "server",
              "included": null,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "timestamp",
              "code": 32,
              "mfgCode": null,
              "side": "server",
              "included": null,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "cluster revision",
              "code": 65533,
              "mfgCode": null,
              "side": "server",
              "included": null,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x0001",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        }
      ]
    }
  ],
  "endpoints": []
}