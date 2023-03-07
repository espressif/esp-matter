#####################################
Partition and Service Design Document
#####################################

:Authors: Summer Qin
:Organization: Arm Limited
:Contact: summer.qin@arm.com
:Status: Accepted

***********
Terminology
***********
Secure Partition - A thread of execution with protected runtime state within the
Secure Processing Environment. Container for the implementation of one or more
RoT Services. Multiple Secure Partitions are allowed in a platform.
SPM - Secure Partition Manager.

***************
Design Overview
***************
As the PSA Firmware Framework described: "A Secure Partition is one execution
environment. Partition provides access to resources, protection of its own code
and data and mechanisms to interact with other components in the system. Each
Secure Partition is a single thread of execution and is the smallest unit of
isolation: if the strongest isolation level is implemented, every Secure
Partition is isolated from every other Secure Partition.
Security functionality is exposed by PSA as a collection of Root of Trust
Services. Each RoT Service is a set of related security functionality. RoT
Service are typically implemented within a Secure Partition."

******************
Partition Database
******************
Partition Database collects partition information of all existing partitions.
These partitions are built together as built-in partitions at the current stage.
Partition's code and private date are put into dedicated regions, while
Partition Database are put in TF-M SPM region. There is a defined structure for
partition information:

.. code-block:: c

    struct spm_partition_desc_t {
        struct spm_partition_runtime_data_t runtime_data;
        const struct spm_partition_static_data_t *static_data;
        const struct tfm_spm_partition_platform_data_t *platform_data;
    #if TFM_PSA_API
        const struct tfm_spm_partition_memory_data_t *memory_data;
    #endif
    };

The structure describes the partition information from four aspects and every
structure member has its own detailed members. These members can be recorded in
partition information:

- Runtime data contains runtime change-able members like context, stack and so
  on.
- Static data contains partition id, flags, priority and init function entry.
- Platform data contains a single peripheral's information this partition owns.
  This is a little different from the PSA Firmware Framework defines.
- Memory data contains partition memory region address and size.

These data types with different names also have different accessing attribute
requirements and can be put in memory with different attributes. The listed four
types of data can be defined with a different qualifier to indicate the
accessing attribute and finally, these data are linked into a global structure
type 'spm_partition_desc_t' for usage. Define the global partition_list array to
store all the partitions information and all members are set to zero so that
partition_list will be stored in bss segment. For static information, like
static member, platform member, and some memory data, assign this information
with 'const' qualifier. This would avoid involving unnecessary read-only data
into the read-write area and cause storage waste. The partition's database is
managed by the following steps:

#. The four types of data are defined with different qualifier in
   'tfm_spm_db.inc'. For example, static data is qualified with 'const' to
   indicate it is a read-only data.
#. Include partition predefined static information in spm_api.c by adding the
   tfm_spm_db.inc file.
#. Initialize the partition runtime information
#. Assign the static data, platform data and memory data to corresponding
   partition.

Partition Database Generating
=============================
Partition Database is represented as a global array that contains objects of
type spm_partition_desc_t. The include file named tfm_spm_db.inc contains this
array which is auto-generated from tfm_spm_db.inc.template by a tool script
named as 'tfm_parse_manifest_list.py'. This tool parses partition manifests and
converts members in manifest into the array member.

Partition Database Initialization
=================================
The dedicated partition information file will be added into spm initialization
file to initialize the global spm data array g_spm_partition_db.

.. code-block:: c

    #include "secure_fw/services/tfm_spm_db.inc"

Built-in Partitions
===================
Currently, there are two built-in internal partitions which have no manifest,
and all the information is statically defined. These two partitions are:
non-secure partition and core partition. They are added to the first and second
position of the array.

****************
Service Database
****************
Each Secure Partition can host one or more RoT Services. Typically, related
services that share underlying functionality or data would be implemented within
the same Secure Partition.
All the services are registered in every partition's manifest. There is a
defined structure for service information:

.. code-block:: c

    struct tfm_spm_service_t {
        const struct tfm_spm_service_db_t *service_db;
        struct spm_partition_desc_t *partition;
        struct tfm_list_node_t handle_list;
        struct tfm_msg_queue_t msg_queue;
        struct tfm_list_node_t list;
    };

These members are necessary for a service and the following bullets explain the
members:

- Service database contains service name, partition id, service signal, service
  identifier, non-secure client(if it can be called by non-secure client),
  version and version_policy.
- Partition points to the secure partition data.
- Handle list contains the handle connected to the service.
- Message queue contains the message for the service.
- List is the service list indicator. It is a double-chain list node.

The member tfm_spm_service_db_t contains statically defined service information.
This variable can be defined statically with a qualifier 'const' to put it into
the read-only data.
The service information is managed by the following steps:

#. Define five types of data with different qualifiers in
   'tfm_service_list.inc'. For example, service db is qualified with 'const' to
   indicate it is a read-only data.
#. Include service predefined static information in spm_api_ipc.c by adding the
   tfm_service_list.inc file.
#. Assign the service db to the corresponding service.
#. Get the corresponding partition information and link with the service.
#. Initialize the handle_list of every service.

--------------

*Copyright (c) 2019, Arm Limited. All rights reserved.*
