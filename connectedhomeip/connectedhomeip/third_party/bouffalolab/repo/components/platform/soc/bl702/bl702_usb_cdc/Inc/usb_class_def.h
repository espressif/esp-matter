#ifndef __USB_CLASS_DEF_H
#define __USB_CLASS_DEF_H

#define CDC_IN_EP                                   0x82  /* EP2 for data IN */
#define CDC_OUT_EP                                  0x01  /* EP1 for data OUT */
#define CDC_CMD_EP                                  0x03  /* EP3 for CDC commands */

#define USB_IN_EP                                   (CDC_IN_EP&0xF)
#define USB_OUT_EP                                  (CDC_OUT_EP&0xF)
#define USB_CMD_EP                                  (CDC_CMD_EP&0xF)

#define USE_USB_EP0_IN_BUFF                         (1)
#define USE_USB_EP1_IN_BUFF                         (0)
#define USE_USB_EP2_IN_BUFF                         (1)
#define USE_USB_EP3_IN_BUFF                         (0)
#define USE_USB_EP4_IN_BUFF                         (0)
#define USE_USB_EP5_IN_BUFF                         (0)
#define USE_USB_EP6_IN_BUFF                         (0)
#define USE_USB_EP7_IN_BUFF                         (0)

#define USE_USB_EP0_OUT_BUFF                        (1)
#define USE_USB_EP1_OUT_BUFF                        (1)
#define USE_USB_EP2_OUT_BUFF                        (0)
#define USE_USB_EP3_OUT_BUFF                        (3)
#define USE_USB_EP4_OUT_BUFF                        (0)
#define USE_USB_EP5_OUT_BUFF                        (0)
#define USE_USB_EP6_OUT_BUFF                        (0)
#define USE_USB_EP7_OUT_BUFF                        (0)

#define EP0_BUFF_LEN_MAX                            (64)
#define EP1_BUFF_LEN_MAX                            (64)
#define EP2_BUFF_LEN_MAX                            (64)
#define EP3_BUFF_LEN_MAX                            (64)
#define EP4_BUFF_LEN_MAX                            (64)
#define EP5_BUFF_LEN_MAX                            (64)
#define EP6_BUFF_LEN_MAX                            (64)
#define EP7_BUFF_LEN_MAX                            (64)

#endif

