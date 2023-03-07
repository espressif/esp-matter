#######################################
Audit Logging Service Integration Guide
#######################################

************
Introduction
************
TF-M Audit logging service allows secure services in the system to log critical
system events and information that have security implications. This is required
to post analyse the system behaviour, system events and triage system issues
offline. This offers a mitigation against the repudiation threat.

The types of information that can be logged are the ID of the entity that
originated a secure service request, or the relevant output or data
associated to the authentication mechanism that the requesting service
has performed on the entity that originated the request. The possible types of
information that can be logged can be easily extended to accommodate various
requirements from other secure services.

***************************
Current service limitations
***************************

- **Policy manager** - Currently, there is no policy manager implemented, which
  means that there are no restrictions on the entities which can add or remove
  items from the log. Also, the item replacement in the log is just replacing
  older elements first.
  
- **Encryption** - Support for encryption and authentication is not available
  yet.

- **Permanent storage** - Currently the Audit Logging service supports only a
  RAM based storage of the log, permanent storage is not supported yet.


**************
Code structure
**************
The PSA interfaces for the Audit logging service are located in
``interface/include``.

The TF-M Audit logging service source files are located in
``secure_fw/services/audit_logging``.

PSA interfaces
==============
The TF-M Audit logging service exposes the following PSA interfaces:

.. code-block:: c
    
    enum psa_audit_err psa_audit_retrieve_record(const uint32_t record_index,
        const uint32_t buffer_size, const uint8_t *token, const uint32_t token_size,
        uint8_t *buffer, uint32_t *record_size);
    
    enum psa_audit_err psa_audit_get_info(uint32_t *num_records, uint32_t
        *size);
    
    enum psa_audit_err psa_audit_get_record_info(const uint32_t record_index,
        uint32_t *size);
    
    enum psa_audit_err psa_audit_delete_record(const uint32_t record_index,
        const uint8_t *token, const uint32_t token_size);

The TF-M Audit logging service exposes an additional PSA interface which can
only be called from secure services:

.. code-block:: c

    enum psa_audit_err psa_audit_add_record(const struct psa_audit_record
        *record);

Service source files
====================

- ``audit_core.c`` : This file implements core functionalities such as log
  management, record addition and deletion and extraction of record information.
- ``audit_wrappers.c`` : This file implements TF-M compatible wrappers in case
  they are needed by the functions exported by the core.

*********************************
Audit logging service integration
*********************************
In this section, a brief description of each field of a log record is given,
with an example on how to perform a logging request from a secure service.
The secure service that requests the addition of a record to the log has to
provide data as described by the ``psa_audit_record`` type, defined in
``interface\include\psa_audit_defs.h``:

.. code-block:: c

    /*!
     * \struct psa_audit_record
     *
     * \brief This structure contains the record that is added to the audit log
     *        by the requesting secure service
     */
    struct psa_audit_record {
        uint32_t size;      /*!< Size in bytes of the id and payload fields */
        uint32_t id;        /*!< ID of the record */
        uint8_t  payload[]; /*!< Flexible array member for payload */
    };

Each field is described as follows:

- ``size`` - This is the size, in bytes, of the ``id`` and ``payload[]`` fields
  that follow. Given that the ``payload[]`` field is optional, in the current
  implementation the minimum value to be provided in ``size`` is 4 bytes;
- ``id`` - This field is meant to be used to store an ID of the log record from
  the requesting service
- ``payload[]`` - The payload is an optional content which can be made
  of one or more Type-Length-Value entries as described by the following type:

.. code-block:: c

    /*!
      * \struct audit_tlv_entry
      *
      * \brief TLV entry structure with a flexible
      *        array member
      */
     struct audit_tlv_entry {
         enum audit_tlv_type type;
         uint32_t length;
         uint8_t value[];
     };

The possible TLV types described by ``enum audit_tlv_type`` can be extended by
system integrators modifying ``audit_core.h`` as needed. A logging request is
performed by a secure service which calls the
Secure-only API function ``psa_audit_add_record()``.

--------------

*Copyright (c) 2018-2019, Arm Limited. All rights reserved.*
