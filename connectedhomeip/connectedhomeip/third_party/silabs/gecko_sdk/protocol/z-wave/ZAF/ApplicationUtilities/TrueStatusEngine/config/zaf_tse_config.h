/**
 * @file
 * @brief True Status Engine configuration file
 *
 * @copyright 2022 Silicon Laboratories Inc.
 */

#ifndef _ZAF_TSE_CONFIG_
#define _ZAF_TSE_CONFIG_

// <<< Use Configuration Wizard in Context Menu >>>

// <h> True Status Engine configuration parameters

// <o> Association Group Identifier. This represent the Root Device Group identifier
// <i> It is not possible to configure a Multi Channel endpoint Association Group on this module, for which the True Status Engine will report its status.
// <i> Default: 1
#define ZAF_TSE_GROUP_ID                        1

// <o> Maximum number of queued status report waiting to be reported via the Association Group
// <i> Note: This number should not be higher than the maximum number of frames that can be queued for transmission, which is set to 5
// <i> Default: 3
#define ZAF_TSE_MAXIMUM_SIMULTANEOUS_TRIGGERS   3

// <o> Delay (in ms) between the status change and queuing the report command to the transmit queue.
// <i> This setting should be as small as possible but not too small so that it would trigger network collisions.
// <i> Default: 250
#define ZAF_TSE_DELAY_TRIGGER                   250

// </h>

// <<< end of configuration section >>>

#endif // _ZAF_TSE_CONFIG_
