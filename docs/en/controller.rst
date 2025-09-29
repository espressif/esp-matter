Matter Controller
=================

This section introduces the ``esp_matter_controller`` component, which can be used to build Matter controller and commissioner on Espressif SoCs.

Several controller/commissioner examples are provided.

- `Basic Commissioner <https://github.com/espressif/esp-matter/tree/main/examples/controller>`__

This example implements a basic Matter commissioner which can be used for commissioning Matter commissionees to its Matter Fabric.

- `Controller with Server Instance <https://github.com/espressif/esp-rainmaker/tree/master/examples/matter/matter_controller>`__

This example implements a Matter controller in RainMaker Matter Fabric. It is also a Matter server so it is commissioned to RainMaker Matter Fabic via standard Matter commissioning flow. And it uses a custom Matter cluster to update its Node Operational Credentials (NOC) and obtain the device list of the Matter Fabric.

- `Client-only Controller <https://github.com/espressif/esp-rainmaker/tree/master/examples/matter/client_only_matter_controller>`__

This example implements a Matter controller in RainMaker Matter Fabric without a Matter server instance. It can be provisioned to a Wi-Fi network via `network_provisioning <https://github.com/espressif/idf-extra-components/tree/master/network_provisioning>`_. And it uses a custom RainMaker service to obtain Administer NOC and device list of the Matter Fabric.

Once you have flashed the controller example onto the device, you can use the `device console <./developing.html#device-console>`__ to commission the device and send commands to the end-device. All of the controller commands begin with the prefix ``matter esp controller``.

1 Controller features
----------------------
The controller is the role of a node that has permissions to enable it to control one or more nodes. It has a complete chain of Node Operational Credentials (NOC) and acts as an administer or an operator in its Matter fabric. And the NodeId of the controller or CASE Authenticated Tag (CAT) of the controller's NOC should in the Access Control List of other end-devices with administer/operator privilege.

The controller should support the following features:

- Send Cluster Invoking commands to other nodes.
- Send Read Attribute/Event commands to other nodes.
- Send Write Attribute commands to other nodes
- Send Subscribe Attribute/Event commands to other nodes.
- Join Matter Groups and manage the Group Key Set.

1.1 Cluster Invoking commands
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The ``invoke-cmd`` command is used for sending cluster commands to the end-devices. It utilizes a ``cluster_command`` class to establish the sessions and send the command packets. The class constructor function could accept two callback inputs:

- **Success callback**:
  This callback will be called upon the reception of the success response. It could be used to handle the response data for the command that requires a reponse. Now the default success callback will print the response data for GroupKeyManagement, Groups, Scenes, Thermostat, and DoorLock clusters. If you want to handle the response data in your example, you can register your success callback when creating the ``cluster_command`` object.

- **Error callback**:
  This callback will be called upon the reception of the failure response or reponse timeout.

^^^^^^^^^^^^^^^^

- Send the cluster command:

  ::

    matter esp controller invoke-cmd <node-id | group-id> <endpoint-id> <cluster-id> <command-id> <command-data>

.. note::

    - The ``command-data`` should utilize a JSON object string and the name of each item in this object should be ``\"<TagNumber>:<DataType>\"`` or ``\"<TagName>:<TagNumber>:<DataType>\"``. The TagNumber should be the same as the command parameter ID in Matter SPEC and the supported DataTypes are listed in ``$ESP_MATTER_PATH/components/esp_matter/utils/json_to_tlv.h``

    - For the DataType ``bytes``, the value should be a Base64-Encoded string.


Here are some examples of the ``command-data`` format.

- For MoveToLevel command in LevelControl cluster, the ``command-data`` (``{"level": 10, "transitionTime": 0, "optionsMask": 0, "optionsOverride": 0}``) should be:

  ::

    matter esp controller invoke-cmd <node-id> <endpoint-id> 8 0 "{\"0:U8\": 10, \"1:U16\": 0, \"2:U8\": 0, \"3:U8\": 0}"

- For KeySetWrite command in GroupKeyManagement cluster, the ``command-data`` (``{"groupKeySet":{"groupKeySetID": 42, "groupKeySecurityPolicy": 0, "epochKey0": d0d1d2d3d4d5d6d7d8d9dadbdcdddedf, "epochStartTime0": 2220000, "epochKey1": null, "epochStartTime1": null, "epochKey2": null, "epochStartTime2": null}}``) should be:

  ::

    matter esp controller invoke-cmd <node-id> <endpoint-id> 63 0 "{\"0:OBJ\": {\"0:U16\": 42, \"1:U8\": 0, \"2:BYT\": \"0NHS09TV1tfY2drb3N3e3w==\", \"3:U64\": 2220000, \"4:NULL\": null, \"5:NULL\": null, \"6:NULL\": null, \"7:NULL\": null}}"

- For AddGroup command in Groups cluster, the ``command-data`` (``{"groupID": 1, "groupName": "grp1"}``) should be:

  ::

    matter esp controller invoke-cmd <node-id> <endpoint-id> 0x4 0 "{\"0:U16\": 1, \"1:STR\": \"grp1\"}"

1.2 Read commands
~~~~~~~~~~~~~~~~~
The ``read_command`` class is used for sending read commands to other end-devices. Its constructor function could accept two callback inputs:

- **Attribute report callback**:
  This callback will be called upon the reception of the attribute report for read-attribute commands.

- **Event report callback**:
  This callback will be called upon the reception of the event report for read-event commands.

1.2.1 Read attribute commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The ``read-attr`` commands are used for sending the commands of reading attributes on end-devices.

- Send the read-attribute command:

  ::

    matter esp controller read-attr <node-id> <endpoint-ids> <cluster-ids> <attribute-ids>

.. note::

    - endpoint-ids can represent a single or multiple endpoints, e.g. '0' or '0,1'. And the same applies to cluster-ids, attribute-ids, and event-ids below.

1.2.2 Read event commands
^^^^^^^^^^^^^^^^^^^^^^^^^
The ``read-event`` commands are used for sending the commands of reading events on end-devices.

- Send the read-event command:

  ::

    matter esp controller read-event <node-id> <endpoint-ids> <cluster-ids> <event-ids>

1.3 Write attribute commands
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The ``write-attr`` command is used for sending the commands of writing attributes on the end-device.

- Send the write-attribute command:

  ::

    matter esp controller write-attr <node-id> <endpoint-id> <cluster-ids> <attribute-ids> <attribute-value>

.. note::

    - ``attribute_value`` should utilize a JSON object string. And the format of this string is the same as the ``command_data`` in `cluster commands <./developing.html#cluster-commands>`__. This JSON object should contain only one item that represents the attribute value.


Here are some examples of the ``attribute_value`` format.

For StartUpOnOff attribute of OnOff Cluster, you should use the following JSON structures as the ``attribute_value`` to represent the StartUpOnOff ``2`` and ``null``:

  ::

    matter esp controller write-attr <node_id> <endpoint_id> 6 0x4003 "{\"0:U8\": 2}"
    matter esp controller write-attr <node_id> <endpoint_id> 6 0x4003 "{\"0:NULL\": null}"

For Binding attribute of Binding cluster, you should use the following JSON structure as the ``attribute_value`` to represent the binding list ``[{"node":1, "endpoint":1, "cluster":6}]``:

  ::

    matter esp controller write-attr <node_id> <endpoint_id> 30 0 "{\"0:ARR-OBJ\":[{\"1:U64\":1, \"3:U16\":1, \"4:U32\": 6}]}"

For ACL attribute of AccessControl cluster, you should use the following JSON structure as the ``attribute_value`` to represent the AccessControlList ``[{"privilege": 5, "authMode": 2, "subjects": [112233], "targets": null}, {"privilege": 4, "authMode": 3, "subjects": [1], "targets": null}]``:

  ::

      matter esp controller write-attr <node_id> <endpoint_id> 31 0 "{\"0:ARR-OBJ\":[{\"1:U8\": 5, \"2:U8\": 2, \"3:ARR-U64\": [112233], \"4:NULL\": null}, {\"1:U8\": 4, \"2:U8\": 3, \"3:ARR-U64\": [1], \"4:NULL\": null}]}"

To write multiple attributes in one commands, the ``attribute_value`` should be a JSON array. For example, to write the ACL attribute and Binding attribute above, you should use the following JSON structure as the ``attribute_value``:

  ::

    matter esp controller write-attr <node_id> <endpoint_id1>,<endpoint_id2> 31,30 0,0 "[{\"0:ARR-OBJ\":[{\"1:U8\": 5, \"2:U8\": 2, \"3:ARR-U64\": [112233], \"4:NULL\": null}, {\"1:U8\": 4, \"2:U8\": 3, \"3:ARR-U64\": [1], \"4:NULL\": null}]}, {\"0:ARR-OBJ\":[{\"1:U64\":1, \"3:U16\":1, \"4:U32\": 6}]}]"

For attributes of type uint64_t or int64_t, if the absolute value is greater than (2^53), you should use string to represent number in JSON structure for precision

  ::

    matter esp controller write-attr <node_id> <endpoint_id> 42 0 "{\"0:ARR-OBJ\":[{\"1:U64\": \"9007199254740993\", \"2:U8\": 0}]}"

1.4 Subscribe commands
~~~~~~~~~~~~~~~~~~~~~~
The ``subscribe_command`` class is used for sending subscribe commands to other end-devices. Its constructor function could accept four callback
inputs:

- **Attribute report callback**:
  This callback will be invoked upon the reception of the attribute report for subscribe-attribute commands.

- **Event report callback**:
  This callback will be invoked upon the reception of the event report for subscribe-event commands.

- **Subscribe done callback**:
  This callback will be invoked when the subscription is terminated or shutdown.

- **Subscribe failure callback**:
  This callback will be invoked upon the failure of establishing CASE session.

1.4.1 Subscribe attribute commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The ``subs-attr`` commands are used for sending the commands of subscribing attributes on end-devices.

- Send the subscribe-attribute command:

  ::

    matter esp controller subs-attr <node-id> <endpoint-ids> <cluster-ids> <attribute-ids> <min-interval> <max-interval>

1.4.2 Subscribe event commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The ``subs-event`` commands are used for sending the commands of subscribing events on end-devices.

- Send the subscribe-event command:

  ::

    matter esp controller subs-event <node-id> <endpoint-ids> <cluster-ids> <event-ids> <min-interval> <max-interval>

1.5 Group settings commands
~~~~~~~~~~~~~~~~~~~~~~~~~~~
The ``group-settings`` commands are used to set group information of the controller. If the controller wants to send multicast commands to end-devices, it should be in the same group as the end-devices.

- Set group information of the controller:

  ::

    matter esp controller group-settings show-groups
    matter esp controller group-settings add-group <group-id> <group-name>
    matter esp controller group-settings remove-group <group-id>
    matter esp controller group-settings show-keysets
    matter esp controller group-settings add-keyset <ketset-id> <policy> <validity-time> <epoch-key-oct-str>
    matter esp controller group-settings remove-keyset <ketset-id>
    matter esp controller group-settings bind-keyset <group-id> <ketset-id>
    matter esp controller group-settings unbind-keyset <group-id> <ketset-id>

2 Commissioner features
-----------------------
The commissioner is an enhanced controller that can perform commissioning which is the sequence of operations to bring a Node into a Fabric by assigning an Operational Node ID and Node Operational credentials.

The commissioner should support the additional features:

- Obtain the onboarding payload (QR code or manual code) and use it to starting commissioning.
- Verify the commissionee's Device Attestation Certificate (DAC) chain and Certificate Declaration (CD) during commissioning.
- Receive the Certificate Signing Request (CSR) and issue NOC for it during commissioning.

2.1 Pairing commands
~~~~~~~~~~~~~~~~~~~~
The ``pairing`` commands are used for commissioning end-devices and are available when the ``Enable matter commissioner`` option is enabled. Here are three standard pairing methods:

- **Onnetwork pairing:** Prior to executing this commissioning method, it is necessary to connect both the controller and the end-device to the same network and ensure that the commissioning window of the end-device is open. To complete this process, you can use the command ``matter esp wifi connect``. After the devices are connected, the pairing process can be initiated.

  ::

    matter esp wifi connect <ssid> <password>
    matter esp controller pairing onnetwork <node_id> <setup_passcode>

- **Ble-wifi pairing:** This pairing method is supported for ESP32S3. Before you execute this commissioning method, connect the controller to the Wi-Fi network and ensure that the end-device is in commissioning mode. You can use the command ``matter esp wifi connect`` to connect the controller to your wifi network. Then we can start the pairing.

  ::

    matter esp wifi connect <ssid> <password>
    matter esp controller pairing ble-wifi <node_id> <ssid> <password> <pincode> <discriminator>

- **Ble-thread pairing:** This pairing method is supported for ESP32S3. Before you execute this commissioning method, connect the controller to the Wi-Fi network in which there is a Thread Border Router (BR). And please ensure that the end-device is in commissioning mode. You can use the command ``matter esp wifi connect`` to connect the controller to your Wi-Fi network. Get the dataset tlvs of the Thread network of the Thread BR. Then we can start the pairing.

  ::

    matter esp wifi connect <ssid> <password>
    matter esp controller pairing ble-thread <node_id> <dataset_tlvs> <pincode> <discriminator>

- **Matter payload based pairing:** This method is similar to the previously mentioned pairing methods, but instead of accepting a PIN code and discriminator, it uses a Matter setup payload as input. The setup payload is parsed to extract the necessary information, which then initiates the pairing process.

For the ``code`` pairing method, commissioner tries to discover the end-device only on the IP network. However, when using ``code-wifi``, ``code-thread``, or ``code-wifi-thread``, and if ``CONFIG_ENABLE_ESP32_BLE_CONTROLLER`` is enabled, controller tries to discover the end-device on both the IP and BLE networks.

Below are supported commands:

  ::

    matter esp controller pairing code <node_id> <setup_payload>

  ::

    matter esp controller pairing code-wifi <node_id> <ssid> <passphrase> <setup_payload>

  ::

    matter esp controller pairing code-thread <node_id> <operationalDataset> <setup_payload>

  ::

    matter esp controller pairing code-wifi-thread <node_id> <ssid> <passphrase> <operationalDataset> <setup_payload>

2.2 Attestation Verification
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

2.2.1 Attestation Trust Storage
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The commissioner offers four options for the Attestation Trust Storage which is used to store and utilize the PAA certificates for the Device Attestation verification. This feature is available when the ``Enable matter commissioner`` option is enabled in menuconfig. You can modify this setting in menuconfig ``Components`` -> ``ESP Matter Controller`` -> ``Attestation Trust Store``.

- ``Attestation Trust Store - Test``

  Use two hardcoded PAA certificates (Chip-Test-PAA-FFF1-Cert&Chip-Test-PAA-NoVID-Cert) in the firmware.

- ``Attestation Trust Store - Spiffs``

  Read the PAA root certificates from the spiffs partition. The PAA der files should be placed in ``paa_cert`` directory so that they can be flashed into the spiffs partition of the controller.

- ``Attestation Trust Store - DCL``

  Fetch the PAA root certificates from the DCL MainNet/TestNet. The commissioner will fetch PAA certificates from DCL during commissioning and use the fetched PAA certificates to verifying the DAC chains of commissioned end-devices.

- ``Attestation Trust Store - Custom``

  Use the custom Attestation Trust Storage. You should call ``set_custom_attestation_trust_store()`` to set the custom Attestation Trust Store before setting up the commissioner.

2.3 NOC Issuer
~~~~~~~~~~~~~~

In the `esp_matter commissioner <https://github.com/espressif/esp-matter/tree/main/examples/controller>`_ example, the commissioner offers two options to issue the NOC chains for itself and other operational nodes.

- ``Operational Credentials Issuer - Test``

  Generate Root CA Certificate (RCAC) and RCAC Private Key and issue a Commissioner NOC when setting up the commissioner, and then issue NOCs for other nodes during commissioning with the generated RCAC Certificate and Key.

- ``Operational Credentials Issuer - Custom``

  Obtain the NOC chains for the commissioner and other operational nodes with a custom issuer class. The NOC chains can be issued from the cloud with the custom issuer.

3 Production Considerations
---------------------------

3.1 Controller Production
~~~~~~~~~~~~~~~~~~~~~~~~~
The Matter Controller should always work with a commissioner which is typically a mobile application that assists with its setup and onboarding.

3.1.1 Access Control Privilege
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The controller should possess either Administrator or Operator privileges in order to access other nodes within the same Matter fabric. A common approach is to issue a NOC containing an Administrator/Operator CAT for the controller, and to initialize the Access Control List (ACL) of Matter end devices with the corresponding CATs.

3.1.2 Controller NOC
^^^^^^^^^^^^^^^^^^^^

To access other nodes within a Matter fabric, the controller must be part of the same fabric. Therefore, there must be a mechanism to deliver the NOC chain to the controller. Upon receiving the chain, the controller can add or update it in its Fabric Table.

3.1.3 Device List
^^^^^^^^^^^^^^^^^

The controller should be able to retrieve the list of devices within the same Matter fabric. This list should include the Node IDs and device type information of each node, enabling the controller to determine how to interact with and control the respective Matter end devices.

3.2 Commissioner Production
~~~~~~~~~~~~~~~~~~~~~~~~~~~

3.2.1 Onboarding Payload
^^^^^^^^^^^^^^^^^^^^^^^^

The Matter Commissioner should be able to get the QR Code or Manual Code of Matter end-device so that it can start commissioning that device.

3.2.2 Device Attestation Verification
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The Matter Commissioner should be able to process the following DA verification during commissioning:

- Verify that the DAC chain of the commissionee is issued by a trusted Product Attestation Authority (PAA) Certificate in the Connectivity Standards Alliance (CSA) ’s Distributed Compliance Ledger(DCL).
- Verify that the CD of the commissionee is issued by CSA.
- Verify that the DAC or PAI Certificate is not revoked in CSA's DCL.

3.2.3 NOC Issuer
^^^^^^^^^^^^^^^^

The Matter Commissioner should be able to install NOC chain on the commissionee during commissioning. The custom NOC issuer should be implemented so that the Matter Commissioner could generate or otherwise obtain NOC chain after receiving CSRResponse command from the commissionee. In production, the NOC issuer is typically a cloud service: the Commissioner retrieves the CSR from the commissionee and forwards it to the cloud service for signing. The local RCAC is intended for testing only and must not be used in production.

3.2.4 Access Control List Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The Matter Commissioner should configure the ACL on the Commissionee over PASE session to grant Administer/Operator privilege over CASE authentication type for all the controllers in the Matter fabric.
