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
      "name": "default",
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
          "enabled": 1,
          "commands": [],
          "attributes": [
            {
              "name": "cluster revision",
              "code": 65533,
              "mfgCode": null,
              "side": "client",
              "included": 1,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "1",
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
              "included": 1,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "1",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "application version",
              "code": 1,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "1",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "stack version",
              "code": 2,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": null,
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "0",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "hardware version",
              "code": 3,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "manufacturer name",
              "code": 4,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": null,
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "model identifier",
              "code": 5,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": null,
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "date code",
              "code": 6,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": null,
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "",
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
              "included": 1,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0",
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
              "included": 1,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "1",
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
          "enabled": 1,
          "commands": [],
          "attributes": []
        },
        {
          "name": "Device Temperature Configuration",
          "code": 2,
          "mfgCode": null,
          "define": "DEVICE_TEMP_CLUSTER",
          "side": "client",
          "enabled": 1,
          "commands": [],
          "attributes": []
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
          "name": "Identify",
          "code": 3,
          "mfgCode": null,
          "define": "IDENTIFY_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [],
          "attributes": []
        },
        {
          "name": "Time",
          "code": 10,
          "mfgCode": null,
          "define": "TIME_CLUSTER",
          "side": "client",
          "enabled": 0,
          "commands": [],
          "attributes": [
            {
              "name": "cluster revision",
              "code": 65533,
              "mfgCode": null,
              "side": "client",
              "included": 1,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "1",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Time",
          "code": 10,
          "mfgCode": null,
          "define": "TIME_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [],
          "attributes": [
            {
              "name": "time",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 1,
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
              "name": "time status",
              "code": 1,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0",
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
              "included": 1,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "1",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Price",
          "code": 1792,
          "mfgCode": null,
          "define": "PRICE_CLUSTER",
          "side": "client",
          "enabled": 0,
          "commands": [],
          "attributes": [
            {
              "name": "cluster revision",
              "code": 65533,
              "mfgCode": null,
              "side": "client",
              "included": 1,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "1",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Price",
          "code": 1792,
          "mfgCode": null,
          "define": "PRICE_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [],
          "attributes": [
            {
              "name": "cluster revision",
              "code": 65533,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": null,
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "1",
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