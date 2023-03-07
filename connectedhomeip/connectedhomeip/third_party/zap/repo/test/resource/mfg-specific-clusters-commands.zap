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
    }
  ],
  "endpointTypes": [
    {
      "name": "Anonymous Endpoint Type",
      "deviceTypeName": "LO-dimmablelight",
      "deviceTypeCode": 257,
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
          "attributes": [
            {
              "name": "cluster revision",
              "code": 65533,
              "mfgCode": null,
              "side": "client",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 1,
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
              "included": 1,
              "storageOption": "RAM",
              "singleton": 1,
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
              "defaultValue": "0x00",
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
              "singleton": 1,
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
              "singleton": 1,
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
              "storageOption": "RAM",
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
              "storageOption": "RAM",
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
              "storageOption": "RAM",
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "0x00",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "generic device class",
              "code": 8,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "0xFF",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "generic device type",
              "code": 9,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "0xFF",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "product code",
              "code": 10,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "product url",
              "code": 11,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "sw build id",
              "code": 16384,
              "mfgCode": null,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 1,
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
              "included": 1,
              "storageOption": "RAM",
              "singleton": 1,
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
          "name": "Identify",
          "code": 3,
          "mfgCode": null,
          "define": "IDENTIFY_CLUSTER",
          "side": "client",
          "enabled": 0,
          "commands": [
            {
              "name": "Identify",
              "code": 0,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "IdentifyQuery",
              "code": 1,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "TriggerEffect",
              "code": 64,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            }
          ],
          "attributes": [
            {
              "name": "cluster revision",
              "code": 65533,
              "mfgCode": null,
              "side": "client",
              "included": 1,
              "storageOption": "RAM",
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
              "incoming": 0,
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
            },
            {
              "name": "cluster revision",
              "code": 65533,
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
            }
          ]
        },
        {
          "name": "Groups",
          "code": 4,
          "mfgCode": null,
          "define": "GROUPS_CLUSTER",
          "side": "client",
          "enabled": 0,
          "commands": [
            {
              "name": "AddGroup",
              "code": 0,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "ViewGroup",
              "code": 1,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "GetGroupMembership",
              "code": 2,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "RemoveGroup",
              "code": 3,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "RemoveAllGroups",
              "code": 4,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "AddGroupIfIdentifying",
              "code": 5,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            }
          ],
          "attributes": [
            {
              "name": "cluster revision",
              "code": 65533,
              "mfgCode": null,
              "side": "client",
              "included": 1,
              "storageOption": "RAM",
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
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "ViewGroupResponse",
              "code": 1,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "GetGroupMembershipResponse",
              "code": 2,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "RemoveGroupResponse",
              "code": 3,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
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
            },
            {
              "name": "cluster revision",
              "code": 65533,
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
            }
          ]
        },
        {
          "name": "Scenes",
          "code": 5,
          "mfgCode": null,
          "define": "SCENES_CLUSTER",
          "side": "client",
          "enabled": 0,
          "commands": [
            {
              "name": "AddScene",
              "code": 0,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "ViewScene",
              "code": 1,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "RemoveScene",
              "code": 2,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "RemoveAllScenes",
              "code": 3,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "StoreScene",
              "code": 4,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "RecallScene",
              "code": 5,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "GetSceneMembership",
              "code": 6,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "EnhancedAddScene",
              "code": 64,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "EnhancedViewScene",
              "code": 65,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "CopyScene",
              "code": 66,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            }
          ],
          "attributes": [
            {
              "name": "cluster revision",
              "code": 65533,
              "mfgCode": null,
              "side": "client",
              "included": 1,
              "storageOption": "RAM",
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
          "name": "Scenes",
          "code": 5,
          "mfgCode": null,
          "define": "SCENES_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [
            {
              "name": "AddSceneResponse",
              "code": 0,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "ViewSceneResponse",
              "code": 1,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "RemoveSceneResponse",
              "code": 2,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "RemoveAllScenesResponse",
              "code": 3,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "StoreSceneResponse",
              "code": 4,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "GetSceneMembershipResponse",
              "code": 6,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
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
            },
            {
              "name": "cluster revision",
              "code": 65533,
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
            },
            {
              "name": "OffWithEffect",
              "code": 64,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "OnWithRecallGlobalScene",
              "code": 65,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "OnWithTimedOff",
              "code": 66,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            }
          ],
          "attributes": [
            {
              "name": "cluster revision",
              "code": 65533,
              "mfgCode": null,
              "side": "client",
              "included": 1,
              "storageOption": "RAM",
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
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "global scene control",
              "code": 16384,
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
              "name": "on time",
              "code": 16385,
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
              "name": "off wait time",
              "code": 16386,
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
              "name": "start up on off",
              "code": 16387,
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
              "name": "cluster revision",
              "code": 65533,
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
            }
          ]
        },
        {
          "name": "Level Control",
          "code": 8,
          "mfgCode": null,
          "define": "LEVEL_CONTROL_CLUSTER",
          "side": "client",
          "enabled": 0,
          "commands": [
            {
              "name": "MoveToLevel",
              "code": 0,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "Move",
              "code": 1,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "Step",
              "code": 2,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "Stop",
              "code": 3,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "MoveToLevelWithOnOff",
              "code": 4,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "MoveWithOnOff",
              "code": 5,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "StepWithOnOff",
              "code": 6,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "StopWithOnOff",
              "code": 7,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            }
          ],
          "attributes": [
            {
              "name": "cluster revision",
              "code": 65533,
              "mfgCode": null,
              "side": "client",
              "included": 1,
              "storageOption": "RAM",
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
          "name": "Level Control",
          "code": 8,
          "mfgCode": null,
          "define": "LEVEL_CONTROL_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [],
          "attributes": [
            {
              "name": "current level",
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
              "name": "remaining time",
              "code": 1,
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
              "name": "options",
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
              "name": "start up current level",
              "code": 16384,
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
              "name": "cluster revision",
              "code": 65533,
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
            }
          ]
        },
        {
          "name": "Over the Air Bootloading",
          "code": 25,
          "mfgCode": null,
          "define": "OTA_BOOTLOAD_CLUSTER",
          "side": "client",
          "enabled": 1,
          "commands": [
            {
              "name": "QueryNextImageRequest",
              "code": 1,
              "mfgCode": null,
              "source": "client",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "ImageBlockRequest",
              "code": 3,
              "mfgCode": null,
              "source": "client",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "UpgradeEndRequest",
              "code": 6,
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
              "included": 1,
              "storageOption": "RAM",
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
          "name": "Over the Air Bootloading",
          "code": 25,
          "mfgCode": null,
          "define": "OTA_BOOTLOAD_CLUSTER",
          "side": "server",
          "enabled": 0,
          "commands": [
            {
              "name": "ImageNotify",
              "code": 0,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "QueryNextImageResponse",
              "code": 2,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "ImageBlockResponse",
              "code": 5,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "UpgradeEndResponse",
              "code": 7,
              "mfgCode": null,
              "source": "server",
              "incoming": 1,
              "outgoing": 0
            }
          ],
          "attributes": [
            {
              "name": "cluster revision",
              "code": 65533,
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
            }
          ]
        },
        {
          "name": "Occupancy Sensing",
          "code": 1030,
          "mfgCode": null,
          "define": "OCCUPANCY_SENSING_CLUSTER",
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
              "storageOption": "RAM",
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
          "name": "Occupancy Sensing",
          "code": 1030,
          "mfgCode": null,
          "define": "OCCUPANCY_SENSING_CLUSTER",
          "side": "server",
          "enabled": 0,
          "commands": [],
          "attributes": [
            {
              "name": "cluster revision",
              "code": 65533,
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
            }
          ]
        },
        {
          "name": "ZLL Commissioning",
          "code": 4096,
          "mfgCode": null,
          "define": "ZLL_COMMISSIONING_CLUSTER",
          "side": "client",
          "enabled": 0,
          "commands": [
            {
              "name": "ScanRequest",
              "code": 0,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "DeviceInformationRequest",
              "code": 2,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "IdentifyRequest",
              "code": 6,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "ResetToFactoryNewRequest",
              "code": 7,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "NetworkStartRequest",
              "code": 16,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "NetworkJoinRouterRequest",
              "code": 18,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "NetworkJoinEndDeviceRequest",
              "code": 20,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "NetworkUpdateRequest",
              "code": 22,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "GetGroupIdentifiersRequest",
              "code": 65,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "GetEndpointListRequest",
              "code": 66,
              "mfgCode": null,
              "source": "client",
              "incoming": 1,
              "outgoing": 0
            }
          ],
          "attributes": [
            {
              "name": "cluster revision",
              "code": 65533,
              "mfgCode": null,
              "side": "client",
              "included": 1,
              "storageOption": "RAM",
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
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "DeviceInformationResponse",
              "code": 3,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "NetworkStartResponse",
              "code": 17,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "NetworkJoinRouterResponse",
              "code": 19,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "NetworkJoinEndDeviceResponse",
              "code": 21,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "EndpointInformation",
              "code": 64,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "GetGroupIdentifiersResponse",
              "code": 65,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "GetEndpointListResponse",
              "code": 66,
              "mfgCode": null,
              "source": "server",
              "incoming": 0,
              "outgoing": 1
            }
          ],
          "attributes": [
            {
              "name": "cluster revision",
              "code": 65533,
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
            }
          ]
        },
        {
          "name": "MFGLIB Cluster",
          "code": 64514,
          "mfgCode": 4098,
          "define": "MFGLIB_CLUSTER",
          "side": "client",
          "enabled": 1,
          "commands": [
            {
              "name": "stream",
              "code": 0,
              "mfgCode": 4098,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "tone",
              "code": 1,
              "mfgCode": 4098,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "rxMode",
              "code": 2,
              "mfgCode": 4098,
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
              "included": 1,
              "storageOption": "RAM",
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
          "name": "MFGLIB Cluster",
          "code": 64514,
          "mfgCode": 4098,
          "define": "MFGLIB_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [],
          "attributes": [
            {
              "name": "Number of packets received while in MFGLIB mode.",
              "code": 0,
              "mfgCode": 4098,
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
              "name": "RSSI of the first received packet.",
              "code": 1,
              "mfgCode": 4098,
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
              "name": "LQI of the first received packet.",
              "code": 2,
              "mfgCode": 4098,
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
              "name": "cluster revision",
              "code": 65533,
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
            }
          ]
        },
        {
          "name": "SL Works With All Hubs",
          "code": 64599,
          "mfgCode": 4631,
          "define": "SL_WWAH_CLUSTER",
          "side": "client",
          "enabled": 1,
          "commands": [
            {
              "name": "EnableApsLinkKeyAuthorization",
              "code": 0,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "DisableApsLinkKeyAuthorization",
              "code": 1,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "ApsLinkKeyAuthorizationQuery",
              "code": 2,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "RequestNewApsLinkKey",
              "code": 3,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "EnableWwahAppEventRetryAlgorithm",
              "code": 4,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "DisableWwahAppEventRetryAlgorithm",
              "code": 5,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "RequestTime",
              "code": 6,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "EnableWwahRejoinAlgorithm",
              "code": 7,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "DisableWwahRejoinAlgorithm",
              "code": 8,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "SetIasZoneEnrollmentMethod",
              "code": 9,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "ClearBindingTable",
              "code": 10,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "EnablePeriodicRouterCheckIns",
              "code": 11,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "DisablePeriodicRouterCheckIns",
              "code": 12,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "SetMacPollFailureWaitTime",
              "code": 13,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "SetPendingNetworkUpdate",
              "code": 14,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "RequireApsAcksOnUnicasts",
              "code": 15,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 1,
              "outgoing": 1
            },
            {
              "name": "RemoveApsAcksOnUnicastsRequirement",
              "code": 16,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "ApsAckRequirementQuery",
              "code": 17,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "DebugReportQuery",
              "code": 18,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "SurveyBeacons",
              "code": 19,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "DisableOtaDowngrades",
              "code": 20,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "DisableMgmtLeaveWithoutRejoin",
              "code": 21,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "DisableTouchlinkInterpanMessageSupport",
              "code": 22,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "EnableWwahParentClassification",
              "code": 23,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "DisableWwahParentClassification",
              "code": 24,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "EnableTcSecurityOnNtwkKeyRotation",
              "code": 25,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "EnableWwahBadParentRecovery",
              "code": 26,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "DisableWwahBadParentRecovery",
              "code": 27,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "EnableConfigurationMode",
              "code": 28,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "DisableConfigurationMode",
              "code": 29,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "UseTrustCenterForClusterServer",
              "code": 30,
              "mfgCode": 4631,
              "source": "client",
              "incoming": 0,
              "outgoing": 1
            },
            {
              "name": "TrustCenterForClusterServerQuery",
              "code": 31,
              "mfgCode": 4631,
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
              "included": 1,
              "storageOption": "RAM",
              "singleton": 1,
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
          "name": "SL Works With All Hubs",
          "code": 64599,
          "mfgCode": 4631,
          "define": "SL_WWAH_CLUSTER",
          "side": "server",
          "enabled": 1,
          "commands": [
            {
              "name": "ApsLinkKeyAuthorizationQueryResponse",
              "code": 0,
              "mfgCode": 4631,
              "source": "server",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "PoweringOffNotification",
              "code": 1,
              "mfgCode": 4631,
              "source": "server",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "PoweringOnNotification",
              "code": 2,
              "mfgCode": 4631,
              "source": "server",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "ShortAddressChange",
              "code": 3,
              "mfgCode": 4631,
              "source": "server",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "ApsAckEnablementQueryResponse",
              "code": 4,
              "mfgCode": 4631,
              "source": "server",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "PowerDescriptorChange",
              "code": 5,
              "mfgCode": 4631,
              "source": "server",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "NewDebugReportNotification",
              "code": 6,
              "mfgCode": 4631,
              "source": "server",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "DebugReportQueryResponse",
              "code": 7,
              "mfgCode": 4631,
              "source": "server",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "TrustCenterForClusterServerQueryResponse",
              "code": 8,
              "mfgCode": 4631,
              "source": "server",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "SurveyBeaconsResponse",
              "code": 9,
              "mfgCode": 4631,
              "source": "server",
              "incoming": 1,
              "outgoing": 0
            },
            {
              "name": "UseTrustCenterForClusterServerResponse",
              "code": 158,
              "mfgCode": 4631,
              "source": "server",
              "incoming": 1,
              "outgoing": 0
            }
          ],
          "attributes": [
            {
              "name": "disable ota downgrades",
              "code": 2,
              "mfgCode": 4631,
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
            },
            {
              "name": "mgmt leave without rejoin enabled",
              "code": 3,
              "mfgCode": 4631,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "0x01",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "network retry count",
              "code": 4,
              "mfgCode": 4631,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "0xFF",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "mac retry count",
              "code": 5,
              "mfgCode": 4631,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "0xFF",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "router checkin enabled",
              "code": 6,
              "mfgCode": 4631,
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
            },
            {
              "name": "touchlink interpan enabled",
              "code": 7,
              "mfgCode": 4631,
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
            },
            {
              "name": "wwah parent classification enabled",
              "code": 8,
              "mfgCode": 4631,
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
            },
            {
              "name": "wwah app event retry enabled",
              "code": 9,
              "mfgCode": 4631,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "0x01",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "wwah app event retry queue size",
              "code": 10,
              "mfgCode": 4631,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "0x0A",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "wwah rejoin enabled",
              "code": 11,
              "mfgCode": 4631,
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
            },
            {
              "name": "mac poll failure wait time",
              "code": 12,
              "mfgCode": 4631,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "0x03",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "configuration mode enabled",
              "code": 13,
              "mfgCode": 4631,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "0x01",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "current debug report id",
              "code": 14,
              "mfgCode": 4631,
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
            },
            {
              "name": "tc security on ntwk key rotation enabled",
              "code": 15,
              "mfgCode": 4631,
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
            },
            {
              "name": "wwah bad parent recovery enabled",
              "code": 16,
              "mfgCode": 4631,
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
            },
            {
              "name": "pending network update channel",
              "code": 17,
              "mfgCode": 4631,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "0xFF",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "pending network update pan id",
              "code": 18,
              "mfgCode": 4631,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "0xFFFF",
              "reportable": 0,
              "minInterval": 0,
              "maxInterval": 65534,
              "reportableChange": 0
            },
            {
              "name": "ota max offline duration",
              "code": 19,
              "mfgCode": 4631,
              "side": "server",
              "included": 1,
              "storageOption": "RAM",
              "singleton": 1,
              "bounded": 0,
              "defaultValue": "0x0000",
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
              "storageOption": "RAM",
              "singleton": 1,
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
  "endpoints": [
    {
      "endpointTypeName": "Anonymous Endpoint Type",
      "endpointTypeIndex": 0,
      "profileId": 260,
      "endpointId": 1,
      "networkId": 0,
      "endpointVersion": 1,
      "deviceIdentifier": 257
    }
  ]
}