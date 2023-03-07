/**
 * This is a front header, that provides support for Radio Board currently being build for.
 *
 * @copyright 2021 Silicon Laboratories Inc.
 */

#ifndef TARGET_BOARDS_H
#define TARGET_BOARDS_H

/*************************************************************************/
/* Extension Board Definitions                                           */
/*************************************************************************/

#if defined(EXT_BOARD_8029A)
#include "extension_board_8029a.h"

#elif defined(EXT_BOARD_4001A)
#include "extension_board_4001a.h"

#endif

/*************************************************************************/
/* Radio Board Definitions                                               */
/*************************************************************************/

#if defined(RADIO_BOARD_EFR32ZG13L)
#include "radio_board_efr32zg13l.h"

#elif defined(RADIO_BOARD_EFR32ZG13P32)
#include "radio_board_efr32zg13p32.h"

#elif defined(RADIO_BOARD_EFR32ZG13P48)
#include "radio_board_efr32zg13p48.h"

#elif defined(RADIO_BOARD_EFR32ZG13S)
#include "radio_board_efr32zg13s.h"

#elif defined(RADIO_BOARD_EFR32ZG14)
#include "radio_board_efr32zg14.h"

#elif defined(RADIO_BOARD_EFR32ZG23)
#include "radio_board_efr32zg23.h"

#elif defined(RADIO_BOARD_ZGM130S)
#include "radio_board_zgm130s.h"

#elif defined(RADIO_BOARD_ZGM230SA) || defined(RADIO_BOARD_ZGM230SB)
#include "radio_board_zgm230s.h"

#elif defined(RADIO_BOARD_BRD2603A)
#include "radio_board_brd2603a.h"

#endif

#endif /* TARGET_BOARDS_H */
