2. Developing with the SDK
==========================

Please refer the :project_file:`Release Notes <RELEASE_NOTES.txt>` to know more about
the releases

2.1 Development Setup
---------------------

This section talks about setting up your development host, fetching the
Git repositories, and instructions to build and flash.

2.1.1 Host Setup
~~~~~~~~~~~~~~~~

You should install drivers and support packages for your development
host. Linux and Mac OS-X are the supported development hosts in Matter, the recommended host versions:

- Ubuntu 20.04 or 22.04 LTS
- macOS 10.15 or later

Additionally, we also support developing on Windows Host using WSL.

The Prerequisites for ESP-IDF and Matter:

- Please see `Prerequisites <https://docs.espressif.com/projects/esp-idf/en/v4.4.2/esp32/get-started/index.html#step-1-install-prerequisites>`__ for ESP IDF.
- Please get the `Prerequisites <https://github.com/project-chip/connectedhomeip/blob/master/docs/guides/BUILDING.md#prerequisites>`__ for Matter.



2.1.1.1 Windows 10
~~~~~~~~~~~~~~~~~~

Development on Windows is supported using Windows Subsystem for Linux (WSL). Please follow the below instructions to set up host.

- Install and enable `Windows Subsystem for Linux 2 (WSL2) <https://docs.microsoft.com/en-us/windows/wsl/install-win10>`__.
- Install Ubuntu 20.04 or 22.04 from the `Windows App Store <https://apps.microsoft.com/store/search/Ubuntu>`__.
- Start Ubuntu (search into start menu) and run command ``uname -a``, it should report a kernel version of ``5.10.60.1`` or later.
  If not please upgrade the WSL2. To upgrade the kernel, run ``wsl --upgrade`` from Windows Power Shell.
- Windows does not support exposing COM ports to WSL distros. Install usbipd-win on `Windows <https://github.com/dorssel/usbipd-win>`__
  and `WSL <https://github.com/espressif/vscode-esp-idf-extension/blob/master/docs/WSL.md#usbipd>`__ (usbipd-win `WSL Support <https://github.com/dorssel/usbipd-win/wiki/WSL-support>`__).
- Here onwards process for setting esp-matter and building examples is same as other hosts.

For using VSCode for developement, please check `Developing in WSL <https://code.visualstudio.com/docs/remote/wsl>`__.


2.1.2 Getting the Repositories
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. only:: esp32 or esp32c3

   ::

      git clone --recursive https://github.com/espressif/esp-idf.git
      cd esp-idf; git checkout v4.4.2; git submodule update --init --recursive;
      ./install.sh
      source ./export.sh
      cd ..

.. only:: esp32h2

   ::

      git clone --recursive https://github.com/espressif/esp-idf.git
      cd esp-idf; git checkout 20949d444f; git submodule update --init --recursive;
      ./install.sh
      source ./export.sh
      cd ..

Cloning the esp-matter repository takes a while due to a lot of submodules in the upstream connectedhomeip,
so if you want to do a shallow clone use the following command:

::

   git clone --depth 1 https://github.com/espressif/esp-matter.git
   cd esp-matter
   git submodule update --init --depth 1
   ./connectedhomeip/connectedhomeip/scripts/checkout_submodules.py --platform esp32 --shallow
   ./install.sh
   cd ..

To clone the esp-matter repository with all the submodules, use the following command:

::

   git clone --recursive https://github.com/espressif/esp-matter.git
   cd esp-matter
   ./install.sh
   cd ..

Note: If it runs into some errors like:

   ::

      dial tcp 108.160.167.174:443: connect: connection refused

   ::

      ConnectionResetError: [Errno 104] Connection reset by peer

It's probably caused by some network connectivity issue, a VPN is required for most of the cases.

2.1.3 Configuring the Environment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

2.1.4 Building Applications
~~~~~~~~~~~~~~~~~~~~~~~~~~~

-  :project_file:`Light <examples/light/README.md>`
-  :project_file:`Light Switch <examples/light_switch/README.md>`
-  :project_file:`Zap Light <examples/zap_light/README.md>`
-  :project_file:`Zigbee Bridge <examples/zigbee_bridge/README.md>`
-  :project_file:`BLE Mesh Bridge <examples/blemesh_bridge/README.md>`

2.1.5 Flashing the Firmware
~~~~~~~~~~~~~~~~~~~~~~~~~~~

Choose IDF target.

.. only:: esp32

   ::

      idf.py set-target esp32

.. only:: esp32c3

   ::

      idf.py set-target esp32c3

.. only:: esp32h2

   ::

      idf.py --preview set-target esp32h2

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

(When flashing the SDK for the first time, it is recommended to do
``idf.py erase_flash`` to wipe out entire flash and start out fresh.)

::

   idf.py flash monitor

-  Note: If you are getting build errors like:

   ::

      ERROR: This script was called from a virtual environment, can not create a virtual environment again
          
   Run:

   ::

      pip install -r $IDF_PATH/requirements.txt

2.2 Commissioning and Control
-----------------------------

There are a few implementations of Matter commissioner present in the `connectedhomeip <https://github.com/project-chip/connectedhomeip/tree/master/src/controller#implementations>`__ repository.

CHIP Tool is an example implementation of Matter commissioner and used for development purposes.

Espressif also has an iOS application, `Espressif-Matter <https://apps.apple.com/in/app/espressif-matter/id1604739172>`__, to commission and control the Matter devices. Please follow `profile installation instructions <https://github.com/project-chip/connectedhomeip/blob/master/docs/guides/darwin.md#profile-installation>`__ in order to use the application.

2.2.1 Test Setup (CHIP Tool)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A host-based chip-tool can be used as a commissioner to commission and control a Matter device. During the previous ``install.sh`` step, the ``chip-tool`` is generated under the folder:

::

   $ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/out/host

2.2.1.1 Commissioning
^^^^^^^^^^^^^^^^^^^^^

Use ``chip-tool`` in interactive mode to commission the device:

::

   chip-tool interactive start


.. only:: esp32 or esp32c3

   ::

      pairing ble-wifi 0x7283 <ssid> <passphrase> 20202021 3840

.. only:: esp32h2

   ::

      pairing ble-thread 0x7283 hex:<operationalDataset> 20202021 3840

In the above commands:

-  ``0x7283`` is the randomly chosen ``node_id``
-  ``20202021`` is the ``setup_passcode``
-  ``3840`` is the ``discriminator``


Above method commissions the device using setup passcode and discriminator. Device can also be commissioned using manual pairing code or QR code.

To Commission the device using manual pairing code 34970112332

.. only:: esp32 or esp32c3

    ::

        pairing code-wifi 0x7283 <ssid> <passphrase> 34970112332

.. only:: esp32h2

    ::
        pairing code-thread 0x7283 hex:<operationalDataset> 34970112332

Above default manual pairing code contains following values:

::

    Version:             0
    Custom flow:         0      (STANDARD)
    Discriminator:       3840
    Passcode:            20202021

To commission the device using QR code MT:Y.K9042C00KA0648G00

.. only:: esp32 or esp32c3

    ::

        pairing code-wifi 0x7283 <ssid> <passphrase> MT:Y.K9042C00KA0648G00

.. only:: esp32h2

    ::

        pairing code-thread 0x7283 hex:<operationalDataset> MT:Y.K9042C00KA0648G00

Above QR Code contains the below default values:
::

    Version:             0
    Vendor ID:           65521
    ProductID:           32768
    Custom flow:         0        (STANDARD)
    Discovery Bitmask:   0x02     (BLE)
    Long discriminator:  3840     (0xf00)
    Passcode:            20202021

Alternatively, you can scan the below QR code image using Matter commissioners.

.. figure:: ../_static/matter_qrcode_20202021_3840.png
    :align: center
    :alt: Default QR Code
    :figclass: align-center

If QR code is not visible, paste the below link into the browser and scan the QR code.
::

    https://project-chip.github.io/connectedhomeip/qrcode.html?data=MT:Y.K9042C00KA0648G00

If you want to use different values for commissioning the device, please use the
`mfg-tool <https://github.com/espressif/esp-matter/tree/main/tools/mfg_tool#readme>`__
to generate the factory partition which has to be flashed on the device.
It also generates the new pairing code and QR code image using which you can commission the device.

2.2.1.2 Post Commissioning Setup
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The device would need additional configuration depending on the example,
for it to work. Check the "Post Commissioning Setup" section in examples for more information.

-  :project_file:`Light <examples/light/README.md>`
-  :project_file:`Light Switch <examples/light_switch/README.md>`
-  :project_file:`Zap Light <examples/zap_light/README.md>`
-  :project_file:`Zigbee Bridge <examples/zigbee_bridge/README.md>`
-  :project_file:`BLE Mesh Bridge <examples/blemesh_bridge/README.md>`

2.2.1.3 Cluster Control
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

   colorcontrol move-to-saturation 200 0 0 0 0x7283 0x1

::

   colorcontrol move-to-hue 150 0 0 0 0 0x7283 0x1

chip-tool when used in interactive mode uses CASE resumption as against establishing CASE for cluster control commands. This results into shorter execution times, thereby improving the overall experience.

For more details on chip-tool usage, check https://github.com/project-chip/connectedhomeip/tree/master/examples/chip-tool

2.3 Device console
------------------

The console on the device can be used to run commands for testing. It is configurable through menuconfig and enabled by default in the firmware. Here are some useful commands:

-  BLE commands: Start and stop BLE advertisement:

   ::

      matter ble [start|stop|state]

-  Wi-Fi commands: Set and get the Wi-Fi mode:

   ::

      matter wifi mode [disable|ap|sta]

-  Wi-Fi connect: Connect to the Access Point

   ::

      matter wifi connect <ssid> <password>

-  Device configuration: Dump the device static configuration:

   ::

      matter config

-  Factory reset:

   ::

      matter device factoryreset

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

2.4 Developing your Product
---------------------------

Understanding the structure before actually modifying and customising
the device is helpful.

2.4.1 Building a Color Temperature Lightbulb
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A device is represented in Matter in terms of its data model. As a first
step of building your product, you will have to define the data model for your
device. Matter has a standard set of device types already defined that you
can use. Please refer to the
`Espressif Matter Blog <https://blog.espressif.com/matter-clusters-attributes-commands-82b8ec1640a0>`__
for clarity on the terms like endpoints, clusters, etc. that are used in this section.

2.4.1.1 Data Model
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

2.4.1.2 Attribute Callback
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

2.4.1.3 Device Drivers
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


2.4.2 Defining your own data model
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This section demonstrates creating standard endpoints, clusters, attributes,
and commands that are defined in the Matter specification

2.4.2.1 Endpoints
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


2.4.2.2 Clusters
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

2.4.2.3 Attributes and Commands
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

2.4.3 Adding custom data model fields
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This section demonstrates creating custom endpoints, clusters, attributes,
and commands that are not defined in the Matter specification and can be
specific to the vendor.

2.4.3.1 Endpoints
^^^^^^^^^^^^^^^^^

Non-Standard endpoint can be created, without any clusters.

-  Endpoint create:

   ::

      endpoint_t *endpoint = endpoint::create(node, ENDPOINT_FLAG_NONE);

2.4.3.2 Clusters
^^^^^^^^^^^^^^^^

Non-Standard/Custom clusters can also be created: 

-  Cluster create:

   ::
      
      uint32_t custom_cluster_id = 0x131bfc00;
      cluster_t *cluster = cluster::create(endpoint, custom_cluster_id, CLUSTER_FLAG_SERVER);

2.4.3.3 Attributes and Commands
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

2.4.4 Advanced Setup
~~~~~~~~~~~~~~~~~~~~
This section explains adding external platforms for Matter. This step is **optional** for most devices. Espressif's SDK for Matter provides support for overriding the default platform layer, so the BLE and Wi-Fi implementations can be customized. Here are the required steps for adding an external platform layer.

2.4.4.1 Creating the external platform directory
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Create a directory ``platform/${NEW_PLATFORM_NAME}`` in your codebase.
You can typically copy
``${ESP_MATTER_PATH}/connectedhomeip/connectedhomeip/src/platform/ESP32``
as a start. Note that the new platform name should be something other than
``ESP32``. In this article we'll use ``ESP32_custom`` as an example. The
directory must be under ``platform`` folder to meet the Matter include
path conventions.

2.4.4.2 Modifying the BUILD.gn target
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There is an example :project_file:`BUILD.gn <examples/common/external_platform/BUILD.gn>` file for
the ``ESP32_custom`` example platform. It simply compiles the ESP32
platform in Matter without any modifications.

-  The new platform directory must be added to the Matter include path. See
   the ``ESP32_custom_include`` config in the above mentioned file.
-  Multiple build configs must be exported to the build system. See the
   ``buildconfig_header`` section in the file for the required definitions.

2.4.4.3 Editing Kconfigs
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

2.4.4.4 Example Usage
^^^^^^^^^^^^^^^^^^^^^

As an example, you can build *light* example on ``ESP32_custom`` platform with following steps:

::

   mkdir $ESP_MATTER_PATH/../platform
   cp -r $ESP_MATTER_PATH/connectedhomeip/connectedhomeip/src/platform/ESP32 $ESP_MATTER_PATH/../platform/ESP32_custom
   cp $ESP_MATTER_PATH/examples/common/external_platform/BUILD.gn $ESP_MATTER_PATH/../platform/ESP32_custom
   cd $ESP_MATTER_PATH/examples/light
   cp sdkconfig.defaults.ext_plat_ci sdkconfig.defaults
   idf.py build

2.4.5 Controller Example
~~~~~~~~~~~~~~~~~~~~~~~~
This section introduces the Matter controller example. Now this example supports 4 features of the standard Matter controller, including onnetwork-pairing, unicast-cluster-commands(onoff, levelcontrol, colorcontrol), read-attributes-commands, and unicast-write-attributes-commands(onoff, levelcontrol, colorcontrol).

2.4.5.1 Starting with device console
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
After you flash the controller example to the device. you can use `device console<https://docs.espressif.com/projects/esp-matter/en/main/esp32/developing.html#device-console>` to commission and send commands to the end-device. All of the controller commands start with *matter esp controller*.

2.4.5.2 Pairing commands
^^^^^^^^^^^^^^^^^^^^^^^^
The *pairing* command is used for commissioning the end-devices. Here are three standard pairing methods:

- Onnetwork pairing. Before you execute this commissioning method, you should connect both controller and end-device to the same network and ensure the commissioning window of the end-device is opened. You can use the command *matter wifi connect* to complete this process. Then we can start the pairing.

   ::

      matter esp controller pairing onnetwork <node_id> <setup_passcode>

- Ble-wifi pairing. This commissioning method is still not supported on current controller example.

- Ble-thread pairing. This commissioning method is still not supported on current controller example.

2.4.5.3 Cluster commands
^^^^^^^^^^^^^^^^^^^^^^^^
The *invoke-cmd* command is used for sending cluster commands to the end-devices. Currently the controller only supports commands of on-off, level-control, and color-control clusters.

- Send the cluster command:

   ::

      matter esp controller invoke-cmd <node_id> <endpoint_id> <cluster_id> <command_id> <command_data>

2.4.5.4 Read attribute commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The *read-attr* command is used for sending the commands of reading attributes on the end-device.

- Send the read-attribute command:

   ::

      matter esp controller read-attr <node_id> <endpoint_id> <cluster_id> <attribute_id>

2.4.5.4 Read event commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^
The *read-event* command is used for sending the commands of reading events on the end-device.

- Send the read-event command:

  ::

      matter esp controller read-event <node_id> <endpoint_id> <cluster_id> <event_id>

2.4.5.5 Write attribute commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The *write-attr* command is used for sending the commands of writing attributes on the end-device. Currently the controller only supports unicast-attributes-writing of on-off, level-control, and color-control clusters.

- Send the write-attribute command:

   ::

      matter esp controller write-attr <node_id> <endpoint_id> <cluster_id> <attribute_id> <attribute_value>

2.4.5.6 Subscribe attribute commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The *subs-attr* command is used for sending the commands of subscribing attributes on the end-device.

- Send the subscribe-attribute command:

  ::

     matter esp controller subs-attr <node_id> <endpoint_id> <cluster_id> <attribute_id> <min-interval> <max-interval>

2.4.5.7 Subscribe event commands
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The *subs-event* command is used for sending the commands of subscribing events on the end-device.

- Send the subscribe-event command:

  ::

     matter esp controller subs-event <node_id> <endpoint_id> <cluster_id> <event_id> <min-interval> <max-interval>


2.5 Using esp_secure_cert partition
-----------------------------------

2.5.1 Configuration Options
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


2.5.2 Certification Declaration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If you do not have an certification declaration file then you can generate the test CD with the help of below mentioned steps.
We need to generate the new CD because it SHALL match the VID PID in DAC and the ones reported by basic cluster.

- Build the host tools if not done already

::

    cd connectedhomeip/connectedhomeip
    gn gen out/host
    ninja -C build

Generate the Test CD, please make sure to change the ``-V`` (vendor_id) and ``-p`` (product-id) options based on the ones that are being used.
For more into about the arguments, please check `here <https://github.com/project-chip/connectedhomeip/tree/master/src/tools/chip-cert#gen-cd>`__.

::

    out/host/chip-cert gen-cd -f 1 -V 0xFFF1 -p 0x8001 -d 0x0016 \
                              -c "CSA00000SWC00000-01" -l 0 -i 0 -n 1 -t 0 \
                              -K credentials/test/certification-declaration/Chip-Test-CD-Signing-Key.pem \
                              -C credentials/test/certification-declaration/Chip-Test-CD-Signing-Cert.pem \
                              -O TEST_CD_FFF1_8001.der


2.5.3 Factory Partition
~~~~~~~~~~~~~~~~~~~~~~~

Factory partition contains basic information like VID, PID, etc, and CD.

Export the dependent tools path

::

    cd esp-matter/tools/mfg_tool
    export PATH=$PATH:$PWD/../../connectedhomeip/connectedhomeip/out/host


Generate the factory partition, please use the APPROPRIATE values for ``-v`` (Vendor Id), ``-p`` (Product Id), and ``-cd`` (Certification Declaration).

::

    ./mfg_tool.py --passcode 89674523 \
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


2.5.4 Flashing firmware, secure cert and factory partition
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Flashing secure cert partition. Please check partition table for ``esp_secure_cert`` partition address.
NOTE: Flash only if not flashed on manufacturing line.

::

    esptool.py -p (PORT) write_flash 0xd000 secure_cert_partition.bin

Flashing factory partition, Please check the ``CONFIG_CHIP_FACTORY_NAMESPACE_PARTITION_LABEL`` for factory partition label.
Then check the partition table for address and flash at that address.

::

    esptool.py -p (PORT) write_flash 0x10000 path/to/partition/generated/using/mfg_tool/uuid.bin


Flash application

::

    idf.py flash


2.5.5 Test commissioning using chip-tool
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If using the DACs signed by custom PAA that is not present in connectedhomeip repository,
then download the PAA certificate, please make sure it is in DER format.

Run the following command from host to commission the device.

::

    ./chip-tool pairing ble-wifi 1234 my_SSID my_PASSPHRASE my_PASSCODE my_DISCRIMINATOR --paa-trust-store-path /path/to/PAA-Certificates/

