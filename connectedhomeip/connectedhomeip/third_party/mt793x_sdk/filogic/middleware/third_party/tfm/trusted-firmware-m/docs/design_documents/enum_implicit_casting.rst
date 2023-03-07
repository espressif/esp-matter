################################################
Fixing implicit casting for C enumeration values
################################################

:Authors: Hugues de Valon
:Organization: Arm Limited
:Contact: hugues.devalon@arm.com
:Status: Accepted

********
Abstract
********

C enumerations provide a nice way to increase readability by creating new
enumerated types but the developer should take extra care when mixing
enumeration and integer values.
This document investigates C enumerations safety and proposes strategies on how
to fix the implicit casting of the enumeration values of TF-M with other types.

**************************
C99 Standard point of view
**************************

In TF-M many implicit casts are done between integer types (``uint32_t``,
``int32_t``, ``size_t``, etc), enumerated types (``enum foobar``) and
enumeration constants (``FOOBAR_ENUM_1``).

According to the C99 standard [1]_:

    **§6.2.5, 16**:
    An enumeration comprises a set of named integer constant values. Each
    distinct enumeration constitutes a different numerated type.

    **§6.7.2.2, 2**:
    The expression that defines the value of an enumeration constant shall be an
    integer constant expression that has a value representable as an int.

    **§6.7.2.2, 3**:
    The identifiers in an enumerator list are declared as constants that have
    type int and may appear wherever such are permitted.

    **§6.7.2.2, 4**:
    Each enumerated type shall be compatible with char, a signed integer type,
    or an unsigned integer type. The choice of type is implementation-defined,
    but shall be capable of representing the values of all the members of the
    enumeration.

From these four quotes from the C99 standard [1]_, the following conclusions can
be made:

* an enumeration defines a new type and should be treated as such
* the enumeration constants must only contains value representable as an ``int``
* the enumeration constants have type ``int``
* the actual type of the enumeration can be between ``char``, signed and
  unsigned ``int``. The compiler chooses the type it wants among those that can
  represent all declared constants of the enumeration.

Example::

    enum french_cities {
        MARSEILLE,
        PARIS,
        LILLE,
        LYON
    };

In that example, ``MARSEILLE``, ``PARIS``, ``LILLE`` and ``LYON`` are
enumeration constants of type ``int`` and ``enum french_cities`` is a enumerated
type which can be of actual type ``char``, ``unsigned int`` or ``int``
(the compiler chooses!).

For these reasons, doing an implicit cast between an enumeration and another
type is the same as doing an implicit cast between two different types. From a
defensive programming point of view, it should be checked that the destination
type can represent the values from the origin type. In this specific case it
means four things for enumerations:

* it is always safe to assign an enumeration constant to an int, but might be
  better to cast to show intent.
* when casting an enumeration constant to another type, it should be checked
  that the constant can fit into the destination type.
* when casting from an integer type (``uint32_t``, ``int32_t``, etc) to an
  enumeration type, it should be checked that the integer's value is one of the
  enumeration constants. The comparison needs to be done on the biggest type of
  the two so that no information is lost. C integer promotion should
  automatically do that for the programmer (check §6.3.1.8, 1 for the rules).
* when casting from an enumeration type to an integer type, it should be checked
  that the enumeration type value fits into the integer type. The value of a
  variable which has the type of an enumeration type is not limited to the
  enumeration constants of the type. An enumeration constant will always fit
  into an ``int``.

*****************
Strategies to fix
*****************

0. Replace the enumerated type with an integer type and replace the enumeration
   constant with preprocessor constants.
1. Whenever possible, try to use matching types to avoid implicit casting.
   It happens, for example, for arithmetic operations, function calls and
   function returns. This strategy always have the lowest performance impact.
2. When using an enumeration constant in an arithmetic operation with another
   type, verify that the constant can fit in the other type and cast it.
3. When converting an integer to an enumeration type, use a conversion function
   to check if the integer matches an enumeration constant. To not impact
   performance too much, this function should be an inline function. If it does
   not match, use (or add) the error constant or return an error value.
4. When converting an enumeration type to an integer, use a conversion function
   to check that the integer type can contain the enumeration value.

************************
Design proposal for TF-M
************************

In TF-M, an action will be taken for all enumerated types and enumeration
constants that are used for implicit casting. The goal of this proposal is to
remove all implicit casting of enumeration values in TF-M.

The following enumerated types will be removed and replaced with preprocessor
constants (strategy 0). These enumerated types are not used in TF-M
but only the constants they declare.

* ``enum spm_part_state_t``
* ``enum spm_part_flag_mask_t``
* ``enum tfm_partition_priority``

The following enumerated types will be kept because they are used in the
prototypes of functions and are useful for debugging. Whenever possible,
strategy 1 will be applied to remove implicit casting related with those
enumerations but dynamic conversions will be used if the first option would
create too much change in the code base.

* ``enum tfm_status_e``: the return type of the following functions will be
  changed to return the ``enum tfm_status_e`` type. These functions are already
  returning the enumeration constants, but implicitly casted to an integer type
  like ``int32_t``.

  * ``int32_t tfm_core_set_secure_exception_priorities``
  * ``int32_t check_address_range``
  * ``int32_t has_access_to_region``
  * ``int32_t tfm_core_check_sfn_parameters``
  * ``int32_t tfm_start_partition``
  * ``int32_t tfm_return_from_partition``
  * ``int32_t tfm_check_sfn_req_integrity``
  * ``int32_t tfm_core_check_sfn_req_rules``
  * ``int32_t tfm_spm_sfn_request_handler``
  * ``int32_t tfm_spm_sfn_request_thread_mode``
* ``enum tfm_buffer_share_region_e``: the following function prototypes will be
  changed:

  * ``tfm_spm_partition_set_share(uint32_t partition_idx, uint32_t share)`` -> ``tfm_spm_partition_set_share(uint32_t partition_idx, enum tfm_buffer_share_region_e share)``
* ``enum tfm_memory_access_e``
* ``enum attest_memory_access_t``
* ``enum engine_cipher_mode_t``
* ``mbedtls_cipher_type_t``

The following enumerated types are used for error code values of Secure service
calls. They should be kept as they are part of the interface and might be used
by external parties in Non-Secure code. For the Initial Attestation service,
the enumeration is defined in the PSA Attestation API specifications.

* ``enum psa_attest_err_t``
* ``enum psa_audit_err``
* ``enum tfm_platform_err_t``

Implicit casting related with these enumerations is happening in two locations
of TF-M and need conversion functions in those locations, because the types can
not be changed:

* In the Non-Secure Client library, all of the Secure Service functions
  implicitly cast the ``uint32_t`` returned by ``tfm_ns_lock_dispatch`` to
  these enumerated types. Strategy 3 is needed here.
* In all of the veneer functions, there is an implicit cast from the ``int32_t``
  value returned by the SVC request function (``tfm_core_*_request``) to these
  enumerated types. Strategy 3 is needed here as well. The implicit cast will
  eventually be removed if all of the services are using the Uniform Signatures
  Prototypes so that the veneer functions all return ``psa_status_t`` which is
  an ``int32_t``.

If the interface of those services can be changed, these enumerations could be
removed and replaced with the ``psa_status_t`` type to remove the implicit
casting.

.. [1] C99 standard: http://www.open-std.org/jtc1/sc22/WG14/www/docs/n1256.pdf


--------------


*Copyright (c) 2019-2020, Arm Limited. All rights reserved.*

