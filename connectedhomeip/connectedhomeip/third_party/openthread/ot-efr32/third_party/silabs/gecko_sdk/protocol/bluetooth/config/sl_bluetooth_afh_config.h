#ifndef SL_BT_AFH_CONFIG_H
#define SL_BT_AFH_CONFIG_H

#include "sl_btctrl_linklayer.h"

// <<< Use Configuration Wizard in Context Menu >>>
// <q SL_BT_CONFIG_AFH_ENABLE_AT_BOOT> Enable adaptive frequency hopping on boot
#define SL_BT_CONFIG_AFH_ENABLE_AT_BOOT     (1)
// <<< end of configuration section >>>

static inline void sli_btctrl_enable_afh()
{
  uint32_t flags = 0;
#if SL_BT_CONFIG_AFH_ENABLE_AT_BOOT == 1
  flags |= SL_BTCTRL_CHANNELMAP_FLAG_ACTIVE_ADAPTIVITY;
#endif
  sl_btctrl_init_afh(flags);
}

#endif
