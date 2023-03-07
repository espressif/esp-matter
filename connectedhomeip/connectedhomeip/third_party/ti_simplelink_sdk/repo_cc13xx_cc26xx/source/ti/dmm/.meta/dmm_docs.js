/*
 * Copyright (c) 2019 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== dmm_docs.js ========
 */

"use strict";

const module = {
    description: "DMM Policy Table Configuration",
    longDescription: `
The [__Dynamic Multi-protocol Manager (DMM)__][1] \
allows multiple wireless stacks to coexist and operate concurrently. It acts \
as an arbiter between multiple stacks and the shared RF core resource.

This module allows you to configure the DMM policy table. The DMM policy \
table makes up the complete set of scheduling rules used in the system \
depending on the current stack and application states. The policy table \
defines the role of each stack as well as which scheduling policy to use.

* [Usage Synopsis][2]
* [Examples][3]

[1]: /dmm/dmm_user_guide/dmm-users-guide.html
[2]: /dmm/dmm_user_guide/html/dmm/states-and-policies.html
[3]: /dmm/dmm_user_guide/dmm-examples-users-guide.html
`
};

/*
 *  ======== Internal Configurables ========
 */
const project = {
    description: "Internal variable for the example project name",
    longDescription: `
Internal variable that is set by the example projects. If set, the stackRoles \
configurable is set to readOnly. Otherwise, the user can change the stack \
roles if using DMM in an empty project.
`
};

const projectStackConfiguration = {
    description: "Internal variable listing stacks operating concurrently",
    longDescription: `
Internal variable that is a list of wireless stacks running on the dmm \
project. The names refer to the sysconfig module names of the corresponding \
stacks.
`
};

const lockStackRoles = {
    description: "Internal variable used to lock the stackRole configurable",
    longDescription: `
Internal variable used to set the readOnly property for the stackRole \
configurable by the .syscfg example project script.
`
};

const stackRoles = {
    description: "Wireless stacks to coexist and operate concurrently",
    longDescription: `
The Dynamic Multi-protocol Manager (DMM) \
allows multiple wireless stacks to coexist and operate concurrently. It acts \
as an arbiter between multiple stacks and the shared RF core resource.
`
};

const applicationStates = {
    description: "Configure application states"
};

const numApplicationStates = {
    description: "The number of user defined application states",
    longDescription: `
It may be required to dynamically change stack activity priority at run time \
based on the current state of the application.

__Default__: DMMPOLICY_APPSTATE_STATE

__Acceptable Values__: Accepts any valid C identifier

__Example__: After making a BLE connection, the priority of connection events \
should be increased to allow for the completion of service discovery in a \
timely manner. By defining an application state, such as \
*DMMPOLICY_BLE_HIGH_BANDWIDTH*, the DMM can be updated to increase priority \
of *DMMPOLICY_APPLIED_ACTIVITY_BLE_CONNECTION* during this time using a \
policy table entry.

___Additional Setup Required___:

Application states must be managed within the application code. \
Whenever the application changes states, the DMM should be immediatly \
notified using the \`DMMPolicy_updateApplicationState()\` API. Any additional \
application states added here must also handled by the developer within the \
application code.
`
};

const applicationState = {
    description: "User defined application state"
};

const customActivities = {
    description: "Configure custom activities"
};

const numCustomActivities = {
    description: "The number of custom activities",
    longDescription: `
In the event you wish to use a custom stack you may also want to define your \
own custom activities used in the stack.

Specified stack activities of which priority is/are adjusted by weight. All \
stack activities and their default weights are defined in the global priority \
table (GPT) in dmm_priority_{stacks}.h. Activities are defined as flags \
and OR’ed together, meaning selecting multiple will modify the priority of \
all activities selected.
`
};



/*
 *  ======== Policy Table ========
 */
const applicationStateDropDown = {
    description: "Controls when policy entry is used by scheduler"
};

const policyTable = {
    description: "Configure DMM policy table settings",
    longDescription: `
The DMM Policy table provides a service for the stack applications to update \
the priority of stack activities, which is then used to make scheduling \
decisions.

A DMM policy tells the scheduler how to increase the priority of stack \
activities and whether or not to pause an application based on defined \
states within the application. The policy structure can contain none, one or \
multiple policies depending on the application requirements. Each policy \
within the structure represents a comprehensive system state for each \
application and consists of the following parameters:

Value | Description
--- | ---
Application States | Defined as flags and OR’ed together
Activity Weights | A higher weight represents a higher priority
Paused state | Whether or not the specified stack's application is paused \
during the state
Applied activity | Specified stack activities of which priority is/are \
adjusted by weight
`
};

const stackPolicy = {
    // Intentionally left blank
    description: ""
};

const balancedMode = {
    description: "Dynamically switch priorities between the two applications",
    longDescription: `
In balanced mode, the “high priority” application, as defined by the weight, \
is given guard times to guarantee it high priority for a certain amount of \
time. The guard times consist of an On MIN and Off MAX guard time where the \
MIN time sets the minimum time window for which the application is considered \
high priority. The MAX time sets the maximum time the “high priority” \
application will be considered low priority following a switch in priorities \
between the applications.

Note: In balanced mode, it is important to distinguish between the priority \
assigned to the application in the weight field and actual priority. The \
priority set by the weight parameter only defines which application sets \
the guard time.

When the minimum on guard time has passed for the “high priority” application \
stack, the priority may be shifted following a preemption from another \
application stack. At this point, the maximum off time will take effect. \
When the maximum off time has passed, the original stack may again regain \
high priority following a preemption.

Note: If any of the guard time expires, but no preemption between the two \
application stacks occurs, the priority will not switch. Due to this, \
balanced mode is best fitted for application stacks which uses “Always in RX” \
or “RX on when idle” features such as a Zigbee router.
`
};

const balancedMinWindow = {
    description: "Min time as high priority",
    longDescription: `
Sets the minimum time window for which the application is considered high \
priority.
`
};

const balancedMaxWindow = {
    description: "Max time swapped to low priority",
    longDescription: `
Sets the maximum time the “high priority” application will be considered low \
priority following a switch in priorities between the applications. When the \
minimum on guard time has passed for the “high priority” application stack, \
the priority may be shifted following a preemption from another application \
stack.
`
};

const weight = {
    description: "Stack priority for this policy",
    longDescription: `
Sets the weighted priority adjustment amount for specified applied activities.

__Default__: 0

__Range__: 0 to 255
`
};

const pause = {
    description: "Temporarily suspend this protocol while in this state."
};

const states = {
    description: "List of application states for this policy",
    longDescription: `
The policy table entry will be used when the application is currently in one \
of the states selected here. Both wireless stacks must be in one of the states \
specified for the policy to be used.
`
};

const appliedActivity = {
    description: "Activitiy priority to modify by weight",
    longDescription: `
Specified stack activities of which priority is/are adjusted by weight. All \
stack activities and their default weights are defined in the global priority \
table (GPT) in dmm_priority_{stacks}.h. Activities are defined as flags \
and OR’ed together, meaning selecting multiple will modify the priority of \
all activities selected.

__Note__: Activity Priority = GPT (Stack level priority) + weight adjustment.

Value | Description
--- | ---
DMM_POLICY_APPLIED_ACTIVITY_NONE | Weight has no affect on applied activies.
DMM_POLICY_APPLIED_ACTIVITY_{STACK_ACTIVITY} | Only specified activity will \
be affected.
DMM_POLICY_APPLIED_ACTIVITY_ALL | Weight affects all activies defined in GPT.
`
};

const isDefineSpecific = {
    description: "Use policy entry only when define is set",
    longDescription: `
If this checkbox is selected the policy entry will be wrapped in an #ifdef \
'Define'. This means the policy entry will only be applied if the \
corresponding name is defined in the project in the .opts located at \
TOOLS/defines, in the predifined symbol list, or defined in a file included \
in ti_dmm_application_policy.c.
`
};

const buildDefine = {
    description: "Define to enable this policy entry"
};

const genLibs = {
    description: "Set this option to false in order to disable genLibs usage"
};

/*
 *  ======== exports ========
 *  Export documentation
 */
exports = {
    module: module,
    project: project,
    projectStackConfiguration: projectStackConfiguration,
    lockStackRoles: lockStackRoles,
    stackRoles: stackRoles,
    applicationStates: applicationStates,
    numApplicationStates: numApplicationStates,
    customActivities: customActivities,
    numCustomActivities: numCustomActivities,
    applicationState: applicationState,
    applicationStateDropDown: applicationStateDropDown,
    policyTable: policyTable,
    balancedMode: balancedMode,
    balancedMinWindow: balancedMinWindow,
    balancedMaxWindow: balancedMaxWindow,
    stackPolicy: stackPolicy,
    weight: weight,
    pause: pause,
    states: states,
    appliedActivity: appliedActivity,
    isDefineSpecific: isDefineSpecific,
    buildDefine: buildDefine,
    genLibs: genLibs,
};
