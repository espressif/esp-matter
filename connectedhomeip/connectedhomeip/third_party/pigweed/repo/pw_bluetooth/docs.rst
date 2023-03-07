.. _module-pw_bluetooth:

================
pw_bluetooth
================
The ``pw_bluetooth`` module contains APIs for the host layer of Bluetooth Low
Energy. The APIs are a collection of virtual interfaces that must implemented by
a BLE host stack.

.. note::
  This module is still under construction, the API is not yet stable.

Callbacks
===========
This module contains callback-heavy APIs. Callbacks must not call into the
``pw_bluetooth`` APIs unless otherwise noted. This includes calls made by
destroying objects returned by the API. Additionally, callbacks must not block.

