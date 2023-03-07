#ifndef __PDS_PROV_API_H__
#define __PDS_PROV_API_H__

#define PDS_STATUS          1
#define NORMAL_RUN_STATUS   0
#define PDS_MODE_PIN        4
#define PDS_WAKEUP_MS         500

void pds_mode_entry(void);
uint8_t check_whether_enter_pds(void);
#endif
