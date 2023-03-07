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
      "name": "Endpoint Type A",
      "deviceTypeName": "TA-billingunit",
      "deviceTypeCode": 515,
      "deviceTypeProfileId": 263,
      "clusters": [
        {
          "name": "Basic",
          "code": 0,
          "mfgCode": null,
          "define": "BASIC_CLUSTER",
          "side": "client",
          "enabled": 0,
          "commands": [],
          "attributes": []
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
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x08",
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
              "storageOption": "NVM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "Test manufacturer",
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
              "storageOption": "NVM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "Test model identifier",
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
              "storageOption": "NVM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "Test date code",
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
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
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
              "name": "battery percentage remaining",
              "code": 33,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "battery alarm state",
              "code": 62,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00000000",
              "reportable": 1,
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
          "enabled": 0,
          "commands": [],
          "attributes": [
            {
              "name": "current temperature",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Groups",
          "code": 4,
          "mfgCode": null,
          "define": "GROUPS_CLUSTER",
          "side": "client",
          "enabled": 1,
          "commands": [
            {
              "name": "AddGroup",
              "code": 0,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "ViewGroup",
              "code": 1,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "GetGroupMembership",
              "code": 2,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "RemoveGroup",
              "code": 3,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "RemoveAllGroups",
              "code": 4,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "AddGroupIfIdentifying",
              "code": 5,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": []
        },
        {
          "name": "Groups",
          "code": 4,
          "mfgCode": null,
          "define": "GROUPS_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [
            {
              "name": "AddGroupResponse",
              "code": 0,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "ViewGroupResponse",
              "code": 1,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "GetGroupMembershipResponse",
              "code": 2,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "RemoveGroupResponse",
              "code": 3,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": [
            {
              "name": "name support",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Scenes",
          "code": 5,
          "mfgCode": null,
          "define": "SCENES_CLUSTER",
          "side": "client",
          "enabled": 1,
          "commands": [
            {
              "name": "AddScene",
              "code": 0,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "ViewScene",
              "code": 1,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "RemoveScene",
              "code": 2,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "RemoveAllScenes",
              "code": 3,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "StoreScene",
              "code": 4,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "RecallScene",
              "code": 5,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "GetSceneMembership",
              "code": 6,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": []
        },
        {
          "name": "Scenes",
          "code": 5,
          "mfgCode": null,
          "define": "SCENES_CLUSTER",
          "side": "server",
          "enabled": 0,
          "commands": [
            {
              "name": "AddSceneResponse",
              "code": 0,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "ViewSceneResponse",
              "code": 1,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "RemoveSceneResponse",
              "code": 2,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "RemoveAllScenesResponse",
              "code": 3,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "StoreSceneResponse",
              "code": 4,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "GetSceneMembershipResponse",
              "code": 6,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": [
            {
              "name": "scene count",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "current scene",
              "code": 1,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "current group",
              "code": 2,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x0000",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "scene valid",
              "code": 3,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "name support",
              "code": 4,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "On/off",
          "code": 6,
          "mfgCode": null,
          "define": "ON_OFF_CLUSTER",
          "side": "client",
          "enabled": 0,
          "commands": [
            {
              "name": "Off",
              "code": 0,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "On",
              "code": 1,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "Toggle",
              "code": 2,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": []
        },
        {
          "name": "On/off",
          "code": 6,
          "mfgCode": null,
          "define": "ON_OFF_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [],
          "attributes": [
            {
              "name": "on/off",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "On/off Switch Configuration",
          "code": 7,
          "mfgCode": null,
          "define": "ON_OFF_SWITCH_CONFIG_CLUSTER",
          "side": "client",
          "enabled": 0,
          "commands": [],
          "attributes": []
        },
        {
          "name": "On/off Switch Configuration",
          "code": 7,
          "mfgCode": null,
          "define": "ON_OFF_SWITCH_CONFIG_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [],
          "attributes": [
            {
              "name": "switch type",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "switch actions",
              "code": 16,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
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
          "side": "client",
          "enabled": 1,
          "commands": [],
          "attributes": []
        },
        {
          "name": "Time",
          "code": 10,
          "mfgCode": null,
          "define": "TIME_CLUSTER",
          "side": "server",
          "enabled": 0,
          "commands": [],
          "attributes": [
            {
              "name": "time",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
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
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Binary Input (Basic)",
          "code": 15,
          "mfgCode": null,
          "define": "BINARY_INPUT_BASIC_CLUSTER",
          "side": "client",
          "enabled": 0,
          "commands": [],
          "attributes": []
        },
        {
          "name": "Binary Input (Basic)",
          "code": 15,
          "mfgCode": null,
          "define": "BINARY_INPUT_BASIC_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [],
          "attributes": [
            {
              "name": "out of service",
              "code": 81,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "present value",
              "code": 85,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "status flags",
              "code": 111,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Green Power",
          "code": 33,
          "mfgCode": null,
          "define": "GREEN_POWER_CLUSTER",
          "side": "client",
          "enabled": 0,
          "commands": [],
          "attributes": [
            {
              "name": "gpp max proxy table entries",
              "code": 16,
              "mfgCode": null,
              "side": "client",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x14",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "proxy table",
              "code": 17,
              "mfgCode": null,
              "side": "client",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "gpp functionality",
              "code": 22,
              "mfgCode": null,
              "side": "client",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x09AC2F",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "gpp active functionality",
              "code": 23,
              "mfgCode": null,
              "side": "client",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0xFFFFFF",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "gp link key",
              "code": 34,
              "mfgCode": null,
              "side": "client",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x5a6967426565416c6c69616e63653039",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Green Power",
          "code": 33,
          "mfgCode": null,
          "define": "GREEN_POWER_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [],
          "attributes": [
            {
              "name": "gps max sink table entries",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x05",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "sink table",
              "code": 1,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "27 bytes long default value",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "gps communication mode",
              "code": 2,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x01",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "gps commissioning exit mode",
              "code": 3,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x02",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "gps security level",
              "code": 5,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x01",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "gps functionality",
              "code": 6,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "gps active functionality",
              "code": 7,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0xFFFFFF",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "gp link key",
              "code": 34,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x5a6967426565416c6c69616e63653039",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Shade Configuration",
          "code": 256,
          "mfgCode": null,
          "define": "SHADE_CONFIG_CLUSTER",
          "side": "client",
          "enabled": 0,
          "commands": [],
          "attributes": []
        },
        {
          "name": "Shade Configuration",
          "code": 256,
          "mfgCode": null,
          "define": "SHADE_CONFIG_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [],
          "attributes": [
            {
              "name": "status",
              "code": 2,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "closed limit",
              "code": 16,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x0001",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "mode",
              "code": 17,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Door Lock",
          "code": 257,
          "mfgCode": null,
          "define": "DOOR_LOCK_CLUSTER",
          "side": "client",
          "enabled": 0,
          "commands": [
            {
              "name": "LockDoor",
              "code": 0,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "UnlockDoor",
              "code": 1,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": []
        },
        {
          "name": "Door Lock",
          "code": 257,
          "mfgCode": null,
          "define": "DOOR_LOCK_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [
            {
              "name": "LockDoorResponse",
              "code": 0,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "UnlockDoorResponse",
              "code": 1,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": [
            {
              "name": "lock state",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "lock type",
              "code": 1,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "actuator enabled",
              "code": 2,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "door state",
              "code": 3,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "enable logging",
              "code": 32,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "language",
              "code": 33,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "led settings",
              "code": 34,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "auto relock time",
              "code": 35,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "sound volume",
              "code": 36,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "operating mode",
              "code": 37,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "default configuration register",
              "code": 39,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "enable local programming",
              "code": 40,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x01",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "enable one touch locking",
              "code": 41,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "enable inside status led",
              "code": 42,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "enable privacy mode button",
              "code": 43,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "wrong code entry limit",
              "code": 48,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "user code temporary disable time",
              "code": 49,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "send pin over the air",
              "code": 50,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "require pin for rf operation",
              "code": 51,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "zigbee security level",
              "code": 52,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "alarm mask",
              "code": 64,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x0000",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "keypad operation event mask",
              "code": 65,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x0000",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "RF operation event mask",
              "code": 66,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x0000",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "manual operation event mask",
              "code": 67,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x0000",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "rfid operation event mask",
              "code": 68,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x0000",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "keypad programming event mask",
              "code": 69,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x0000",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "rf programming event mask",
              "code": 70,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x0000",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "rfid programming event mask",
              "code": 71,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x0000",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Color Control",
          "code": 768,
          "mfgCode": null,
          "define": "COLOR_CONTROL_CLUSTER",
          "side": "client",
          "enabled": 0,
          "commands": [
            {
              "name": "MoveToColor",
              "code": 7,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "MoveColor",
              "code": 8,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "StepColor",
              "code": 9,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": []
        },
        {
          "name": "Color Control",
          "code": 768,
          "mfgCode": null,
          "define": "COLOR_CONTROL_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [],
          "attributes": [
            {
              "name": "current hue",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "current saturation",
              "code": 1,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "current x",
              "code": 3,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x616B",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "current y",
              "code": 4,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x607D",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "color temperature",
              "code": 7,
              "mfgCode": null,
              "side": "server",
              "included": 0,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00FA",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "color control options",
              "code": 15,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "couple color temp to level min-mireds",
              "code": 16397,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "start up color temperature mireds",
              "code": 16400,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Ballast Configuration",
          "code": 769,
          "mfgCode": null,
          "define": "BALLAST_CONFIGURATION_CLUSTER",
          "side": "client",
          "enabled": 1,
          "commands": [],
          "attributes": []
        },
        {
          "name": "Ballast Configuration",
          "code": 769,
          "mfgCode": null,
          "define": "BALLAST_CONFIGURATION_CLUSTER",
          "side": "server",
          "enabled": 0,
          "commands": [],
          "attributes": [
            {
              "name": "ballast status",
              "code": 2,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "ISO 7816 Protocol Tunnel",
          "code": 1557,
          "mfgCode": null,
          "define": "ISO7816_PROTOCOL_TUNNEL_CLUSTER",
          "side": "client",
          "enabled": 1,
          "commands": [
            {
              "name": "InsertSmartCard",
              "code": 1,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "ExtractSmartCard",
              "code": 2,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": []
        },
        {
          "name": "ISO 7816 Protocol Tunnel",
          "code": 1557,
          "mfgCode": null,
          "define": "ISO7816_PROTOCOL_TUNNEL_CLUSTER",
          "side": "either",
          "enabled": 0,
          "commands": [
            {
              "name": "TransferApdu",
              "code": 0,
              "mfgCode": null,
              "source": "either",
              "incoming": 1,
              "outgoing": 0
            }
          ],
          "attributes": []
        },
        {
          "name": "ISO 7816 Protocol Tunnel",
          "code": 1557,
          "mfgCode": null,
          "define": "ISO7816_PROTOCOL_TUNNEL_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [],
          "attributes": [
            {
              "name": "status",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Key Establishment",
          "code": 2048,
          "mfgCode": null,
          "define": "KEY_ESTABLISHMENT_CLUSTER",
          "side": "client",
          "enabled": 0,
          "commands": [
            {
              "name": "InitiateKeyEstablishmentRequest",
              "code": 0,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "EphemeralDataRequest",
              "code": 1,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "ConfirmKeyDataRequest",
              "code": 2,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": [
            {
              "name": "key establishment suite (client)",
              "code": 0,
              "mfgCode": null,
              "side": "client",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x0000",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Key Establishment",
          "code": 2048,
          "mfgCode": null,
          "define": "KEY_ESTABLISHMENT_CLUSTER",
          "side": "either",
          "enabled": 0,
          "commands": [
            {
              "name": "TerminateKeyEstablishment",
              "code": 3,
              "mfgCode": null,
              "source": "either",
              "incoming": 1,
              "outgoing": 0
            }
          ],
          "attributes": []
        },
        {
          "name": "Key Establishment",
          "code": 2048,
          "mfgCode": null,
          "define": "KEY_ESTABLISHMENT_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [
            {
              "name": "InitiateKeyEstablishmentResponse",
              "code": 0,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "EphemeralDataResponse",
              "code": 1,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "ConfirmKeyDataResponse",
              "code": 2,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": [
            {
              "name": "key establishment suite (server)",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x0000",
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
          "side": "client",
          "enabled": 1,
          "commands": [
            {
              "name": "RequestInformation",
              "code": 0,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "PushInformationResponse",
              "code": 1,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": []
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
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "PushInformation",
              "code": 1,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": [
            {
              "name": "node description",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
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
              "included": 1,
              "storageOption": "RAM",
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
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Gaming",
          "code": 2306,
          "mfgCode": null,
          "define": "GAMING_CLUSTER",
          "side": "client",
          "enabled": 1,
          "commands": [
            {
              "name": "SearchGame",
              "code": 0,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "JoinGame",
              "code": 1,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "StartGame",
              "code": 2,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "PauseGame",
              "code": 3,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "ResumeGame",
              "code": 4,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "QuitGame",
              "code": 5,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "EndGame",
              "code": 6,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "StartOver",
              "code": 7,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "ActionControl",
              "code": 8,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": []
        },
        {
          "name": "Gaming",
          "code": 2306,
          "mfgCode": null,
          "define": "GAMING_CLUSTER",
          "side": "server",
          "enabled": 0,
          "commands": [
            {
              "name": "GameAnnouncement",
              "code": 0,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "GeneralResponse",
              "code": 1,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": [
            {
              "name": "player name",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "nb of games",
              "code": 1,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "list of games",
              "code": 2,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "announcement interval",
              "code": 3,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "game id",
              "code": 16,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "name of game",
              "code": 17,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "game master",
              "code": 18,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "status",
              "code": 19,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "current nb of players",
              "code": 20,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "list of current players",
              "code": 21,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "max nb of players",
              "code": 22,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "min nb of players",
              "code": 23,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "current game level",
              "code": 24,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "score of this player",
              "code": 25,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "timer1",
              "code": 26,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "timer2",
              "code": 27,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "timer3",
              "code": 28,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "counter1",
              "code": 29,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "counter2",
              "code": 30,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Voice over ZigBee",
          "code": 2308,
          "mfgCode": null,
          "define": "VOICE_OVER_ZIGBEE_CLUSTER",
          "side": "client",
          "enabled": 1,
          "commands": [
            {
              "name": "EstablishmentRequest",
              "code": 0,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "VoiceTransmission",
              "code": 1,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": []
        },
        {
          "name": "Voice over ZigBee",
          "code": 2308,
          "mfgCode": null,
          "define": "VOICE_OVER_ZIGBEE_CLUSTER",
          "side": "server",
          "enabled": 0,
          "commands": [
            {
              "name": "EstablishmentResponse",
              "code": 0,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "VoiceTransmissionResponse",
              "code": 1,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": [
            {
              "name": "codec type",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "sampling frequency",
              "code": 1,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "codec rate",
              "code": 2,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "establishment timeout",
              "code": 3,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
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
          "attributes": []
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
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "SendBillRecord",
              "code": 1,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": [
            {
              "name": "user id",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "Very long user id",
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
              "included": 1,
              "storageOption": "RAM",
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
              "included": 1,
              "storageOption": "RAM",
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
              "included": 1,
              "storageOption": "RAM",
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
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Meter Identification",
          "code": 2817,
          "mfgCode": null,
          "define": "METER_IDENTIFICATION_CLUSTER",
          "side": "client",
          "enabled": 1,
          "commands": [],
          "attributes": []
        },
        {
          "name": "Meter Identification",
          "code": 2817,
          "mfgCode": null,
          "define": "METER_IDENTIFICATION_CLUSTER",
          "side": "server",
          "enabled": 0,
          "commands": [],
          "attributes": [
            {
              "name": "company name",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "meter type id",
              "code": 1,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "data quality id",
              "code": 4,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "pod",
              "code": 12,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "available power",
              "code": 13,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "power threshold",
              "code": 14,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Electrical Measurement",
          "code": 2820,
          "mfgCode": null,
          "define": "ELECTRICAL_MEASUREMENT_CLUSTER",
          "side": "client",
          "enabled": 1,
          "commands": [],
          "attributes": []
        },
        {
          "name": "Electrical Measurement",
          "code": 2820,
          "mfgCode": null,
          "define": "ELECTRICAL_MEASUREMENT_CLUSTER",
          "side": "server",
          "enabled": 0,
          "commands": [],
          "attributes": []
        },
        {
          "name": "ZLL Commissioning",
          "code": 4096,
          "mfgCode": null,
          "define": "ZLL_COMMISSIONING_CLUSTER",
          "side": "client",
          "enabled": 1,
          "commands": [
            {
              "name": "ScanRequest",
              "code": 0,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "DeviceInformationRequest",
              "code": 2,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "IdentifyRequest",
              "code": 6,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "ResetToFactoryNewRequest",
              "code": 7,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "NetworkStartRequest",
              "code": 16,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "NetworkJoinRouterRequest",
              "code": 18,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "NetworkJoinEndDeviceRequest",
              "code": 20,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "NetworkUpdateRequest",
              "code": 22,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "GetGroupIdentifiersRequest",
              "code": 65,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "GetEndpointListRequest",
              "code": 66,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": []
        },
        {
          "name": "ZLL Commissioning",
          "code": 4096,
          "mfgCode": null,
          "define": "ZLL_COMMISSIONING_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [
            {
              "name": "ScanResponse",
              "code": 1,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "DeviceInformationResponse",
              "code": 3,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "NetworkStartResponse",
              "code": 17,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "NetworkJoinRouterResponse",
              "code": 19,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "NetworkJoinEndDeviceResponse",
              "code": 21,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "EndpointInformation",
              "code": 64,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "GetGroupIdentifiersResponse",
              "code": 65,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "GetEndpointListResponse",
              "code": 66,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": []
        }
      ]
    },
    {
      "name": "Endpoint Type C",
      "deviceTypeName": "HA-meterinterface",
      "deviceTypeCode": 83,
      "deviceTypeProfileId": 260,
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
              "incoming": 0,
              "outgoing": 0
            }
          ],
          "attributes": []
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
              "storageOption": "RAM",
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "6",
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
              "storageOption": "RAM",
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "42",
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
              "storageOption": "RAM",
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "5",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Identify",
          "code": 3,
          "mfgCode": null,
          "define": "IDENTIFY_CLUSTER",
          "side": "client",
          "enabled": 1,
          "commands": [
            {
              "name": "Identify",
              "code": 0,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "IdentifyQuery",
              "code": 1,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": []
        },
        {
          "name": "Identify",
          "code": 3,
          "mfgCode": null,
          "define": "IDENTIFY_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [
            {
              "name": "IdentifyQueryResponse",
              "code": 0,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": [
            {
              "name": "identify time",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x0000",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Demand Response and Load Control",
          "code": 1793,
          "mfgCode": null,
          "define": "DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER",
          "side": "client",
          "enabled": 1,
          "commands": [
            {
              "name": "ReportEventStatus",
              "code": 0,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "GetScheduledEvents",
              "code": 1,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            }
          ],
          "attributes": [
            {
              "name": "utility enrollment group",
              "code": 0,
              "mfgCode": null,
              "side": "client",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 1,
              "bounded": 1,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 10,
              "reportableChange": 0
            },
            {
              "name": "start randomization minutes",
              "code": 1,
              "mfgCode": null,
              "side": "client",
              "included": 1,
              "storageOption": "NVM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x1E",
              "reportable": 1,
              "minInterval": 1,
              "maxInterval": 11,
              "reportableChange": 2
            },
            {
              "name": "duration randomization minutes",
              "code": 2,
              "mfgCode": null,
              "side": "client",
              "included": 1,
              "storageOption": "NVM",
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 2,
              "maxInterval": 12,
              "reportableChange": 4
            },
            {
              "name": "device class value",
              "code": 3,
              "mfgCode": null,
              "side": "client",
              "included": 1,
              "storageOption": "External",
              "singleton": 0,
              "bounded": 1,
              "defaultValue": "0",
              "reportable": 1,
              "minInterval": 3,
              "maxInterval": 13,
              "reportableChange": 6
            }
          ]
        },
        {
          "name": "Demand Response and Load Control",
          "code": 1793,
          "mfgCode": null,
          "define": "DEMAND_RESPONSE_LOAD_CONTROL_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [
            {
              "name": "LoadControlEvent",
              "code": 0,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "CancelLoadControlEvent",
              "code": 1,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "CancelAllLoadControlEvents",
              "code": 2,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
              "outgoing": 1
            }
          ],
          "attributes": []
        },
        {
          "name": "Simple Metering",
          "code": 1794,
          "mfgCode": null,
          "define": "SIMPLE_METERING_CLUSTER",
          "side": "client",
          "enabled": 0,
          "commands": [],
          "attributes": []
        },
        {
          "name": "Simple Metering",
          "code": 1794,
          "mfgCode": null,
          "define": "SIMPLE_METERING_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [],
          "attributes": [
            {
              "name": "current summation delivered",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "status",
              "code": 512,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "unit of measure",
              "code": 768,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "summation formatting",
              "code": 771,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "metering device type",
              "code": 774,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Meter Identification",
          "code": 2817,
          "mfgCode": null,
          "define": "METER_IDENTIFICATION_CLUSTER",
          "side": "client",
          "enabled": 0,
          "commands": [],
          "attributes": []
        },
        {
          "name": "Meter Identification",
          "code": 2817,
          "mfgCode": null,
          "define": "METER_IDENTIFICATION_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [],
          "attributes": [
            {
              "name": "company name",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "meter type id",
              "code": 1,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "data quality id",
              "code": 4,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "pod",
              "code": 12,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "available power",
              "code": 13,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "power threshold",
              "code": 14,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        }
      ]
    },
    {
      "name": "Endpoint Type B",
      "deviceTypeName": "HA-smartplug",
      "deviceTypeCode": 81,
      "deviceTypeProfileId": 260,
      "clusters": [
        {
          "name": "Basic",
          "code": 0,
          "mfgCode": null,
          "define": "BASIC_CLUSTER",
          "side": "client",
          "enabled": 0,
          "commands": [],
          "attributes": []
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
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x08",
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
              "storageOption": "RAM",
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "42",
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
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
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
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
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
              "storageOption": "RAM",
              "singleton": 0,
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
              "storageOption": "RAM",
              "singleton": 0,
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
              "storageOption": "RAM",
              "singleton": 0,
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
              "storageOption": "RAM",
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Identify",
          "code": 3,
          "mfgCode": null,
          "define": "IDENTIFY_CLUSTER",
          "side": "client",
          "enabled": 1,
          "commands": [
            {
              "name": "Identify",
              "code": 0,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "IdentifyQuery",
              "code": 1,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": []
        },
        {
          "name": "Identify",
          "code": 3,
          "mfgCode": null,
          "define": "IDENTIFY_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [
            {
              "name": "IdentifyQueryResponse",
              "code": 0,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 1
            }
          ],
          "attributes": [
            {
              "name": "identify time",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x0000",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "On/off",
          "code": 6,
          "mfgCode": null,
          "define": "ON_OFF_CLUSTER",
          "side": "client",
          "enabled": 0,
          "commands": [
            {
              "name": "Off",
              "code": 0,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "On",
              "code": 1,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "Toggle",
              "code": 2,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            }
          ],
          "attributes": []
        },
        {
          "name": "On/off",
          "code": 6,
          "mfgCode": null,
          "define": "ON_OFF_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [],
          "attributes": [
            {
              "name": "on/off",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 1,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            }
          ]
        },
        {
          "name": "Simple Metering",
          "code": 1794,
          "mfgCode": null,
          "define": "SIMPLE_METERING_CLUSTER",
          "side": "client",
          "enabled": 0,
          "commands": [],
          "attributes": []
        },
        {
          "name": "Simple Metering",
          "code": 1794,
          "mfgCode": null,
          "define": "SIMPLE_METERING_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [],
          "attributes": [
            {
              "name": "current summation delivered",
              "code": 0,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "status",
              "code": 512,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "unit of measure",
              "code": 768,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "summation formatting",
              "code": 771,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "metering device type",
              "code": 774,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 0,
              "bounded": 0,
              "defaultValue": "",
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
  "endpoints": [
    {
      "endpointTypeName": "Endpoint Type A",
      "endpointTypeIndex": 0,
      "profileId": 263,
      "endpointId": 41,
      "networkId": 1,
      "endpointVersion": 1,
      "deviceIdentifier": null
    },
    {
      "endpointTypeName": "Endpoint Type C",
      "endpointTypeIndex": 1,
      "profileId": 260,
      "endpointId": 42,
      "networkId": 1,
      "endpointVersion": 2,
      "deviceIdentifier": null
    },
    {
      "endpointTypeName": "Endpoint Type B",
      "endpointTypeIndex": 2,
      "profileId": 260,
      "endpointId": 43,
      "networkId": 2,
      "endpointVersion": 3,
      "deviceIdentifier": null
    }
  ]
}