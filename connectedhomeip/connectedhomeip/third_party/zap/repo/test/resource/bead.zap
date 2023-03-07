{
  "featureLevel": 67,
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
      "value": "0x1049"
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
    },
    {
      "pathRelativity": "relativeToZap",
      "path": "custom-cluster/custom-bead-cluster.xml",
      "version": null,
      "type": "zcl-xml-standalone"
    }
  ],
  "endpointTypes": [
    {
      "name": "Anonymous Endpoint Type",
      "deviceTypeName": "Custom ZCL Device Type",
      "deviceTypeCode": 65535,
      "deviceTypeProfileId": 65535,
      "clusters": [
        {
          "name": "Custom Bead",
          "code": 64682,
          "mfgCode": 48813,
          "define": "BEAD_CLUSTER",
          "side": "client",
          "enabled": 1,
          "commands": [],
          "attributes": []
        },
        {
          "name": "Custom Bead",
          "code": 64682,
          "mfgCode": 48813,
          "define": "BEAD_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [],
          "attributes": []
        }
      ]
    }
  ],
  "endpoints": [
    {
      "endpointTypeName": "Anonymous Endpoint Type",
      "endpointTypeIndex": 0,
      "profileId": 263,
      "endpointId": 1,
      "networkId": 0,
      "endpointVersion": 1,
      "deviceIdentifier": 65535
    }
  ],
  "log": []
}