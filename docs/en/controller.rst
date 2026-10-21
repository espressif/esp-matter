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

Controller features
-------------------
The controller is the role of a node that has permissions to enable it to control one or more nodes. It has a complete chain of Node Operational Credentials (NOC) and acts as an administer or an operator in its Matter fabric. And the NodeId of the controller or CASE Authenticated Tag (CAT) of the controller's NOC should in the Access Control List of other end-devices with administer/operator privilege.

The controller should support the following features:

- Send Cluster Invoking commands to other nodes.
- Send Read Attribute/Event commands to other nodes.
- Send Write Attribute commands to other nodes
- Send Subscribe Attribute/Event commands to other nodes.
- Join Matter Groups and manage the Group Key Set.

Cluster Invoking commands
~~~~~~~~~~~~~~~~~~~~~~~~~
The ``invoke-cmd`` command sends cluster commands to end-devices. In C++, use
``invoke_command::interaction`` with a request and an explicit delegate pointer.
Pass ``nullptr`` to discard successful responses while retaining failure logs.
``OnResponse()`` receives the concrete command path, status, and optional response TLV;
``OnDone()`` reports terminal completion. See `Ownership and callbacks`_ for
ownership and error-handling requirements.

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

Read commands
~~~~~~~~~~~~~
Use ``read_command::interaction`` with attribute and/or event path spans.
Its delegate receives ``OnReportBegin()``, ``OnAttribute()``, ``OnEvent()``,
``OnReportEnd(err)``, and terminal ``OnDone(err)`` callbacks. A synchronous
``start()`` failure produces no callbacks.

Read attribute commands
^^^^^^^^^^^^^^^^^^^^^^^
The ``read-attr`` commands are used for sending the commands of reading attributes on end-devices.

- Send the read-attribute command:

  ::

    matter esp controller read-attr <node-id> <endpoint-ids> <cluster-ids> <attribute-ids>

.. note::

    - endpoint-ids can represent a single or multiple endpoints, e.g. '0' or '0,1'. And the same applies to cluster-ids, attribute-ids, and event-ids below.

Read event commands
^^^^^^^^^^^^^^^^^^^
The ``read-event`` commands are used for sending the commands of reading events on end-devices.

- Send the read-event command:

  ::

    matter esp controller read-event <node-id> <endpoint-ids> <cluster-ids> <event-ids>

Write attribute commands
~~~~~~~~~~~~~~~~~~~~~~~~
The ``write-attr`` command writes one whole attribute on an end-device.
In C++, use ``write_command::interaction`` with a concrete attribute path, value
encoder, and explicit delegate pointer. Pass ``nullptr`` to discard successful
responses while retaining failure logs. A delegate gets the path status in
``OnResponse()`` and terminal completion in ``OnDone(err)``.

^^^^^^^^^^^^^^^^

- Send the write-attribute command:

  ::

    matter esp controller write-attr <node-id> <endpoint-id> <cluster-id> <attribute-id> <attribute-value>

.. note::

    - ``attribute_value`` should utilize a JSON object string. And the format of this string is the same as the ``command_data`` in `cluster commands <./developing.html#cluster-control>`__. This JSON object should contain only one item that represents the attribute value.


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

To write multiple attributes, issue separate commands. For example, write the ACL
and Binding attributes in order with:

  ::

    matter esp controller write-attr <node_id> <endpoint_id1> 31 0 "{\"0:ARR-OBJ\":[{\"1:U8\": 5, \"2:U8\": 2, \"3:ARR-U64\": [112233], \"4:NULL\": null}, {\"1:U8\": 4, \"2:U8\": 3, \"3:ARR-U64\": [1], \"4:NULL\": null}]}"
    matter esp controller write-attr <node_id> <endpoint_id2> 30 0 "{\"0:ARR-OBJ\":[{\"1:U64\":1, \"3:U16\":1, \"4:U32\": 6}]}"

Wait for the first Write's completion before submitting the second if ordering
is required; console submission success alone does not indicate completion.

For attributes of type uint64_t or int64_t, if the absolute value is greater than (2^53), you should use string to represent number in JSON structure for precision

  ::

    matter esp controller write-attr <node_id> <endpoint_id> 42 0 "{\"0:ARR-OBJ\":[{\"1:U64\": \"9007199254740993\", \"2:U8\": 0}]}"

Subscribe commands
~~~~~~~~~~~~~~~~~~
Use ``subscribe_command::subscription`` for remote attribute and event reports.
Its delegate receives report callbacks and ``OnStateChanged(state, cause)`` for
connection, retry, and termination. Keep the subscription alive until stopped
or terminated, and perform cleanup outside its callbacks under the CHIP lock.

Subscribe attribute commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The ``sub-attr`` commands are used for sending the commands of subscribing attributes on end-devices.

- Send the subscribe-attribute command:

  ::

    matter esp controller sub-attr <node-id> <endpoint-ids> <cluster-ids> <attribute-ids> <min-interval> <max-interval>

Subscribe event commands
^^^^^^^^^^^^^^^^^^^^^^^^
The ``sub-event`` commands are used for sending the commands of subscribing events on end-devices.

- Send the subscribe-event command:

  ::

    matter esp controller sub-event <node-id> <endpoint-ids> <cluster-ids> <event-ids> <min-interval> <max-interval>

List and remove subscriptions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Use ``sub-list`` to display console-owned subscriptions, including entries that are
connecting or waiting to retry. Each entry shows its fabric, node, state, and
subscription ID when available.

::

    matter esp controller sub-list
    matter esp controller sub-remove <node-id> <subscription-id>
    matter esp controller sub-remove <node-id> all
    matter esp controller sub-remove all

Node-specific removal applies to the active fabric. The literal ``all`` must be
provided explicitly: missing arguments, invalid IDs, and extra arguments are
rejected without removing anything. Node and subscription IDs accept decimal or
``0x``-prefixed hexadecimal notation.

.. warning::

    ``sub-remove all`` removes subscriptions across all nodes and fabrics.
    Removal also affects subscriptions created outside the console, whereas
    ``sub-list`` displays only console-owned subscriptions.

Group settings commands
~~~~~~~~~~~~~~~~~~~~~~~
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

Commissioner features
---------------------
The commissioner is an enhanced controller that can perform commissioning which is the sequence of operations to bring a Node into a Fabric by assigning an Operational Node ID and Node Operational credentials.

The commissioner should support the additional features:

- Obtain the onboarding payload (QR code or manual code) and use it to starting commissioning.
- Verify the commissionee's Device Attestation Certificate (DAC) chain and Certificate Declaration (CD) during commissioning.
- Receive the Certificate Signing Request (CSR) and issue NOC for it during commissioning.

Pairing commands
~~~~~~~~~~~~~~~~
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

Attestation Verification
~~~~~~~~~~~~~~~~~~~~~~~~

Attestation Trust Storage
^^^^^^^^^^^^^^^^^^^^^^^^^
The commissioner offers four options for the Attestation Trust Storage which is used to store and utilize the PAA certificates for the Device Attestation verification. This feature is available when the ``Enable matter commissioner`` option is enabled in menuconfig. You can modify this setting in menuconfig ``Components`` -> ``ESP Matter Controller`` -> ``Attestation Trust Store``.

- ``Attestation Trust Store - Test``

  Use two hardcoded PAA certificates (Chip-Test-PAA-FFF1-Cert&Chip-Test-PAA-NoVID-Cert) in the firmware.

- ``Attestation Trust Store - SPIFFS``

  Read the PAA root certificates from the SPIFFS partition. Please refer to the `SPIFFS FileSystem <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/storage/spiffs.html>`__ to configure the SPIFFS partition and directory. After this, please configure the partition label in menuconfig ``Components`` -> ``ESP Matter Controller`` -> ``SPIFFS Attestation Trust Store Partition Label``.

- ``Attestation Trust Store - DCL``

  Read the PAA root certificates from the DCL(Distributed Compliance Ledger). You can choose to use `MainNet <https://webui.dcl.csa-iot.org/>`__ or `TestNet <https://testnet.iotledger.io/>`__ by calling ``chip::Credentials::dcl_attestation_trust_store::get_instance().SetDCLNetType()``.

- ``Attestation Trust Store - Custom``

  Read the PAA root certificates with the custom method. You should call ``chip::Credentials::set_custom_attestation_trust_store()`` before ``esp_matter::controller::matter_controller_client::setup_commissioner()`` to set the custom attestation trust store.

Revoked DAC Chain Check
^^^^^^^^^^^^^^^^^^^^^^^
The commissioner can check whether the DAC chain on the commissionee is revoked during commissioning. This feature is available when the ``Enable matter commissioner`` option is enabled in menuconfig. You can configure the DAC Revocation Delegate in menuconfig ``Components`` -> ``ESP Matter Controller`` -> ``Check for Revoked DAC Chain``.

- ``Revoked DAC Chain Check - None``
  None check for revoked DAC chain.

- ``Revoked DAC Chain Check - DCL Revocation Points``
  Check the Revoked DAC chain with the revocation points on DCL. The controller will fetch revocation points from DCL. You can choose to use `MainNet <https://webui.dcl.csa-iot.org/>`__ or `TestNet <https://testnet.iotledger.io/>`__ by calling ``chip::Credentials::dcl_revocation_point_da_revocation_delegate::get_instance().set_dcl_net_type()``.

- ``Revoked DAC Chain Check - Custom``
  Check the Revoked DAC chain with the custom method. You should call ``chip::Credentials::set_custom_dac_revocation_delegate()`` before ``esp_matter::controller::matter_controller_client::setup_commissioner()`` to set the custom DAC revocation delegate.

Certification Declaration Verification
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The commissoner can commission a device that is not certified and using test CD signed by `Test CD signing Cert <https://github.com/project-chip/connectedhomeip/blob/master/credentials/test/certification-declaration/Chip-Test-CD-Signing-Cert.pem>`__. This feature is available when the ``Enable matter commissioner`` option is enabled in menuconfig. You can modify this setting in menuconfig ``Components`` -> ``ESP Matter Controller`` -> ``Support Test Certification Declaration``. If the option is not enable, only certified device can be commissioned by the commissioner.

NOC Issuer
~~~~~~~~~~

In the `esp_matter commissioner <https://github.com/espressif/esp-matter/tree/main/examples/controller>`_ example, the commissioner offers two options to issue the NOC chains for itself and other operational nodes.

- ``Operational Credentials Issuer - Test``

  Generate Root CA Certificate (RCAC) and RCAC Private Key and issue a Commissioner NOC when setting up the commissioner, and then issue NOCs for other nodes during commissioning with the generated RCAC Certificate and Key.

- ``Operational Credentials Issuer - Custom``

  Obtain the NOC chains for the commissioner and other operational nodes with a custom issuer class. The NOC chains can be issued from the cloud with the custom issuer.

Production Considerations
-------------------------

Controller Production
~~~~~~~~~~~~~~~~~~~~~
The Matter Controller should always work with a commissioner which is typically a mobile application that assists with its setup and onboarding.

Access Control Privilege
^^^^^^^^^^^^^^^^^^^^^^^^

The controller should possess either Administrator or Operator privileges in order to access other nodes within the same Matter fabric. A common approach is to issue a NOC containing an Administrator/Operator CAT for the controller, and to initialize the Access Control List (ACL) of Matter end devices with the corresponding CATs.

Controller NOC
^^^^^^^^^^^^^^

To access other nodes within a Matter fabric, the controller must be part of the same fabric. Therefore, there must be a mechanism to deliver the NOC chain to the controller. Upon receiving the chain, the controller can add or update it in its Fabric Table.

Device List
^^^^^^^^^^^

The controller should be able to retrieve the list of devices within the same Matter fabric. This list should include the Node IDs and device type information of each node, enabling the controller to determine how to interact with and control the respective Matter end devices.

Commissioner Production
~~~~~~~~~~~~~~~~~~~~~~~

Onboarding Payload
^^^^^^^^^^^^^^^^^^

The Matter Commissioner should be able to get the QR Code or Manual Code of Matter end-device so that it can start commissioning that device.

Device Attestation Verification
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The Matter Commissioner should be able to process the following DA verification during commissioning:

- Verify that the DAC chain of the commissionee is issued by a trusted Product Attestation Authority (PAA) Certificate in the Connectivity Standards Alliance (CSA) ’s Distributed Compliance Ledger(DCL).
- Verify that the CD of the commissionee is issued by CSA.
- Verify that the DAC or PAI Certificate is not revoked in CSA's DCL.

NOC Issuer
^^^^^^^^^^

The Matter Commissioner should be able to install NOC chain on the commissionee during commissioning. The custom NOC issuer should be implemented so that the Matter Commissioner could generate or otherwise obtain NOC chain after receiving CSRResponse command from the commissionee. In production, the NOC issuer is typically a cloud service: the Commissioner retrieves the CSR from the commissionee and forwards it to the cloud service for signing. The local RCAC is intended for testing only and must not be used in production.

Access Control List Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The Matter Commissioner should configure the ACL on the Commissionee over PASE session to grant Administer/Operator privilege over CASE authentication type for all the controllers in the Matter fabric.

Client interactions
-------------------

``esp_matter`` provides the generic interaction APIs. Include
``esp_matter_client.h`` and use the ``invoke_command``, ``read_command``,
``write_command``, and ``subscribe_command`` namespaces under ``esp_matter::client``.
Keep each caller-owned operation and its delegate alive until completion or
``stop()``.

Targets and authorization
~~~~~~~~~~~~~~~~~~~~~~~~~

Use ``chip::ScopedNodeId(node_id, fabric_index)`` for unicast destinations. The
node must be a remote operational node, and the fabric must exist in the
initialized Interaction Model runtime. Self-targets are rejected. For group
Invoke and Write, use ``start(group_id, fabric_index, request, &delegate)``.

Apply application authorization policy before submitting a request. The remote
peer enforces Matter ACL checks.

Ownership and callbacks
~~~~~~~~~~~~~~~~~~~~~~~

``start()`` copies path spans and encodes values and command fields before
returning. A synchronous error rejects the request and schedules
no delegate callbacks.

Invoke and Write require an explicit delegate pointer. Pass ``nullptr`` to log
asynchronous failures and discard successful responses, or pass a delegate
address to receive callbacks. Read and Subscribe require a delegate. TLV readers
are borrowed only for the callback duration.

Starting another Invoke, Read, or Write releases completed state. An active
operation rejects restart with ``CHIP_ERROR_INCORRECT_STATE`` for Invoke or
``CHIP_ERROR_BUSY`` for Read and Write. ``stop()`` synchronously detaches the
delegate and prevents later callbacks. Repeated calls are safe, but stopping
cannot undo work already sent to the network.

Invoke, Read, and Write allow stop, destruction, and reuse in terminal
``OnDone()`` callbacks. Do not perform these actions in report, response, or
subscription callbacks. Subscription states are ``connecting``, ``active``,
``backoff``, and ``terminated``. Call ``retry_now()`` only during automatic
resubscription backoff.

Invoke and JSON fields
~~~~~~~~~~~~~~~~~~~~~~

An Invoke request with ``fields == nullptr`` encodes an empty command structure.
``client::json_encodable`` encodes exactly one tagged JSON value, whether a
scalar, an ``OBJ`` structure, or an ``ARR-*`` array. The encoder replaces the
outer JSON tag with the tag supplied by the interaction. Invoke fields must be
an ``OBJ`` value. Use ``{"0:OBJ":{}}`` for empty fields. The temporary TLV
buffer is limited to ``chip::kMaxAppMessageLen``. For example:

.. code-block:: cpp

    client::json_encodable fields(R"({"0:OBJ":{"0:U8":2}})");
    client::invoke_command::request_t request = {
        .endpoint_id = 1,
        .cluster_id = 6,
        .command_id = 1,
        .fields = &fields,
    };
    CHIP_ERROR err = invoke.start(target, request, &delegate);

The JSON text need only remain valid through ``start()``.
Group Invoke does not support timed or response-timeout options. Its
``OnDone(CHIP_NO_ERROR)`` callback means the Interaction Model accepted the
groupcast. It does not confirm that group members received it. Group Invoke
does not call ``OnResponse()``.

Read, Write, and Subscribe
~~~~~~~~~~~~~~~~~~~~~~~~~~

Write encoders emit the complete attribute value, including any structure or
array container. ``client::json_encodable`` accepts, for example, ``{"0:U16":42}``,
``{"0:OBJ":{"1:U16":42}}``, or ``{"0:ARR-U8":[1,2]}``.

Read and Subscribe requests take spans of Matter attribute and event paths and
copy them during ``start()``. Read supports wildcard paths and is fabric-filtered
by default. Subscribe uses CHIP automatic resubscription according to its request options.

Use ``write.start(group_id, fabric_index, request, &delegate)`` for Group Write.
Group Write omits endpoints and does not support data versions, timed writes, or
chunking. ``OnDone()`` reports dispatch after ``start()`` returns. There are
no path responses or member acknowledgments.

Write always uses a span, including the one-attribute case:

.. code-block:: cpp

    client::json_encodable value("{\"0:U8\":2}");
    client::write_command::attribute_write_t attribute = {
        .path = chip::app::ConcreteDataAttributePath(1, 6, 0x4003),
        .value = &value,
    };
    client::write_command::request_t request = {
        .attributes = chip::Span<const client::write_command::attribute_write_t>(&attribute, 1),
    };
    CHIP_ERROR err = write.start(target, request, &delegate);

Write prepares all paths and values before accepting the request, then sends
it through one ``WriteClient``. Complete lists use ReplaceAll and may use
Matter chunking for non-timed requests. A timed request that requires chunking
fails synchronously.

A multi-path Write is not atomic. Inspect every ``OnResponse(path, status)``.
Later paths may proceed after a recoverable path failure, and earlier successful
writes are not rolled back. ``OnDone()`` reports interaction completion, not
success for every path. Supply an explicit JSON value for each write, including
``{"0:NULL":null}`` for null.

Binding fan-out
~~~~~~~~~~~~~~~

Server applications use ``client::binding::dispatcher`` after Binding Manager
initialization. ``dispatch(local_endpoint, request, delegate)`` accepts native
Invoke, Read, Write and Subscribe requests selecting one concrete cluster.
Requests are copied before return; bindings supply the fabric and remote endpoint.
Read and Subscribe skip group bindings. Invoke and Write accept a nullable
delegate pointer; Read and Subscribe require a delegate reference.

Native callbacks identify each destination through ``source``. ``OnDone()`` is
per destination, not aggregate completion. Dispatches may overlap on one
dispatcher. ``stop()`` cancels all its pending dispatches and native operations
across local endpoints, detaches delegates and suppresses later callbacks.
Keep delegates alive until then. The next ``dispatch()`` or ``stop()`` releases
completed handles.

Removed bindings stop subscriptions; dispatched finite operations may finish.
Fabric removal cancels affected work. New bindings apply on the next dispatch.
SDK notifications may be replaced, CASE failures may produce no callback, and
native start errors are logged. Returned errors do not undo dispatched work.

Call methods in CHIP context or under the stack lock. ``OnDone()`` may dispatch,
stop or destroy the dispatcher. Other callbacks may not stop or destroy it;
``dispatch()`` returns ``CHIP_ERROR_INCORRECT_STATE`` during nonterminal native
client callbacks. Subscription state callbacks may occur during ``dispatch()``.

Controller console interactions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Controller commands retain the ``matter esp controller`` prefix. The supported
subscription commands are ``sub-attr``, ``sub-event``, ``sub-list``, and
``sub-remove``. Removal takes ``<node-id> <subscription-id>``, ``<node-id> all``,
or ``all``. Missing arguments never imply all subscriptions. ``sub-list`` shows
console-owned subscriptions; removal can affect other owners. Node-specific
removal uses the active fabric, while ``sub-remove all`` covers every fabric.
