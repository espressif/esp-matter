#######################
Adding Secure Partition
#######################

***********************
Terms and abbreviations
***********************
This document uses the following terms and abbreviations.

.. table:: term table
   :widths: auto

   ================== ==================================
    **Term**          **Meaning**
   ================== ==================================
   FF                 Firmware Framework
   ID                 Identifier
   IPC                Interprocess communication
   IPC model          The secure IPC framework
   irqs               Interrupt requests
   Library model      The secure function call framework
   MMIO               Memory Mapped I/O
   PSA                Platform Security Architecture
   RoT                Root of Trust
   SID                RoT Service ID
   SP                 Secure Partition
   SPM                Secure Partition Manager
   TF-M               Trusted firmware M
   ================== ==================================

************
Introduction
************
Secure Partition is an execution environment that provides the following
functions to Root of Trust (RoT) Services:

- Access to resources, protection of its own code and data.
- Mechanisms to interact with other components in the system.

Each Secure Partition is a single thread of execution and the smallest unit of
isolation.

This document mainly describes how to add a secure partition in TF-M and
focuses on the configuration, manifest, implement rules. The actual
source-level implementation is not included in this document.

.. Note::
   If not otherwise specified, the steps are identical for library and IPC
   model.

   The IPC model conforms the *PSA Firmware Framework (FF) v 1.0.0*. Refer to
   `PSA Firmware Framework specification`_ for details.

*******
Process
*******
The main steps to add a secure partition are as follows:

- `Add source folder`_
- `Add manifest`_
- `Add configuration`_
- `Generate files`_
- `Implement the RoT services`_

Add source folder
=================
Add a source folder under ``<TF-M base folder>/secure_fw/services`` for the new
secure partition (Let's take EXAMPLE as the folder name):

This folder should include those parts:

- Manifest file: EXAMPLE.yaml
- CMake configuration files
- Source code files

Add manifest
============
Each Secure Partition must have resource requirements declared in a manifest
file. The Secure Partition Manager (SPM) uses the manifest file to assemble and
allocate resources within the SPE. The manifest includes the following:

- A Secure Partition name.
- A list of implemented RoT Services.
- Access to other RoT Services.
- Memory requirements.
- Scheduling hints.
- Peripheral memory-mapped I/O regions and interrupts.

.. Note::
   The current manifest format in TF-M is "yaml" which is different from the
   requirement of PSA FF.

Here is a manifest reference example for the IPC model, please refer to
`Library model support`_ for the library extend:

.. code-block:: yaml

  {
    "psa_framework_version": 1.0,
    "name": "TFM_SP_EXAMPLE",
    "type": "PSA-ROT",
    "priority": "HIGH",
    "entry_point": "EXAMPLE_main",
    "stack_size": "0x0200",
    "services" : [
      {
        "name": "ROT_A",
        "sid": "0x0000F000",
        "non_secure_clients": true,
        "version": 1,
        "version_policy": "STRICT"
      }
    ],
    "mmio_regions": [
      {
        "name": "TFM_PERIPHERAL_A",
        "permission": "READ-WRITE"
      }
    ],
    "irqs": [
      {
        "source": "TFM_A_IRQ",
        "signal": "SPM_CORE_A_IRQ",
        "tfm_irq_priority": 64,
      }
    ],
    "linker_pattern": {
      "object_list": [
        "*EXAMPLE.*"
      ]
    }
  }

Secure Partition ID Distribution
--------------------------------
Every Secure Partition has an identifier (ID). TF-M will generate a header file
that includes definitions of the Secure Partition IDs. The header file is
``<TF-M base folder>/interface/include/psa_manifest/pid.h``. Each definition
uses the ``name`` attribute in the manifest as its name and the value is
allocated by SPM.

.. code-block:: c

   #define name id-value

Here is the Secure Partition ID table used in TF-M.

.. table:: PID table
   :widths: auto

   =============================== =================
    **Partition name**              **Partition ID**
   =============================== =================
   Reserved                        0-255
   TFM_SP_STORAGE                  256
   TFM_SP_ITS                      257
   TFM_SP_AUDIT_LOG                258
   TFM_SP_CRYPTO                   259
   TFM_SP_PLATFORM                 260
   TFM_SP_INITIAL_ATTESTATION      261
   TFM_SP_CORE_TEST                262
   TFM_SP_CORE_TEST_2              263
   TFM_SP_SECURE_TEST_PARTITION    264
   TFM_SP_IPC_SERVICE_TEST         265
   TFM_SP_IPC_CLIENT_TEST          266
   TFM_IRQ_TEST_1                  267
   TFM_SP_SST_TEST                 268
   =============================== =================

About where to add the definition, please refer to the chapter `Add
configuration`_.

RoT Service ID (SID) Distribution
---------------------------------
An RoT Service is identified by its RoT Service ID (SID). A SID is a 32-bit
number that is associated with a symbolic name in the Secure Partition
manifest. The bits [31:12] uniquely identify the vendor of the RoT Service.
The remaining bits [11:0] can be used at the discretion of the vendor.

Here is the RoT Service ID table used in TF-M.

.. table:: SID table
   :widths: auto

   =========================== ====================== ========================
   **Services**                **Vendor ID(20 bits)** **Function ID(12 bits)**
   =========================== ====================== ========================
   audit_logging               0x00000                0x000-0x01F
   initial_attestation         0x00000                0x020-0x03F
   platform                    0x00000                0x040-0x05F
   secure_storage              0x00000                0x060-0x07F
   crypto                      0x00000                0x080-0x09F
   internal_trusted_storage    0x00000                0x0A0-0x0BF
   test_secure_service         0x0000F                0x000-0x01F
   core_test                   0x0000F                0x020-0x03F
   core_test_2                 0x0000F                0x040-0x05F
   tfm_ipc_client              0x0000F                0x060-0x07F
   tfm_ipc_service             0x0000F                0x080-0x09F
   tfm_irq_test_service_1      0x0000F                0x0A0-0x0BF
   tfm_sst_test_service        0x0000F                0x0C0-0x0DF
   =========================== ====================== ========================

mmio_regions
------------
This attribute is a list of MMIO region objects which the Secure Partition
needs access to. TF-M only supports the ``named_region`` current. Please refer
to PSA FF for more details about it. The user needs to provide a name macro to
indicate the variable of the memory region.

TF-M uses the below structure to indicate a peripheral memory.

.. code-block:: c

  struct tfm_spm_partition_platform_data_t {
    uint32_t periph_start;
    uint32_t periph_limit;
    int16_t periph_ppc_bank;
    int16_t periph_ppc_loc;
  };

.. Note::
   This structure is not expected by TF-M, it's only that the current
   implementations are using. Other peripherals that need different information
   to create isolation need to define a different structure with the same name.

Here is a example for it:

.. code-block:: c

   struct tfm_spm_partition_platform_data_t tfm_peripheral_A;
   #define TFM_PERIPHERAL_A                 (&tfm_peripheral_A)

linker_pattern
--------------
``linker_pattern`` is a legacy region which contains the minimum information
required to link a Secure Partition’s compiled static objects. Now, it is
required as 'IMPLEMENTATION DEFINED' in PSA FF 1.0.0.

Library model support
---------------------
For the library model, the user needs to add a ``secure_functions`` item. The
main difference between ``secure_function`` and ``services`` is the extra
``signal`` key for secure function entry.

The ``signal`` must be the upper case of the secure function name.

.. code-block:: yaml

  "secure_functions": [
    {
      "name": "TFM_EXAMPLE_A",
      "signal": "EXAMPLE_A_FUNC",
      "sid": "0x00000000",
      "non_secure_clients": true,
      "version": 1,
      "version_policy": "STRICT"
    },

Add configuration
=================
The following configuration tasks are required for the newly added secure
partition:

Add CMake configure files
-------------------------
Two CMake configure files need to be added:

- CMakeLists.inc, which is used to add the definition of what files are needed
  to build.
- CMakeLists.txt, which is the compilation configuration for this module.

.. Note::
   The CMakeLists.inc is not mandatory, the user can put everything in
   CMakeLists.txt.

Please refer to the source code of TF-M for more detail.

Update manifest list
--------------------
The ``<TF-M base folder>/tools/tfm_manifest_list.yaml`` is used to collect
necessary information of secure partition.

- ``name``: The name string of the secure partition.
- ``short_name``: should be the same as the ``name`` in the secure partition
  manifest file.
- ``manifest``: the relative path of the manifest file to TF-M root.
- ``tfm_partition_ipc``: indicate if this partition is compatible with the IPC
  model.
- ``conditional``: Optional. Configure control macro for this partition.
- ``version_major``: major version the partition manifest.
- ``version_minor``: minor version the partition manifest.
- ``pid``: Secure Partition ID value distributed in chapter `Secure Partition
  ID Distribution`_.

Reference configuration example:

.. code-block:: yaml

    {
      "name": "Example Service",
      "short_name": "TFM_SP_EXAMPLE",
      "manifest": "secure_fw/services/EXAMPLE/tfm_example.yaml",
      "tfm_extensions": true,
      "tfm_partition_ipc": true,
      "conditional": "TFM_PARTITION_EXAMPLE_ENABLE",
      "version_major": 0,
      "version_minor": 1,
      "pid": 256
    }

Generate files
==============
After finishing the configuration works, the user needs to generate necessary
files from manifest by using TF-M tools.

.. code-block:: bash

   cd <TF-M base folder>
   cd trusted-firmware-m
   python ./tools/tfm_parse_manifest_list.py

Implement the RoT services
==========================
The following not-binding rules, as currently implemented, can be used as
guidelines:

- In the IPC model, Use PSA FF proposed memory accessing mechanism. SPM
  provides APIs and checking between isolation boundaries, a free accessing
  of memory can cause program panic.
- In the IPC model, the memory checking inside partition runtime is
  unnecessary. SPM handles the checking while memory accessing APIs are
  called.
- In the IPC model, the client ID had been included in the message structure
  and secure partition can get it when calling psa_get() function. The secure
  partition does not need to call ``tfm_core_get_caller_client_id()`` to get
  the caller client ID anymore.
- In the IPC model, SPM will check the security policy and partition
  dependence between client and service. So the service does not need to
  validate the secure caller anymore.

*********
Reference
*********

| `PSA Firmware Framework specification`_

.. _PSA Firmware Framework specification: https://pages.arm.com/psa-
  resources-ff.html?_ga=2.156169596.61580709.1542617040-1290528876.1541647333

--------------

*Copyright (c) 2019-2020, Arm Limited. All rights reserved.*