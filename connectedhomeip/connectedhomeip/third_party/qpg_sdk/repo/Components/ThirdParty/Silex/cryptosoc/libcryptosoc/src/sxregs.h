/**
 * @file
 * @brief Implement the register access API declared in "sxregs.h"
 *          for baremetal
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#ifndef SXREGS_H
#define SXREGS_H

#include <stdint.h>

#define WR_REG32(reg, value) (reg) = (value)
#define RD_REG32(reg)        (reg)

#endif
