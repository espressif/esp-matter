2. Developing with the SDK
==========================

Please refer the :project_file:`Release Notes <RELEASE_NOTES.md>` to know more about
the releases

2.1 ESP-IDF Setup
---------------------

This section talks about setting up ESP-IDF.

2.1.1 Host Setup
~~~~~~~~~~~~~~~~

You should install drivers and support packages for your development
host. Linux and Mac OS-X are the supported development hosts in Matter, the recommended host versions:

- Ubuntu 20.04 or 22.04 LTS
- macOS 10.15 or later

Additionally, we also support developing on Windows Host using WSL.


2.1.1.1 Windows 10
^^^^^^^^^^^^^^^^^^

Development on Windows is supported using Windows Subsystem for Linux (WSL). Please follow the below instructions to set up host.

- Install and enable `Windows Subsystem for Linux 2 (WSL2) <https://docs.microsoft.com/en-us/windows/wsl/install-win10>`__.
- Install Ubuntu 20.04 or 22.04 from the `Windows App Store <https://apps.microsoft.com/store/search/Ubuntu>`__.
- Start Ubuntu (search into start menu) and run command ``uname -a``, it should report a kernel version of ``5.10.60.1`` or later.
  If not please upgrade the WSL2. To upgrade the kernel, run ``wsl --upgrade`` from Windows Power Shell.
- Windows does not support exposing COM ports to WSL distros. Install usbipd-win on `Windows <https://github.com/dorssel/usbipd-win>`__
  and `WSL <https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/WSL.md#usbipd>`__ (usbipd-win `WSL Support <https://github.com/dorssel/usbipd-win/wiki/WSL-support>`__).
- Here onwards process for setting esp-matter and building examples is same as other hosts.
- Please clone the repositories from inside the WSL environment and not inside a mounted directory.

For using CHIP tool on WSL, please check `Using CHIP-tool in WSL <https://github.com/espressif/esp-matter/blob/main/docs/en/using_chip_tool.rst>`__.

For using VSCode for development, please check `Developing in WSL <https://code.visualstudio.com/docs/remote/wsl>`__.

.. toctree::
   :hidden:
   :maxdepth: 1

   Using CHIP-tool in WSL <using_chip_tool>

2.1.2 Getting the Repository
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The Prerequisites for ESP-IDF:

- Please get the `Prerequisites for ESP-IDF`_. For beginners, please check `step by step installation guide`_ for esp-idf.

.. note::

    ``git clone`` command accepts the optional argument ``--jobs N``, which can significantly speed up the
    process by parallelizing submodule cloning. Consider using this option when cloning repositories.

Cloning esp-idf:

   ::

      git clone --recursive https://github.com/espressif/esp-idf.git
      cd esp-idf; git checkout v5.2.3; git submodule update --init --recursive;
      ./install.sh
      cd ..


2.1.3 Configuring the Environment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This should be done each time a new terminal is opened

::

   cd esp-idf; source ./export.sh; cd ..


2.2 ESP Matter Setup
--------------------

There are two options to setup esp-matter, you can select one according to demand:

- ESP matter repository, including esp-matter SDK and tools (e.g., CHIP-tool, CHIP-cert, ZAP, ...).
- ESP matter component, including esp-matter SDK.

2.2.1  ESP-Matter Repository
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

2.2.1.1 Getting the Repository
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The Prerequisites for Matter:

- Please get the `Prerequisites for Matter`_.

Cloning the esp-matter repository takes a while due to a lot of submodules in the upstream connectedhomeip,
so if you want to do a shallow clone use the following command:

- For Linux host:

    ::

        cd esp-idf
        source ./export.sh
        cd ..

        git clone --depth 1 https://github.com/espressif/esp-matter.git
        cd esp-matter
        git submodule update --init --depth 1
        cd ./connectedhomeip/connectedhomeip
        ./scripts/checkout_submodules.py --platform esp32 linux --shallow
        cd ../..
        ./install.sh
        cd ..

- For Mac OS-X host:

    ::

        cd esp-idf
        source ./export.sh
        cd ..

        git clone --depth 1 https://github.com/espressif/esp-matter.git
        cd esp-matter
        git submodule update --init --depth 1
        cd ./connectedhomeip/connectedhomeip
        ./scripts/checkout_submodules.py --platform esp32 darwin --shallow
        cd ../..
        ./install.sh
        cd ..


.. note::

    The modules for platform ``linux`` or ``darwin`` are required for the host tools building.

.. note::

    If you don't want to install host tools (chip-tool, chip-cert etc.) you can use ``./install.sh --no-host-tool``.


To clone the esp-matter repository with all the submodules, use the following command:

::

   cd esp-idf
   source ./export.sh
   cd ..

   git clone --recursive https://github.com/espressif/esp-matter.git
   cd esp-matter
   ./install.sh
   cd ..

.. note::

    If it runs into some errors like:
    ::

      dial tcp 108.160.167.174:443: connect: connection refused

    ::

      ConnectionResetError: [Errno 104] Connection reset by peer

    It's probably caused by some network connectivity issue, a VPN is required for most of the cases.


2.2.1.2 Configuring the Environment
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This should be done each time a new terminal is opened

::

   cd esp-idf; source ./export.sh; cd ..
   cd esp-matter; source ./export.sh; cd ..

Enable Ccache for faster IDF builds.

Ccache is a compiler cache.
Matter builds are very slow and takes a lot of time.
Ccache caches the previous compilations and speeds up recompilation in subsequent builds.

::

   export IDF_CCACHE_ENABLE=1

Above can also be added to your shell’s profile file (.profile, .bashrc, .zprofile, etc.)
to enable ccache every time you open a new terminal.

2.2.2 ESP Matter Component (experimental)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

You can check the component in `Espressif Component Registry <https://components.espressif.com/components/espressif/esp_matter>`__.

To add the esp_matter component to your project, run:

::

   idf.py add-dependency "espressif/esp_matter^1.4.0"

An example with esp_matter component is offered:

-  :project_file:`Managed Component Light <examples/managed_component_light/README.md>`

.. note::

    To use this component, the version of IDF component management should be ``1.4.*`` or ``>= 2.0``.
    Use ``compote version`` to show the version. Use ``pip install 'idf-component-manager~=1.4.0'``
    or ``pip install 'idf-component-manager~=2.0.0'`` to install.

2.2.3 Building Applications
~~~~~~~~~~~~~~~~~~~~~~~~~~~

-  :project_file:`Light <examples/light/README.md>`
-  :project_file:`Light Switch <examples/light_switch/README.md>`
-  :project_file:`Zap Light <examples/zap_light/README.md>`
-  :project_file:`Zigbee Bridge <examples/bridge_apps/zigbee_bridge/README.md>`
-  :project_file:`BLE Mesh Bridge <examples/bridge_apps/blemesh_bridge/README.md>`

2.2.4 Flashing the Firmware
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Choose IDF target.

.. only:: esp32

   ::

      idf.py set-target esp32

.. only:: esp32s3

   ::

      idf.py set-target esp32s3

.. only:: esp32c3

   ::

      idf.py set-target esp32c3

.. only:: esp32c2

   ::

      idf.py set-target esp32c2

.. only:: esp32h2

   ::

      idf.py set-target esp32h2

.. only:: esp32c6

   ::

      idf.py set-target esp32c6

.. only:: esp32p4

   ::

      idf.py set-target esp32p4

-  If IDF target has not been set explicitly, then ``esp32`` is
   considered as default.
-  The default device for ``esp32``/``esp32c3`` is
   ``esp32-devkit-c``/``esp32c3-devkit-m``. If you want to use another
   device, you can export ``ESP_MATTER_DEVICE_PATH`` after choosing
   the correct target, e.g. for ``m5stack`` device:
   ``export ESP_MATTER_DEVICE_PATH=/path/to/esp_matter/device_hal/device/m5stack``

   -  If the device that you have is of a different revision, and is not
      working as expected, you can create a new device and export your
      device path.
   -  The other peripheral components like led_driver, button_driver,
      etc. are selected based on the device selected.
   -  The configuration of the peripheral components can be found in
      ``$ESP_MATTER_DEVICE_PATH/esp_matter_device.cmake``.

.. only:: esp32p4

    - Setup the slave device for ESP32-P4

        It is possible to use Wi-Fi and BLE connection on ESP32-P4 that does not support native Wi-Fi and BLE peripherals, which requires another ESP target with native Wi-Fi support physically connected to the ESP32-P4.
        This uses `esp_hosted <https://components.espressif.com/components/espressif/esp_hosted>`__ component, please refer to its documentation for more details.
        We recommend to use `ESP32-P4 Function_EV_Board <https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32p4/esp32-p4-function-ev-board/index.html>`__ to build the examples for ESP32-P4. It is composed of an ESP32-P4 and an ESP32-C6.
        After setting IDF target, the ``esp_hosted`` will be downloaded at managed_components directory, build and flash the slave device for ESP32-C6. Note that you need the `ESP-Prog Board <https://docs.espressif.com/projects/esp-iot-solution/en/latest/hw-reference/ESP-Prog_guide.html>`__ to flash the slave firmware to ESP32-C6.

        ::

            idf.py -C managed_components/espressif__esp_hosted/slave/ -B build_slave set-target esp32c6
            idf.py -C managed_components/espressif__esp_hosted/slave/ -B build_slave build flash monitor

.. only:: esp32c6

    -  ESP32-C6 supports both the Wi-Fi and IEEE 802.15.4 radio, so you can run Wi-Fi or Thread matter example on it.

        -  To enable Thread, you should change the menuconfig options to ``CONFIG_OPENTHREAD_ENABLED=y``, ``CONFIG_ENABLE_WIFI_STATION=n``, and  ``CONFIG_USE_MINIMAL_MDNS=n``.
        -  To enable Wi-Fi. you should change the menuconfig options to ``CONFIG_OPENTHREAD_ENABLED=n``, ``CONFIG_ENABLE_WIFI_STATION=y``, and ``CONFIG_USE_MINIMAL_MDNS=y``.

(When flashing the SDK for the first time, it is recommended to do
``idf.py erase_flash`` to wipe out entire flash and start out fresh.)

::

   idf.py flash monitor

.. note::

    If you are getting build errors like:
    ::

      ERROR: This script was called from a virtual environment, can not create a virtual environment again

    It can be fixed by running below command:
    ::

      pip install -r $IDF_PATH/requirements.txt


2.3 Commissioning and Control
-----------------------------

There are a few implementations of Matter commissioners present in the `connectedhomeip`_ repository.

CHIP Tool is an example implementation of Matter commissioner and used for development purposes.
An in-depth guide on how to use chip-tool can be found in the `CHIP Tool User Guide`_.

Espressif's ESP RainMaker iOS and Android applications support commissioning and control of Matter devices.

- `ESP-RainMaker Android App`_
- `ESP-RainMaker iOS App`_


2.3.1 Test Setup (CHIP Tool)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A host-based chip-tool can be used as a commissioner to commission and control a Matter device. During the previous ``install.sh`` step, the ``chip-tool`` is generated under the folder:

::

   ${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/out/host

2.3.1.1 Commissioning
^^^^^^^^^^^^^^^^^^^^^

Use ``chip-tool`` in interactive mode to commission the device:

::

   chip-tool interactive start


.. only:: esp32 or esp32s3 or esp32c3 or esp32c2 or esp32c6 or esp32p4

   ::

      pairing ble-wifi 0x7283 <ssid> <passphrase> 20202021 3840

.. only:: esp32c6

    or

.. only:: esp32h2 or esp32c6

   ::

      pairing ble-thread 0x7283 hex:<operationalDataset> 20202021 3840

In the above commands:

-  ``0x7283`` is the randomly chosen ``node_id``
-  ``20202021`` is the ``setup_passcode``
-  ``3840`` is the ``discriminator``


Above method commissions the device using setup passcode and discriminator. Device can also be commissioned using manual pairing code or QR code.

To Commission the device using manual pairing code 34970112332

.. only:: esp32 or esp32s3 or esp32c3 or esp32c2 or esp32c6 or esp32p4

    ::

        pairing code-wifi 0x7283 <ssid> <passphrase> 34970112332

.. only:: esp32c6

    or

.. only:: esp32h2 or esp32c6

    ::

        pairing code-thread 0x7283 hex:<operationalDataset> 34970112332

Above default manual pairing code contains following values:

::

    Version:             0
    Custom flow:         0      (STANDARD)
    Discriminator:       3840
    Passcode:            20202021

To commission the device using QR code MT:Y.K9042C00KA0648G00

.. only:: esp32 or esp32s3 or esp32c3 or esp32c2 or esp32c6 or esp32p4

    ::

        pairing code-wifi 0x7283 <ssid> <passphrase> MT:Y.K9042C00KA0648G00

.. only:: esp32c6

    or

.. only:: esp32h2 or esp32c6

    ::

        pairing code-thread 0x7283 hex:<operationalDataset> MT:Y.K9042C00KA0648G00

Above QR Code contains the below default values:
::

    Version:             0
    Vendor ID:           65521    (0xFFF1)
    ProductID:           32768    (0x8000)
    Custom flow:         0        (STANDARD)
    Discovery Bitmask:   0x02     (BLE)
    Long discriminator:  3840     (0xf00)
    Passcode:            20202021

Alternatively, you can scan the below QR code image using Matter commissioners.

.. figure:: ../_static/matter_qrcode_20202021_3840.png
    :align: center
    :alt: MT:Y.K9042C00KA0648G00
    :scale: 70%
    :figclass: align-center

If QR code is not visible, paste the below link into the browser and scan the QR code.
::

    https://project-chip.github.io/connectedhomeip/qrcode.html?data=MT:Y.K9042C00KA0648G00

If you want to use different values for commissioning the device, please use the
`esp-matter-mfg-tool`_ to generate the factory partition which has to be flashed on the device.
It also generates the new pairing code and QR code image using which you can commission the device.

2.3.1.2 Post Commissioning Setup
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The device would need additional configuration depending on the example,
for it to work. Check the "Post Commissioning Setup" section in examples for more information.

-  :project_file:`Light <examples/light/README.md>`
-  :project_file:`Light Switch <examples/light_switch/README.md>`
-  :project_file:`Zap Light <examples/zap_light/README.md>`
-  :project_file:`Zigbee Bridge <examples/bridge_apps/zigbee_bridge/README.md>`
-  :project_file:`BLE Mesh Bridge <examples/bridge_apps/blemesh_bridge/README.md>`

2.3.1.3 Cluster Control
^^^^^^^^^^^^^^^^^^^^^^^

Use the cluster commands to control the attributes.

::

   onoff toggle 0x7283 0x1

::

   onoff on 0x7283 0x1

::

   levelcontrol move-to-level 10 0 0 0 0x7283 0x1

::

   levelcontrol move-to-level 100 0 0 0 0x7283 0x1

::

   colorcontrol move-to-color-temperature 0 10 0 0 0x7283 0x1

chip-tool when used in interactive mode uses CASE resumption as against establishing CASE for cluster control commands. This results into shorter execution times, thereby improving the overall experience.

For more details on chip-tool usage, check https://github.com/espressif/connectedhomeip/tree/v1.0.0.2/examples/chip-tool

2.4 Device console
------------------

The console on the device can be used to run commands for testing. It is configurable through menuconfig and enabled by default in the firmware. Here are some useful commands:

-  BLE commands: Start and stop BLE advertisement:

   ::

      matter ble [start|stop|state]

-  Wi-Fi commands: Set and get the Wi-Fi mode:

   ::

      matter wifi mode [disable|ap|sta]

-  Device configuration: Dump the device static configuration:

   ::

      matter config

-  Factory reset:

   ::

      matter esp factoryreset

-  On-boarding codes: Dump the on-boarding pairing code payloads:

   ::

      matter onboardingcodes

Additional Matter specific commands:

-  Get attribute: (The IDs are in hex):

   ::

      matter esp attribute get <endpoint_id> <cluster_id> <attribute_id>

   -  Example: on_off::on_off:

      ::

         matter esp attribute get 0x1 0x6 0x0

-  Set attribute: (The IDs are in hex):

   ::

      matter esp attribute set <endpoint_id> <cluster_id> <attribute_id> <attribute value>

   -  Example: on_off::on_off:

      ::

         matter esp attribute set 0x1 0x6 0x0 1

-  Diagnostics:

   ::

      matter esp diagnostics mem-dump

-  Wi-Fi

   ::

      matter esp wifi connect <ssid> <password>

.. only:: esp32h2 or esp32c6

   -  OpenThread command line:

      ::

         matter esp ot_cli <command>

      -  Example: state of OpenThread:

         ::

            matter esp ot_cli state

-  Bridge device:

   ::

      matter esp bridge <command>

   -  Example: add (Parent endpoint should have aggregator device type):

      ::

         matter esp bridge add <parent_endpoint_id> <device_type_id>

2.5 Developing your Product
---------------------------

Understanding the structure before actually modifying and customising
the device is helpful.

2.5.1 Building a Color Temperature Lightbulb
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A device is represented in Matter in terms of its data model. As a first
step of building your product, you will have to define the data model for your
device. Matter has a standard set of device types already defined that you
can use. Please refer to the
`Espressif Matter Blog <https://blog.espressif.com/matter-clusters-attributes-commands-82b8ec1640a0>`__
for clarity on the terms like endpoints, clusters, etc. that are used in this section.

2.5.1.1 Data Model
^^^^^^^^^^^^^^^^^^

-  Typically, the data model is defined in the example's *app_main.cpp*.
   First off we start by creating a Matter node, which is the root of
   the Data Model.

   ::

      node::config_t node_config;
      node_t *node = node::create(&node_config, app_attribute_update_cb, NULL);

-  We will use the ``color_temperature_light`` standard device type in this
   case. All standard device types are available in :project_file:`esp_matter_endpoint.h <components/esp_matter/esp_matter_endpoint.h>` header file.
   Each device type has a set of default configuration that can be
   specific as well.

   ::

      color_temperature_light::config_t light_config;
      light_config.on_off.on_off = DEFAULT_POWER;
      light_config.level_control.current_level = DEFAULT_BRIGHTNESS;
      endpoint_t *endpoint = color_temperature_light::create(node, &light_config, ENDPOINT_FLAG_NONE);

   In this case, we create the light using the ``color_temperature_light::create()`` function. Similarly, multiple
   endpoints can be created on the same node. Check the following
   sections for more info.

2.5.1.2 Attribute Callback
^^^^^^^^^^^^^^^^^^^^^^^^^^

-  Whenever a Matter client makes changes to the device, they end up
   updating the attributes in the data model.

-  When an attribute is updated, the attribute_update_cb is used
   to notify the application of this change. You would typically call
   device driver specific APIs for executing the required action. Here,
   if the callback type is ``PRE_UPDATE``, the driver is updated first.
   If that is a success, only then the attribute value is actually
   updated in the database.

   ::

      esp_err_t app_attribute_update_cb(callback_type_t type, uint16_t endpoint_id, uint32_t cluster_id,
                                        uint32_t attribute_id, esp_matter_attr_val_t *val, void *priv_data)
      {
          esp_err_t err = ESP_OK;

          if (type == PRE_UPDATE) {
              /* Driver update */
              err = app_driver_attribute_update(endpoint_id, cluster_id, attribute_id, val);
          }

          return err;
      }

2.5.1.3 Device Drivers
^^^^^^^^^^^^^^^^^^^^^^

-  The drivers, depending on the device, are typically initialized and
   updated in the example's *app_driver.cpp*.

   ::

      esp_err_t app_driver_init()
      {
          ESP_LOGI(TAG, "Initialising driver");

          /* Initialize button */
          button_config_t button_config = button_driver_get_config();
          button_handle_t handle = iot_button_create(&button_config);
          iot_button_register_cb(handle, BUTTON_PRESS_DOWN, app_driver_button_toggle_cb);
          app_reset_button_register(handle);

          /* Initialize led */
          led_driver_config_t led_config = led_driver_get_config();
          led_driver_init(&led_config);

          app_driver_attribute_set_defaults();
          return ESP_OK;
      }

-  The driver's attribute update API just handles the attributes that
   are actually relevant for the device. For example, a
   color_temperature_light handles the power, brightness, hue,
   saturation and temperature.

   ::

      esp_err_t app_driver_attribute_update(uint16_t endpoint_id, uint32_t cluster_id, uint32_t attribute_id,
                                            esp_matter_attr_val_t *val)
      {
          esp_err_t err = ESP_OK;
          if (endpoint_id == light_endpoint_id) {
              if (cluster_id == OnOff::Id) {
                  if (attribute_id == OnOff::Attributes::OnOff::Id) {
                      err = app_driver_light_set_power(val);
                  }
              } else if (cluster_id == LevelControl::Id) {
                  if (attribute_id == LevelControl::Attributes::CurrentLevel::Id) {
                      err = app_driver_light_set_brightness(val);
                  }
              } else if (cluster_id == ColorControl::Id) {
                  if (attribute_id == ColorControl::Attributes::CurrentHue::Id) {
                      err = app_driver_light_set_hue(val);
                  } else if (attribute_id == ColorControl::Attributes::CurrentSaturation::Id) {
                      err = app_driver_light_set_saturation(val);
                  } else if (attribute_id == ColorControl::Attributes::ColorTemperature::Id) {
                      err = app_driver_light_set_temperature(val);
                  }
              }
          }
          return err;
      }


2.5.2 Defining your own data model
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This section demonstrates creating standard endpoints, clusters, attributes,
and commands that are defined in the Matter specification

2.5.2.1 Endpoints
^^^^^^^^^^^^^^^^^

The device can be customized by editing the endpoint/device_type
creating in the *app_main.cpp* of the example. Examples:

-  on_off_light:

   ::

      on_off_light::config_t light_config;
      endpoint_t *endpoint = on_off_light::create(node, &light_config, ENDPOINT_FLAG_NONE);

-  fan:

   ::

      fan::config_t fan_config;
      endpoint_t *endpoint = fan::create(node, &fan_config, ENDPOINT_FLAG_NONE);


-  door_lock:

   ::

      door_lock::config_t door_lock_config;
      endpoint_t *endpoint = door_lock::create(node, &door_lock_config, ENDPOINT_FLAG_NONE);

-  window_covering_device:

   ::

      window_covering_device::config_t window_covering_device_config(static_cast<uint8_t>(chip::app::Clusters::WindowCovering::EndProductType::kTiltOnlyInteriorBlind));
      endpoint_t *endpoint = window_covering_device::create(node, &window_covering_config, ENDPOINT_FLAG_NONE);

   The ``window_covering_device`` ``config_t`` structure includes a constructor that allows specifying
   an end product type different than the default one, which is "Roller shade".
   Once a ``config_t`` instance has been instantiated, its end product type cannot be modified.

- pump

   ::

      pump::config_t pump_config(1, 10, 20);
      endpoint_t *endpoint = pump::create(node, &pump_config, ENDPOINT_FLAG_NONE);

   The ``pump`` ``config_t`` structure includes a constructor that allows specifying
   maximum pressure, maximum speed and maximum flow values. If they aren't set, they will be set to null by default.
   Once a ``config_t`` instance has been instantiated, these three values cannot be modified.


2.5.2.2 Clusters
^^^^^^^^^^^^^^^^

Additional clusters can also be added to an endpoint. Examples:

-  on_off:

   ::

      on_off::config_t on_off_config;
      cluster_t *cluster = on_off::create(endpoint, &on_off_config, CLUSTER_FLAG_SERVER, on_off::feature::lighting::get_id());

-  temperature_measurement:

   ::

      temperature_measurement::config_t temperature_measurement_config;
      cluster_t *cluster = temperature_measurement::create(endpoint, &temperature_measurement_config, CLUSTER_FLAG_SERVER);

- window_covering:

      ::

         window_covering::config_t window_covering_config(static_cast<uint8_t>(chip::app::Clusters::WindowCovering::EndProductType::kTiltOnlyInteriorBlind));
         cluster_t *cluster = window_covering::create(endpoint, &window_covering_config, CLUSTER_FLAG_SERVER);

   The ``window_covering`` ``config_t`` structure includes a constructor that allows specifying
   an end product type different than the default one, which is "Roller shade".
   Once a ``config_t`` instance has been instantiated, its end product type cannot be modified.

- pump_configuration_and_control:

   ::

      pump_configuration_and_control::config_t pump_configuration_and_control_config(1, 10, 20);
      cluster_t *cluster = pump_configuration_and_control::create(endpoint, &pump_configuration_and_control_config, CLUSTER_FLAG_SERVER);

   The ``pump_configuration_and_control`` ``config_t`` structure includes a constructor that allows specifying
   maximum pressure, maximum speed and maximum flow values. If they aren't set, they will be set to null by default.
   Once a ``config_t`` instance has been instantiated, these three values cannot be modified.

2.5.2.3 Attributes and Commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Additional attributes and commands can also be added to a cluster.
Examples:

-  attribute: on_off:

   ::

      bool default_on_off = true;
      attribute_t *attribute = on_off::attribute::create_on_off(cluster, default_on_off);

-  attribute: cluster_revision:

   ::

      uint16_t default_cluster_revision = 1;
      attribute_t *attribute = global::attribute::create_cluster_revision(cluster, default_cluster_revision);

-  command: toggle:

   ::

      command_t *command = on_off::command::create_toggle(cluster);

-  command: move_to_level:

   ::

      command_t *command = level_control::command::create_move_to_level(cluster);

2.5.2.4 Features
^^^^^^^^^^^^^^^^^^
Optional features which are applicable to a cluster can also be added.

- feature: taglist: Descriptor cluster:

  ::

      cluster_t* cluster = cluster::get(endpoint, Descriptor::Id);
      descriptor::feature::taglist::add(cluster);

2.5.3 Adding custom data model fields
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This section demonstrates creating custom endpoints, clusters, attributes,
and commands that are not defined in the Matter specification and can be
specific to the vendor.

2.5.3.1 Endpoints
^^^^^^^^^^^^^^^^^

Non-Standard endpoint can be created, without any clusters.

-  Endpoint create:

   ::

      endpoint_t *endpoint = endpoint::create(node, ENDPOINT_FLAG_NONE);

2.5.3.2 Clusters
^^^^^^^^^^^^^^^^

Non-Standard/Custom clusters can also be created:

-  Cluster create:

   ::

      uint32_t custom_cluster_id = 0x131bfc00;
      cluster_t *cluster = cluster::create(endpoint, custom_cluster_id, CLUSTER_FLAG_SERVER);

2.5.3.3 Attributes and Commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Non-Standard/Custom attributes can also be created on any cluster:

-  Attribute create:

   ::

      uint32_t custom_attribute_id = 0x0;
      uint16_t default_value = 100;
      attribute_t *attribute = attribute::create(cluster, custom_attribute_id, ATTRIBUTE_FLAG_NONE, esp_matter_uint16(default_value);

-  Command create:

   ::

      static esp_err_t command_callback(const ConcreteCommandPath &command_path, TLVReader &tlv_data, void
      *opaque_ptr)
      {
         ESP_LOGI(TAG, "Custom command callback");
         return ESP_OK;
      }

      uint32_t custom_command_id = 0x0;
      command_t *command = command::create(cluster, custom_command_id, COMMAND_FLAG_ACCEPTED, command_callback);

2.5.4 Advanced Setup
~~~~~~~~~~~~~~~~~~~~
This section explains adding external platforms for Matter. This step is **optional** for most devices. Espressif's SDK for Matter provides support for overriding the default platform layer, so the BLE and Wi-Fi implementations can be customized. Here are the required steps for adding an external platform layer.

2.5.4.1 Creating the external platform directory
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Create a directory ``platform/${NEW_PLATFORM_NAME}`` in your codebase.
You can typically copy
``${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/src/platform/ESP32``
as a start. Note that the new platform name should be something other than
``ESP32``. In this article we'll use ``ESP32_custom`` as an example. The
directory must be under ``platform`` folder to meet the Matter include
path conventions.

2.5.4.2 Modifying the BUILD.gn target
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There is an example :project_file:`BUILD.gn <examples/common/external_platform/BUILD.gn>` file for
the ``ESP32_custom`` example platform. It simply compiles the ESP32
platform in Matter without any modifications.

-  The new platform directory must be added to the Matter include path. See
   the ``ESP32_custom_include`` config in the above mentioned file.
-  Multiple build configs must be exported to the build system. See the
   ``buildconfig_header`` section in the file for the required definitions.

2.5.4.3 Editing Kconfigs
^^^^^^^^^^^^^^^^^^^^^^^^

-  Enable ``CONFIG_CHIP_ENABLE_EXTERNAL_PLATFORM``.
-  Set ``CONFIG_CHIP_EXTERNAL_PLATFORM_DIR`` to the relative path from
   ``${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/config/esp32`` to
   the external platform directory. For instance, if your source tree is:

   ::

      my_project
      ├── esp-matter
      └── platform
         └── ESP32_custom

   Then ``CONFIG_CHIP_EXTERNAL_PLATFORM_DIR`` would be ``../../../../../platform/ESP32_custom``.

-  Disable ``CONFIG_BUILD_CHIP_TESTS``.
-  If your external platform does not support the *connectedhomeip/connectedhomeip/src/lib/shell/*
   provided in the Matter shell library, then disable ``CONFIG_ENABLE_CHIP_SHELL``.

2.5.4.4 Example Usage
^^^^^^^^^^^^^^^^^^^^^

As an example, you can build *light* example on ``ESP32_custom`` platform with following steps:

::

   mkdir $ESP_MATTER_PATH/../platform
   cp -r $ESP_MATTER_PATH/connectedhomeip/connectedhomeip/src/platform/ESP32 $ESP_MATTER_PATH/../platform/ESP32_custom
   cp $ESP_MATTER_PATH/examples/common/external_platform/BUILD.gn $ESP_MATTER_PATH/../platform/ESP32_custom
   cd $ESP_MATTER_PATH/examples/light
   cp sdkconfig.defaults.ext_plat sdkconfig.defaults
   idf.py build

2.6 Factory Data Providers
--------------------------

2.6.1 Providers Introduction
~~~~~~~~~~~~~~~~~~~~~~~~~~~~
There are four factory data providers, each with its own implementation, that need to be configured. These providers supply the device with necessary factory data, which is then read by the device according to their respective implementations.

- ``Commissionable Data Provider``

  This particular provider is responsible for retrieving commissionable data, which includes information such as setup-discriminator, spake2p-iteration-count, spake2p-salt, spake2p-verifier, and setup-passcode.

- ``Device Attestation Credentials(DAC) Provider``

  This particular provider is responsible for retrieving device attestation credentials, which includes information such as CD, firmware-information, DAC, and PAI certificate. And it can also sign message with the DAC private key.

- ``Device Instance Info Provider``

  This particular provider is responsible for retrieving device instance information, which includes vendor-name, vendor-id, product-name, product-id, product-url, product-label, hardware-version-string, hardware-version, rotating-device-id-unique-id, serial-number, manufacturing-data, and part-number.

- ``Device Info Provider``

  This particular provider is responsible for retrieving device information, which includes fixed-labels, user-labels, supported-locales, and supported-calendar-types.

2.6.2 Configuration Options
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Different implementations of the four providers can be chosen in meuconfig:

- ``Commissionable Data Provider options`` in ``→ Component config → ESP Matter``

  When selecting ``Commissionable Data - Test``, the device will use the hardcoded Commissionable Data. This uses the legacy commissionable data
  provider and provides the test values. These test values are enclosed in ``CONFIG_ENABLE_TEST_SETUP_PARAMS`` option and enabled by default.

  When selecting ``Commissionable Data - Factory``, the device will use commissionable data information from the factory partition. This option is visable only when ``CONFIG_ENABLE_ESP32_FACTORY_DATA_PROVIDER`` is selected.

  When selecting ``Commissionable Data - Custom``, the device will use the custom defined commissionable data provider to obtain commissionable data information. ``esp_matter::set_custom_commissionable_data_provider()`` should be called before ``esp_matter::start()`` to set the custom provider.

  If you are using ``Factory`` or ``Custom`` commissionable data provides, then disable the ``CONFIG_ENABLE_TEST_SETUP_PARAMS`` option.

- ``DAC Provider options`` in ``→ Component config → ESP Matter``

  When selecting ``Attestation - Test``, the device will use the hardcoded Device Attestation Credentials.

  When selecting ``Attestation - Factory``, the device will use the Device Attestation Credentials in the factory partition binary. This option is visable only when ``CONFIG_ENABLE_ESP32_FACTORY_DATA_PROVIDER`` is selected.

  When selecting ``Attestation - Secure Cert``, the device will use the Device Attestation Credentials in the secure cert partition. This option is for the `Pre-Provisioned Modules <./production.html#pre-provisioned-modules>`__. And the original vendor ID and product ID should be added to the CD file for the Pre-Provisioned Modules. Please contact your Espressif contact person for more information.

  When selecting ``Attestation - Custom``, the device will use the custom defined DAC provider to obtain the Device Attestation Credentials. ``esp_matter::set_custom_dac_provider()`` should be called before ``esp_matter::start()`` to set the custom provider.

- ``Device Instance Info Provider options`` in ``→ Component config → ESP Matter``

  When selecting ``Device Instance Info - Test``, the device will use the hardcoded Device Instance Information.

  When selecting ``Device Instance Info - Factory``, the device will use device instance information from the factory partition. This option is visable only when ``CONFIG_ENABLE_ESP32_FACTORY_DATA_PROVIDER`` and ``ENABLE_ESP32_DEVICE_INSTANCE_INFO_PROVIDER`` is selected.

  When selecting ``Device Instance Info - Custom``, the device will use custom defined Device Instance Info Provider to obtain the Device Instance Information. ``esp_matter::set_custom_device_instance_info_provider`` should be called before ``esp_matter::start()`` to set the custom provider.

- ``Device Info Provider options`` in ``→ Component config → ESP Matter``

  When selecting ``Device Info - None``, the device will not use any device information provider. It should be selected when there are not related clusters on the device.

  When selecting ``Device Info - Factory``, the device will use device information from the factory partition. This option is visable only when ``CONFIG_ENABLE_ESP32_FACTORY_DATA_PROVIDER`` and ``ENABLE_ESP32_DEVICE_INFO_PROVIDER`` is selected.

  When selecting ``Device Info - Custom``, the device will use custom defined Device Info Provider to obtain the Device Information. ``esp_matter::set_custom_device_info_provider`` should be called before ``esp_matter::start()`` to set the custom provider.

2.6.3 Custom Providers
~~~~~~~~~~~~~~~~~~~~~~

In order to use custom providers, you need to define implementations of the four base classes of the providers and override the functions within them. And the custom providers should be set before ``esp_matter::start()`` is called.

2.7 Using esp_secure_cert partition
-----------------------------------

2.7.1 Configuration Options
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Build the firmware with below configuration options

::

    # Disable the DS Peripheral support
    CONFIG_ESP_SECURE_CERT_DS_PERIPHERAL=n

    # Use DAC Provider implementation which reads attestation data from secure cert partition
    CONFIG_SEC_CERT_DAC_PROVIDER=y

    # Enable some options which reads CD and other basic info from the factory partition
    CONFIG_ENABLE_ESP32_FACTORY_DATA_PROVIDER=y
    CONFIG_ENABLE_ESP32_DEVICE_INSTANCE_INFO_PROVIDER=y
    CONFIG_FACTORY_COMMISSIONABLE_DATA_PROVIDER=y
    CONFIG_FACTORY_DEVICE_INSTANCE_INFO_PROVIDER=y


2.7.2 Certification Declaration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you do not have an certification declaration file then you can generate the test CD with the help of below mentioned steps.
We need to generate the new CD because it SHALL match the VID, PID in DAC and the ones reported by basic cluster.

- Build the host tools if not done already

::

    cd connectedhomeip/connectedhomeip
    gn gen out/host
    ninja -C build

Generate the Test CD, please make sure to change the ``-V`` (vendor_id) and ``-p`` (product-id) options based on the ones that are being used.
For more info about the arguments, please check `here <https://github.com/espressif/connectedhomeip/tree/v1.0.0.2/src/tools/chip-cert#gen-cd>`__.

::

    out/host/chip-cert gen-cd -f 1 -V 0xFFF1 -p 0x8001 -d 0x0016 \
                              -c "CSA00000SWC00000-01" -l 0 -i 0 -n 1 -t 0 \
                              -K credentials/test/certification-declaration/Chip-Test-CD-Signing-Key.pem \
                              -C credentials/test/certification-declaration/Chip-Test-CD-Signing-Cert.pem \
                              -O TEST_CD_FFF1_8001.der


2.7.3 Factory Partition
~~~~~~~~~~~~~~~~~~~~~~~

Factory partition contains basic information like VID, PID, etc.

By default, the CD(Certification Declaration) is stored in the factory partition and we need to add the ``-cd`` option when generating the factory partition.

Alternatively, if you’d like to embed the CD in the firmware, you can enable the
``CONFIG_ENABLE_SET_CERT_DECLARATION_API`` option and use the ``SetCertificationDeclaration()`` API to set the CD.
You can refer to the reference implementation in :project_file: `light example <https://github.com/espressif/esp-matter/tree/main/examples/light>`__.

Export the dependent tools path

::

    cd esp-matter
    export PATH=$PATH:$PWD/connectedhomeip/connectedhomeip/out/host


Generate the factory partition, please use the APPROPRIATE values for ``-v`` (Vendor Id), ``-p`` (Product Id), and ``-cd`` (Certification Declaration).

::

    esp-matter-mfg-tool --passcode 89674523 \
                  --discriminator 2245 \
                  -cd TEST_CD_FFF1_8001.der \
                  -v 0xFFF1 --vendor-name Espressif \
                  -p 0x8001 --product-name Bulb \
                  --hw-ver 1 --hw-ver-str DevKit


Few important output lines are mentioned below. Please take a note of onboarding codes, these can be used for commissioning the device.

::

    [2022-12-02 11:18:12,059] [   INFO] - Generated QR code: MT:-24J06PF150QJ850Y10
    [2022-12-02 11:18:12,059] [   INFO] - Generated manual code: 20489154736

Factory partition binary will be generated at the below path. Please check for <uuid>.bin file in this directory.

::

    [2022-12-02 11:18:12,381] [   INFO] - Generated output files at: out/fff1_8001/e17c95e1-521e-4979-b90b-04da648e21bb


2.7.4 Flashing firmware, secure cert and factory partition
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Flashing secure cert partition. Please check partition table for ``esp_secure_cert`` partition address.

.. note::

    Flash only if not flashed on manufacturing line.

::

    esptool.py -p (PORT) write_flash 0xd000 secure_cert_partition.bin

Flashing factory partition, Please check the ``CONFIG_CHIP_FACTORY_NAMESPACE_PARTITION_LABEL`` for factory partition label.
Then check the partition table for address and flash at that address.

::

    esptool.py -p (PORT) write_flash 0x10000 path/to/partition/generated/using/mfg_tool/uuid.bin


Flash application

::

    idf.py flash


2.7.5 Test commissioning using chip-tool
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If using the DACs signed by custom PAA that is not present in connectedhomeip repository,
then download the PAA certificate, please make sure it is in DER format.

Run the following command from host to commission the device.

::

    ./chip-tool pairing ble-wifi 1234 my_SSID my_PASSPHRASE my_PASSCODE my_DISCRIMINATOR --paa-trust-store-path /path/to/PAA-Certificates/


2.8 Matter OTA
--------------

- Enable the ``CONFIG_ENABLE_OTA_REQUESTOR`` option to enable Matter OTA Requestor functionality.

Please follow the `guide <https://github.com/project-chip/connectedhomeip/blob/master/docs/guides/esp32/ota.md>`__
in the connectedhomeip repository for generating a Matter OTA image and performing OTA.

2.8.1 Encrypted Matter OTA
~~~~~~~~~~~~~~~~~~~~~~~~~~

The esp-matter SDK supports using a pre-encrypted application image for OTA upgrades.
Please follow the steps below to enable and use encrypted application images for OTA upgrades.

- Enable the ``CONFIG_ENABLE_OTA_REQUESTOR`` and ``CONFIG_ENABLE_ENCRYPTED_OTA`` options
- The application code must make an API call to ``esp_matter_ota_requestor_encrypted_init()`` after calling
  ``esp_matter::start()``. You can use the following code as a reference:

::

    #include <esp_matter_ota.h>

    {
        const char *rsa_private_key;    // Please set this to the buffer containing RSA 3072 private key in PEM format
        uint16_t rsa_private_key_len;   // Please set this to the length of RSA 3072 private key

        esp_err_t err = esp_matter_ota_requestor_encrypted_init(rsa_private_key, rsa_private_key_len);
    }


- Please refer to the `guide <https://github.com/project-chip/connectedhomeip/blob/master/docs/guides/esp32/ota.md#encrypted-ota>`__
  in the connectedhomeip repository for instructions on how to generate a private key, encrypted OTA image, and Matter OTA image.

.. note::

    There are several ways to store the private key, such as hardcoding it in the firmware, embedding it as a text
    file, or reading it from the NVS. We have demonstrated the use of the private key by embedding it as a text file in the
    light example.

2.9 Mode Select
---------------

This cluster provides an interface for controlling a characteristic of a device that can be set to one of several predefined values. For example, the light pattern of a disco ball, the mode of a massage chair, or the wash cycle of a laundry machine.

2.9.1 Attribute Supported Modes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This attribute is the list of supported modes that may be selected for the CurrentMode attribute. Each item in this list represents a unique mode as indicated by the Mode field of the ModeOptionStruct. Each entry in this list SHALL have a unique value for the Mode field.
ESP_MATTER uses factory partition to set the values of Supported Modes attribute.

2.9.2 Generate Factory Partition Using esp-matter-mfg-tool
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Use `esp-matter-mfg-tool`_ to generate factory partition of the supported modes attribute.

2.9.2.1 Usage
^^^^^^^^^^^^^

::

    esp-matter-mfg-tool -cn "My bulb" -v 0xFFF2 -p 0x8001 --pai \
    -k path/to/esp-matter/connectedhomeip/connectedhomeip/credentials/test/attestation/Chip-Test-PAI-FFF2-8001-Key.pem \
    -c path/to/esp-matter/connectedhomeip/connectedhomeip/credentials/test/attestation/Chip-Test-PAI-FFF2-8001-Cert.pem \
    -cd path/to/esp-matter/connectedhomeip/connectedhomeip/credentials/test/certification-declaration/Chip-Test-CD-FFF2-8001.der \
    --supported-modes mode1/label1/endpointId/"value\mfgCode, value\mfgCode"  mode2/label2/endpointId/"value\mfgCode, value\mfgCode"

- For empty Semantic Tags list

::

    --supported-modes mode1/label1/endpointId  mode2/label2/endpointId

2.9.3 Build example
~~~~~~~~~~~~~~~~~~~

For example we want to use mode_select cluster in light example.

- Add source and include path to example/light/main/CMakeList.txt

::

    Append "${MATTER_SDK_PATH}/examples/platform/esp32/mode-support" to SRC_DIRS and PRIV_INCLUDE_DIRS

- In file example/light/app_main.cpp.

::

    #include <static-supported-modes-manager.h>

    ModeSelect::StaticSupportedModesManager sStaticSupportedModesManager;
    {
        cluster::mode_select::config_t ms_config;
        cluster_t *ms_cluster = cluster::mode_select::create(endpoint, &ms_config, CLUSTER_FLAG_SERVER, ESP_MATTER_NONE_FEATURE_ID);

        sStaticSupportedModesManager.InitEndpointArray(get_count(node));
        ModeSelect::setSupportedModesManager(&sStaticSupportedModesManager);
    }

2.10 Matter Controller
----------------------
This section introduces the Matter controller example. Now this example supports the following features of the standard Matter controller:

- BLE-WiFi pairing
- BLE-Thread pairing
- On-network pairing
- Invoke cluster commands
- Read attributes commands
- Read events commands
- Write attributes commands
- Subscribe attributes commands
- Subscribe events commands
- Group settings command.

2.10.1 Device console
~~~~~~~~~~~~~~~~~~~~~
Once you have flashed the controller example onto the device, you can use the `device console <./developing.html#device-console>`__ to commission the device and send commands to the end-device. All of the controller commands begin with the prefix ``matter esp controller``.

2.10.2 Pairing commands
~~~~~~~~~~~~~~~~~~~~~~~
The ``pairing`` commands are used for commissioning end-devices and are available when the ``Enable matter commissioner`` option is enabled. Here are three standard pairing methods:

- **Onnetwork pairing:** Prior to executing this commissioning method, it is necessary to connect both the controller and the end-device to the same network and ensure that the commissioning window of the end-device is open. To complete this process, you can use the command ``matter esp wifi connect``. After the devices are connected, the pairing process can be initiated.

   ::

      matter esp wifi connect <ssid> <password>
      matter esp controller pairing onnetwork <node_id> <setup_passcode>

- **Ble-wifi pairing:** This pairing method is supported for ESP32S3. Before you execute this commissioning method, connect the controller to the Wi-Fi network and ensure that the end-device is in commissioning mode. You can use the command ``matter esp wifi connect`` to connect the controller to your wifi network. Then we can start the pairing.

   ::

      matter esp wifi connect <ssid> <password>
      matter esp controller pairing ble-wifi <node_id> <ssid> <password> <pincode> <discriminator>

- **Ble-thread pairing:** This pairing method is supported for ESP32S3. Before you execute this commissioning method, connect the controller to the Wi-Fi network in which there is a Thread Border Router (BR). And please ensure that the end-device is in commissioning mode. You can use the command ``matter esp wifi connect`` to connect the controller to your Wi-Fi network. Get the dataset tlvs of the Thread network that the Thread BR is in. Then we can start the pairing.

  ::

     matter esp wifi connect <ssid> <password>
     matter esp controller pairing ble-thread <node_id> <dataset_tlvs> <pincode> <discriminator>

- **Matter payload based pairing:** This method is similar to the previously mentioned pairing methods, but instead of accepting a PIN code and discriminator, it uses a Matter setup payload as input. The setup payload is parsed to extract the necessary information, which then initiates the pairing process.

For the ``code`` pairing method, commissioner tries to discover the end-device only on the IP network.
However, when using ``code-wifi``, ``code-thread``, or ``code-wifi-thread``, and id
``CONFIG_ENABLE_ESP32_BLE_CONTROLLER`` is enabled, controller tries to discover the end-device on both the IP and BLE networks.

Below are supported commands:

  ::

     matter esp controller pairing code <node_id> <setup_payload>

  ::

     matter esp controller pairing code-wifi <node_id> <ssid> <passphrase> <setup_payload>

  ::

     matter esp controller pairing code-thread <node_id> <operationalDataset> <setup_payload>

  ::

     matter esp controller pairing code-wifi-thread <node_id> <ssid> <passphrase> <operationalDataset> <setup_payload>


2.10.3 Cluster commands
~~~~~~~~~~~~~~~~~~~~~~~
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

2.10.4 Read commands
~~~~~~~~~~~~~~~~~~~~
The ``read_command`` class is used for sending read commands to other end-devices. Its constructor function could accept two callback inputs:

- **Attribute report callback**:
  This callback will be called upon the reception of the attribute report for read-attribute commands.

- **Event report callback**:
  This callback will be called upon the reception of the event report for read-event commands.

2.10.4.1 Read attribute commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The ``read-attr`` commands are used for sending the commands of reading attributes on end-devices.

- Send the read-attribute command:

   ::

      matter esp controller read-attr <node-id> <endpoint-ids> <cluster-ids> <attribute-ids>

.. note::

    - endpoint-ids can represent a single or multiple endpoints, e.g. '0' or '0,1'. And the same applies to cluster-ids, attribute-ids, and event-ids below.

2.10.4.2 Read event commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The ``read-event`` commands are used for sending the commands of reading events on end-devices.

- Send the read-event command:

  ::

      matter esp controller read-event <node-id> <endpoint-ids> <cluster-ids> <event-ids>

2.10.5 Write attribute commands
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
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

2.10.6 Subscribe commands
~~~~~~~~~~~~~~~~~~~~~~~~~
The ``subscribe_command`` class is used for sending subscribe commands to other end-devices. Its constructor function could accept four callback inputs:

- **Attribute report callback**:
  This callback will be invoked upon the reception of the attribute report for subscribe-attribute commands.

- **Event report callback**:
  This callback will be invoked upon the reception of the event report for subscribe-event commands.

- **Subscribe done callback**:
  This callback will be invoked when the subscription is terminated or shutdown.

- **Subscribe failure callback**:
  This callback will be invoked upon the failure of establishing CASE session.

2.10.6.1 Subscribe attribute commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The ``subs-attr`` commands are used for sending the commands of subscribing attributes on end-devices.

- Send the subscribe-attribute command:

  ::

     matter esp controller subs-attr <node-id> <endpoint-ids> <cluster-ids> <attribute-ids> <min-interval> <max-interval>

2.10.6.2 Subscribe event commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The ``subs-event`` commands are used for sending the commands of subscribing events on end-devices.

- Send the subscribe-event command:

  ::

     matter esp controller subs-event <node-id> <endpoint-ids> <cluster-ids> <event-ids> <min-interval> <max-interval>

2.10.7 Group settings commands
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The ``group-settings`` commands are used to set group information of the controller. They are available when the ``Enable matter commissioner`` option is enabled in menuconfig. If the controller wants to send multicast commands to end-devices, it should be in the same group as the end-devices.

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

2.10.8 Attestation Trust Storage
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
The controller example offers two options for the Attestation Trust Storage which is used to store and utilize the PAA certificates for the Device Attestation verification. This feature is available when the ``Enable matter commissioner`` option is enabled in menuconfig. You can modify this setting in menuconfig ``Components`` -> ``ESP Matter Controller`` -> ``Attestation Trust Store``

- ``Attestation Trust Store - Test``

  Use two hardcoded PAA certificates(Chip-Test-PAA-FFF1-Cert&Chip-Test-PAA-NoVID-Cert) in the firmware.

- ``Attestation Trust Store - Spiffs``

  Read the PAA root certificates from the spiffs partition. The PAA der files should be placed in ``paa_cert`` directory so that they can be flashed into the spiffs partition of the controller.

2.11 Custom Cluster
-------------------

Matter enables users to implement custom clusters for unique features. This section introduces how to add a custom cluster.

2.11.1 Cluster XML Template
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Before adding a custom cluster, you should design the attributes, commands, and events it will include, and create the cluster XML template file based on your design.

Example:

::

    <?xml version="1.0"?>
    <configurator>
      <domain name="CHIP"/>
      <cluster>
        <domain>General</domain>
        <name>Sample ESP</name>
        <!-- The MSB 16 bits of <code> are the VendorID. Replace this with your
             VendorID. 0x131B is the VendorId of Espressif.
             The LSB 16 bits of <code> are a self-assigned ClusterID -->
        <code>0x131BFC20</code>
        <define>SAMPLE_ESP_CLUSTER</define>
        <description>The Sample ESP cluster showcases a manufacturer custom cluster</description>

        <!-- Attributes -->
        <!-- A simple test boolean attribute -->
        <attribute side="server" code="0x0000" define="SAMPLE_BOOLEAN" type="boolean" writable="true" default="false" optional="false">SampleBoolean</attribute>
        <attribute side="server" code="0x0001" define="SAMPLE_CHAR_STR" type="char_string" writable="false" optional="false">SampleCharStr</attribute>

        <!-- Commands -->
        <command source="client" code="0x00" name="CommandwithoutArgs" optional="false">
          <description>
            Simple command without any parameters and without a response.
          </description>
        </command>

        <command source="client" code="0x01" name="CommandWithArgs" response="CommandWithArgsResponse" optional="false">
          <description>
            Command that takes two uint8 arguments.
          </description>
          <arg name="Arg1" type="int8u"/>
          <arg name="Arg2" type="int8u"/>
        </command>

        <!-- Command Responses -->
        <command source="server" code="0x02" name="CommandWithArgsResponse" optional="false" disableDefaultResponse="true">
          <description>
            Response for CommandwithArgs.
          </description>
          <arg name="ResponseArg" type="int8u"/>
        </command>

        <!-- Events -->
        <event side="server" code="0x0000" name="TestEvent" priority="info" isFabricSensitive="true" optional="false">
          <description>
            Example event with a event data
          </description>
          <field id="1" name="EventData" type="int32u"/>
        </event>
      </cluster>
    </configurator>

The example XML file above illustrates a cluster with two attributes, two accepted commands, one generated command(command response), and one event.

After creating the custom cluster XML template file, add the root directory of your template file to the ``xmlRoot`` array and the template file name to the ``xmlFile`` array in both the `zcl configuration file`_ and the `zcl test configuration file`_.

Run ``zap_regen_all.py`` in Matter virtual environment to generate common code and client code for the custom cluster.

  ::

    cd esp_matter/connectedhomeip/connectedhomeip
    source ./scripts/active.sh
    ./scripts/tools/zap_regen_all.py

The code generation script will create client code for the custom cluster, supporting Android, Darwin, and Python controllers, as well as the chip-tool. It will also generate app-common code for the new custom cluster.
The chip-tool can be used to test the custom cluster after recompiling.

2.11.2 Cluster Implementation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The custom cluster should be implemented after the app-common code has been generated.

2.11.2.1 Custom Cluster Attributes
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The attributes in a cluster can be managed with the two following methods. A cluster can utilize both the methods to manage its attributes.

- Attribute Accessors

  By default, all the attributes are stored in the ZCL data model and can be managed with the Attribute Accessors generated in the app-common code. You can set/get the attribute values with the Accessors APIs.

- Attribute Access Interface (AAI)

  Matter provides a virtual class, ``AttributeAccessInterface``, which can be inherited by the custom cluster to manage its attributes.
  Attributes managed by AAI should be added to ``attributeAccessInterfaceAttributes`` array in both the `zcl configuration file`_ and the `zcl test configuration file`_. Then, run the ``zap_regen_all.py`` to regenerate the app-common code.
  Once the code is regenerated, the Attribute Accessors APIs for attributes managed by AAI will be removed.

  Notes that attributes of complex types(structure or array) cannot be handled by Attribute Accessors and MUST be managed using AAI.

2.11.2.2 Custom Cluster Commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The commands in a cluster can be handled with one of the two following methods. A cluster can only choose one method to handle its commands.

- Ember Command callbacks

  By default, all the commands are handled using Ember command callbacks. The ``zap`` tool generates declarations for these callbacks in the app-common code. And the corresponding definitions should be implemented to use the commands within the clusters.

- Command Handler Interface (CHI)

  Matter also provides a virtual class, ``CommandHandlerInterface``, which can be inherited to handle commands within the cluster.
  If the commands in a cluster are handled by CHI. The cluster should be added to the ``CommandHandlerInterfaceOnlyClusters`` array in the `zap configuration data`_ file.
  After modifying the `zap configuration data`_, the code should be regenerated, which will remove the Ember command callback declarations.

2.11.2.3 Custom Cluster Events
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

All the events are managed by the `EventLogging <https://github.com/project-chip/connectedhomeip/blob/master/src/app/EventLogging.h>`__.
If an event is triggered, ``chip::app::LogEvent()`` can be called to record it. The event will then be reported to the subscriber that has subscribed to it.

2.11.2.4 Custom Cluster Functions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A custom cluster might requires special funtions to handle initialization, attribute changes, shutdown, and pre-attribute changes.
For instance, the AAI and CHI need to be registered so that they can be accessed by the Matter data model. Therefore, the cluster requires an initialization function to register them.
To enable these functions, the cluster should be added to the appropriate entry in the `zap configuration data`_ file.

2.11.3 Example Usage
~~~~~~~~~~~~~~~~~~~~

- Zap Example

  If the example uses ``zap`` tool to generate its data model, the custom cluster should be added to the example's zap file. The ``zap`` tool will then generate the data model code, including the custom cluster, during the building process.

- ESP-Matter Example

  If the example uses ESP-Matter APIs to define its data model, the custom data model should be created and added to the data model using the esp-matter APIs, following the instructions in `Adding custom data model fields <./developing.html#adding-custom-data-model-fields>`__

.. _`step by step installation guide`: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html
.. _`Prerequisites for ESP-IDF`: https://docs.espressif.com/projects/esp-idf/en/v5.0.1/esp32/get-started/index.html#step-1-install-prerequisites
.. _`Prerequisites for Matter`: https://github.com/espressif/connectedhomeip/blob/v1.1-branch/docs/guides/BUILDING.md#prerequisites
.. _`esp-matter-mfg-tool`: https://github.com/espressif/esp-matter-tools/tree/main/mfg_tool
.. _`zcl configuration file`: https://github.com/project-chip/connectedhomeip/blob/master/src/app/zap-templates/zcl/zcl.json
.. _`zcl test configuration file`: https://github.com/project-chip/connectedhomeip/blob/master/src/app/zap-templates/zcl/zcl-with-test-extensions.json
.. _`zap configuration data`: <https://github.com/project-chip/connectedhomeip/blob/master/src/app/common/templates/config-data.yaml>
.. _`ESP-Rainmaker iOS App`: https://apps.apple.com/app/esp-rainmaker/id1497491540
.. _`ESP-Rainmaker Android App`: https://play.google.com/store/apps/details?id=com.espressif.rainmaker
.. _`connectedhomeip`: https://github.com/project-chip/connectedhomeip
.. _`CHIP Tool User Guide`: https://github.com/project-chip/connectedhomeip/blob/master/docs/development_controllers/chip-tool/chip_tool_guide.md
.. _`profile installation instructions`: https://github.com/project-chip/connectedhomeip/blob/master/docs/guides/darwin.md#profile-installation
