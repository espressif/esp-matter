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

- Ubuntu 20.04 LTS
- macOS 10.15 or later

The Prerequisites for ESP-IDF and Matter:

- Please see `Prerequisites <https://docs.espressif.com/projects/esp-idf/en/v4.4.1/esp32/get-started/index.html#step-1-install-prerequisites>`__ for ESP IDF.
- Please get the `Prerequisites <https://github.com/project-chip/connectedhomeip/blob/master/docs/guides/BUILDING.md#prerequisites>`__ for Matter.

2.1.2 Getting the Repositories
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. only:: esp32 or esp32c3

   ::

      git clone --recursive https://github.com/espressif/esp-idf.git
      cd esp-idf; git checkout v4.4.1; git submodule update --init --recursive;
      ./install.sh
      cd ..

.. only:: esp32h2

   ::

      git clone --recursive https://github.com/espressif/esp-idf.git
      cd esp-idf; git checkout 047903c; git submodule update --init --recursive;
      ./install.sh
      cd ..

Cloning the esp-matter repository takes a while due to a lot of submodules in the upstream connectedhomeip,
so if you want to do a shallow clone use the following command:

::

   git clone --depth 1 --shallow-submodules --recursive https://github.com/espressif/esp-matter.git
   cd esp-matter
   ./install.sh
   cd ..

To clone the esp-matter repository with all the submodules, use the following command:

::

   git clone --recursive https://github.com/espressif/esp-matter.git
   cd esp-matter
   ./install.sh
   cd ..

2.1.3 Configuring the Environment
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This should be done each time a new terminal is opened

::

   cd esp-idf; . ./export.sh; cd ..
   cd esp-matter; . ./export.sh; cd ..

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

-  For a Wi-Fi device, a Wi-Fi AP which supports IPv6 is required.
-  For a Thread device, a Thread Border Router is required.

2.2.1 Test Setup (CHIP Tool)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

A host-based chip-tool can be used as a commissioner to commission and control a Matter device.

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
