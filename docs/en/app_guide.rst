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

9.1.1 Mode Base Cluster
-----------------------

It is a base cluster for ModeEVSE, ModeOven, ModeRVSRun, ModeRVSClean, ModeDishwasher,
ModeWaterHeater, ModeRefrigerator, ModeLaundryWasher and ModeMicrowaveOven.

.. csv-table:: Delegate and its impl
  :header: "Delegate Class", "Reference Implementation"

  `Mode Base`_, `Refrigeratore And TCC Mode`_
              , `Laundry Washer Mode`_
              , `Dish Washer Mode`_
              , `Rvc Run And Rvc Clean Mode`_
              , `Energy Evse Mode`_
              , `Microwave Oven Mode`_
              , `Device Energy Management Mode`_

9.1.2 Energy Evse Cluster
-------------------------

.. csv-table:: Delegate and its impl
  :header: "Delegate Class", "Reference Implementation"

  `Energy Evse`_, `Energy Evse Delegate`_

9.1.3 Operational State Cluster
-------------------------------

.. csv-table:: Delegate and its impl
  :header: "Delegate Class", "Reference Implementation"

  `Operational State`_, `Operational State Delegate`_

9.1.4 Microwave Oven Control Cluster
------------------------------------

.. csv-table:: Delegate and its impl
  :header: "Delegate Class", "Reference Implementation"

  `Microwave Oven Control`_, None

9.1.5 Fan Control Cluster
-------------------------

.. csv-table:: Delegate and its impl
  :header: "Delegate Class", "Reference Implementation"

  `Fan Control`_, `Fan Control Delegate`_

9.1.6 Resource Monitoring Cluster
---------------------------------

.. csv-table:: Delegate and its impl
  :header: "Delegate Class", "Reference Implementation"

  `Resource Monitoring`_, `Resource Monitoring Delegate`_

9.1.7 Laundry Dryer Controls Cluster
------------------------------------

.. csv-table:: Delegate and its impl
  :header: "Delegate Class", "Reference Implementation"

  `Laundry Dryer Controls`_, `Laundry Dryer Controls Delegate`_


9.1.8 Valve Configuration And Control Cluster
---------------------------------------------

.. csv-table:: Delegate and its impl
  :header: "Delegate Class", "Reference Implementation"

  `Valve Configuration And Control`_, None

9.1.9 Device Energy Management Cluster
--------------------------------------

.. csv-table:: Delegate and its impl
  :header: "Delegate Class", "Reference Implementation"

  `Device Energy Management`_, `Device Energy Management Delegate`_


.. note::
    Make sure that after implementing delegate class, you set the delegate class pointer at the time of creating cluster.

   ::

      robotic_vacuum_cleaner::config_t rvc_config;
      rvc_config.rvc_run_mode.delegate = object_of_delegate_class;
      endpoint_t *endpoint = robotic_vacuum_cleaner::create(node, & rvc_config, ENDPOINT_FLAG_NONE);

.. _`Mode Base`: https://github.com/project-chip/connectedhomeip/blob/master/src/app/clusters/mode-base-server/mode-base-server.h
.. _`Refrigeratore And TCC Mode`: https://github.com/project-chip/connectedhomeip/blob/master/examples/all-clusters-app/all-clusters-common/include/tcc-mode.h
.. _`Laundry Washer Mode`: https://github.com/project-chip/connectedhomeip/blob/master/examples/all-clusters-app/all-clusters-common/include/laundry-washer-mode.h
.. _`Dish Washer Mode`: https://github.com/project-chip/connectedhomeip/blob/master/examples/all-clusters-app/all-clusters-common/include/dishwasher-mode.h
.. _`Rvc Run And Rvc Clean Mode`: https://github.com/project-chip/connectedhomeip/blob/master/examples/all-clusters-app/all-clusters-common/include/rvc-modes.h
.. _`Energy Evse Mode`: https://github.com/project-chip/connectedhomeip/blob/master/examples/all-clusters-app/all-clusters-common/include/energy-evse-modes.h
.. _`Microwave Oven Mode`: https://github.com/project-chip/connectedhomeip/blob/master/examples/all-clusters-app/all-clusters-common/include/microwave-oven-mode.h
.. _`Device Energy Management Mode`: https://github.com/project-chip/connectedhomeip/blob/master/examples/energy-management-app/energy-management-common/include/device-energy-management-modes.h
.. _`Energy Evse`: https://github.com/project-chip/connectedhomeip/blob/master/src/app/clusters/energy-evse-server/energy-evse-server.h
.. _`Energy Evse Delegate`: https://github.com/project-chip/connectedhomeip/blob/master/examples/energy-management-app/energy-management-common/include/EnergyEvseDelegateImpl.h
.. _`Operational State`: https://github.com/project-chip/connectedhomeip/blob/master/src/app/clusters/operational-state-server/operational-state-server.h
.. _`Operational State Delegate`: https://github.com/project-chip/connectedhomeip/blob/master/examples/all-clusters-app/all-clusters-common/include/operational-state-delegate-impl.h
.. _`Microwave Oven Control`: https://github.com/project-chip/connectedhomeip/blob/master/src/app/clusters/microwave-oven-control-server/microwave-oven-control-server.h
.. _`Fan Control`: https://github.com/project-chip/connectedhomeip/blob/master/src/app/clusters/fan-control-server/fan-control-delegate.h
.. _`Fan Control Delegate`: https://github.com/project-chip/connectedhomeip/blob/master/examples/all-clusters-app/all-clusters-common/src/fan-stub.cpp
.. _`Resource Monitoring`: https://github.com/project-chip/connectedhomeip/blob/master/src/app/clusters/resource-monitoring-server/resource-monitoring-server.h
.. _`Resource Monitoring Delegate`: https://github.com/project-chip/connectedhomeip/blob/master/examples/all-clusters-app/all-clusters-common/include/resource-monitoring-delegates.h
.. _`Laundry Dryer Controls`: https://github.com/project-chip/connectedhomeip/blob/master/src/app/clusters/laundry-dryer-controls-server/laundry-dryer-controls-server.h
.. _`Laundry Dryer Controls Delegate`: https://github.com/project-chip/connectedhomeip/blob/master/examples/all-clusters-app/all-clusters-common/include/laundry-dryer-controls-delegate-impl.h
.. _`Valve Configuration And Control`: https://github.com/project-chip/connectedhomeip/blob/master/src/app/clusters/valve-configuration-and-control-server/valve-configuration-and-control-delegate.h
.. _`Device Energy Management`: https://github.com/project-chip/connectedhomeip/blob/master/src/app/clusters/device-energy-management-server/device-energy-management-server.h
.. _`Device Energy Management Delegate`: https://github.com/project-chip/connectedhomeip/blob/master/examples/energy-management-app/energy-management-common/include/DeviceEnergyManagementDelegateImpl.h
