/*
 * Copyright (c) 2020, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
 
/*
 * ======== Semaphore.syscfg.js ========
 */

/* global exports, system */

exports = {
    staticOnly: true,
    displayName: "Semaphore",
    moduleStatic: {
        name: "moduleGlobal",
        config: [
            {
                name: "supportsPriority",
                displayName: "Supports Priority",
                description: `Support Task priority pend queuing?`,
                longDescription: `
When set to 'true', a Semaphore can be configured to support task priority-based
queueing.

When set to 'false', all Semaphores support only first-in, first-out task queueing.

The default for this parameter is true.
                `,
                default: true
            },
            {
                name: "supportsEvents",
                displayName: "Supports Events",
                description: `Support Semaphores with Events?`,
                longDescription: `
When set to 'true', a Semaphore can be configuered with an embedded Event object
which will get 'posted' whenever Semaphore_post() is called.

When set to 'false' no Semaphores can be configured with embedded Event objects.

The default for this parameter is false.
                `,
                default: false
            }
        ]
    },
    templates: {
        bios_config_h: "/ti/sysbios/knl/Semaphore_config.h.xdt",
        "/ti/utils/rov/syscfg_c.rov.xs.xdt": "/ti/sysbios/knl/Semaphore.rov.js"
    },
    getCFiles: () => { return ["ti/sysbios/knl/Semaphore.c"] }
};

