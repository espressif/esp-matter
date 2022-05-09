1. Introduction
===============

1.1 Solution Architecture
-------------------------

The typical solution architecture of the product is shown as below.

.. figure:: ../_static/solution_architecture.png
    :align: center
    :alt: ESP Matter Solution Architecture
    :figclass: align-center

todo. Add image. Add explanation.

1.2 The Software
----------------

todo. Add some general explanation.

.. figure:: ../_static/software_components.png
    :align: center
    :alt: ESP Matter Software Components
    :figclass: align-center

The above block diagram indicates the various components of the ESP Matter SDK.

1.3 Solutions
-------------

1.3.1 Simple Device
~~~~~~~~~~~~~~~~~~~

These are typically basic/simple devices.

   -  Light Bulb
   -  Switch
   -  Temperature sensor

1.3.2 Multi-Device
~~~~~~~~~~~~~~~~~~

These are a combination of more than one simple devices.

   -  Fan with Light
   -  Temperature sensor and Proximity sensor

1.3.3 Bridge
~~~~~~~~~~~~

These devices facilitate using non-Matter devices in a Matter network.

   -  Zigbee bridge
   -  BLE mesh bridge

1.3.4 Thread Border Router
~~~~~~~~~~~~~~~~~~~~~~~~~~

These devices typically connect a Thread network to other IP-based networks, such as Wi-Fi or Ethernet.

   -  https://github.com/espressif/esp-idf/tree/master/examples/openthread/ot_br

1.3.5 Hub
~~~~~~~~~

These can be used to control other Matter devices.

   -  Touch-screen control panel
   -  Hub with internet connectivity for remote control

1.4 Examples
------------

1.4.1 Light
~~~~~~~~~~~

This application creates a Color Dimmable Light device using the ESP
Matter data model.

1.4.2 RainMaker Light
~~~~~~~~~~~~~~~~~~~~~

This application creates a Color Dimmable Light device using the ESP
Matter data model.

It also initializes ESP RainMaker which enables Device Management and
OTA using the RainMaker cloud. If user node association is done, it also
enables Remote Control through RainMaker.

1.4.3 Switch
~~~~~~~~~~~~

This application creates an On/Off Light Switch device using the ESP Matter
data model.

It creates the On/Off client and other devices can be binded to the
switch and then controlled from the switch.

1.4.4 Zap Light
~~~~~~~~~~~~~~~

This application creates a Color Dimmable Light device using the Zap
data model instead of the ESP Matter data model.

1.4.5 Zigbee Bridge
~~~~~~~~~~~~~~~~~~~

todo.

1.5 Try it yourself
-------------------

1.5.1 ESP Launchpad
~~~~~~~~~~~~~~~~~~~

This allows you to quickly try out Matter on Espressif devices through a web browser.


ESP Launchpad: https://espressif.github.io/esp-launchpad/.
