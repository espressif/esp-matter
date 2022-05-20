1. Introduction
===============

1.1 One-stop Matter Solution
----------------------------

Espressif's **One-stop Matter Solution** consists of:

   - A Full Spectrum of Matter Device Platforms
   - Production ready ESP Matter SDK
   - Matter and ESP RainMaker Ecosystem Solution

todo. Add an image similar to the first diagram here: https://rainmaker.espressif.com/ and here: https://github.com/espressif/esp-va-sdk/tree/master#11-solution-architecture?

.. figure:: ../_static/solution_architecture.png
    :align: center
    :alt: ESP Matter Solution Architecture
    :figclass: align-center

1.1.1 Espressif Matter Platforms
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Espressif provides a full spectrum Matter device platforms:

.. figure:: ../_static/esp_matter_platform.png
    :align: center
    :alt: ESP Matter Platform
    :figclass: align-center

- The Wi-Fi-enabled SoCs and modules, such as ESP32, ESP32-C and ESP32-S series can be used to build Matter Wi-Fi devices.
- ESP32-H SoCs and modules integrated with 802.15.4 can be used to build Matter Thread devices.
- By efficiently combining ESP32-H and Espressif's Wi-Fi SoC, a Thread border router can be built to connect the Thread network with the Wi-Fi network to interconnect devices. Espressif provides hardware devkits, reference design and production-ready SDK, which supports the latest Thread 1.3 feature for Matter, and other features like coexist and OTA.
- Espressif also provides Matter-ZigBee and Matter-BLE Mesh bridge solutions that enable non-matter devices based on ZigBee, Bluetooth LE Mesh and other protocols to connect to the Matter ecosystem. A Matter-ZigBee Bridge uses ESP32-H and another Wi-Fi SoC, while a Matter-BLE Mesh Bridge can be done on single SoC with both Wi-Fi and Bluetooth LE interfaces.

1.1.2 ESP Matter SDK
~~~~~~~~~~~~~~~~~~~~

Espressif's Matter SDK is built on top of `connectedhomeip <https://github.com/project-chip/connectedhomeip/>`__, and integrates ESP Data Model APIs, Tools, Documentation, etc. It is a production ready Matter SDK with rich examples, which can simplify the development process of Matter products.

.. figure:: ../_static/software_components.png
    :align: center
    :alt: ESP Matter Software Components
    :figclass: align-center

In addition, ESP Matter SDK also integrates `ESP RainMaker <https://rainmaker.espressif.com/>`__ and `ESP Insights <https://github.com/espressif/esp-insights>`__ for Cloud services and remote diagnostics.

1.1.3 Matter and ESP RainMaker Ecosystem Solution
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Espressif's AIoT cloud platform `ESP RainMaker <https://rainmaker.espressif.com/>`__ can provide remote control for Matter devices and enable the Cloud-based device management of Matter devices' massive data resources.

todo. Add a supported services diagram similar to the one ad the end of this page https://rainmaker.espressif.com/?

By combining the above-mentioned Matter hardware and software solutions with ESP RainMaker, this one-stop Matter ecosystem solution provides:

   -  Interconnection with Amazon, Google and Apple
   -  Full-fledged Cloud deployment through own private account
   -  Privately deployable cloud applications
   -  Ready-made phone apps supporting all common smart-home scenarios and voice-assistant integrations

Relying on the privatization feature of ESP RainMaker, manufacturers
can even build their own brand of an IoT ecosystem, and provide more value-added services to end customers
through their own cloud services.

1.2 Matter Devices
------------------

1.2.1 Simple Device
~~~~~~~~~~~~~~~~~~~

These are typically basic/simple devices.

todo. list all supported device types that listed in Matter device library spec.

   -  Light Bulb
   -  Switch
   -  Temperature sensor

1.2.2 Multi-Device
~~~~~~~~~~~~~~~~~~

These are a combination of more than one simple devices.

   -  Fan with Light
   -  Temperature sensor and Proximity sensor

1.2.3 Bridge
~~~~~~~~~~~~

These devices facilitate using non-Matter devices in a Matter network.

   -  ZigBee bridge
   -  BLE mesh bridge

1.2.4 Thread Border Router
~~~~~~~~~~~~~~~~~~~~~~~~~~

These devices typically connect a Thread network to other IP-based networks, such as Wi-Fi or Ethernet.

   -  https://github.com/espressif/esp-idf/tree/master/examples/openthread/ot_br

1.2.5 Hub
~~~~~~~~~

These can be used to control other Matter devices.

   -  Touch-screen control panel
   -  Hub with internet connectivity for remote control

1.3 Examples
------------

1.3.1 Light
~~~~~~~~~~~

This application creates a Color Dimmable Light device using the ESP
Matter data model.

1.3.2 RainMaker Light
~~~~~~~~~~~~~~~~~~~~~

This application creates a Color Dimmable Light device using the ESP
Matter data model.

It also initializes ESP RainMaker which enables Device Management and
OTA using the RainMaker cloud. If user node association is done, it also
enables Remote Control through RainMaker.

1.3.3 Switch
~~~~~~~~~~~~

This application creates an On/Off Light Switch device using the ESP Matter
data model.

It creates the On/Off client and other devices can be binded to the
switch and then controlled from the switch.

1.3.4 Zap Light
~~~~~~~~~~~~~~~

This application creates a Color Dimmable Light device using the Zap
data model instead of the ESP Matter data model.

1.3.5 ZigBee Bridge
~~~~~~~~~~~~~~~~~~~

This application demonstrates a Matter-ZigBee Bridge that bridges ZigBee devices to Matter fabric.

1.4 Try it yourself
-------------------

1.4.1 ESP Launchpad
~~~~~~~~~~~~~~~~~~~

This allows you to quickly try out Matter on Espressif devices through a web browser.


ESP Launchpad: https://espressif.github.io/esp-launchpad/.
