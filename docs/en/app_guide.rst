9. Application User Guide
<<<<<<<<<<<<<<<<<<<<<<<<<

9.1. Delegate Implementation
============================

As per the implementation in the connectedhomeip repository, some of the clusters
require an application defined delegate to consume specific data and actions.
In order to provide this flexibity to the application, esp-matter facilitates delegate
initilization callbacks in the cluster create API. It is expected that application
will define it's data and actions in the form of delegate-impl class and set the
delegate while creating cluster/device type.

List of clusters with delegate:
    - Mode Base Cluster (all derived types of clusters).
    - Content Launch Cluster.
    - Fan Control Cluster.
    - Audio Output Cluster.
    - Energy EVSE Cluster.
    - Device Energy Management Cluster.
    - Microwave Oven Control Cluster.
    - Door Lock Cluster.
    - Messages Cluster.
    - Operational State Cluster.
    - Electrical Power Measurement Cluster.
    - Media Playback Cluster.
    - Power Topology Cluster.
    - Content App Observer Cluster.
    - Channel Cluster.
    - Resource Monitoring Cluster.
    - Application Basic Cluster.
    - Dishwasher Alarm Cluster.
    - Boolean State Configuration Cluster.
    - Laundry Dryer Controls Cluster.
    - Media Input Cluster.
    - Application Launcher Cluster.
    - Laundry Washer Controls Cluster.
    - Valve Configuration And Control Cluster.
    - Window Covering Cluster.
    - Content Control Cluster.
    - Time Synchronization Cluster.
    - Low Power Cluster.
    - Keypad Input Cluster.
    - Account Login Cluster.
    - Wake On Lan Cluster.
    - Target Navigator Cluster.
    - Mode Select Cluster.
    - Water Heater Management Cluster.
    - Energy Preference Cluster.
    - Commissioner Control Cluster.

9.1.1 Mode Base Cluster
-----------------------

It is a base cluster for ModeEVSE, ModeOven, ModeRVSRun, ModeRVSClean, ModeDishwasher,
ModeWaterHeater, ModeRefrigerator, ModeLaundryWasher and ModeMicrowaveOven.

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Mode Base`_, `Refrigeratore And TCC Mode`_
              , `Laundry Washer Mode`_
              , `Dish Washer Mode`_
              , `Rvc Run And Rvc Clean Mode`_
              , `Energy Evse Mode`_
              , `Microwave Oven Mode`_
              , `Device Energy Management Mode`_
              , `Water Heater Mode`_

9.1.2 Energy Evse Cluster
-------------------------

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Energy Evse`_, `Energy Evse Delegate`_

9.1.3 Operational State Cluster
-------------------------------

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Operational State`_, `Operational State Delegate`_

9.1.4 Microwave Oven Control Cluster
------------------------------------

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Microwave Oven Control`_, `Microwave Oven Control Delegate`_

9.1.5 Fan Control Cluster
-------------------------

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Fan Control`_, `Fan Control Delegate`_

9.1.6 Resource Monitoring Cluster
---------------------------------

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Resource Monitoring`_, `Resource Monitoring Delegate`_

9.1.7 Laundry Dryer Controls Cluster
------------------------------------

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Laundry Dryer Controls`_, `Laundry Dryer Controls Delegate`_


9.1.8 Valve Configuration And Control Cluster
---------------------------------------------

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Valve Configuration And Control`_, `Valve Configuration And Control Delegate`_

9.1.9 Device Energy Management Cluster
--------------------------------------

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Device Energy Management`_, `Device Energy Management Delegate`_

9.1.10 Door Lock Cluster
------------------------

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Door Lock`_, None

9.1.11 Boolean State Configuration Cluster
------------------------------------------

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Boolean State Configuration`_, None

9.1.12 Time Synchronization Cluster
-----------------------------------

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Time Synchronization`_, `Time Synchronization Delegate`_

9.1.13 Application Basic Cluster
--------------------------------

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Application Basic`_, None

9.1.14 Power Topology Cluster
-----------------------------

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Power Topology`_, `Power Topology Delegate`_

9.1.15 Electrical Power Measurement Cluster
-------------------------------------------

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Electrical Power Measurement`_, `Electrical Power Measurement Delegate`_

9.1.16 Laundry Washer Controls Cluster
--------------------------------------

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Laundry Washer Controls`_, `Laundry Washer Controls Delegate`_

9.1.17 Window Covering Cluster
------------------------------

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Window Covering`_, `Window Covering Delegate`_

9.1.18 Dishwasher Alarm Cluster
-------------------------------

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Dishwasher Alarm`_, `Dishwasher Alarm Delegate`_

9.1.19 Keypad Input Cluster
---------------------------

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Keypad Input`_, `Keypad Input Delegate`_

9.1.20 Mode Select Cluster
--------------------------

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Mode Select`_, `Mode Select Delegate`_

9.1.21 Water Heater Management Cluster
--------------------------------------

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Water Heater Management`_, `Water Heater Management Delegate`_

9.1.22 Energy Preference Cluster
--------------------------------

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Energy Preference`_, `Energy Preference Delegate`_

9.1.23 Commissioner Control Cluster
-----------------------------------

.. csv-table::
  :header: "Delegate Class", "Reference Implementation"

  `Commissioner Control`_, `Commissioner Control Delegate`_


.. note::
    Make sure that after implementing delegate class, you set the delegate class pointer at the time of creating cluster.

   ::

      robotic_vacuum_cleaner::config_t rvc_config;
      rvc_config.rvc_run_mode.delegate = object_of_delegate_class;
      endpoint_t *endpoint = robotic_vacuum_cleaner::create(node, & rvc_config, ENDPOINT_FLAG_NONE);

.. _`Mode Base`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/mode-base-server/mode-base-server.h
.. _`Refrigeratore And TCC Mode`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/all-clusters-app/all-clusters-common/include/tcc-mode.h
.. _`Laundry Washer Mode`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/all-clusters-app/all-clusters-common/include/laundry-washer-mode.h
.. _`Dish Washer Mode`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/all-clusters-app/all-clusters-common/include/dishwasher-mode.h
.. _`Rvc Run And Rvc Clean Mode`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/all-clusters-app/all-clusters-common/include/rvc-modes.h
.. _`Energy Evse Mode`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/energy-management-app/energy-management-common/energy-evse/include/energy-evse-modes.h
.. _`Microwave Oven Mode`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/all-clusters-app/all-clusters-common/include/microwave-oven-mode.h
.. _`Device Energy Management Mode`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/energy-management-app/energy-management-common/device-energy-management/include/device-energy-management-modes.h
.. _`Water Heater Mode`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/energy-management-app/energy-management-common/water-heater/include/water-heater-mode.h
.. _`Energy Evse`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/energy-evse-server/energy-evse-server.h
.. _`Energy Evse Delegate`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/energy-management-app/energy-management-common/energy-evse/include/EnergyEvseDelegateImpl.h
.. _`Operational State`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/operational-state-server/operational-state-server.h
.. _`Operational State Delegate`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/all-clusters-app/all-clusters-common/include/operational-state-delegate-impl.h
.. _`Microwave Oven Control`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/microwave-oven-control-server/microwave-oven-control-server.h
.. _`Microwave Oven Control Delegate`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/microwave-oven-app/microwave-oven-common/include/microwave-oven-device.h
.. _`Fan Control`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/fan-control-server/fan-control-delegate.h
.. _`Fan Control Delegate`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/all-clusters-app/all-clusters-common/src/fan-stub.cpp
.. _`Resource Monitoring`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/resource-monitoring-server/resource-monitoring-server.h
.. _`Resource Monitoring Delegate`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/all-clusters-app/all-clusters-common/include/resource-monitoring-delegates.h
.. _`Laundry Dryer Controls`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/laundry-dryer-controls-server/laundry-dryer-controls-server.h
.. _`Laundry Dryer Controls Delegate`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/all-clusters-app/all-clusters-common/include/laundry-dryer-controls-delegate-impl.h
.. _`Valve Configuration And Control`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/valve-configuration-and-control-server/valve-configuration-and-control-delegate.h
.. _`Valve Configuration And Control Delegate`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/all-clusters-app/linux/ValveControlDelegate.h
.. _`Device Energy Management`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/device-energy-management-server/device-energy-management-server.h
.. _`Device Energy Management Delegate`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/energy-management-app/energy-management-common/device-energy-management/include/DeviceEnergyManagementDelegateImpl.h
.. _`Door Lock`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/door-lock-server/door-lock-delegate.h
.. _`Boolean State Configuration`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/boolean-state-configuration-server/boolean-state-configuration-delegate.h
.. _`Time Synchronization`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/time-synchronization-server/time-synchronization-delegate.h
.. _`Time Synchronization Delegate`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/time-synchronization-server/DefaultTimeSyncDelegate.h
.. _`Application Basic`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/application-basic-server/application-basic-delegate.h
.. _`Power Topology`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/power-topology-server/power-topology-server.h
.. _`Power Topology Delegate`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/energy-management-app/energy-management-common/energy-reporting/include/PowerTopologyDelegate.h
.. _`Electrical Power Measurement`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/electrical-power-measurement-server/electrical-power-measurement-server.h
.. _`Electrical Power Measurement Delegate`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/energy-management-app/energy-management-common/energy-reporting/include/ElectricalPowerMeasurementDelegate.h
.. _`Laundry Washer Controls`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/laundry-washer-controls-server/laundry-washer-controls-server.h
.. _`Laundry Washer Controls Delegate`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/all-clusters-app/all-clusters-common/include/laundry-washer-controls-delegate-impl.h
.. _`Window Covering`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/window-covering-server/window-covering-server.h
.. _`Window Covering Delegate`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/all-clusters-app/linux/WindowCoveringManager.h
.. _`Dishwasher Alarm`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/dishwasher-alarm-server/dishwasher-alarm-server.h
.. _`Dishwasher Alarm Delegate`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/all-clusters-app/all-clusters-common/src/dishwasher-alarm-stub.cpp
.. _`Keypad Input`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/keypad-input-server/keypad-input-server.h
.. _`Keypad Input Delegate`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/chef/common/clusters/keypad-input/KeypadInputManager.h
.. _`Mode Select`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/mode-select-server/supported-modes-manager.h
.. _`Mode Select Delegate`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/all-clusters-app/all-clusters-common/include/static-supported-modes-manager.h
.. _`Water Heater Management`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/water-heater-management-server/water-heater-management-server.h
.. _`Water Heater Management Delegate`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/energy-management-app/energy-management-common/water-heater/include/WhmDelegate.h
.. _`Energy Preference`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/energy-preference-server/energy-preference-server.h
.. _`Energy Preference Delegate`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/all-clusters-app/all-clusters-common/src/energy-preference-delegate.cpp
.. _`Commissioner Control`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/src/app/clusters/commissioner-control-server/commissioner-control-server.h
.. _`Commissioner Control Delegate`: https://github.com/espressif/connectedhomeip/blob/ea679d2dc674f576f4d391d1d71af1489010e580/examples/fabric-bridge-app/linux/include/CommissionerControl.h
