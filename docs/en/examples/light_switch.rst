Light Switch
============

1. Additional Environment Setup
-------------------------------

No additional setup is required.

2. Post Commissioning Setup
---------------------------

2.1 Bind light to switch
~~~~~~~~~~~~~~~~~~~~~~~~

Using the chip-tool, commission 2 devices, the switch and a light. Then
use the below commands to bind the light to the switch.

For the commands below:
-  Node Id of switch used during commissioning is 1
-  Node Id of light used during commissioning is 2
-  Cluster Id for OnOff cluster is 6
-  Binding cluster is currently present on endpoint 1 on the switch

Update the light's acl attribute to add the entry of remote device
(switch) in the access control list:

::

   chip-tool accesscontrol write acl '[{"fabricIndex": 1, "privilege": 5, "authMode": 2, "subjects": [ 112233, 1 ], "targets": null}]' 2 0

Update the switch's binding attribute to add the entry of remote device
(light) in the binding table:

::

   chip-tool binding write binding '[{"fabricIndex": 1, "node":2, "endpoint":1, "cluster":6}]' 1 1

2.2 Device console
~~~~~~~~~~~~~~~~~~

Switch specific console commands:

-  Send command to all the binded devices on the specified cluster: (The IDs are in hex):

   ::

      matter esp bound invoke <endpoint_id> <cluster_id> <command_id>

   -  Example: Off:

      ::

         matter esp bound invoke 0x1 0x6 0x0

   -  Example: On:

      ::

         matter esp bound invoke 0x1 0x6 0x1

   -  Example: Toggle:

      ::

         matter esp bound invoke 0x1 0x6 0x2

3. Device Performance
---------------------

3.1 CPU and Memory usage
~~~~~~~~~~~~~~~~~~~~~~~~

The following is the CPU and Memory Usage.

-  ``Bootup`` == Device just finished booting up. Device is not
   commissionined or connected to wifi yet.
-  ``After Commissioning`` == Device is conneted to wifi and is also
   commissioned and is rebooted.
-  device used: esp32c3_devkit_m
-  tested on: `bd951b8 <https://github.com/espressif/esp-matter/commit/bd951b84993d9d0b5742872be4f51bb6c9ccf15e>`__ (2022-05-05)

======================== =========== ===================
\                        Bootup      After Commissioning
======================== =========== ===================
**Free Internal Memory** 113KB       110KB
**CPU Usage**            \-          \-
======================== =========== ===================

**Flash Usage**: Firmware binary size: 1.24MB

This should give you a good idea about the amount of CPU and free memory
that is available for you to run your application's code.

A2 Appendix FAQs
----------------

A2.1 Binding Failed
~~~~~~~~~~~~~~~~~~~

My light is not getting binded to my switch:

-  Make sure the light's acl is updated. You can read it again to make
   sure it is correct:
   ``chip-tool accesscontrol read acl 2 0``.
-  If you are still facing issues, reproduce the issue on the default
   example for the device and then raise an `issue <https://github.com/espressif/esp-matter/issues>`. Make sure
   to share these:

   -  The complete device logs for both the devices taken over UART.
   -  The complete chip-tool logs.
   -  The esp-matter and esp-idf branch you are using.

A2.2 Command Send Failed
~~~~~~~~~~~~~~~~~~~~~~~~

I cannot send commands to the light from my switch:

-  Make sure the binding command was a success.
-  Make sure you are passing the local endpoint_id, and not the remote
   endpoint_id, to the cluster_update() API.
-  If using device console, make sure you are running the ``bound invoke``
   command and not the ``client invoke`` command. The client commands are for devices
   which have not been binded.
-  If you are still facing issues, reproduce the issue on the default
   example for the device and then raise an `issue <https://github.com/espressif/esp-matter/issues>`. Make sure
   to share these:

   -  The complete device logs for both the devices taken over UART.
   -  The complete chip-tool logs.
   -  The esp-matter and esp-idf branch you are using.
