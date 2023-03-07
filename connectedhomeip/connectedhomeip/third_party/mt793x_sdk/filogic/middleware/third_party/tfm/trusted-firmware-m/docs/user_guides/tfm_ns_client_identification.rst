###########################
Non-Secure Identity Manager
###########################
The ID of the current application/thread is known by TF-M, and the SST service
queries the ID of the currently running client via a dedicated API.

The identity of secure clients can be tracked by TF-M core, because it also
manages the contexts of the partitions. However to differentiate NS clients, it
relies on the services provided by the NS OS.

Tracking of context changes are possible by relying on the NS OS calling the
Thread Context Management for Armv8-M TrustZone APIs, as described
`here <https://www.keil.com/pack/doc/CMSIS/Core/html/group__context__trustzone__functions.html>`__

However TF-M needs an extra API, to assign a client ID to the TZ context created
as a result of the
``TZ_MemoryId_t TZ_AllocModuleContext_S (TZ_ModuleId_t module)`` call.

To do this, the
``enum tfm_status_e tfm_register_client_id (int32_t ns_client_id)`` have to be
called from an SVC handler, with the client ID of the currently running client.

In the current implementation of TF-M, an SVC call is provided for the NS
clients to be called at the beginning of their main function.

``SVC(SVC_TFM_NSPM_REGISTER_CLIENT_ID);``

The SVC call handler of the above SVC maps the name of the current thread to a
hardcoded client id, and sends it to the TF-M core via the earlier discussed
API.

The mapping is implemented in ``interface/src/tfm_nspm_svc_handler.c``.

The system integrators **may** implement the non-secure ID mapping based on
their application/threat model.

In case the NS OS doesn't use the Thread Context Management for Armv8-M
TrustZone APIs, then TF-M considers the NS SW as a single client, and assigns a
client ID to it automatically.

--------------

*Copyright (c) 2018-2019, Arm Limited. All rights reserved.*
