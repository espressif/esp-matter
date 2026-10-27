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

The `device console <./developing.html#device-console>`__ uses these command prefixes:

- ``matter esp controller`` runs controller commands.
- ``matter esp group`` configures groups and keys.
- ``matter esp device`` manages devices in the controller example. See the example
  README for its commands.

Console modules
---------------

Include ``esp_matter_controller_console.h`` and register each module you need
once, before starting the shell. The first controller module registers the
command root.

.. code-block:: cpp

    esp_matter::controller::console::register_read_commands();
    esp_matter::controller::console::register_write_commands();
    esp_matter::controller::console::register_invoke_commands();
    esp_matter::controller::console::register_subscribe_commands();
    esp_matter::controller::console::register_groups_commands();
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_matter::console::init());

The Read module handles attributes and events. The Subscribe module creates,
lists, and removes subscriptions. Register window, pairing, or ICD commands
with ``register_window_commands()``, ``register_pairing_commands()``, or
``register_icd_commands()``.

Pairing requires ``CONFIG_ESP_MATTER_COMMISSIONER_ENABLE``. ICD and group-settings
registration require a build without Matter server support. Group settings use
the separate ``matter esp group`` root. Group Invoke and Write belong to the
Invoke and Write modules and work with or without a Matter server.

Registration can fail if the console has no command-set capacity left. Modules
use separate translation units and do not register themselves automatically.
The linker can remove unreferenced handlers, help strings, and state. Referenced
APIs and shared dependencies remain linked.

An application with its own pairing CLI can call ``pairing::interaction``
without registering the generic pairing console.

Console input
~~~~~~~~~~~~~

Only registered modules provide commands. Put options such as ``--timed-ms``
before ``--``, followed by one JSON argument. Write takes a tagged attribute
value. Invoke takes a command-fields object, or ``{}`` for no fields. Use shell
quoting and escape embedded double quotes and backslashes.

The ESP-IDF line editor filters non-ASCII input. Use JSON Unicode escapes such
as ``\uXXXX``, entered as ``\\uXXXX`` through shell escaping.
``CONFIG_CHIP_SHELL_CMD_LINE_BUF_MAX_LENGTH`` limits line length, and
``CONFIG_CHIP_SHELL_CMD_LINE_ARG_MAX_COUNT`` limits the argument count.

Controller features
-------------------
The controller is the role of a node that has permissions to enable it to control one or more nodes. It has a complete chain of Node Operational Credentials (NOC) and acts as an administer or an operator in its Matter fabric. And the NodeId of the controller or CASE Authenticated Tag (CAT) of the controller's NOC should in the Access Control List of other end-devices with administer/operator privilege.

The controller should support the following features:

- Send Cluster Invoking commands to other nodes.
- Send Read Attribute/Event commands to other nodes.
- Send Write Attribute commands to other nodes
- Send Subscribe Attribute/Event commands to other nodes.
- Join Matter Groups and manage the Group Key Set.
- Register and unregister ICD clients.

Cluster Invoking commands
~~~~~~~~~~~~~~~~~~~~~~~~~
Use ``invoke`` to send unicast or group cluster commands. In C++, use
``invoke_command::interaction`` with a request and an explicit delegate pointer.
Pass ``nullptr`` to discard successful responses while retaining failure logs.
``OnResponse()`` receives the concrete command path, status, and optional response TLV;
``OnDone()`` reports terminal completion. See `Ownership and callbacks`_ for
ownership and error-handling requirements.

^^^^^^^^^^^^^^^^

- Send the cluster command:

  ::

    matter esp controller invoke <node-id> <endpoint-id> <cluster-id> <command-id> [--timed-ms N] -- <command-data>
    matter esp controller invoke group <group-id> <cluster-id> <command-id> -- <command-data>

.. note::

    - The ``command-data`` should utilize a JSON object string and the name of each item in this object should be ``\"<TagNumber>:<DataType>\"`` or ``\"<TagName>:<TagNumber>:<DataType>\"``. The TagNumber should be the same as the command parameter ID in Matter SPEC and the supported DataTypes are listed in ``$ESP_MATTER_PATH/components/esp_matter/utils/json_to_tlv.h``

    - For the DataType ``bytes``, the value should be a Base64-Encoded string.


Here are some examples of the ``command-data`` format.

- For MoveToLevel command in LevelControl cluster, the ``command-data`` (``{"level": 10, "transitionTime": 0, "optionsMask": 0, "optionsOverride": 0}``) should be:

  ::

    matter esp controller invoke <node-id> <endpoint-id> 8 0 -- "{\"0:U8\": 10, \"1:U16\": 0, \"2:U8\": 0, \"3:U8\": 0}"

- For KeySetWrite command in GroupKeyManagement cluster, the ``command-data`` (``{"groupKeySet":{"groupKeySetID": 42, "groupKeySecurityPolicy": 0, "epochKey0": d0d1d2d3d4d5d6d7d8d9dadbdcdddedf, "epochStartTime0": 2220000, "epochKey1": null, "epochStartTime1": null, "epochKey2": null, "epochStartTime2": null}}``) should be:

  ::

    matter esp controller invoke <node-id> <endpoint-id> 63 0 -- "{\"0:OBJ\": {\"0:U16\": 42, \"1:U8\": 0, \"2:BYT\": \"0NHS09TV1tfY2drb3N3e3w==\", \"3:U64\": 2220000, \"4:NULL\": null, \"5:NULL\": null, \"6:NULL\": null, \"7:NULL\": null}}"

- For AddGroup command in Groups cluster, the ``command-data`` (``{"groupID": 1, "groupName": "grp1"}``) should be:

  ::

    matter esp controller invoke <node-id> <endpoint-id> 0x4 0 -- "{\"0:U16\": 1, \"1:STR\": \"grp1\"}"

Read commands
~~~~~~~~~~~~~
Use ``read_command::interaction`` with attribute and/or event path spans.
Its delegate receives ``OnReportBegin()``, ``OnAttribute()``, ``OnEvent()``,
``OnReportEnd(err)``, and terminal ``OnDone(err)`` callbacks. A synchronous
``start()`` failure produces no callbacks.

Read attribute commands
^^^^^^^^^^^^^^^^^^^^^^^
Use ``read attr`` to read attributes from end-devices.

- Attribute read syntax:

  ::

    matter esp controller read attr <node-id> <endpoint-ids> <cluster-ids> <attribute-ids>

.. note::

    - endpoint-ids can represent a single or multiple endpoints, e.g. '0' or '0,1'. And the same applies to cluster-ids, attribute-ids, and event-ids below.

Read event commands
^^^^^^^^^^^^^^^^^^^
Use ``read event`` to read events from end-devices.

- Event read syntax:

  ::

    matter esp controller read event <node-id> <endpoint-ids> <cluster-ids> <event-ids>

Write attribute commands
~~~~~~~~~~~~~~~~~~~~~~~~
``write attr`` writes one complete attribute per command.
In C++, use ``write_command::interaction`` with a concrete attribute path, value
encoder, and explicit delegate pointer. Pass ``nullptr`` to discard successful
responses while retaining failure logs. A delegate gets the path status in
``OnResponse()`` and terminal completion in ``OnDone(err)``.

^^^^^^^^^^^^^^^^

- Attribute write syntax:

  ::

    matter esp controller write attr <node-id> <endpoint-id> <cluster-id> <attribute-id> [--timed-ms N] -- <attribute-value>
    matter esp controller write attr group <group-id> <cluster-id> <attribute-id> -- <attribute-value>

.. note::

    - ``attribute_value`` should utilize a JSON object string. And the format of this string is the same as the ``command_data`` in `cluster commands <./developing.html#cluster-control>`__. This JSON object should contain only one item that represents the attribute value.


Here are some examples of the ``attribute_value`` format.

For StartUpOnOff attribute of OnOff Cluster, you should use the following JSON structures as the ``attribute_value`` to represent the StartUpOnOff ``2`` and ``null``:

  ::

    matter esp controller write attr <node_id> <endpoint_id> 6 0x4003 -- "{\"0:U8\": 2}"
    matter esp controller write attr <node_id> <endpoint_id> 6 0x4003 -- "{\"0:NULL\": null}"

For Binding attribute of Binding cluster, you should use the following JSON structure as the ``attribute_value`` to represent the binding list ``[{"node":1, "endpoint":1, "cluster":6}]``:

  ::

    matter esp controller write attr <node_id> <endpoint_id> 30 0 -- "{\"0:ARR-OBJ\":[{\"1:U64\":1, \"3:U16\":1, \"4:U32\": 6}]}"

For ACL attribute of AccessControl cluster, you should use the following JSON structure as the ``attribute_value`` to represent the AccessControlList ``[{"privilege": 5, "authMode": 2, "subjects": [112233], "targets": null}, {"privilege": 4, "authMode": 3, "subjects": [1], "targets": null}]``:

  ::

      matter esp controller write attr <node_id> <endpoint_id> 31 0 -- "{\"0:ARR-OBJ\":[{\"1:U8\": 5, \"2:U8\": 2, \"3:ARR-U64\": [112233], \"4:NULL\": null}, {\"1:U8\": 4, \"2:U8\": 3, \"3:ARR-U64\": [1], \"4:NULL\": null}]}"

To write multiple attributes, issue separate commands. For example, write the ACL
and Binding attributes in order with:

  ::

    matter esp controller write attr <node_id> <endpoint_id1> 31 0 -- "{\"0:ARR-OBJ\":[{\"1:U8\": 5, \"2:U8\": 2, \"3:ARR-U64\": [112233], \"4:NULL\": null}, {\"1:U8\": 4, \"2:U8\": 3, \"3:ARR-U64\": [1], \"4:NULL\": null}]}"
    matter esp controller write attr <node_id> <endpoint_id2> 30 0 -- "{\"0:ARR-OBJ\":[{\"1:U64\":1, \"3:U16\":1, \"4:U32\": 6}]}"

Wait for the first Write's completion before submitting the second if ordering
is required; console submission success alone does not indicate completion.

For attributes of type uint64_t or int64_t, if the absolute value is greater than (2^53), you should use string to represent number in JSON structure for precision

  ::

    matter esp controller write attr <node_id> <endpoint_id> 42 0 -- "{\"0:ARR-OBJ\":[{\"1:U64\": \"9007199254740993\", \"2:U8\": 0}]}"

Subscribe commands
~~~~~~~~~~~~~~~~~~
Use ``subscribe_command::subscription`` for remote attribute and event reports.
Its delegate receives report callbacks and ``OnStateChanged(state, cause)`` for
connection, retry, and termination. Keep the subscription alive until stopped
or terminated, and perform cleanup outside its callbacks under the CHIP lock.

Subscribe attribute commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Use ``sub add attr`` to subscribe to attribute reports.

- Attribute subscription syntax:

  ::

    matter esp controller sub add attr <node-id> <endpoint-ids> <cluster-ids> <attribute-ids> <min-interval> <max-interval>

Subscribe event commands
^^^^^^^^^^^^^^^^^^^^^^^^
Use ``sub add event`` to subscribe to event reports.

- Send the subscribe-event command:

  ::

    matter esp controller sub add event <node-id> <endpoint-ids> <cluster-ids> <event-ids> <min-interval> <max-interval>

List and remove subscriptions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
``sub list`` shows console-owned subscriptions, including connecting and
retrying entries. Each entry shows the fabric, node, state, and subscription ID
when available.

::

    matter esp controller sub list
    matter esp controller sub remove <node-id> <subscription-id>
    matter esp controller sub remove <node-id> all
    matter esp controller sub remove all

Node-specific removal applies to the active fabric. The literal ``all`` must be
provided explicitly: missing arguments, invalid IDs, and extra arguments are
rejected without removing anything. Node and subscription IDs accept decimal or
``0x``-prefixed hexadecimal notation.

.. warning::

    ``sub remove all`` removes subscriptions across all nodes and fabrics.
    Removal also affects subscriptions created outside the console, whereas
    ``sub list`` displays only console-owned subscriptions.

Group commands
~~~~~~~~~~~~~~

Group Invoke and Write accept ``group <id>`` or an encoded node ID
``0xFFFFFFFFFFFFXXXX``, where ``XXXX`` is the nonzero 16-bit group ID. Both use
the active fabric at submission time. The ``group`` form has no endpoint
argument; the encoded node form ignores the endpoint argument. Read and
Subscribe reject group destinations.

Completion reports dispatch, not member acknowledgments. Group commands do not
support timed options. Configure matching groups and keys on the controller
and targets before sending requests. Manage remote membership through each
target's Groups cluster.

Equivalent group destinations::

    matter esp controller invoke group 1 6 1 -- {}
    matter esp controller invoke 0xFFFFFFFFFFFF0001 0 6 1 -- {}
    matter esp controller write attr group 1 6 0x4003 -- "{\"0:U8\":2}"
    matter esp controller write attr 0xFFFFFFFFFFFF0001 0 6 0x4003 -- "{\"0:U8\":2}"

Configure controller-local groups and keys in builds without a Matter server::

    matter esp group list
    matter esp group add <group-id> <group-name>
    matter esp group remove <group-id>
    matter esp group keyset list
    matter esp group keyset add <ketset-id> <policy> <validity-time> <epoch-key-oct-str>
    matter esp group keyset remove <ketset-id>
    matter esp group bind <group-id> <ketset-id>
    matter esp group unbind <group-id> <ketset-id>

ICD client management
~~~~~~~~~~~~~~~~~~~~~

Use ICD client management with ``CONFIG_ESP_MATTER_CONTROLLER_ENABLE=y`` and
``CONFIG_ESP_MATTER_ENABLE_MATTER_SERVER=n``. The controller example registers
the console commands when ``CONFIG_ENABLE_CHIP_SHELL=y``. In a custom application,
call ``esp_matter::controller::console::register_icd_commands()`` to register them.

ICD console commands
^^^^^^^^^^^^^^^^^^^^

::

    matter esp controller icd list
    matter esp controller icd register <node> <check-in-node> <subject> <key-hex> --client-type permanent|ephemeral [--verification-key HEX]
    matter esp controller icd unregister <node> <check-in-node> [--verification-key HEX]
    matter esp controller icd status
    matter esp controller icd stop

``node`` identifies the ICD server. ``check-in-node`` identifies the check-in
recipient on the controller's current fabric; use the controller's operational
Node ID to receive check-ins on this controller. ``subject`` identifies the
subject whose activity the ICD monitors. Node IDs and subjects accept decimal
or ``0x``-prefixed hexadecimal notation. Supply each key as 32 hexadecimal
digits, without a ``0x`` prefix. The verification key is optional.

``register`` and ``unregister`` use the current fabric. Unregistration requires
a local registration with the same ICD server and check-in node. ``list`` shows
registration metadata across stored fabrics, including the fabric index, node
IDs, counters, monitored subject, and client type. It omits key material.

The console observes one ICD request at a time. Submission success means the
request was accepted. Wait for the completion log to confirm that both the
remote command and local storage update succeeded. ``status`` reports whether
the console still observes the request.

``stop`` detaches observation and stops work before command dispatch. An
already-sent command can still complete and update local storage. A request for
the same peer can return busy while that work finishes, even after ``status``
reports inactive.

ICD client API
^^^^^^^^^^^^^^

Include ``esp_matter_controller_icd_client.h`` and use the
``esp_matter::controller::icd_client`` namespace. Replace calls to the removed
``controller::list_registered_icd()`` with
``icd_client::for_each_registered_icd(callback, context)``. Its callback receives
an ``entry_t`` containing registration metadata. Return ``CHIP_NO_ERROR`` to
continue iteration, or another error to stop and return that error to the
caller. Keep storage unchanged during iteration; entry references remain valid
for the callback duration.

For registration and unregistration, keep an ``icd_client::interaction`` and
its ``icd_client::delegate`` alive while observing the operation. Populate a
``request_t`` with ``METHOD_REGISTER`` or ``METHOD_UNREGISTER`` and call
``start(request, &delegate)``. Pass the required registration key as 16 bytes;
an omitted verification key uses ``nullptr`` and size zero. ``start()`` copies
borrowed key data before returning. A synchronous error produces no callbacks.
An accepted request produces deferred callbacks and one terminal ``OnDone()``,
unless you detach the delegate with ``stop()``.

``OnResponse()`` provides a valid remote status, including rejection, and an
ICD counter for a successful registration. The response reference remains valid
for the callback duration. ``OnDone()`` reports connection, command, or local
storage errors; ``CHIP_NO_ERROR`` confirms both remote success and the local
storage update. Remote success can precede a local storage failure.

Use the CHIP stack context for API calls, storage iteration, and destruction
of an active interaction. ``OnDone()`` may restart, stop, or destroy the
interaction; ``OnResponse()`` may not. ``stop()`` detaches the delegate and lets
an already-sent command finish updating storage. ``active()`` reports whether
the interaction still observes the request. Pending work for the same scoped
peer can cause ``CHIP_ERROR_BUSY`` even after observation stops.

Commissioner features
---------------------
The commissioner is an enhanced controller that can perform commissioning which is the sequence of operations to bring a Node into a Fabric by assigning an Operational Node ID and Node Operational credentials.

The commissioner should support the additional features:

- Obtain the onboarding payload (QR code or manual code) and use it to starting commissioning.
- Verify the commissionee's Device Attestation Certificate (DAC) chain and Certificate Declaration (CD) during commissioning.
- Receive the Certificate Signing Request (CSR) and issue NOC for it during commissioning.

Pairing commands
~~~~~~~~~~~~~~~~

Enable ``CONFIG_ESP_MATTER_COMMISSIONER_ENABLE`` and connect the controller to
its operational IP network before pairing. Supply a QR payload or manual setup
code. The SDK chooses the discovery transport by default; forcing BLE requires
``CONFIG_ENABLE_ESP32_BLE_CONTROLLER``.

Supplying a resolved address skips discovery. Link-local IPv6 addresses require
an address scope or ``--interface``. Optional Wi-Fi and Thread credentials
provision the device network independently of transport selection.

::

    matter esp controller pair <node> <setup-code> [--transport auto|ble|on-network] [--wifi SSID PASSWORD] [--thread HEX]
    matter esp controller pair <node> --address IP --pin PIN [--port N] [--interface NAME]
    matter esp controller unpair <node>

UDC pairing requires commissioner discovery. It uses the peer address from a
pending UDC entry; supply the node ID and setup PIN. Successful request
submission consumes the entry. An inactive entry expires after 30 seconds.

::

    matter esp controller pair udc status
    matter esp controller pair udc <node> <pin>

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

Commissioning operation APIs
----------------------------

Include ``esp_matter_controller_pairing_command.h`` for
``esp_matter::controller::pairing``, or
``esp_matter_controller_commissioning_window_opener.h`` for
``esp_matter::controller::commissioning_window``.

Keep each caller-owned operation and its delegate alive until completion or
``stop()``. Call APIs, inspect state, and destroy active operations in CHIP
context or while holding the CHIP stack lock. ``start()`` copies any request
data it retains before returning. A synchronous rejection produces no callbacks.

Pairing
~~~~~~~

Pairing defaults to ``method_t::setup_code``. Set
``request_t::params.setup_code.payload`` to a manual code or ``MT:`` QR payload.
For a resolved address, call ``request.set_address({peer_address, setup_pin})``
to select ``method_t::address`` and initialize its union member.

Wi-Fi and Thread credentials are independent ``chip::Optional`` values used to
provision the device network, not to choose a discovery transport. An empty
Wi-Fi password selects an open network. A present but empty Thread dataset is
invalid.

Choose the discovery and PASE transport with
``request_t::params.setup_code.transport``:

- ``transport_t::automatic`` is the default. The SDK discovers the device using
  payload capabilities, including on-network fallback.
- ``transport_t::ble`` uses the payload PIN and discriminator over BLE,
  regardless of advertised transport flags. It requires
  ``CONFIG_ENABLE_ESP32_BLE_CONTROLLER`` and rejects compound QR payloads with
  ``CHIP_ERROR_INVALID_ARGUMENT``.
- ``transport_t::on_network`` uses IP discovery regardless of payload BLE flags.

Forced transports do not fall back to another transport. Payload validation,
authentication, and the configured attestation policy still apply. Operational
Matter connections use IP.

.. code-block:: cpp

    namespace pairing = esp_matter::controller::pairing;

    pairing::interaction operation;
    pairing::request_t request;
    request.node_id = node_id;
    request.params.setup_code.payload = setup_code;
    CHIP_ERROR error = operation.start(request, pairing::options_t(), &delegate);

``OnProgress()`` and ``OnCommissioningStage()`` forward SDK progress. Callbacks
may run before ``start()`` or ``cancel()`` returns, so initialize any state they
access before either call. Once the operation accepts a request, it reports SDK
errors through ``OnDone()``, including errors encountered during ``start()``.
Completion means commissioning has finished, not just PASE establishment.

The result contains the node, error, last observed work phase, and SDK
commissioning failure details when available. Result references and pointers
are valid only during the callback. Do not start, cancel, stop, or destroy a
pairing operation from a delegate callback.

A commissioner allows one pairing reservation at a time, including cancellation
cleanup. ``options_t::timeout_ms`` defaults to 300000 ms and must be nonzero.
Expiry initiates cancellation. ``cancel()`` retains the delegate and reports
``CHIP_ERROR_CANCELLED`` after cleanup. ``stop()`` detaches the delegate
immediately, but holds the commissioner reservation until cleanup finishes.
Starts during cleanup return ``CHIP_ERROR_BUSY``. Neither cancellation nor stop
rolls back remote changes.

If the SDK reports a discovery or PASE failure only through a status update,
the operation cancels and reports ``CHIP_ERROR_CONNECTION_ABORTED`` without
waiting for the overall timeout. A later, more specific PASE error may be
unavailable.

``options_t::register_icd`` enables ICD registration by default. Each operation
generates its own registration key and persists the completed registration.

Commissioning windows
~~~~~~~~~~~~~~~~~~~~~

Choose ``method_t::basic``, ``method_t::passcode``, or ``method_t::verifier`` and
set an explicit ``chip::ScopedNodeId`` destination. The operation does not look
up an application or console fabric. Passcode requests generate a PIN and salt
when omitted, but reject supplied values that are invalid. Basic windows require
target support for Basic Commissioning.

.. code-block:: cpp

    namespace window = esp_matter::controller::commissioning_window;

    window::interaction operation;
    window::request_t request(window::method_t::passcode);
    request.node = chip::ScopedNodeId(node_id, fabric_index);
    request.window_timeout_seconds = 180;
    request.params.passcode.discriminator = discriminator;
    request.params.passcode.iterations = 10000;
    CHIP_ERROR error = operation.start(request, &delegate);

``OnResponse()`` reports remote status and includes the setup payload after a
successful passcode-based opening. ``OnDone()`` reports transport or remote
rejection errors. Callbacks run after submission returns. ``OnDone()`` may
restart, stop, or destroy the operation; ``OnResponse()`` may not.

Each object allows one pending request. Separate objects may operate
concurrently. ``active()`` reports local request state, not whether the remote
window is open. ``stop()`` does not close an already-open window, and a timeout
does not rule out remote execution.

Unpairing
~~~~~~~~~

Call ``matter_controller_client::unpair(node, callback)`` to unpair a node. On
success, it removes the peer's ICD registration before calling the application
callback.

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
subscription commands are ``sub add attr``, ``sub add event``, ``sub list``, and
``sub remove``. Removal takes ``<node-id> <subscription-id>``, ``<node-id> all``,
or ``all``. Missing arguments never imply all subscriptions. ``sub list`` shows
console-owned subscriptions; removal can affect other owners. Node-specific
removal uses the active fabric, while ``sub remove all`` covers every fabric.
