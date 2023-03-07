/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef AOS_EVENT_TYPE_CODE_API_H
#define AOS_EVENT_TYPE_CODE_API_H

/* remote procedure call */
#define EV_RPC 0x0100

/* YunIO event */
#define EV_YUNIO 0x0101
#define CODE_YUNIO_CMD_START 1
#define CODE_YUNIO_CMD_RETRY 2
#define CODE_YUNIO_CMD_DISCONNECT 3
#define CODE_YUNIO_ON_CONNECTED 4
#define CODE_YUNIO_ON_DISCONNECTED 5
#define CODE_YUNIO_ON_CONNECT_FAILED 6
#define CODE_YUNIO_ON_READ_ERROR 7
#define CODE_YUNIO_ON_WRITE_ERROR 8
#define CODE_YUNIO_ON_HEARTBIT_ERROR 9
#define CODE_YUNIO_PUBLISH_SUCCESS 10
#define CODE_YUNIO_PUBLISH_TIMEOUT 11

/** OTA event define */
#define EV_OTA 0x0102
#define CODE_OTA_ON_RESULT 1

/** register device service */
#define EV_DEVICE_REGISTER 0x0103
#define CODE_REGISTER_ON_RESULT 1
#define VALUE_REGISTER_SUCCESS 0

/** id2 device activate */
#define EV_ACTIVATE_ID2 0x0104
#define CODE_ACTIVATE_ID2_ON_RESULT 1
#define VALUE_ACTIVATE_ID2_SUCCESS 0
#define VALUE_ACTIVATE_ID2_FAILURE 1

#define EV_DDA 0x0105
#define CODE_DDA_ON_CONNECTED 1

#define EV_BZ_COMBO 0x0106
#define CODE_COMBO_AP_INFO_READY 1

#define EV_BZ_AUTH  0x0107
#define EV_BZ_TRANS 0x0108
#define EV_BZ_EXT   0x0109
#define EV_BZ_GAP   0x0110

#if 0
/** General key define */
#define EV_KEY 0x0201
#define CODE_RECORD 1
#define CODE_VOLUME 2 /* Reserve */
#define CODE_VOLUME_INC 3
#define CODE_VOLUME_DEC 4
#define CODE_PLAY_PAUSE 5
#define CODE_MUTE 6
#define CODE_CHANNEL 7
#define CODE_NEXT 8
#define CODE_RECORD_PRE 9
#define CODE_RESET 0x1001
#define CODE_STATUS 11
#define CODE_ELINK 12
#define CODE_BOOT 13
#endif

/** General key define */
#define EV_KEY               0x0201
#define KEY_RESERVED         0
#define KEY_ESC              1
#define KEY_1                2
#define KEY_2                3
#define KEY_3                4
#define KEY_4                5
#define KEY_5                6
#define KEY_6                7
#define KEY_7                8
#define KEY_8                9
#define KEY_9                10
#define KEY_0                11
#define KEY_MINUS            12
#define KEY_EQUAL            13
#define KEY_BACKSPACE        14
#define KEY_TAB              15
#define KEY_Q                16
#define KEY_W                17
#define KEY_E                18
#define KEY_R                19
#define KEY_T                20
#define KEY_Y                21
#define KEY_U                22
#define KEY_I                23
#define KEY_O                24
#define KEY_P                25
#define KEY_LEFTBRACE        26
#define KEY_RIGHTBRACE       27
#define KEY_ENTER            28
#define KEY_LEFTCTRL         29
#define KEY_A                30
#define KEY_S                31
#define KEY_D                32
#define KEY_F                33
#define KEY_G                34
#define KEY_H                35
#define KEY_J                36
#define KEY_K                37
#define KEY_L                38
#define KEY_SEMICOLON        39
#define KEY_APOSTROPHE       40
#define KEY_GRAVE            41
#define KEY_LEFTSHIFT        42
#define KEY_BACKSLASH        43
#define KEY_Z                44
#define KEY_X                45
#define KEY_C                46
#define KEY_V                47
#define KEY_B                48
#define KEY_N                49
#define KEY_M                50
#define KEY_COMMA            51
#define KEY_DOT              52
#define KEY_SLASH            53
#define KEY_RIGHTSHIFT       54
#define KEY_KPASTERISK       55
#define KEY_LEFTALT          56
#define KEY_SPACE            57
#define KEY_CAPSLOCK         58
#define KEY_F1               59
#define KEY_F2               60
#define KEY_F3               61
#define KEY_F4               62
#define KEY_F5               63
#define KEY_F6               64
#define KEY_F7               65
#define KEY_F8               66
#define KEY_F9               67
#define KEY_F10              68
#define KEY_NUMLOCK          69
#define KEY_SCROLLLOCK       70
#define KEY_KP7              71
#define KEY_KP8              72
#define KEY_KP9              73
#define KEY_KPMINUS          74
#define KEY_KP4              75
#define KEY_KP5              76
#define KEY_KP6              77
#define KEY_KPPLUS           78
#define KEY_KP1              79
#define KEY_KP2              80
#define KEY_KP3              81
#define KEY_KP0              82
#define KEY_KPDOT            83

#define KEY_ZENKAKUHANKAKU   85
#define KEY_102ND            86
#define KEY_F11              87
#define KEY_F12              88
#define KEY_RO               89
#define KEY_KATAKANA         90
#define KEY_HIRAGANA         91
#define KEY_HENKAN           92
#define KEY_KATAKANAHIRAGANA 93
#define KEY_MUHENKAN         94
#define KEY_KPJPCOMMA        95
#define KEY_KPENTER          96
#define KEY_RIGHTCTRL        97
#define KEY_KPSLASH          98
#define KEY_SYSRQ            99
#define KEY_RIGHTALT         100
#define KEY_LINEFEED         101
#define KEY_HOME             102
#define KEY_UP               103
#define KEY_PAGEUP           104
#define KEY_LEFT             105
#define KEY_RIGHT            106
#define KEY_END              107
#define KEY_DOWN             108
#define KEY_PAGEDOWN         109
#define KEY_INSERT           110
#define KEY_DELETE           111
#define KEY_MACRO            112
#define KEY_MUTE             113
#define KEY_VOLUMEDOWN       114
#define KEY_VOLUMEUP         115
#define KEY_POWER            116 /* SC System Power Down */
#define KEY_KPEQUAL          117
#define KEY_KPPLUSMINUS      118
#define KEY_PAUSE            119
#define KEY_SCALE            120 /* AL Compiz Scale (Expose) */

#define KEY_KPCOMMA          121
#define KEY_HANGEUL          122
#define KEY_HANGUEL          KEY_HANGEUL
#define KEY_HANJA            123
#define KEY_YEN              124
#define KEY_LEFTMETA         125
#define KEY_RIGHTMETA        126
#define KEY_COMPOSE          127

#define KEY_STOP             128 /* AC Stop */
#define KEY_AGAIN            129
#define KEY_PROPS            130 /* AC Properties */
#define KEY_UNDO             131 /* AC Undo */
#define KEY_FRONT            132
#define KEY_COPY             133 /* AC Copy */
#define KEY_OPEN             134 /* AC Open */
#define KEY_PASTE            135 /* AC Paste */
#define KEY_FIND             136 /* AC Search */
#define KEY_CUT              137 /* AC Cut */
#define KEY_HELP             138 /* AL Integrated Help Center */
#define KEY_MENU             139 /* Menu (show menu) */
#define KEY_CALC             140 /* AL Calculator */
#define KEY_SETUP            141
#define KEY_SLEEP            142 /* SC System Sleep */
#define KEY_WAKEUP           143 /* System Wake Up */
#define KEY_FILE             144 /* AL Local Machine Browser */
#define KEY_SENDFILE         145
#define KEY_DELETEFILE       146
#define KEY_XFER             147
#define KEY_PROG1            148
#define KEY_PROG2            149
#define KEY_WWW              150 /* AL Internet Browser */
#define KEY_MSDOS            151
#define KEY_COFFEE           152 /* AL Terminal Lock/Screensaver */
#define KEY_SCREENLOCK       KEY_COFFEE
#define KEY_ROTATE_DISPLAY   153 /* Display orientation for e.g. tablets */
#define KEY_DIRECTION        KEY_ROTATE_DISPLAY
#define KEY_CYCLEWINDOWS     154
#define KEY_MAIL             155
#define KEY_BOOKMARKS        156 /* AC Bookmarks */
#define KEY_COMPUTER         157
#define KEY_BACK             158 /* AC Back */
#define KEY_FORWARD          159 /* AC Forward */
#define KEY_CLOSECD          160
#define KEY_EJECTCD          161
#define KEY_EJECTCLOSECD     162
#define KEY_NEXTSONG         163
#define KEY_PLAYPAUSE        164
#define KEY_PREVIOUSSONG     165
#define KEY_STOPCD           166
#define KEY_RECORD           167
#define KEY_REWIND           168
#define KEY_PHONE            169 /* Media Select Telephone */
#define KEY_ISO              170
#define KEY_CONFIG           171 /* AL Consumer Control Configuration */
#define KEY_HOMEPAGE         172 /* AC Home */
#define KEY_REFRESH          173 /* AC Refresh */
#define KEY_EXIT             174 /* AC Exit */
#define KEY_MOVE             175
#define KEY_EDIT             176
#define KEY_SCROLLUP         177
#define KEY_SCROLLDOWN       178
#define KEY_KPLEFTPAREN      179
#define KEY_KPRIGHTPAREN     180
#define KEY_NEW              181 /* AC New */
#define KEY_REDO             182 /* AC Redo/Repeat */
#define KEY_F13              183
#define KEY_F14              184
#define KEY_F15              185
#define KEY_F16              186
#define KEY_F17              187
#define KEY_F18              188
#define KEY_F19              189
#define KEY_F20              190
#define KEY_F21              191
#define KEY_F22              192
#define KEY_F23              193
#define KEY_F24              194

#define KEY_PLAYCD           200
#define KEY_PAUSECD          201
#define KEY_PROG3            202
#define KEY_PROG4            203
#define KEY_DASHBOARD        204 /* AL Dashboard */
#define KEY_SUSPEND          205
#define KEY_CLOSE            206 /* AC Close */
#define KEY_PLAY             207
#define KEY_FASTFORWARD      208
#define KEY_BASSBOOST        209
#define KEY_PRINT            210 /* AC Print */
#define KEY_HP               211
#define KEY_CAMERA           212
#define KEY_SOUND            213
#define KEY_QUESTION         214
#define KEY_EMAIL            215
#define KEY_CHAT             216
#define KEY_SEARCH           217
#define KEY_CONNECT          218
#define KEY_FINANCE          219 /* AL Checkbook/Finance */
#define KEY_SPORT            220
#define KEY_SHOP             221
#define KEY_ALTERASE         222
#define KEY_CANCEL           223 /* AC Cancel */
#define KEY_BRIGHTNESSDOWN   224
#define KEY_BRIGHTNESSUP     225
#define KEY_MEDIA            226

#define KEY_SWITCHVIDEOMODE  227 /* Cycle between available video
                       outputs (Monitor/LCD/TV-out/etc) */
#define KEY_KBDILLUMTOGGLE   228
#define KEY_KBDILLUMDOWN     229
#define KEY_KBDILLUMUP       230
#define KEY_SEND             231 /* AC Send */
#define KEY_REPLY            232 /* AC Reply */
#define KEY_FORWARDMAIL      233 /* AC Forward Msg */
#define KEY_SAVE             234 /* AC Save */
#define KEY_DOCUMENTS        235

#define KEY_BATTERY          236

#define KEY_BLUETOOTH        237
#define KEY_WLAN             238
#define KEY_UWB              239
#define KEY_UNKNOWN          240

#define KEY_VIDEO_NEXT       241 /* drive next video source */
#define KEY_VIDEO_PREV       242 /* drive previous video source */
#define KEY_BRIGHTNESS_CYCLE 243 /* brightness up, after max is min */
#define KEY_BRIGHTNESS_AUTO  244 /* Set Auto Brightness: manual
                      brightness control is off,
                      rely on ambient */
#define KEY_BRIGHTNESS_ZERO  KEY_BRIGHTNESS_AUTO
#define KEY_DISPLAY_OFF      245 /* display device to off state */

#define KEY_WWAN             246 /* Wireless WAN (LTE, UMTS, GSM, etc.) */
#define KEY_WIMAX            KEY_WWAN
#define KEY_RFKILL           247 /* Key that controls all radios */

#define KEY_MICMUTE          248 /* Mute / unmute the microphone */

/** General key value */
#define VALUE_KEY_UP 0
#define VALUE_KEY_DOWN 1
#define VALUE_KEY_CLICK 2
#define VALUE_KEY_LTCLICK 3
#define VALUE_KEY_LLTCLICK 4
#define VALUE_KEY_DBCLICK 5
/** General channel value */
#define VALUE_SYS_CHN_CONNECTED 0
#define VALUE_SYS_CHN_CONNECTING 1
#define VALUE_SYS_CHN_DISCONNECTED 2

/** Reserve event */
#define EV_KNOD 0x0203

/** SD card event */
#define EV_SD 0x0204
#define CODE_SD_PLUG 1
#define VALUE_SD_OUT 0
#define VALUE_SD_IN 1

/** LAN Driver event */
#define EV_LAN 0x0205
#define VALUE_LAN_OUT 0
#define VALUE_LAN_IN 1

/** Net event define */
#define EV_NET 0x0206
#define CODE_NET_DHCP_START 1
#define CODE_NET_DHCP_RESULT 2
#define CODE_NET_IP_STATIC 3
#define CODE_NET_STATUS 4
#define VALUE_NET_LAN_OK 0x01
#define VALUE_NET_WIFI_OK 0x02

/** Usb driver event */
#define EV_USB 0x0207
#define VALUE_USB_OUT 0
#define VALUE_USB_IN 1

/** PM event */
#define EV_PM 0x0208
#define CODE_PM_ENTER_INFORM 1
#define CODE_PM_ENTER 2
#define CODE_PM_DONE 3
#define CODE_PM_QUIT 4
#define CODE_PM_SHUTDOWN_INFORM 5
#define CODE_PM_STARTUP_INFORM 6
#define CODE_PM_LOWPOWER_INFORM 7
#define CODE_PM_POWERRECOVER_INFORM 8
#define CODE_PM_START_HEARTBEAT_INFORM 9
#define CODE_PM_STOP_HEARTBEAT_INFORM 10
#define VALUE_PM_IDLE_INFORM 1
#define VALUE_PM_POWERBUTTON_INFORM 2

/** File system event */
#define EV_FS 0x0209
#define CODE_FSYS_SD_LOAD 1
#define CODE_FSYS_UDISK_LOAD 2
#define CODE_FSYS_FLASH_LOAD 3
#define CODE_FSYS_SD_SPACE 4
#define CODE_FSYS_UDISK_SPACE 5
#define CODE_FSYS_FLASH_SPACE 6

/** Bluetooth */
#define EV_BT 0x020A
#define CODE_BT_DEVICE_CONNECT 1
#define CODE_BT_DEVICE_DISCONNECT 2
#define CODE_BT_MODE_ON 3
#define CODE_BT_MODE_OFF 4

/** DLNA */
#define EV_DLNA 0x020B
#define CODE_DLNA_DEVICE_CONNECT 1
#define CODE_DLNA_DEVICE_DISCONNECT 2
#define CODE_DLNA_MODE_ON 3
#define CODE_DLNA_MODE_OFF 4

/** AIRPLAY */
#define EV_AIRPLAY 0x020C
#define CODE_AIRPLAY_DEVICE_CONNECT 1
#define CODE_AIRPLAY_DEVICE_DISCONNECT 2
#define CODE_AIRPLAY_MODE_ON 3
#define CODE_AIRPLAY_MODE_OFF 4

/** AT */
#define EV_AT 0x020D
#define CODE_AT_IF_READY 1
#define CODE_AT_IF_DISAPPEAR 2

/** AT */
#define EV_BLE 0x020E


/** ADC */
#define EV_ADC 0x020F
#define CODE_ADC_DATA 1
#define CODE_ADC_POS_SATURATION 2
#define CODE_ADC_NEG_SATURATION 3
#define CODE_ADC_FIFO_UNDERRUN 4
#define CODE_ADC_FIFO_OVERRUN 5


/** I2C */
#define EV_I2C 0x020F

#define CODE_I2C_END 1
#define CODE_I2C_ARB 2          
#define CODE_I2C_NAK 3      
#define CODE_I2C_FER 4


#endif /* AOS_EVENT_TYPE_CODE_API_H */
