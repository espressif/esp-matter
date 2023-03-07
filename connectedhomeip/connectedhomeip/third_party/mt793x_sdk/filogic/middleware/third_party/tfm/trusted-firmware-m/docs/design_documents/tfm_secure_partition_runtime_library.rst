################################
Secure Partition Runtime Library
################################

:Organization: Arm Limited
:Contact: tf-m@lists.trustedfirmware.org

**********
Background
**********
Trusted Firmware - M (TF-M) uses a toolchain provided runtime library and
supervisor calls to easily implement the PSA Firmware Framework (PSA FF) API.
This working model works well under isolation level 1 since there are no data
isolation requirements. While TF-M is evolving, this model is not suitable
because:

  - The high-level isolation requires isolating data but some toolchain library
    interfaces have their own global data which cannot be shared between the
    Secure Partitions.
  - The toolchain libraries are designed without taking security as a core
    design principle.

A TF-M specific runtime library is needed for the following reasons:

  - Easier evaluation or certification by security standards.
  - Source code transparency.
  - Sharing code to save ROM and RAM space for TF-M.

PSA FF specification also describes the requirements of C runtime API for Secure
Partitions.

This runtime library is named the ``Secure Partition Runtime Library``, and the
abbreviation is ``SPRTL``.

****************
Design Principal
****************
The following requirements are mandatory for SPRTL implementation:

.. important::
  - **CODE ONLY** - No read-write data should be introduced into runtime library
    implementation.
  - **Thread safe** - All functions are designed with thread-safe consideration.
    These APIs access caller stack and caller provided memory only.
  - **Isolation** - Runtime API code is set as executable and read-only in
    higher isolation levels.
  - **Security first** - SPRTL is designed for security and it may come with
    some performance loss.

API Catagories
==============
Several known types of functions are included in SPRTL:

  - C runtime API.
  - RoT Service API.
  - PSA Client and Service API.
  - [Future expansion, to be detailed later] other secure API.

Security Implementation Requirements
------------------------------------
If ``malloc/realloc/free`` are provided, they must obey additional requirements
compared to the C standard: newly allocated memory must be initialized to
ZERO, and freed memory must be wiped immediately in case the block contains
sensitive data.

The comparison API ('memcmp' e.g.), they should not return immediately when the
fault case is detected. The implementation should execute in linear time based
on input to avoid execution timing side channel attack.

The pointer validation needs to be considered. In general, at least the
'non-NULL' checking is mandatory. A detection for invalid pointer leads to a
``psa_panic()``.

The following section describes the first 3 API types and the implementation
requirements.

C Runtime API
-------------
PSA FF describes a small set of the C standard library. Part of toolchain
library API can be used as default if these APIs meet the `Design Principal`_
and `Security Implementation Requirements`_. The toolchain 'header' and 'types'
can be reused to simplify the implementation.

These APIs can take the toolchain provided version, or separately implemented
in case there are extra requirements:

.. note::
  - 'memcpy()/memmove()/memset()'
  - String API

These APIs are proposed to be implemented with the security consideration
mentioned in `Security Implementation Requirements`_:

.. note::
  - 'memcmp()'
  - Other comparison API if referenced ('strcmp' e.g.).

The following functions are optional, but if present, they must conform to
additional `Security Implementation Requirements`_:

.. note::
  - 'malloc()/free()/realloc()'
  - 'assert()/printf()'

The following APIs are coupled with toolchain library much so applying toolchain
library implementation is recommended:

.. note::
  - Division and modulo - arithmetic operations.
  - Other low level or compiler specific functions (such as 'va_list').

Besides the APIs mentioned above, the following runtime APIs are required for
runtime APIs with private runtime context ('malloc' e.g.):

.. note::
  - '__sprtmain()' - partition entry runtime wrapper.

RoT Service API
---------------
The description of RoT Service API in PSA FF:

.. note::
  Arm recommends that the RoT Service developer also defines an RoT Service API
  and implementation to encapsulate the use of the IPC protocol, and improve the
  usability of the service for client firmware.

Part of the RoT Service API have proposed specifications, such as the PSA
Cryptography API, PSA Storage API, and PSA Attestation API. It is suggested that
the service developer create documents of their RoT Service API and make them
publicly available.

The RoT Service API has a large amount and it is the main part of SPRTL. This
chapter describes the general implementation of the RoT Service API and the
reason for putting them into SPRTL.

In general, a client uses the PSA Client API to access a secure service.
For example:

.. code-block:: c

  /* Example, not a real implementation */
  caller_status_t psa_example_service(void)
  {
    ...
    handle = psa_connect(SERVICE_SID, SERVICE_VERSION);
    if (INVALID_HANDLE(handle)) {
      return INVALID_RETURN;
    }

    status = psa_call(handle, type, invecs, inlen, outvecs, outlen);

    psa_close(handle);

    return TO_CALLER_STATUS(status);
  }

This example encapsulates the PSA Client API, and can be provided as a simpler
and more generic API for clients to call. It is not possible to statically link
this API to each Secure Partition because of the limited storage space. The
ideal solution is to put it inside SPRTL and share it to all Secure Partitions.
This would simplify the caller logic into this:

.. code-block:: c

  if (psa_example_service() != STATUS_SUCCESS) {
    /* do something */
  }

This is the simplest case of encapsulating PSA Client API. If a RoT Service API
is contect heavy, then, the encapsulation can be changed to include a connection
handle inside a context data structure. This context data structure type is
defined in RoT Service headers and the instance is allocated by API caller since
API implementation does not have private data.

.. note::
  - Even the RoT Service APIs are provided in SPRTL for all clients, the SPM
    performs the access check eventually and decides if the access to service
    can be processed.
  - For those RoT Service APIs only get called by a specific client, they can be
    implemented inside the caller client, instead of putting it into SPRTL.

PSA Client and Service API
--------------------------
Most of the PSA APIs can be called directly with supervisor calls. The only
special function is ``psa_call``, because it has **6** parameters. This makes
the supervisor call handler complex because it has to extract the parameters
from the stack. The definition of psa_call is the following:

.. code-block:: c

  psa_status_t psa_call(psa_handle_t handle, int32_t type,
                        const psa_invec *in_vec, size_t in_len,
                        psa_outvec *out_vec, size_t out_len);

The parameters need to be packed to avoid passing parameters on the stack, and
the supervisor call needs to unpack the parameters back to **6** for subsequent
processing.

Privileged Access Supporting
============================
Due to specified API (printf, e.g.) need to access privileged resources, TF-M
Core needs to provide interface for the resources accessing. The permission
checking must happen in Core while caller is calling these interface.

Secure Partition Scratch Area
=============================
For the API needs partition specific private data, there needs to be a way to
pass the partition specific data for the API. Use C language preprocessor to
forward the existing prototype declaration can work, but it has the risks of
breaking the build since this method needs compilers support ('-include' e.g.).
Furthermore, no valid runtime tricks can work due to these limitations on
M-profile architecture:

.. note::
  - We cannot apply the aligned mask on a stack address to get stack bottom
    where the private data pointer stands. This is because aligned stack bottom
    is not supported.
  - We cannot read special registers such as 'PSPLIMIT' for retrieving the
    private data pointer while executing in unprivileged mode.
  - Furthermore, some earlier versions of the ARM architecture do not have
    certain special-purpose registers ('PSPLIMIT' etc.).

A system-provided scratch area is a precondition for implementing APIs that need
to access private data (such as 'malloc'). The requirements for implementing
such an area are:

.. important::
  - The area must be ``READ-ONLY`` for the running Secure Partition.
  - The SPM must put the running Secure Partition's metadata into this area
    while scheduling.

With these requirements, the passed parameters can be retrieved by SPRTL easily
with a read operation on the fixed memory address.

Tooling Support on Partition Entry
==================================
PSA FF requires each Secure Partition to have an entry point. For example:

.. code-block:: c

  /* The entry point function must not return. */
  void entry_point(void);

Each partition has its own dedicated metadata for heap tracking and other
runtime state. The metadata is designed to be saved at the read-write data area
of a partition with a specific name. A generic entry point needs to be available
to get partition metadata and do initialization before calling into the actual
partition entry. This generic entry point is defined as '__sprtmain':

.. code-block:: c

    void __sprtmain(void)
    {
      /* Get current SP private data from scratch area */
      struct sprt_meta_t *m = (struct sprt_meta_t *)tfm_sprt_scratch_data;

      /* Potential heap init - check later chapter */
      if (m->heap_size) {
        m->heap_instance = tfm_sprt_heap_init(m->heap_sa, m->heap_sz);
      }

      /* Call thread entry 'entry_point' */
      m->thread_entry();

      /* SVC back to tell Core end this thread */
      SVC(THREAD_EXIT);
    }

Since SPM is not aware of the '__sprtmain' in SPRTL, it just calls into the
entry point listed in partition runtime data structure. And the partition writer
may be not aware of running of '__sprtmain' as the generic wrapper entry,
tooling support needs to happen to support this magic. Here is an example of
partition manifest:

.. code-block:: sh

  {
    "name": "TFM_SP_SERVICE",
    "type": "PSA-ROT",
    "priority": "NORMAL",
    "entry_point": "tfm_service_entry",
    "stack_size": "0x1800",
    "heap_size": "0x1000",
    ...
  }

Tooling would do manipulation to tell SPM the partition entry as '__sprtmain',
and TF-M SPM would switch the activated metadata into the scratch area. Finally,
the partition entry point gets called and run, tooling helps on the decoupling
of SPM and SPRTL implementation. The pseudo code of a tooling result:

.. code-block:: c

  struct partition_t sp1 {
    .name = "TFM_SP_SERVICE",
    .type = PSA_ROT,
    .priority = NORMAL,
    .id = 0x00000100,
    .entry_point = __sprtmain, /* Tell SPM entry is '__sprtmain' */
    .metadata = { /* struct sprt_meta_t */
      .heap_sa = sp1_heap_buf,
      .heap_sz = sizeof(sp1_heap_buf),
      .thread_entry = sp1_entry, /* Actual Partition Entry */
      .heap_instance = NULL,
    },
  }

Implementation
==============
The SPRTL C Runtime sources are put under:
'$TFM_ROOT/secure_fw/lib/sprt/'

All sources with the fixed prefix for easy symbol collecting:
'tfm\_libsprt\_c\_'

The output of this folder is a static library named as 'libtfmsprt.a'. The code
of 'libtfmsprt.a' is put into a dedicated section so that a hardware protected
region can be applied to contain it.

The RoT Service API are put under service interface folder. These APIs are
marked with the same section attribute where 'libtfmsprt.a' is put.

The Formatting API - 'printf' and variants
------------------------------------------
The 'printf' and its variants need special parameters passing mechanism. To
implement these APIs, the toolchain provided builtin macro 'va_list', 'va_start'
and 'va_end' cannot be avoided. This is because of some scenarios such as when
'stack canaries' are enabled, only the compiler knows the format of the 'canary'
in order to extract the parameters correctly.

To provide a simple implementation, the following requirements are defined for
'printf':

- Format keyword 'xXduscp' needs to be supported.
- Take '%' as escape flag, '%%' shows a '%' in the formatted string.
- To save heap usage, 32 bytes buffer in the stack for collecting formatted
  string.
- Flush string outputting due to: a) buffer full b) function ends.

The interface for flushing can be a logging device.

Function with Implied Parameters
--------------------------------
Take 'malloc' as an example. There is only one parameter for 'malloc' in
the prototype. Heap management code is put in the SPRTL for sharing with caller
partitions. The heap instance belongs to each partition, which means this
instance needs to be passed into the heap management code as a parameter. For
allocation API in heap management, it needs two parameters - 'size' and
'instance', while for 'malloc' caller it needs a 'malloc' with one parameter
'size' only. As mentioned in the upper chapter, this instance can be retrieved
from the Secure Partition scratch area. The implementation can be:

.. code-block:: c

  void *malloc(size_t sz)
  {
      struct sprt_meta_t *m = (struct sprt_meta_t *)tfm_sprt_scratch_data;

      return tfm_sprt_alloc(m->heap_instance, sz);
  }

--------------

*Copyright (c) 2019-2020, Arm Limited. All rights reserved.*
