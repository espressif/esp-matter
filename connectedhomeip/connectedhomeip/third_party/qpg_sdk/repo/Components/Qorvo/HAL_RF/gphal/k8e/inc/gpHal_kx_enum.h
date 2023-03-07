#ifndef GP_WB_ENUM_H
#define GP_WB_ENUM_H

/***************************
 * layout: general_enum
 ***************************/
/* serial_itf_select  */
#define GP_WB_ENUM_SERIAL_ITF_SELECT_USE_SPI         0x0
#define GP_WB_ENUM_SERIAL_ITF_SELECT_USE_I2C         0x1
#define GP_WB_ENUM_SERIAL_ITF_SELECT_USE_UART        0x2
#define GP_WB_ENUM_SERIAL_ITF_SELECT_NO_INTERFACE    0x3

/* nvm_size  */
#define GP_WB_ENUM_NVM_SIZE_KB1024    0x0
#define GP_WB_ENUM_NVM_SIZE_KB896     0x1
#define GP_WB_ENUM_NVM_SIZE_KB768     0x2
#define GP_WB_ENUM_NVM_SIZE_KB640     0x3
#define GP_WB_ENUM_NVM_SIZE_KB512     0x4
#define GP_WB_ENUM_NVM_SIZE_KB384     0x5
#define GP_WB_ENUM_NVM_SIZE_KB256     0x6
#define GP_WB_ENUM_NVM_SIZE_KB128     0x7

/* pbm_vq  */
#define GP_WB_ENUM_PBM_VQ_UNTIMED    0x1
#define GP_WB_ENUM_PBM_VQ_SCHED0     0x2
#define GP_WB_ENUM_PBM_VQ_SCHED1     0x3
#define GP_WB_ENUM_PBM_VQ_SCHED2     0x4
#define GP_WB_ENUM_PBM_VQ_SCHED3     0x5

/* pbm_return_code  */
#define GP_WB_ENUM_PBM_RETURN_CODE_FREE           0x0
#define GP_WB_ENUM_PBM_RETURN_CODE_NOT_DONE       0x1
#define GP_WB_ENUM_PBM_RETURN_CODE_TX_SUCCESS     0x2
#define GP_WB_ENUM_PBM_RETURN_CODE_RX_SUCCESS     0x3
#define GP_WB_ENUM_PBM_RETURN_CODE_CCA_FAILURE    0x4
#define GP_WB_ENUM_PBM_RETURN_CODE_NO_ACK         0x5
#define GP_WB_ENUM_PBM_RETURN_CODE_TX_BUSY        0x6
#define GP_WB_ENUM_PBM_RETURN_CODE_TX_ABORTED     0x7

/* standby_state  */
#define GP_WB_ENUM_STANDBY_STATE_RESET                  0x0
#define GP_WB_ENUM_STANDBY_STATE_ACTIVE                 0x1
#define GP_WB_ENUM_STANDBY_STATE_READY_FOR_POWER_OFF    0x2
#define GP_WB_ENUM_STANDBY_STATE_SLEEP                  0x3

/* ext_clkfreq  */
#define GP_WB_ENUM_EXT_CLKFREQ_MHZ1     0x0
#define GP_WB_ENUM_EXT_CLKFREQ_MHZ2     0x1
#define GP_WB_ENUM_EXT_CLKFREQ_MHZ4     0x2
#define GP_WB_ENUM_EXT_CLKFREQ_MHZ8     0x3
#define GP_WB_ENUM_EXT_CLKFREQ_MHZ16    0x4

/* ssp_mode  */
#define GP_WB_ENUM_SSP_MODE_ENCRYPT    0x0
#define GP_WB_ENUM_SSP_MODE_DECRYPT    0x1
#define GP_WB_ENUM_SSP_MODE_AES        0x2

/* ssp_key_len  */
#define GP_WB_ENUM_SSP_KEY_LEN_KEY_128    0x0
#define GP_WB_ENUM_SSP_KEY_LEN_KEY_192    0x1
#define GP_WB_ENUM_SSP_KEY_LEN_KEY_256    0x2

/* watchdog_function  */
#define GP_WB_ENUM_WATCHDOG_FUNCTION_IDLE             0x0
#define GP_WB_ENUM_WATCHDOG_FUNCTION_GEN_INTERRUPT    0x1
#define GP_WB_ENUM_WATCHDOG_FUNCTION_SOFT_POR         0x2
#define GP_WB_ENUM_WATCHDOG_FUNCTION_RESET_EXT_UC     0x4

/* gpio_mode  */
#define GP_WB_ENUM_GPIO_MODE_FLOAT        0x0
#define GP_WB_ENUM_GPIO_MODE_PULLDOWN     0x1
#define GP_WB_ENUM_GPIO_MODE_PULLUP       0x2
#define GP_WB_ENUM_GPIO_MODE_BUSKEEPER    0x3

/* drive_strength  */
#define GP_WB_ENUM_DRIVE_STRENGTH_DRIVE_4DOT5MA     0x0
#define GP_WB_ENUM_DRIVE_STRENGTH_DRIVE_9MA         0x1
#define GP_WB_ENUM_DRIVE_STRENGTH_DRIVE_13DOT5MA    0x2
#define GP_WB_ENUM_DRIVE_STRENGTH_DRIVE_18MA        0x3

/* gpio_port_sel  */
#define GP_WB_ENUM_GPIO_PORT_SEL_PORTA    0x0
#define GP_WB_ENUM_GPIO_PORT_SEL_PORTB    0x1
#define GP_WB_ENUM_GPIO_PORT_SEL_PORTC    0x2

/* standby_mode  */
#define GP_WB_ENUM_STANDBY_MODE_RC_MODE            0x0
#define GP_WB_ENUM_STANDBY_MODE_XTAL_32KHZ_MODE    0x1
#define GP_WB_ENUM_STANDBY_MODE_XTAL_16MHZ_MODE    0x2

/* oscbenchmark_clock_src  */
#define GP_WB_ENUM_OSCBENCHMARK_CLOCK_SRC_RC32K_CLOCK    0x0
#define GP_WB_ENUM_OSCBENCHMARK_CLOCK_SRC_XT32K_CLOCK    0x1
#define GP_WB_ENUM_OSCBENCHMARK_CLOCK_SRC_RC29M_CLOCK    0x2
#define GP_WB_ENUM_OSCBENCHMARK_CLOCK_SRC_DTC_CLOCK      0x3
#define GP_WB_ENUM_OSCBENCHMARK_CLOCK_SRC_XT32M_CLOCK    0x4
#define GP_WB_ENUM_OSCBENCHMARK_CLOCK_SRC_FRING_CLOCK    0x5

/* wakeup_pin_mode  */
#define GP_WB_ENUM_WAKEUP_PIN_MODE_NO_EDGE         0x0
#define GP_WB_ENUM_WAKEUP_PIN_MODE_RISING_EDGE     0x1
#define GP_WB_ENUM_WAKEUP_PIN_MODE_FALLING_EDGE    0x2
#define GP_WB_ENUM_WAKEUP_PIN_MODE_BOTH_EDGES      0x3

/* por_reason  */
#define GP_WB_ENUM_POR_REASON_HW_POR                           0x0
#define GP_WB_ENUM_POR_REASON_SOFT_POR_BY_REGMAP               0x1
#define GP_WB_ENUM_POR_REASON_SOFT_POR_BY_ISO_TX               0x2
#define GP_WB_ENUM_POR_REASON_SOFT_POR_BY_WATCHDOG             0x3
#define GP_WB_ENUM_POR_REASON_SOFT_POR_BY_ES                   0x4
#define GP_WB_ENUM_POR_REASON_SOFT_POR_BY_BBPLL                0x5
#define GP_WB_ENUM_POR_REASON_POR_BY_WATCHDOG_HEARTBEAT        0x6
#define GP_WB_ENUM_POR_REASON_POR_BY_VDDB_CUTOFF               0x7
#define GP_WB_ENUM_POR_REASON_POR_BY_VDDDIG_NOK                0x8
#define GP_WB_ENUM_POR_REASON_POR_BY_GLOBAL_LDO_NOK            0x9
#define GP_WB_ENUM_POR_REASON_SOFT_POR_RAM_MW_INVALID          0xA
#define GP_WB_ENUM_POR_REASON_SOFT_POR_RAM_CRC_INVALID         0xB
#define GP_WB_ENUM_POR_REASON_SOFT_POR_FLASH_BL_CRC_INVALID    0xC
#define GP_WB_ENUM_POR_REASON_SOFT_POR_BOOTLOADER              0xD
#define GP_WB_ENUM_POR_REASON_SOFT_POR_PRESERVE_DBG_ITF        0xE
#define GP_WB_ENUM_POR_REASON_SOFT_POR_BY_BOD                  0xF

/* wkup_reason  */
#define GP_WB_ENUM_WKUP_REASON_POR                   0x0
#define GP_WB_ENUM_WKUP_REASON_TIMER_EVENT           0x1
#define GP_WB_ENUM_WKUP_REASON_LPCOMP_EVENT          0x2
#define GP_WB_ENUM_WKUP_REASON_GPIO_EVENT            0x3
#define GP_WB_ENUM_WKUP_REASON_IMMEDIATE_EVENT       0x4

/* parity  */
#define GP_WB_ENUM_PARITY_EVEN    0x0
#define GP_WB_ENUM_PARITY_ODD     0x1
#define GP_WB_ENUM_PARITY_OFF     0x2

/* ir_modulation_mode  */
#define GP_WB_ENUM_IR_MODULATION_MODE_MODULATION_OFF    0x0
#define GP_WB_ENUM_IR_MODULATION_MODE_REGISTER_BASED    0x1
#define GP_WB_ENUM_IR_MODULATION_MODE_PATTERN_BASED     0x2
#define GP_WB_ENUM_IR_MODULATION_MODE_TIME_BASED        0x3
#define GP_WB_ENUM_IR_MODULATION_MODE_EVENT_BASED       0x5

/* ir_time_unit  */
#define GP_WB_ENUM_IR_TIME_UNIT_TU_500NS    0x0
#define GP_WB_ENUM_IR_TIME_UNIT_TU_1US      0x1
#define GP_WB_ENUM_IR_TIME_UNIT_TU_2US      0x2
#define GP_WB_ENUM_IR_TIME_UNIT_TU_4US      0x3

/* generic_io_drive  */
#define GP_WB_ENUM_GENERIC_IO_DRIVE_PUSH_PULL     0x0
#define GP_WB_ENUM_GENERIC_IO_DRIVE_OPEN_DRAIN    0x1

/* qta_entry_state  */
#define GP_WB_ENUM_QTA_ENTRY_STATE_WAIT_FOR_INFO_FETCH    0x0
#define GP_WB_ENUM_QTA_ENTRY_STATE_INFO_FETCH_ONGOING     0x1
#define GP_WB_ENUM_QTA_ENTRY_STATE_WAIT_TO_CONFIRM        0x2
#define GP_WB_ENUM_QTA_ENTRY_STATE_CONFIRM_ONGOING        0x3
#define GP_WB_ENUM_QTA_ENTRY_STATE_WAIT_FOR_TX            0x4
#define GP_WB_ENUM_QTA_ENTRY_STATE_TX_ONGOING             0x5
#define GP_WB_ENUM_QTA_ENTRY_STATE_EMPTY                  0x7

/* phy_state_transition  */
#define GP_WB_ENUM_PHY_STATE_TRANSITION_OFF_TO_TX               0x0
#define GP_WB_ENUM_PHY_STATE_TRANSITION_OFF_TO_RX               0x1
#define GP_WB_ENUM_PHY_STATE_TRANSITION_TX_TO_RX                0x2
#define GP_WB_ENUM_PHY_STATE_TRANSITION_RX_TO_TX                0x3
#define GP_WB_ENUM_PHY_STATE_TRANSITION_TO_OFF                  0x4
#define GP_WB_ENUM_PHY_STATE_TRANSITION_TO_CAL                  0x5
#define GP_WB_ENUM_PHY_STATE_TRANSITION_GENERIC_TRANSITION_1    0x6
#define GP_WB_ENUM_PHY_STATE_TRANSITION_GENERIC_TRANSITION_2    0x7
#define GP_WB_ENUM_PHY_STATE_TRANSITION_SET_RX_ON               0x8

/* adc_trigger_mode  */
/* the adc is never triggered */
#define GP_WB_ENUM_ADC_TRIGGER_MODE_NEVER              0x0
#define GP_WB_ENUM_ADC_TRIGGER_MODE_ALWAYS             0x1
/* in this mode, adc measurements are triggered by the ES ADC_START events */
#define GP_WB_ENUM_ADC_TRIGGER_MODE_ES_TRIGGER         0x2
/* in this mode, adc measurements are triggered by a wrap of tmr0 */
#define GP_WB_ENUM_ADC_TRIGGER_MODE_TIMER_TMR0_WRAP    0x3
/* in this mode, adc measurements are triggered by a wrap of tmr1 */
#define GP_WB_ENUM_ADC_TRIGGER_MODE_TIMER_TMR1_WRAP    0x4
/* in this mode, adc measurements are triggered by a wrap of tmr2 */
#define GP_WB_ENUM_ADC_TRIGGER_MODE_TIMER_TMR2_WRAP    0x5
/* in this mode, adc measurements are triggered by a wrap of tmr3 */
#define GP_WB_ENUM_ADC_TRIGGER_MODE_TIMER_TMR3_WRAP    0x6
/* in this mode, adc measurements are triggered by a wrap of tmr4 */
#define GP_WB_ENUM_ADC_TRIGGER_MODE_TIMER_TMR4_WRAP    0x7

/* adc_channel  */
#define GP_WB_ENUM_ADC_CHANNEL_ANIO0           0x0
#define GP_WB_ENUM_ADC_CHANNEL_ANIO1           0x1
#define GP_WB_ENUM_ADC_CHANNEL_RESERVED_2      0x2
#define GP_WB_ENUM_ADC_CHANNEL_RESERVED_3      0x3
#define GP_WB_ENUM_ADC_CHANNEL_ANIO2           0x4
#define GP_WB_ENUM_ADC_CHANNEL_ANIO3           0x5
#define GP_WB_ENUM_ADC_CHANNEL_RESERVED_6      0x6
#define GP_WB_ENUM_ADC_CHANNEL_RESERVED_7      0x7
#define GP_WB_ENUM_ADC_CHANNEL_TESTBUS0        0x8
#define GP_WB_ENUM_ADC_CHANNEL_TESTBUS1        0x9
#define GP_WB_ENUM_ADC_CHANNEL_TESTBUS2        0xA
/* battery voltage */
#define GP_WB_ENUM_ADC_CHANNEL_VBAT            0xB
/* Temperature sensor output voltage */
#define GP_WB_ENUM_ADC_CHANNEL_TEMP            0xC
/* differential input, using anio0(p) and anio1(n) */
#define GP_WB_ENUM_ADC_CHANNEL_ANIO0_1_DIFF    0xD
#define GP_WB_ENUM_ADC_CHANNEL_RESERVED_14     0xE
#define GP_WB_ENUM_ADC_CHANNEL_RESERVED_15     0xF

/* adc_scaler_gain  */
#define GP_WB_ENUM_ADC_SCALER_GAIN_X0_25    0x0
#define GP_WB_ENUM_ADC_SCALER_GAIN_X0_33    0x1
#define GP_WB_ENUM_ADC_SCALER_GAIN_X0_50    0x2
#define GP_WB_ENUM_ADC_SCALER_GAIN_X0_67    0x3
#define GP_WB_ENUM_ADC_SCALER_GAIN_X1_00    0x4
#define GP_WB_ENUM_ADC_SCALER_GAIN_X1_50    0x5
#define GP_WB_ENUM_ADC_SCALER_GAIN_X2_00    0x6
#define GP_WB_ENUM_ADC_SCALER_GAIN_X9_00    0x7

/* anio_channel  */
#define GP_WB_ENUM_ANIO_CHANNEL_ANIO0         0x0
#define GP_WB_ENUM_ANIO_CHANNEL_ANIO1         0x1
#define GP_WB_ENUM_ANIO_CHANNEL_RESERVED_2    0x2
#define GP_WB_ENUM_ANIO_CHANNEL_RESERVED_3    0x3
#define GP_WB_ENUM_ANIO_CHANNEL_ANIO2         0x4
#define GP_WB_ENUM_ANIO_CHANNEL_ANIO3         0x5
#define GP_WB_ENUM_ANIO_CHANNEL_RESERVED_6    0x6
#define GP_WB_ENUM_ANIO_CHANNEL_RESERVED_7    0x7

/* adc_buffer_update_mode  */
#define GP_WB_ENUM_ADC_BUFFER_UPDATE_MODE_NORMAL          0x0
#define GP_WB_ENUM_ADC_BUFFER_UPDATE_MODE_MIN_HOLD        0x1
#define GP_WB_ENUM_ADC_BUFFER_UPDATE_MODE_MAX_HOLD        0x2
#define GP_WB_ENUM_ADC_BUFFER_UPDATE_MODE_OUT_OF_RANGE    0x3

/* dma_trigger_src_select  */
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_NO_TRIGGER_SRC              0x0
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_SPI_SL_TX_NOT_FULL          0x1
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_SPI_SL_RX_NOT_EMPTY         0x2
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_SPI_M_TX_NOT_FULL           0x3
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_SPI_M_RX_NOT_EMPTY          0x4
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_I2S_M_TX_NOT_FULL           0x5
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_I2S_M_RX_NOT_EMPTY          0x6
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_UART_0_TX_NOT_FULL          0x7
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_UART_0_RX_NOT_EMPTY         0x8
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_UART_1_TX_NOT_FULL          0x9
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_UART_1_RX_NOT_EMPTY         0xA
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_PWM_TX_NOT_FULL             0xB
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_PWM_TIMESTAMP0_NOT_EMPTY    0xC
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_PWM_TIMESTAMP1_NOT_EMPTY    0xD
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_PWM_TIMESTAMP2_NOT_EMPTY    0xE
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_PWM_TIMESTAMP3_NOT_EMPTY    0xF
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_ADC_FIFO_NOT_EMPTY          0x10
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_ASP_DATASTREAM_0_VALID      0x11
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_ASP_DATASTREAM_1_VALID      0x12
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_TIMER_TMR0_WRAP             0x13
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_TIMER_TMR1_WRAP             0x14
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_TIMER_TMR2_WRAP             0x15
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_TIMER_TMR3_WRAP             0x16
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_TIMER_TMR4_WRAP             0x17
#define GP_WB_ENUM_DMA_TRIGGER_SRC_SELECT_ES_TRIGGER                  0x18

/* dma_word_mode  */
#define GP_WB_ENUM_DMA_WORD_MODE_BYTE         0x0
#define GP_WB_ENUM_DMA_WORD_MODE_HALF_WORD    0x1
#define GP_WB_ENUM_DMA_WORD_MODE_WORD         0x2

/* dma_buffer_complete_mode  */
#define GP_WB_ENUM_DMA_BUFFER_COMPLETE_MODE_ERROR_MODE     0x0
#define GP_WB_ENUM_DMA_BUFFER_COMPLETE_MODE_STATUS_MODE    0x1

/* antsel_int  */
#define GP_WB_ENUM_ANTSEL_INT_USE_PORT_FROM_DESIGN    0x0
#define GP_WB_ENUM_ANTSEL_INT_USE_PORT_0              0x1
#define GP_WB_ENUM_ANTSEL_INT_USE_PORT_1              0x2

/* antsel_ext  */
#define GP_WB_ENUM_ANTSEL_EXT_USE_PORT_FROM_DESIGN    0x0
#define GP_WB_ENUM_ANTSEL_EXT_USE_EXT_PORT_0          0x1
#define GP_WB_ENUM_ANTSEL_EXT_USE_EXT_PORT_1          0x2

/* attenuator_sel  */
#define GP_WB_ENUM_ATTENUATOR_SEL_NO_ATTENUATION      0x0
#define GP_WB_ENUM_ATTENUATOR_SEL_LOW_ATTENUATION     0x1
#define GP_WB_ENUM_ATTENUATOR_SEL_HIGH_ATTENUATION    0x2

/* clock_speed  */
#define GP_WB_ENUM_CLOCK_SPEED_M64    0x0
#define GP_WB_ENUM_CLOCK_SPEED_M32    0x1

/* ble_mgr_state  */
#define GP_WB_ENUM_BLE_MGR_STATE_STANDBY          0x00
#define GP_WB_ENUM_BLE_MGR_STATE_ADVERTISING      0x01
#define GP_WB_ENUM_BLE_MGR_STATE_SCANNING         0x02
#define GP_WB_ENUM_BLE_MGR_STATE_INITIATING       0x03
#define GP_WB_ENUM_BLE_MGR_STATE_CONNECTION_M     0x04
#define GP_WB_ENUM_BLE_MGR_STATE_CONNECTION_S     0x05
#define GP_WB_ENUM_BLE_MGR_STATE_SUBEVENT         0x06
#define GP_WB_ENUM_BLE_MGR_STATE_BG_SCANNING      0x07
#define GP_WB_ENUM_BLE_MGR_STATE_TEST_MODE        0x08
#define GP_WB_ENUM_BLE_MGR_STATE_CLEANUP          0x09
#define GP_WB_ENUM_BLE_MGR_STATE_ABORT            0x0A
#define GP_WB_ENUM_BLE_MGR_STATE_NOTUSED_STATE    0xFF

/* subev_type  */
#define GP_WB_ENUM_SUBEV_TYPE_EA_TX_PRI                  0x00
#define GP_WB_ENUM_SUBEV_TYPE_EA_TX_SEC_CONN             0x01
#define GP_WB_ENUM_SUBEV_TYPE_EA_TX_SEC_SCAN             0x02
#define GP_WB_ENUM_SUBEV_TYPE_EA_TX_SEC_DATA             0x03
#define GP_WB_ENUM_SUBEV_TYPE_EA_TX_SEC_PERIODIC_SYNC    0x04
#define GP_WB_ENUM_SUBEV_TYPE_EA_TX_SEC_PERIODIC_DATA    0x05
#define GP_WB_ENUM_SUBEV_TYPE_LA_TX_PRI                  0x06
#define GP_WB_ENUM_SUBEV_TYPE_EA_RX_PRI_SCAN             0x10
#define GP_WB_ENUM_SUBEV_TYPE_EA_RX_PRI_INIT_SCAN        0x11
#define GP_WB_ENUM_SUBEV_TYPE_EA_RX_SEC_CONN             0x12
#define GP_WB_ENUM_SUBEV_TYPE_EA_RX_SEC_SCAN             0x13
#define GP_WB_ENUM_SUBEV_TYPE_EA_RX_SEC_DATA             0x14
#define GP_WB_ENUM_SUBEV_TYPE_EA_RX_SEC_PERIODIC_SYNC    0x15
#define GP_WB_ENUM_SUBEV_TYPE_EA_RX_SEC_PERIODIC_DATA    0x16
#define GP_WB_ENUM_SUBEV_TYPE_DBG_GPIO_TOGGLE            0xF0
#define GP_WB_ENUM_SUBEV_TYPE_NOT_VALID                  0xFF

/* subev_drop_reason  */
#define GP_WB_ENUM_SUBEV_DROP_REASON_NO_DROP                              0x00
#define GP_WB_ENUM_SUBEV_DROP_REASON_NOT_VALID                            0x01
#define GP_WB_ENUM_SUBEV_DROP_REASON_EVENT_BUSY                           0x02
#define GP_WB_ENUM_SUBEV_DROP_REASON_TOO_LATE                             0x03
#define GP_WB_ENUM_SUBEV_DROP_REASON_DROP_FLAG_SET                        0x04
#define GP_WB_ENUM_SUBEV_DROP_REASON_NRT_HALT_REQUEST_RESULTED_IN_SKIP    0x05
#define GP_WB_ENUM_SUBEV_DROP_REASON_FAILED_TO_SCHEDULE_SUBSEQUENT_SED    0x06
#define GP_WB_ENUM_SUBEV_DROP_REASON_WRONG_CRC                            0x07
#define GP_WB_ENUM_SUBEV_DROP_REASON_ILLEGAL                              0xFF

/* blefilt_state  */
#define GP_WB_ENUM_BLEFILT_STATE_RESET            0x00
#define GP_WB_ENUM_BLEFILT_STATE_ADV_SRC_ADDR     0x01
#define GP_WB_ENUM_BLEFILT_STATE_ADV_DST_ADDR     0x02
#define GP_WB_ENUM_BLEFILT_STATE_ADV_EXT_HDR      0x03
#define GP_WB_ENUM_BLEFILT_STATE_CTE_INFO         0x04
#define GP_WB_ENUM_BLEFILT_STATE_AUX_PTR          0x05
#define GP_WB_ENUM_BLEFILT_STATE_NOTUSED_STATE    0xFF

/* macfilt_state  */
#define GP_WB_ENUM_MACFILT_STATE_RESET               0x00
#define GP_WB_ENUM_MACFILT_STATE_FRM_CTRL_SEQ_NR     0x01
#define GP_WB_ENUM_MACFILT_STATE_DST_ADDR            0x02
#define GP_WB_ENUM_MACFILT_STATE_SRC_ADDR            0x03
#define GP_WB_ENUM_MACFILT_STATE_SEC_HDR_SEC_CTRL    0x04
#define GP_WB_ENUM_MACFILT_STATE_SEC_HDR             0x05
#define GP_WB_ENUM_MACFILT_STATE_IE_HDR              0x06
#define GP_WB_ENUM_MACFILT_STATE_IE_CONTENT          0x07
#define GP_WB_ENUM_MACFILT_STATE_CMD_BYTE            0x08
#define GP_WB_ENUM_MACFILT_STATE_NOTUSED_STATE       0xFF

/* mmu_exception  */
#define GP_WB_ENUM_MMU_EXCEPTION_PROG_ERR    0x0
#define GP_WB_ENUM_MMU_EXCEPTION_DATA_ERR    0x1

/* oscillator_benchmark_state  */
#define GP_WB_ENUM_OSCILLATOR_BENCHMARK_STATE_IDLE                         0x0
#define GP_WB_ENUM_OSCILLATOR_BENCHMARK_STATE_HUNTING_FOR_FIRST_LP_TICK    0x1
#define GP_WB_ENUM_OSCILLATOR_BENCHMARK_STATE_MEASURING                    0x2

/* asp_datastream_mode  */
#define GP_WB_ENUM_ASP_DATASTREAM_MODE_OFF        0x0
#define GP_WB_ENUM_ASP_DATASTREAM_MODE_RISING     0x1
#define GP_WB_ENUM_ASP_DATASTREAM_MODE_FALLING    0x2
/* The first sample will always be rising edge */
#define GP_WB_ENUM_ASP_DATASTREAM_MODE_BOTH       0x3

/* dcdc_clk_div_idx  */
#define GP_WB_ENUM_DCDC_CLK_DIV_IDX_D14    0x0
#define GP_WB_ENUM_DCDC_CLK_DIV_IDX_D15    0x1
#define GP_WB_ENUM_DCDC_CLK_DIV_IDX_D16    0x2
#define GP_WB_ENUM_DCDC_CLK_DIV_IDX_D17    0x3
#define GP_WB_ENUM_DCDC_CLK_DIV_IDX_D23    0x4

/* pa_slope  */
#define GP_WB_ENUM_PA_SLOPE_RAMP_8US      0x0
#define GP_WB_ENUM_PA_SLOPE_RAMP_4US      0x1
#define GP_WB_ENUM_PA_SLOPE_RAMP_2US      0x2
#define GP_WB_ENUM_PA_SLOPE_RAMP_1US      0x3
#define GP_WB_ENUM_PA_SLOPE_RAMP_500NS    0x4
#define GP_WB_ENUM_PA_SLOPE_RAMP_250NS    0x5
#define GP_WB_ENUM_PA_SLOPE_RAMP_125NS    0x6
#define GP_WB_ENUM_PA_SLOPE_IMMEDIATE     0x7

/* circular_buffer  */
#define GP_WB_ENUM_CIRCULAR_BUFFER_SRC_BUFFER     0x0
#define GP_WB_ENUM_CIRCULAR_BUFFER_DEST_BUFFER    0x1

/* tmr_clk_sel  */
#define GP_WB_ENUM_TMR_CLK_SEL_INT_CLK    0x0
#define GP_WB_ENUM_TMR_CLK_SEL_TMR0       0x1
#define GP_WB_ENUM_TMR_CLK_SEL_TMR1       0x2
#define GP_WB_ENUM_TMR_CLK_SEL_TMR2       0x3
#define GP_WB_ENUM_TMR_CLK_SEL_TMR3       0x4
#define GP_WB_ENUM_TMR_CLK_SEL_TMR4       0x5

/* tmr0_clk_sel  */
#define GP_WB_ENUM_TMR0_CLK_SEL_INT_CLK    0x0

/* tmr1_clk_sel  */
#define GP_WB_ENUM_TMR1_CLK_SEL_INT_CLK    0x0
#define GP_WB_ENUM_TMR1_CLK_SEL_TMR0       0x1

/* tmr2_clk_sel  */
#define GP_WB_ENUM_TMR2_CLK_SEL_INT_CLK    0x0
#define GP_WB_ENUM_TMR2_CLK_SEL_TMR0       0x1
#define GP_WB_ENUM_TMR2_CLK_SEL_TMR1       0x2

/* tmr3_clk_sel  */
#define GP_WB_ENUM_TMR3_CLK_SEL_INT_CLK    0x0
#define GP_WB_ENUM_TMR3_CLK_SEL_TMR0       0x1
#define GP_WB_ENUM_TMR3_CLK_SEL_TMR1       0x2
#define GP_WB_ENUM_TMR3_CLK_SEL_TMR2       0x3

/* tmr4_clk_sel  */
#define GP_WB_ENUM_TMR4_CLK_SEL_INT_CLK    0x0
#define GP_WB_ENUM_TMR4_CLK_SEL_TMR0       0x1
#define GP_WB_ENUM_TMR4_CLK_SEL_TMR1       0x2
#define GP_WB_ENUM_TMR4_CLK_SEL_TMR2       0x3
#define GP_WB_ENUM_TMR4_CLK_SEL_TMR3       0x4

/* tmr_sel  */
#define GP_WB_ENUM_TMR_SEL_TMR0    0x0
#define GP_WB_ENUM_TMR_SEL_TMR1    0x1
#define GP_WB_ENUM_TMR_SEL_TMR2    0x2
#define GP_WB_ENUM_TMR_SEL_TMR3    0x3
#define GP_WB_ENUM_TMR_SEL_TMR4    0x4
#define GP_WB_ENUM_TMR_SEL_NONE    0x7

/* i2s_clk_sel  */
#define GP_WB_ENUM_I2S_CLK_SEL_MAIN_CLK     0x0
#define GP_WB_ENUM_I2S_CLK_SEL_FRACT_CLK    0x1
#define GP_WB_ENUM_I2S_CLK_SEL_IOB_CLK      0x2

/* rt_cmd_type  */
/* Initialize the BLE manager. Enables the processing of BLE event interrupts. Initializes the internal state. Connects the necessary hardware interrupts to the RT processor.  No parameters. */
#define GP_WB_ENUM_RT_CMD_TYPE_INIT_BLE_MGR              0x01
/* Disables the BLE manager. Disables the processing of BLE event interrupts. Disconnects the hardware interrupts from the RT processor. No parameters. */
#define GP_WB_ENUM_RT_CMD_TYPE_DISABLE_BLE_MGR           0x02
/* Start a BLE event. Paramter is structure that describes the event to be started. */
#define GP_WB_ENUM_RT_CMD_TYPE_START_EVENT               0x03
/* Stop an ongoing event and disable it. If the event was not ongoing (currently executed), it will only be disabled. Param: ES event number (UInt8) of the event to be stopped. */
#define GP_WB_ENUM_RT_CMD_TYPE_STOP_EVENT                0x04
/* Wake up an event in case it went to sleep due to slave latency. Sleeping events can only happen on connection slave events with slave latency and no_wakeup feature enabled. Param: ES event number (UInt8) of the event to be woken up. */
#define GP_WB_ENUM_RT_CMD_TYPE_WAKEUP_EVENT              0x05
/* Start the BLE direct test mode. This can only be done when there are no BLE event scheduled or active. This command can return unsuccessful when the RT processor was unable to start the direct test mode. Param: info_ptr (UInt16) : pointer in the GPMicro address space to the ble_test_info structure. */
#define GP_WB_ENUM_RT_CMD_TYPE_START_DIRECT_TEST_MODE    0x06
/* Stop the BLE direct test mode. No parameters. */
#define GP_WB_ENUM_RT_CMD_TYPE_STOP_DIRECT_TEST_MODE     0x07
/* Halt the GPMicro from the moment the RT system is IDLE so halts the processor in a clean way so internal state is not corrupted. This command is completed immediately, but the RT processor is only stopped when the STANDBY_RESET_GPMICRO asserted. BLE_MGR_SHUTDOWN_DURING_CLEANUP has impact on the behavior of this command.  No parameters. */
#define GP_WB_ENUM_RT_CMD_TYPE_HALT_GPMICRO              0x08
/* Calibrate the channel independant FLL parameters (DTC, coarse, fine_gain). No parameters. */
#define GP_WB_ENUM_RT_CMD_TYPE_FLL_CAL_NRT               0x09
/* Schedule a SubEvDsc. RT will add the de SED into the time-sorted PDL. Param: sed_idx (UInt8): index of the SED to be scheduled. */
#define GP_WB_ENUM_RT_CMD_TYPE_SCHEDULE_SED              0x0A
/* Release a SubEvDsc claimed by RT. Param: sed_idx (UInt8): index of the SED to be released. should be an index claimed by RT. (ie sed_idx is in BLE_MGR_SUBEV_DSC_ENTRY_RT_MASK ) */
#define GP_WB_ENUM_RT_CMD_TYPE_RELEASE_RT_SED            0x0B
/* Release all Scan ASCs, make the duplicate filtering list empty. No parameters. */
#define GP_WB_ENUM_RT_CMD_TYPE_RELEASE_ALL_SCANASC       0x0C
/* Does nothing, handy if we just want to wake up the RT processor to service the main loop once. */
#define GP_WB_ENUM_RT_CMD_TYPE_DUMMY                     0x63
#define GP_WB_ENUM_RT_CMD_TYPE_NOTUSED                   0xFF

/* rt_error_type  */
/* Indicates a PARBLE_PACKET_DONE interrupt was received while the RT system was not in an active BLE state. Param: rx_pbm_nr (UInt8) : number of the BLE PBM that was received in the non-active state.  The NRT software is responsible for releasing this PBM and handling this error. */
#define GP_WB_ENUM_RT_ERROR_TYPE_PACKET_DONE    0x01
/* Indicate a BLE RIB_RX_WD_DONE interrupt is received while the RT system was not in an active BLE state. No parameters. */
#define GP_WB_ENUM_RT_ERROR_TYPE_RX_WD_DONE     0x02
#define GP_WB_ENUM_RT_ERROR_TYPE_NOTUSED        0xFF

/* rt_stat_type  */
/* indicate an ES event is executed too late by the ES hardwar. Returns es_trigger_too_late info record. */
#define GP_WB_ENUM_RT_STAT_TYPE_ES_TRIGGER_TOO_LATE    0x01
#define GP_WB_ENUM_RT_STAT_TYPE_NOTUSED                0xFF

/* trigger_type  */
#define GP_WB_ENUM_TRIGGER_TYPE_CLEANUP    0x00
#define GP_WB_ENUM_TRIGGER_TYPE_EVENT      0x01
#define GP_WB_ENUM_TRIGGER_TYPE_NOTUSED    0xFF

/***************************
 * layout: event_enum
 ***************************/
/* event_state  */
#define GP_WB_ENUM_EVENT_STATE_INVALID                              0x0
#define GP_WB_ENUM_EVENT_STATE_SCHEDULED                            0x1
#define GP_WB_ENUM_EVENT_STATE_SCHEDULED_FOR_IMMEDIATE_EXECUTION    0x2
#define GP_WB_ENUM_EVENT_STATE_RESCHEDULED                          0x3
#define GP_WB_ENUM_EVENT_STATE_DONE                                 0x4

/* event_result  */
#define GP_WB_ENUM_EVENT_RESULT_UNKNOWN              0x0
#define GP_WB_ENUM_EVENT_RESULT_EXECUTED_ON_TIME     0x1
#define GP_WB_ENUM_EVENT_RESULT_EXECUTED_TOO_LATE    0x2
#define GP_WB_ENUM_EVENT_RESULT_MISSED_TOO_LATE      0x3

/* event_type  */
#define GP_WB_ENUM_EVENT_TYPE_MAC_RX_ON0                      0x00
#define GP_WB_ENUM_EVENT_TYPE_MAC_RX_ON1                      0x01
#define GP_WB_ENUM_EVENT_TYPE_MAC_RX_ON2                      0x02
#define GP_WB_ENUM_EVENT_TYPE_MAC_RX_ON3                      0x03
#define GP_WB_ENUM_EVENT_TYPE_MAC_RX_OFF0                     0x04
#define GP_WB_ENUM_EVENT_TYPE_MAC_RX_OFF1                     0x05
#define GP_WB_ENUM_EVENT_TYPE_MAC_RX_OFF2                     0x06
#define GP_WB_ENUM_EVENT_TYPE_MAC_RX_OFF3                     0x07
#define GP_WB_ENUM_EVENT_TYPE_MAC_TX_QUEUE0                   0x08
#define GP_WB_ENUM_EVENT_TYPE_MAC_TX_QUEUE1                   0x09
#define GP_WB_ENUM_EVENT_TYPE_MAC_TX_QUEUE2                   0x0A
#define GP_WB_ENUM_EVENT_TYPE_MAC_TX_QUEUE3                   0x0B
#define GP_WB_ENUM_EVENT_TYPE_MAC_TX_UNTIMED_QUEUE_HALT       0x0C
#define GP_WB_ENUM_EVENT_TYPE_MAC_TX_UNTIMED_QUEUE_RESUME     0x0D
#define GP_WB_ENUM_EVENT_TYPE_ADC_START                       0x0E
#define GP_WB_ENUM_EVENT_TYPE_KEYSCAN_START                   0x0F
#define GP_WB_ENUM_EVENT_TYPE_DMA_START                       0x10
#define GP_WB_ENUM_EVENT_TYPE_RESET_INTERNAL_UC               0x11
#define GP_WB_ENUM_EVENT_TYPE_RESET_EXTERNAL_UC               0x12
#define GP_WB_ENUM_EVENT_TYPE_RESET_DEVICE                    0x13
#define GP_WB_ENUM_EVENT_TYPE_OSCILLATOR_BENCHMARK_TRIGGER    0x14
#define GP_WB_ENUM_EVENT_TYPE_UPDATE_DITHER_SEED              0x30
#define GP_WB_ENUM_EVENT_TYPE_IR_MODULATION_ON                0x40
#define GP_WB_ENUM_EVENT_TYPE_IR_MODULATION_OFF               0x41
#define GP_WB_ENUM_EVENT_TYPE_IR_START                        0x42
#define GP_WB_ENUM_EVENT_TYPE_IR_SET_ALT_CARRIER              0x43
#define GP_WB_ENUM_EVENT_TYPE_IR_UNSET_ALT_CARRIER            0x44
#define GP_WB_ENUM_EVENT_TYPE_BLE_ADVERTISING                 0x50
#define GP_WB_ENUM_EVENT_TYPE_BLE_SCANNING                    0x51
#define GP_WB_ENUM_EVENT_TYPE_BLE_INITIATING                  0x52
#define GP_WB_ENUM_EVENT_TYPE_BLE_CONNECTION_M                0x53
#define GP_WB_ENUM_EVENT_TYPE_BLE_CONNECTION_S                0x54
#define GP_WB_ENUM_EVENT_TYPE_BLE_SUBEVENT                    0x55
#define GP_WB_ENUM_EVENT_TYPE_BLE_BG_SCANNING                 0x56
#define GP_WB_ENUM_EVENT_TYPE_BLE_VIRTUAL                     0x57
#define GP_WB_ENUM_EVENT_TYPE_UNSET_PIN_0                     0x80
#define GP_WB_ENUM_EVENT_TYPE_SET_PIN_0                       0x81
#define GP_WB_ENUM_EVENT_TYPE_TOGGLE_PIN_0                    0x82
#define GP_WB_ENUM_EVENT_TYPE_PULSE_PIN_0                     0x83
#define GP_WB_ENUM_EVENT_TYPE_UNSET_PIN_1                     0x90
#define GP_WB_ENUM_EVENT_TYPE_SET_PIN_1                       0x91
#define GP_WB_ENUM_EVENT_TYPE_TOGGLE_PIN_1                    0x92
#define GP_WB_ENUM_EVENT_TYPE_PULSE_PIN_1                     0x93
#define GP_WB_ENUM_EVENT_TYPE_UNSET_PIN_2                     0xA0
#define GP_WB_ENUM_EVENT_TYPE_SET_PIN_2                       0xA1
#define GP_WB_ENUM_EVENT_TYPE_TOGGLE_PIN_2                    0xA2
#define GP_WB_ENUM_EVENT_TYPE_PULSE_PIN_2                     0xA3
#define GP_WB_ENUM_EVENT_TYPE_UNSET_PIN_3                     0xB0
#define GP_WB_ENUM_EVENT_TYPE_SET_PIN_3                       0xB1
#define GP_WB_ENUM_EVENT_TYPE_TOGGLE_PIN_3                    0xB2
#define GP_WB_ENUM_EVENT_TYPE_PULSE_PIN_3                     0xB3
#define GP_WB_ENUM_EVENT_TYPE_DUMMY                           0xFF

/***************************
 * layout: rib_enum
 ***************************/
/* simple_trc_state  */
#define GP_WB_ENUM_SIMPLE_TRC_STATE_IDLE                        0x0
#define GP_WB_ENUM_SIMPLE_TRC_STATE_CAL_RX                      0x1
#define GP_WB_ENUM_SIMPLE_TRC_STATE_RX_ON_IDLE                  0x2
#define GP_WB_ENUM_SIMPLE_TRC_STATE_EARLY_DATA_IND_IDLE         0x3
#define GP_WB_ENUM_SIMPLE_TRC_STATE_DATA_IND_IDLE               0x4
#define GP_WB_ENUM_SIMPLE_TRC_STATE_ACK_TX_WARMUP_IDLE          0x5
#define GP_WB_ENUM_SIMPLE_TRC_STATE_ACK_TX_PREPARE_IDLE         0x6
#define GP_WB_ENUM_SIMPLE_TRC_STATE_ACK_TX_PHY_IDLE             0x7
#define GP_WB_ENUM_SIMPLE_TRC_STATE_RX_WINDOW                   0x8
#define GP_WB_ENUM_SIMPLE_TRC_STATE_EARLY_DATA_IND_WINDOW       0x9
#define GP_WB_ENUM_SIMPLE_TRC_STATE_DATA_IND_WINDOW             0xA
#define GP_WB_ENUM_SIMPLE_TRC_STATE_ACK_TX_WARMUP_WINDOW        0xB
#define GP_WB_ENUM_SIMPLE_TRC_STATE_ACK_TX_PREPARE_WINDOW       0xC
#define GP_WB_ENUM_SIMPLE_TRC_STATE_ACK_TX_PHY_WINDOW           0xD
#define GP_WB_ENUM_SIMPLE_TRC_STATE_CAL_TX                      0xE
#define GP_WB_ENUM_SIMPLE_TRC_STATE_TX_FLOW_DECIDE              0xF
#define GP_WB_ENUM_SIMPLE_TRC_STATE_TX_PREPARE                  0x10
#define GP_WB_ENUM_SIMPLE_TRC_STATE_TX_WARMUP                   0x11
#define GP_WB_ENUM_SIMPLE_TRC_STATE_TX_PHY                      0x12
#define GP_WB_ENUM_SIMPLE_TRC_STATE_CSMA_CCA_PHASE_WARMUP       0x13
#define GP_WB_ENUM_SIMPLE_TRC_STATE_CSMA_BACKOFF_PHASE          0x14
#define GP_WB_ENUM_SIMPLE_TRC_STATE_CSMA_CCA_PHASE              0x15
#define GP_WB_ENUM_SIMPLE_TRC_STATE_ED_SCAN_WARMUP              0x16
#define GP_WB_ENUM_SIMPLE_TRC_STATE_ED_SCAN_EXECUTE             0x17
#define GP_WB_ENUM_SIMPLE_TRC_STATE_WAIT_FOR_ACK_PREPARE        0x18
#define GP_WB_ENUM_SIMPLE_TRC_STATE_WAIT_FOR_ACK_WARMUP         0x19
#define GP_WB_ENUM_SIMPLE_TRC_STATE_WAIT_FOR_ACK_START          0x1A
#define GP_WB_ENUM_SIMPLE_TRC_STATE_WAIT_FOR_ACK_DONE           0x1B
#define GP_WB_ENUM_SIMPLE_TRC_STATE_CAL_POST_TX_DECIDE          0x1C
#define GP_WB_ENUM_SIMPLE_TRC_STATE_CAL_POST_TX                 0x1D
#define GP_WB_ENUM_SIMPLE_TRC_STATE_TX_DONE                     0x1E
#define GP_WB_ENUM_SIMPLE_TRC_STATE_ACK_TX_SYNC_ON_RX_IDLE      0x1F
#define GP_WB_ENUM_SIMPLE_TRC_STATE_ACK_TX_SYNC_ON_RX_WINDOW    0x20
#define GP_WB_ENUM_SIMPLE_TRC_STATE_RX_WINDOW_SYNC              0x21

/* rci_queue_entry_type  */
#define GP_WB_ENUM_RCI_QUEUE_ENTRY_TYPE_INVALID             0x0
#define GP_WB_ENUM_RCI_QUEUE_ENTRY_TYPE_DATA_IND            0x1
#define GP_WB_ENUM_RCI_QUEUE_ENTRY_TYPE_DATA_CNF_0          0x2
#define GP_WB_ENUM_RCI_QUEUE_ENTRY_TYPE_DATA_CNF_1          0x3
#define GP_WB_ENUM_RCI_QUEUE_ENTRY_TYPE_DATA_CNF_2          0x4
#define GP_WB_ENUM_RCI_QUEUE_ENTRY_TYPE_DATA_CNF_3          0x5
#define GP_WB_ENUM_RCI_QUEUE_ENTRY_TYPE_BLE_DATA_IND        0x6
#define GP_WB_ENUM_RCI_QUEUE_ENTRY_TYPE_BLE_ADV_IND         0x7
#define GP_WB_ENUM_RCI_QUEUE_ENTRY_TYPE_BLE_CONN_REQ_IND    0x8
#define GP_WB_ENUM_RCI_QUEUE_ENTRY_TYPE_BLE_CONN_RSP_IND    0x9
#define GP_WB_ENUM_RCI_QUEUE_ENTRY_TYPE_BLE_DATA_CNF        0xA

/* msg_status_return_code  */
#define GP_WB_ENUM_MSG_STATUS_RETURN_CODE_NOT_DONE                  0x0
#define GP_WB_ENUM_MSG_STATUS_RETURN_CODE_EXECUTED_UNTIMED          0x1
#define GP_WB_ENUM_MSG_STATUS_RETURN_CODE_EXECUTED_ON_TIME          0x2
#define GP_WB_ENUM_MSG_STATUS_RETURN_CODE_EXECUTED_TOO_LATE         0x3
#define GP_WB_ENUM_MSG_STATUS_RETURN_CODE_FAILED_TOO_LATE           0x4
#define GP_WB_ENUM_MSG_STATUS_RETURN_CODE_FAILED_LOCK_LOSS          0x5
#define GP_WB_ENUM_MSG_STATUS_RETURN_CODE_FAILED_COEX_BLOCK         0x6
#define GP_WB_ENUM_MSG_STATUS_RETURN_CODE_FAILED_PHY_NOT_SETTLED    0x7
#define GP_WB_ENUM_MSG_STATUS_RETURN_CODE_FAILED_TX_NOT_ALLOWED     0x8
#define GP_WB_ENUM_MSG_STATUS_RETURN_CODE_TX_ABORTED                0x9

/***************************
 * layout: phy_enum
 ***************************/
/* cca_mode  */
#define GP_WB_ENUM_CCA_MODE_ENERGY_ONLY      0x0
#define GP_WB_ENUM_CCA_MODE_ENERGY_AND_CS    0x1

/* rx_drop_reason  */
#define GP_WB_ENUM_RX_DROP_REASON_NO_DROP                                0x0
#define GP_WB_ENUM_RX_DROP_REASON_WRONG_FRAME_TYPE                       0x1
#define GP_WB_ENUM_RX_DROP_REASON_WRONG_VERSION                          0x2
#define GP_WB_ENUM_RX_DROP_REASON_WRONG_SRC_PAN_ID                       0x3
#define GP_WB_ENUM_RX_DROP_REASON_WRONG_DST_PAN_ID                       0x4
#define GP_WB_ENUM_RX_DROP_REASON_WRONG_SRC_ADDRESS                      0x5
#define GP_WB_ENUM_RX_DROP_REASON_WRONG_DST_ADDRESS                      0x6
#define GP_WB_ENUM_RX_DROP_REASON_WRONG_COMMAND                          0x7
#define GP_WB_ENUM_RX_DROP_REASON_WRONG_CRC                              0x8
#define GP_WB_ENUM_RX_DROP_REASON_WRONG_SRC_ADDRESSING_MODE              0x9
#define GP_WB_ENUM_RX_DROP_REASON_WRONG_DST_ADDRESSING_MODE              0xA
#define GP_WB_ENUM_RX_DROP_REASON_PACKET_ENDED_BEFORE_PROCESSING_DONE    0xB
#define GP_WB_ENUM_RX_DROP_REASON_NO_FREE_PBM                            0xC
#define GP_WB_ENUM_RX_DROP_REASON_WRONG_ACCESS_CODE                      0xD
#define GP_WB_ENUM_RX_DROP_REASON_RT_PROC_DROP                           0xE

/* parble_drop_reason  */
#define GP_WB_ENUM_PARBLE_DROP_REASON_WRONG_CRC                0x0
#define GP_WB_ENUM_PARBLE_DROP_REASON_RT_PROC_DROP             0x1
#define GP_WB_ENUM_PARBLE_DROP_REASON_NO_FREE_PBM              0x2
#define GP_WB_ENUM_PARBLE_DROP_REASON_WRONG_ACCESS_CODE        0x3
#define GP_WB_ENUM_PARBLE_DROP_REASON_INVALID_LEVEL_TRIGGER    0x4
#define GP_WB_ENUM_PARBLE_DROP_REASON_PACKET_TOO_LONG          0x5
#define GP_WB_ENUM_PARBLE_DROP_REASON_NO_DROP                  0x7

/* lff_int_coupling_factor  */
#define GP_WB_ENUM_LFF_INT_COUPLING_FACTOR_LFF_CF_64     0x0
#define GP_WB_ENUM_LFF_INT_COUPLING_FACTOR_LFF_CF_128    0x1
#define GP_WB_ENUM_LFF_INT_COUPLING_FACTOR_LFF_CF_256    0x2
#define GP_WB_ENUM_LFF_INT_COUPLING_FACTOR_LFF_CF_512    0x3

/* phy_state  */
#define GP_WB_ENUM_PHY_STATE_TRX_OFF    0x0
#define GP_WB_ENUM_PHY_STATE_RX_ON      0x1
#define GP_WB_ENUM_PHY_STATE_TX_ON      0x2
#define GP_WB_ENUM_PHY_STATE_CAL        0x3

/* rssi_offset_sel  */
#define GP_WB_ENUM_RSSI_OFFSET_SEL_RSSI_OFFSET_A    0x0
#define GP_WB_ENUM_RSSI_OFFSET_SEL_RSSI_OFFSET_B    0x1

/* multi_std_mode  */
/* BLE is disabled */
#define GP_WB_ENUM_MULTI_STD_MODE_ZB_ONLY              0x0
/* ZB is disabled */
#define GP_WB_ENUM_MULTI_STD_MODE_BLE_ONLY             0x1
/* Both zigbee and bluetooth are enabled, but zb will be activated if rx is requested for both */
#define GP_WB_ENUM_MULTI_STD_MODE_ZB_PRIORITY          0x2
/* Both zigbee and bluetooth are enabled, but ble will be activated if rx is requested for both */
#define GP_WB_ENUM_MULTI_STD_MODE_BLE_PRIORITY         0x3
/* Used when in multistandart listening mode and both zigbee an ble enabled */
#define GP_WB_ENUM_MULTI_STD_MODE_ZB_BLE_CONCURRENT    0x4

/* receiver_mode  */
#define GP_WB_ENUM_RECEIVER_MODE_LPL        0x0
#define GP_WB_ENUM_RECEIVER_MODE_LPL_AD     0x1
#define GP_WB_ENUM_RECEIVER_MODE_MCH        0x2
#define GP_WB_ENUM_RECEIVER_MODE_MCH_AD     0x3
#define GP_WB_ENUM_RECEIVER_MODE_BLE        0x4
#define GP_WB_ENUM_RECEIVER_MODE_BLE_HDR    0x5

/* zb_receiver_mode  */
#define GP_WB_ENUM_ZB_RECEIVER_MODE_LPL       0x0
#define GP_WB_ENUM_ZB_RECEIVER_MODE_LPL_AD    0x1
#define GP_WB_ENUM_ZB_RECEIVER_MODE_MCH       0x2
#define GP_WB_ENUM_ZB_RECEIVER_MODE_MCH_AD    0x3

/* ble_receiver_mode  */
#define GP_WB_ENUM_BLE_RECEIVER_MODE_BLE        0x0
#define GP_WB_ENUM_BLE_RECEIVER_MODE_BLE_HDR    0x1

/* transmitter_mode  */
#define GP_WB_ENUM_TRANSMITTER_MODE_ZB         0x0
#define GP_WB_ENUM_TRANSMITTER_MODE_BLE        0x1
#define GP_WB_ENUM_TRANSMITTER_MODE_BLE_HDR    0x2

/* ble_transmitter_mode  */
#define GP_WB_ENUM_BLE_TRANSMITTER_MODE_BLE        0x0
#define GP_WB_ENUM_BLE_TRANSMITTER_MODE_BLE_HDR    0x1

/* ble_phy_mode  */
#define GP_WB_ENUM_BLE_PHY_MODE_BLE        0x0
#define GP_WB_ENUM_BLE_PHY_MODE_BLE_HDR    0x1

/* fe_period  */
#define GP_WB_ENUM_FE_PERIOD_MEAS_81     0x0
#define GP_WB_ENUM_FE_PERIOD_MEAS_161    0x1
#define GP_WB_ENUM_FE_PERIOD_MEAS_321    0x2

/* fe_sampler  */
#define GP_WB_ENUM_FE_SAMPLER_RF     0x0
#define GP_WB_ENUM_FE_SAMPLER_DIV    0x1
#define GP_WB_ENUM_FE_SAMPLER_CAL    0x2

/* dtc_div  */
#define GP_WB_ENUM_DTC_DIV_DIV_1     0x1
#define GP_WB_ENUM_DTC_DIV_DIV_8     0x2
#define GP_WB_ENUM_DTC_DIV_DIV_32    0x3
#define GP_WB_ENUM_DTC_DIV_DIV_2     0x5
#define GP_WB_ENUM_DTC_DIV_DIV_16    0x6
#define GP_WB_ENUM_DTC_DIV_DIV_64    0x7

/* fll_state  */
#define GP_WB_ENUM_FLL_STATE_IDLE              0x0
#define GP_WB_ENUM_FLL_STATE_CAL               0x1
#define GP_WB_ENUM_FLL_STATE_PREP_FOR_CL       0x2
#define GP_WB_ENUM_FLL_STATE_LOOP_IS_CLOSED    0x3

/* zb_frame_det_sel  */
#define GP_WB_ENUM_ZB_FRAME_DET_SEL_PRE_PREAMBLE    0x0
#define GP_WB_ENUM_ZB_FRAME_DET_SEL_PREAMBLE        0x1
#define GP_WB_ENUM_ZB_FRAME_DET_SEL_EARLY_SFD       0x2
#define GP_WB_ENUM_ZB_FRAME_DET_SEL_SFD             0x3

/* ble_frame_det_sel  */
#define GP_WB_ENUM_BLE_FRAME_DET_SEL_RSSI            0x0
#define GP_WB_ENUM_BLE_FRAME_DET_SEL_PRE_PREAMBLE    0x1
#define GP_WB_ENUM_BLE_FRAME_DET_SEL_PREAMBLE        0x2
#define GP_WB_ENUM_BLE_FRAME_DET_SEL_EARLY_SFD       0x3
#define GP_WB_ENUM_BLE_FRAME_DET_SEL_SFD             0x4

/***************************
 * layout: iob_enum
 ***************************/
/* gpio_0_alternates  */
#define GP_WB_ENUM_GPIO_0_ALTERNATES_UCSLAVE_INTOUTN       0x0
#define GP_WB_ENUM_GPIO_0_ALTERNATES_ASP_DATA              0x1
#define GP_WB_ENUM_GPIO_0_ALTERNATES_ASP_CLK               0x2
#define GP_WB_ENUM_GPIO_0_ALTERNATES_IR_DO                 0x6
#define GP_WB_ENUM_GPIO_0_ALTERNATES_LED_DO_0              0x7
#define GP_WB_ENUM_GPIO_0_ALTERNATES_PWM_DRV_DO_0          0x8
#define GP_WB_ENUM_GPIO_0_ALTERNATES_PWM_DRV_DO_6          0x9
#define GP_WB_ENUM_GPIO_0_ALTERNATES_PWM_TIMER_DI_0        0xA
#define GP_WB_ENUM_GPIO_0_ALTERNATES_SPI_SL_SSN            0xB
#define GP_WB_ENUM_GPIO_0_ALTERNATES_SWJDP_SWDIO_TMS       0xC
#define GP_WB_ENUM_GPIO_0_ALTERNATES_UART_0_RX             0xD
#define GP_WB_ENUM_GPIO_0_ALTERNATES_UART_1_TX             0xE
#define GP_WB_ENUM_GPIO_0_ALTERNATES_PHY_MODE_CTRL_3       0xF
#define GP_WB_ENUM_GPIO_0_ALTERNATES_PHY_CHANNEL_CTRL_3    0x10
#define GP_WB_ENUM_GPIO_0_ALTERNATES_PHY_ANTSW_CTRL_3      0x11
#define GP_WB_ENUM_GPIO_0_ALTERNATES_SPI_M_MISO            0x12
#define GP_WB_ENUM_GPIO_0_ALTERNATES_I2S_M_SDI             0x13
#define GP_WB_ENUM_GPIO_0_ALTERNATES_I2C_M_SDA             0x14
#define GP_WB_ENUM_GPIO_0_ALTERNATES_I2C_SL_SDA            0x15

/* gpio_1_alternates  */
#define GP_WB_ENUM_GPIO_1_ALTERNATES_ASP_CLK               0x0
#define GP_WB_ENUM_GPIO_1_ALTERNATES_ASP_DATA              0x1
#define GP_WB_ENUM_GPIO_1_ALTERNATES_LED_DO_1              0x5
#define GP_WB_ENUM_GPIO_1_ALTERNATES_PWM_DRV_DO_1          0x6
#define GP_WB_ENUM_GPIO_1_ALTERNATES_PWM_DRV_DO_7          0x7
#define GP_WB_ENUM_GPIO_1_ALTERNATES_PWM_TIMER_DI_1        0x8
#define GP_WB_ENUM_GPIO_1_ALTERNATES_SPI_SL_MISO           0x9
#define GP_WB_ENUM_GPIO_1_ALTERNATES_SPI_SL_SCLK           0xA
#define GP_WB_ENUM_GPIO_1_ALTERNATES_SWJDP_SWV_TDO         0xB
#define GP_WB_ENUM_GPIO_1_ALTERNATES_SWJDP_SWCLK_TCK       0xC
#define GP_WB_ENUM_GPIO_1_ALTERNATES_UART_0_TX             0xD
#define GP_WB_ENUM_GPIO_1_ALTERNATES_UART_1_RX             0xE
#define GP_WB_ENUM_GPIO_1_ALTERNATES_PHY_MODE_CTRL_2       0xF
#define GP_WB_ENUM_GPIO_1_ALTERNATES_PHY_CHANNEL_CTRL_2    0x10
#define GP_WB_ENUM_GPIO_1_ALTERNATES_PHY_ANTSW_CTRL_2      0x11
#define GP_WB_ENUM_GPIO_1_ALTERNATES_SPI_M_SSN             0x12
#define GP_WB_ENUM_GPIO_1_ALTERNATES_SPI_M_MISO            0x13
#define GP_WB_ENUM_GPIO_1_ALTERNATES_I2S_M_WS              0x14
#define GP_WB_ENUM_GPIO_1_ALTERNATES_I2S_M_WS_IN           0x15
#define GP_WB_ENUM_GPIO_1_ALTERNATES_I2C_M_SCL             0x16
#define GP_WB_ENUM_GPIO_1_ALTERNATES_I2C_SL_SCL            0x17

/* gpio_2_alternates  */
#define GP_WB_ENUM_GPIO_2_ALTERNATES_UCSLAVE_INTOUTN       0x0
#define GP_WB_ENUM_GPIO_2_ALTERNATES_ASP_DATA              0x1
#define GP_WB_ENUM_GPIO_2_ALTERNATES_ASP_CLK               0x2
#define GP_WB_ENUM_GPIO_2_ALTERNATES_IR_DO                 0x6
#define GP_WB_ENUM_GPIO_2_ALTERNATES_LED_DO_2              0x7
#define GP_WB_ENUM_GPIO_2_ALTERNATES_PWM_DRV_DO_2          0x8
#define GP_WB_ENUM_GPIO_2_ALTERNATES_PWM_DRV_DO_5          0x9
#define GP_WB_ENUM_GPIO_2_ALTERNATES_PWM_TIMER_DI_2        0xA
#define GP_WB_ENUM_GPIO_2_ALTERNATES_SPI_SL_MOSI           0xB
#define GP_WB_ENUM_GPIO_2_ALTERNATES_SPI_SL_SSN            0xC
#define GP_WB_ENUM_GPIO_2_ALTERNATES_SWJDP_TDI             0xD
#define GP_WB_ENUM_GPIO_2_ALTERNATES_SWJDP_SWDIO_TMS       0xE
#define GP_WB_ENUM_GPIO_2_ALTERNATES_UART_0_RX             0xF
#define GP_WB_ENUM_GPIO_2_ALTERNATES_UART_1_TX             0x10
#define GP_WB_ENUM_GPIO_2_ALTERNATES_PHY_MODE_CTRL_1       0x11
#define GP_WB_ENUM_GPIO_2_ALTERNATES_PHY_CHANNEL_CTRL_1    0x12
#define GP_WB_ENUM_GPIO_2_ALTERNATES_PHY_ANTSW_CTRL_1      0x13
#define GP_WB_ENUM_GPIO_2_ALTERNATES_SPI_M_SCLK            0x14
#define GP_WB_ENUM_GPIO_2_ALTERNATES_I2S_M_SCK             0x15
#define GP_WB_ENUM_GPIO_2_ALTERNATES_I2S_M_SCK_IN          0x16
#define GP_WB_ENUM_GPIO_2_ALTERNATES_I2C_M_SDA             0x17
#define GP_WB_ENUM_GPIO_2_ALTERNATES_I2C_SL_SDA            0x18

/* gpio_3_alternates  */
#define GP_WB_ENUM_GPIO_3_ALTERNATES_ASP_CLK               0x0
#define GP_WB_ENUM_GPIO_3_ALTERNATES_ASP_DATA              0x1
#define GP_WB_ENUM_GPIO_3_ALTERNATES_LED_DO_3              0x6
#define GP_WB_ENUM_GPIO_3_ALTERNATES_PWM_DRV_DO_3          0x7
#define GP_WB_ENUM_GPIO_3_ALTERNATES_PWM_DRV_DO_4          0x8
#define GP_WB_ENUM_GPIO_3_ALTERNATES_PWM_TIMER_DI_3        0x9
#define GP_WB_ENUM_GPIO_3_ALTERNATES_SPI_SL_SCLK           0xA
#define GP_WB_ENUM_GPIO_3_ALTERNATES_SPI_SL_MISO           0xB
#define GP_WB_ENUM_GPIO_3_ALTERNATES_SWJDP_SWCLK_TCK       0xC
#define GP_WB_ENUM_GPIO_3_ALTERNATES_SWJDP_SWV_TDO         0xD
#define GP_WB_ENUM_GPIO_3_ALTERNATES_UART_0_TX             0xE
#define GP_WB_ENUM_GPIO_3_ALTERNATES_UART_1_RX             0xF
#define GP_WB_ENUM_GPIO_3_ALTERNATES_PHY_CHANNEL_CTRL_2    0x10
#define GP_WB_ENUM_GPIO_3_ALTERNATES_PHY_ANTSW_CTRL_0      0x11
#define GP_WB_ENUM_GPIO_3_ALTERNATES_SPI_M_MOSI            0x12
#define GP_WB_ENUM_GPIO_3_ALTERNATES_I2S_M_SDO             0x13
#define GP_WB_ENUM_GPIO_3_ALTERNATES_I2S_M_REF_CLK         0x14
#define GP_WB_ENUM_GPIO_3_ALTERNATES_I2C_M_SCL             0x15
#define GP_WB_ENUM_GPIO_3_ALTERNATES_I2C_SL_SCL            0x16

/* gpio_4_alternates  */
#define GP_WB_ENUM_GPIO_4_ALTERNATES_LED_DO_0              0x2
#define GP_WB_ENUM_GPIO_4_ALTERNATES_PWM_DRV_DO_0          0x3
#define GP_WB_ENUM_GPIO_4_ALTERNATES_PWM_TIMER_DI_0        0x4
#define GP_WB_ENUM_GPIO_4_ALTERNATES_SPI_SL_SSN            0x5
#define GP_WB_ENUM_GPIO_4_ALTERNATES_SPI_SL_MOSI           0x6
#define GP_WB_ENUM_GPIO_4_ALTERNATES_SWJDP_SWDIO_TMS       0x7
#define GP_WB_ENUM_GPIO_4_ALTERNATES_SWJDP_TDI             0x8
#define GP_WB_ENUM_GPIO_4_ALTERNATES_UART_0_RX             0x9
#define GP_WB_ENUM_GPIO_4_ALTERNATES_PHY_CHANNEL_CTRL_3    0xA
#define GP_WB_ENUM_GPIO_4_ALTERNATES_PHY_ANTSW_CTRL_1      0xB
#define GP_WB_ENUM_GPIO_4_ALTERNATES_SPI_M_MISO            0xC
#define GP_WB_ENUM_GPIO_4_ALTERNATES_I2S_M_SDI             0xD
#define GP_WB_ENUM_GPIO_4_ALTERNATES_I2S_M_REF_CLK         0xE

/* gpio_5_alternates  */
#define GP_WB_ENUM_GPIO_5_ALTERNATES_LED_DO_1              0x2
#define GP_WB_ENUM_GPIO_5_ALTERNATES_PWM_DRV_DO_1          0x3
#define GP_WB_ENUM_GPIO_5_ALTERNATES_PWM_TIMER_DI_1        0x4
#define GP_WB_ENUM_GPIO_5_ALTERNATES_SPI_SL_MISO           0x5
#define GP_WB_ENUM_GPIO_5_ALTERNATES_SWJDP_SWV_TDO         0x6
#define GP_WB_ENUM_GPIO_5_ALTERNATES_UART_0_TX             0x7
#define GP_WB_ENUM_GPIO_5_ALTERNATES_PHY_MODE_CTRL_0       0x8
#define GP_WB_ENUM_GPIO_5_ALTERNATES_PHY_CHANNEL_CTRL_0    0x9
#define GP_WB_ENUM_GPIO_5_ALTERNATES_PHY_ANTSW_CTRL_2      0xA
#define GP_WB_ENUM_GPIO_5_ALTERNATES_SPI_M_SSN             0xB
#define GP_WB_ENUM_GPIO_5_ALTERNATES_I2S_M_WS              0xC
#define GP_WB_ENUM_GPIO_5_ALTERNATES_I2S_M_WS_IN           0xD
#define GP_WB_ENUM_GPIO_5_ALTERNATES_I2S_M_REF_CLK         0xE

/* gpio_6_alternates  */
#define GP_WB_ENUM_GPIO_6_ALTERNATES_LED_DO_0              0x3
#define GP_WB_ENUM_GPIO_6_ALTERNATES_PWM_DRV_DO_4          0x4
#define GP_WB_ENUM_GPIO_6_ALTERNATES_PWM_DRV_DO_6          0x5
#define GP_WB_ENUM_GPIO_6_ALTERNATES_SPI_SL_SSN            0x6
#define GP_WB_ENUM_GPIO_6_ALTERNATES_SWJDP_SWDIO_TMS       0x7
#define GP_WB_ENUM_GPIO_6_ALTERNATES_UART_0_RX             0x8
#define GP_WB_ENUM_GPIO_6_ALTERNATES_UART_1_TX             0x9
#define GP_WB_ENUM_GPIO_6_ALTERNATES_PHY_CHANNEL_CTRL_1    0xA
#define GP_WB_ENUM_GPIO_6_ALTERNATES_PHY_ANTSW_CTRL_3      0xB
#define GP_WB_ENUM_GPIO_6_ALTERNATES_SPI_M_MOSI            0xC
#define GP_WB_ENUM_GPIO_6_ALTERNATES_SPI_M_MISO            0xD
#define GP_WB_ENUM_GPIO_6_ALTERNATES_I2S_M_SDO             0xE

/* gpio_7_alternates  */
#define GP_WB_ENUM_GPIO_7_ALTERNATES_LED_DO_1              0x4
#define GP_WB_ENUM_GPIO_7_ALTERNATES_PWM_DRV_DO_5          0x5
#define GP_WB_ENUM_GPIO_7_ALTERNATES_PWM_DRV_DO_7          0x6
#define GP_WB_ENUM_GPIO_7_ALTERNATES_SPI_SL_SCLK           0x7
#define GP_WB_ENUM_GPIO_7_ALTERNATES_SWJDP_SWCLK_TCK       0x8
#define GP_WB_ENUM_GPIO_7_ALTERNATES_UART_0_TX             0x9
#define GP_WB_ENUM_GPIO_7_ALTERNATES_UART_1_RX             0xA
#define GP_WB_ENUM_GPIO_7_ALTERNATES_PHY_MODE_CTRL_0       0xB
#define GP_WB_ENUM_GPIO_7_ALTERNATES_PHY_CHANNEL_CTRL_2    0xC
#define GP_WB_ENUM_GPIO_7_ALTERNATES_PHY_ANTSW_CTRL_2      0xD
#define GP_WB_ENUM_GPIO_7_ALTERNATES_I2C_M_SCL             0xE
#define GP_WB_ENUM_GPIO_7_ALTERNATES_I2C_SL_SCL            0xF

/* gpio_8_alternates  */
#define GP_WB_ENUM_GPIO_8_ALTERNATES_LED_DO_2              0x3
#define GP_WB_ENUM_GPIO_8_ALTERNATES_PWM_DRV_DO_4          0x4
#define GP_WB_ENUM_GPIO_8_ALTERNATES_PWM_DRV_DO_6          0x5
#define GP_WB_ENUM_GPIO_8_ALTERNATES_SPI_SL_MOSI           0x6
#define GP_WB_ENUM_GPIO_8_ALTERNATES_SWJDP_TDI             0x7
#define GP_WB_ENUM_GPIO_8_ALTERNATES_UART_0_RX             0x8
#define GP_WB_ENUM_GPIO_8_ALTERNATES_UART_1_TX             0x9
#define GP_WB_ENUM_GPIO_8_ALTERNATES_PHY_CHANNEL_CTRL_3    0xA
#define GP_WB_ENUM_GPIO_8_ALTERNATES_PHY_ANTSW_CTRL_1      0xB
#define GP_WB_ENUM_GPIO_8_ALTERNATES_I2C_M_SDA             0xC
#define GP_WB_ENUM_GPIO_8_ALTERNATES_I2C_SL_SDA            0xD

/* gpio_9_alternates  */
#define GP_WB_ENUM_GPIO_9_ALTERNATES_LED_DO_3              0x2
#define GP_WB_ENUM_GPIO_9_ALTERNATES_PWM_DRV_DO_5          0x3
#define GP_WB_ENUM_GPIO_9_ALTERNATES_PWM_DRV_DO_7          0x4
#define GP_WB_ENUM_GPIO_9_ALTERNATES_SPI_SL_MISO           0x5
#define GP_WB_ENUM_GPIO_9_ALTERNATES_SWJDP_SWV_TDO         0x6
#define GP_WB_ENUM_GPIO_9_ALTERNATES_UART_0_TX             0x7
#define GP_WB_ENUM_GPIO_9_ALTERNATES_UART_1_RX             0x8
#define GP_WB_ENUM_GPIO_9_ALTERNATES_PHY_CHANNEL_CTRL_0    0x9
#define GP_WB_ENUM_GPIO_9_ALTERNATES_PHY_ANTSW_CTRL_0      0xA

/* gpio_10_alternates  */
#define GP_WB_ENUM_GPIO_10_ALTERNATES_ASP_DATA              0x0
#define GP_WB_ENUM_GPIO_10_ALTERNATES_IR_DO                 0x3
#define GP_WB_ENUM_GPIO_10_ALTERNATES_LED_DO_0              0x4
#define GP_WB_ENUM_GPIO_10_ALTERNATES_PWM_DRV_DO_0          0x5
#define GP_WB_ENUM_GPIO_10_ALTERNATES_PWM_TIMER_DI_0        0x6
#define GP_WB_ENUM_GPIO_10_ALTERNATES_SPI_SL_SSN            0x7
#define GP_WB_ENUM_GPIO_10_ALTERNATES_SWJDP_SWDIO_TMS       0x8
#define GP_WB_ENUM_GPIO_10_ALTERNATES_UART_0_RX             0x9
#define GP_WB_ENUM_GPIO_10_ALTERNATES_UART_0_TX             0xA
#define GP_WB_ENUM_GPIO_10_ALTERNATES_UART_1_TX             0xB
#define GP_WB_ENUM_GPIO_10_ALTERNATES_PHY_CHANNEL_CTRL_0    0xC
#define GP_WB_ENUM_GPIO_10_ALTERNATES_PHY_ANTSW_CTRL_3      0xD
#define GP_WB_ENUM_GPIO_10_ALTERNATES_SPI_M_SCLK            0xE
#define GP_WB_ENUM_GPIO_10_ALTERNATES_I2S_M_WS              0xF
#define GP_WB_ENUM_GPIO_10_ALTERNATES_I2S_M_WS_IN           0x10

/* gpio_11_alternates  */
#define GP_WB_ENUM_GPIO_11_ALTERNATES_ASP_CLK               0x0
#define GP_WB_ENUM_GPIO_11_ALTERNATES_LED_DO_1              0x3
#define GP_WB_ENUM_GPIO_11_ALTERNATES_PWM_DRV_DO_1          0x4
#define GP_WB_ENUM_GPIO_11_ALTERNATES_PWM_TIMER_DI_1        0x5
#define GP_WB_ENUM_GPIO_11_ALTERNATES_DEBUG_DBG_5           0x6
#define GP_WB_ENUM_GPIO_11_ALTERNATES_SPI_SL_SCLK           0x7
#define GP_WB_ENUM_GPIO_11_ALTERNATES_SWJDP_SWCLK_TCK       0x8
#define GP_WB_ENUM_GPIO_11_ALTERNATES_UART_0_TX             0x9
#define GP_WB_ENUM_GPIO_11_ALTERNATES_UART_0_RX             0xA
#define GP_WB_ENUM_GPIO_11_ALTERNATES_UART_1_RX             0xB
#define GP_WB_ENUM_GPIO_11_ALTERNATES_PHY_MODE_CTRL_3       0xC
#define GP_WB_ENUM_GPIO_11_ALTERNATES_PHY_CHANNEL_CTRL_3    0xD
#define GP_WB_ENUM_GPIO_11_ALTERNATES_PHY_ANTSW_CTRL_2      0xE
#define GP_WB_ENUM_GPIO_11_ALTERNATES_SPI_M_MOSI            0xF
#define GP_WB_ENUM_GPIO_11_ALTERNATES_I2S_M_SCK             0x10
#define GP_WB_ENUM_GPIO_11_ALTERNATES_I2S_M_SCK_IN          0x11

/* gpio_12_alternates  */
#define GP_WB_ENUM_GPIO_12_ALTERNATES_UCSLAVE_MCU_CLK       0x0
#define GP_WB_ENUM_GPIO_12_ALTERNATES_UCSLAVE_FRING         0x1
#define GP_WB_ENUM_GPIO_12_ALTERNATES_ASP_DATA              0x2
#define GP_WB_ENUM_GPIO_12_ALTERNATES_IR_DO                 0x5
#define GP_WB_ENUM_GPIO_12_ALTERNATES_LED_DO_2              0x6
#define GP_WB_ENUM_GPIO_12_ALTERNATES_PWM_DRV_DO_2          0x7
#define GP_WB_ENUM_GPIO_12_ALTERNATES_PWM_DRV_DO_7          0x8
#define GP_WB_ENUM_GPIO_12_ALTERNATES_PWM_TIMER_DI_2        0x9
#define GP_WB_ENUM_GPIO_12_ALTERNATES_DEBUG_DBG_1           0xA
#define GP_WB_ENUM_GPIO_12_ALTERNATES_SPI_SL_MOSI           0xB
#define GP_WB_ENUM_GPIO_12_ALTERNATES_SPI_SL_SSN            0xC
#define GP_WB_ENUM_GPIO_12_ALTERNATES_SWJDP_TDI             0xD
#define GP_WB_ENUM_GPIO_12_ALTERNATES_SWJDP_SWDIO_TMS       0xE
#define GP_WB_ENUM_GPIO_12_ALTERNATES_UART_0_RX             0xF
#define GP_WB_ENUM_GPIO_12_ALTERNATES_UART_0_TX             0x10
#define GP_WB_ENUM_GPIO_12_ALTERNATES_UART_1_TX             0x11
#define GP_WB_ENUM_GPIO_12_ALTERNATES_UART_1_RX             0x12
#define GP_WB_ENUM_GPIO_12_ALTERNATES_PHY_CHANNEL_CTRL_2    0x13
#define GP_WB_ENUM_GPIO_12_ALTERNATES_PHY_ANTSW_CTRL_1      0x14
#define GP_WB_ENUM_GPIO_12_ALTERNATES_SPI_M_MISO            0x15
#define GP_WB_ENUM_GPIO_12_ALTERNATES_SPI_M_SCLK            0x16
#define GP_WB_ENUM_GPIO_12_ALTERNATES_I2S_M_SDO             0x17
#define GP_WB_ENUM_GPIO_12_ALTERNATES_I2S_M_REF_CLK         0x18

/* gpio_13_alternates  */
#define GP_WB_ENUM_GPIO_13_ALTERNATES_UCSLAVE_MCU_RSTN      0x0
#define GP_WB_ENUM_GPIO_13_ALTERNATES_UCSLAVE_DIG_TEST      0x1
#define GP_WB_ENUM_GPIO_13_ALTERNATES_ASP_CLK               0x2
#define GP_WB_ENUM_GPIO_13_ALTERNATES_IR_DO                 0x5
#define GP_WB_ENUM_GPIO_13_ALTERNATES_LED_DO_3              0x6
#define GP_WB_ENUM_GPIO_13_ALTERNATES_PWM_DRV_DO_3          0x7
#define GP_WB_ENUM_GPIO_13_ALTERNATES_PWM_DRV_DO_6          0x8
#define GP_WB_ENUM_GPIO_13_ALTERNATES_PWM_TIMER_DI_3        0x9
#define GP_WB_ENUM_GPIO_13_ALTERNATES_DEBUG_DBG_0           0xA
#define GP_WB_ENUM_GPIO_13_ALTERNATES_SPI_SL_MISO           0xB
#define GP_WB_ENUM_GPIO_13_ALTERNATES_SPI_SL_SCLK           0xC
#define GP_WB_ENUM_GPIO_13_ALTERNATES_SWJDP_SWV_TDO         0xD
#define GP_WB_ENUM_GPIO_13_ALTERNATES_SWJDP_SWCLK_TCK       0xE
#define GP_WB_ENUM_GPIO_13_ALTERNATES_UART_0_TX             0xF
#define GP_WB_ENUM_GPIO_13_ALTERNATES_UART_0_RX             0x10
#define GP_WB_ENUM_GPIO_13_ALTERNATES_UART_1_RX             0x11
#define GP_WB_ENUM_GPIO_13_ALTERNATES_UART_1_TX             0x12
#define GP_WB_ENUM_GPIO_13_ALTERNATES_PHY_MODE_CTRL_3       0x13
#define GP_WB_ENUM_GPIO_13_ALTERNATES_PHY_CHANNEL_CTRL_1    0x14
#define GP_WB_ENUM_GPIO_13_ALTERNATES_PHY_ANTSW_CTRL_0      0x15
#define GP_WB_ENUM_GPIO_13_ALTERNATES_SPI_M_SSN             0x16
#define GP_WB_ENUM_GPIO_13_ALTERNATES_SPI_M_MOSI            0x17
#define GP_WB_ENUM_GPIO_13_ALTERNATES_I2S_M_SDI             0x18
#define GP_WB_ENUM_GPIO_13_ALTERNATES_I2S_M_WS              0x19
#define GP_WB_ENUM_GPIO_13_ALTERNATES_I2S_M_WS_IN           0x1A
#define GP_WB_ENUM_GPIO_13_ALTERNATES_I2C_M_SDA             0x1B
#define GP_WB_ENUM_GPIO_13_ALTERNATES_I2C_SL_SDA            0x1C

/* gpio_14_alternates  */
#define GP_WB_ENUM_GPIO_14_ALTERNATES_UCSLAVE_FRING         0x0
#define GP_WB_ENUM_GPIO_14_ALTERNATES_ASP_DATA              0x1
#define GP_WB_ENUM_GPIO_14_ALTERNATES_ASP_CLK               0x2
#define GP_WB_ENUM_GPIO_14_ALTERNATES_IR_INP                0x5
#define GP_WB_ENUM_GPIO_14_ALTERNATES_LED_DO_0              0x6
#define GP_WB_ENUM_GPIO_14_ALTERNATES_PWM_DRV_DO_0          0x7
#define GP_WB_ENUM_GPIO_14_ALTERNATES_PWM_DRV_DO_4          0x8
#define GP_WB_ENUM_GPIO_14_ALTERNATES_PWM_TIMER_DI_0        0x9
#define GP_WB_ENUM_GPIO_14_ALTERNATES_DEBUG_DBG_4           0xA
#define GP_WB_ENUM_GPIO_14_ALTERNATES_SPI_SL_MOSI           0xB
#define GP_WB_ENUM_GPIO_14_ALTERNATES_SWJDP_TDI             0xC
#define GP_WB_ENUM_GPIO_14_ALTERNATES_UART_0_RX             0xD
#define GP_WB_ENUM_GPIO_14_ALTERNATES_UART_1_TX             0xE
#define GP_WB_ENUM_GPIO_14_ALTERNATES_PHY_MODE_CTRL_2       0xF
#define GP_WB_ENUM_GPIO_14_ALTERNATES_PHY_CHANNEL_CTRL_2    0x10
#define GP_WB_ENUM_GPIO_14_ALTERNATES_PHY_ANTSW_CTRL_3      0x11
#define GP_WB_ENUM_GPIO_14_ALTERNATES_SPI_M_SCLK            0x12
#define GP_WB_ENUM_GPIO_14_ALTERNATES_I2S_M_SCK             0x13
#define GP_WB_ENUM_GPIO_14_ALTERNATES_I2S_M_SCK_IN          0x14
#define GP_WB_ENUM_GPIO_14_ALTERNATES_I2C_M_SCL             0x15
#define GP_WB_ENUM_GPIO_14_ALTERNATES_I2C_SL_SCL            0x16

/* gpio_15_alternates  */
#define GP_WB_ENUM_GPIO_15_ALTERNATES_UCSLAVE_DIG_TEST      0x0
#define GP_WB_ENUM_GPIO_15_ALTERNATES_ASP_CLK               0x1
#define GP_WB_ENUM_GPIO_15_ALTERNATES_ASP_DATA              0x2
#define GP_WB_ENUM_GPIO_15_ALTERNATES_LED_DO_1              0x5
#define GP_WB_ENUM_GPIO_15_ALTERNATES_PWM_DRV_DO_1          0x6
#define GP_WB_ENUM_GPIO_15_ALTERNATES_PWM_DRV_DO_5          0x7
#define GP_WB_ENUM_GPIO_15_ALTERNATES_PWM_TIMER_DI_1        0x8
#define GP_WB_ENUM_GPIO_15_ALTERNATES_DEBUG_DBG_3           0x9
#define GP_WB_ENUM_GPIO_15_ALTERNATES_SPI_SL_MISO           0xA
#define GP_WB_ENUM_GPIO_15_ALTERNATES_SWJDP_SWV_TDO         0xB
#define GP_WB_ENUM_GPIO_15_ALTERNATES_UART_0_TX             0xC
#define GP_WB_ENUM_GPIO_15_ALTERNATES_UART_1_RX             0xD
#define GP_WB_ENUM_GPIO_15_ALTERNATES_PHY_MODE_CTRL_1       0xE
#define GP_WB_ENUM_GPIO_15_ALTERNATES_PHY_CHANNEL_CTRL_1    0xF
#define GP_WB_ENUM_GPIO_15_ALTERNATES_PHY_ANTSW_CTRL_2      0x10
#define GP_WB_ENUM_GPIO_15_ALTERNATES_SPI_M_MOSI            0x11
#define GP_WB_ENUM_GPIO_15_ALTERNATES_SPI_M_MISO            0x12
#define GP_WB_ENUM_GPIO_15_ALTERNATES_I2S_M_SDO             0x13
#define GP_WB_ENUM_GPIO_15_ALTERNATES_I2S_M_REF_CLK         0x14
#define GP_WB_ENUM_GPIO_15_ALTERNATES_I2C_M_SDA             0x15
#define GP_WB_ENUM_GPIO_15_ALTERNATES_I2C_SL_SDA            0x16

/* gpio_16_alternates  */
#define GP_WB_ENUM_GPIO_16_ALTERNATES_ASP_DATA              0x0
#define GP_WB_ENUM_GPIO_16_ALTERNATES_ASP_CLK               0x1
#define GP_WB_ENUM_GPIO_16_ALTERNATES_IR_DO                 0x6
#define GP_WB_ENUM_GPIO_16_ALTERNATES_LED_DO_2              0x7
#define GP_WB_ENUM_GPIO_16_ALTERNATES_PWM_DRV_DO_2          0x8
#define GP_WB_ENUM_GPIO_16_ALTERNATES_PWM_DRV_DO_6          0x9
#define GP_WB_ENUM_GPIO_16_ALTERNATES_PWM_TIMER_DI_2        0xA
#define GP_WB_ENUM_GPIO_16_ALTERNATES_DEBUG_DBG_2           0xB
#define GP_WB_ENUM_GPIO_16_ALTERNATES_SPI_SL_SSN            0xC
#define GP_WB_ENUM_GPIO_16_ALTERNATES_SWJDP_SWDIO_TMS       0xD
#define GP_WB_ENUM_GPIO_16_ALTERNATES_UART_0_RX             0xE
#define GP_WB_ENUM_GPIO_16_ALTERNATES_UART_1_TX             0xF
#define GP_WB_ENUM_GPIO_16_ALTERNATES_PHY_MODE_CTRL_0       0x10
#define GP_WB_ENUM_GPIO_16_ALTERNATES_PHY_CHANNEL_CTRL_0    0x11
#define GP_WB_ENUM_GPIO_16_ALTERNATES_PHY_ANTSW_CTRL_1      0x12
#define GP_WB_ENUM_GPIO_16_ALTERNATES_SPI_M_MISO            0x13
#define GP_WB_ENUM_GPIO_16_ALTERNATES_SPI_M_SSN             0x14
#define GP_WB_ENUM_GPIO_16_ALTERNATES_I2S_M_SDI             0x15
#define GP_WB_ENUM_GPIO_16_ALTERNATES_I2S_M_WS              0x16
#define GP_WB_ENUM_GPIO_16_ALTERNATES_I2S_M_WS_IN           0x17
#define GP_WB_ENUM_GPIO_16_ALTERNATES_I2C_M_SCL             0x18
#define GP_WB_ENUM_GPIO_16_ALTERNATES_I2C_SL_SCL            0x19

/* gpio_17_alternates  */
#define GP_WB_ENUM_GPIO_17_ALTERNATES_ASP_CLK               0x0
#define GP_WB_ENUM_GPIO_17_ALTERNATES_ASP_DATA              0x1
#define GP_WB_ENUM_GPIO_17_ALTERNATES_LED_DO_2              0x3
#define GP_WB_ENUM_GPIO_17_ALTERNATES_PWM_DRV_DO_4          0x4
#define GP_WB_ENUM_GPIO_17_ALTERNATES_PWM_TIMER_DI_2        0x5
#define GP_WB_ENUM_GPIO_17_ALTERNATES_SPI_SL_SSN            0x6
#define GP_WB_ENUM_GPIO_17_ALTERNATES_SWJDP_SWDIO_TMS       0x7
#define GP_WB_ENUM_GPIO_17_ALTERNATES_UART_0_RX             0x8
#define GP_WB_ENUM_GPIO_17_ALTERNATES_UART_0_TX             0x9
#define GP_WB_ENUM_GPIO_17_ALTERNATES_UART_1_RX             0xA
#define GP_WB_ENUM_GPIO_17_ALTERNATES_PHY_MODE_CTRL_0       0xB
#define GP_WB_ENUM_GPIO_17_ALTERNATES_PHY_CHANNEL_CTRL_0    0xC
#define GP_WB_ENUM_GPIO_17_ALTERNATES_PHY_ANTSW_CTRL_3      0xD
#define GP_WB_ENUM_GPIO_17_ALTERNATES_SPI_M_SCLK            0xE
#define GP_WB_ENUM_GPIO_17_ALTERNATES_I2S_M_SCK             0xF
#define GP_WB_ENUM_GPIO_17_ALTERNATES_I2S_M_SCK_IN          0x10
#define GP_WB_ENUM_GPIO_17_ALTERNATES_I2S_M_REF_CLK         0x11
#define GP_WB_ENUM_GPIO_17_ALTERNATES_I2C_M_SDA             0x12
#define GP_WB_ENUM_GPIO_17_ALTERNATES_I2C_SL_SDA            0x13

/* gpio_18_alternates  */
#define GP_WB_ENUM_GPIO_18_ALTERNATES_ASP_DATA              0x0
#define GP_WB_ENUM_GPIO_18_ALTERNATES_ASP_CLK               0x1
#define GP_WB_ENUM_GPIO_18_ALTERNATES_IR_DO                 0x3
#define GP_WB_ENUM_GPIO_18_ALTERNATES_LED_DO_3              0x4
#define GP_WB_ENUM_GPIO_18_ALTERNATES_PWM_DRV_DO_5          0x5
#define GP_WB_ENUM_GPIO_18_ALTERNATES_PWM_TIMER_DI_3        0x6
#define GP_WB_ENUM_GPIO_18_ALTERNATES_SPI_SL_MISO           0x7
#define GP_WB_ENUM_GPIO_18_ALTERNATES_SWJDP_SWV_TDO         0x8
#define GP_WB_ENUM_GPIO_18_ALTERNATES_UART_0_TX             0x9
#define GP_WB_ENUM_GPIO_18_ALTERNATES_UART_0_RX             0xA
#define GP_WB_ENUM_GPIO_18_ALTERNATES_UART_1_TX             0xB
#define GP_WB_ENUM_GPIO_18_ALTERNATES_PHY_MODE_CTRL_1       0xC
#define GP_WB_ENUM_GPIO_18_ALTERNATES_PHY_CHANNEL_CTRL_1    0xD
#define GP_WB_ENUM_GPIO_18_ALTERNATES_PHY_ANTSW_CTRL_2      0xE
#define GP_WB_ENUM_GPIO_18_ALTERNATES_SPI_M_MOSI            0xF
#define GP_WB_ENUM_GPIO_18_ALTERNATES_I2S_M_SDO             0x10
#define GP_WB_ENUM_GPIO_18_ALTERNATES_I2C_M_SCL             0x11
#define GP_WB_ENUM_GPIO_18_ALTERNATES_I2C_SL_SCL            0x12

/* gpio_19_alternates  */
#define GP_WB_ENUM_GPIO_19_ALTERNATES_ASP_DATA           0x0
#define GP_WB_ENUM_GPIO_19_ALTERNATES_PWM_TIMER_DI_2     0x2
#define GP_WB_ENUM_GPIO_19_ALTERNATES_UART_1_RX          0x3
#define GP_WB_ENUM_GPIO_19_ALTERNATES_I2S_M_REF_CLK      0x4

/* gpio_20_alternates  */
#define GP_WB_ENUM_GPIO_20_ALTERNATES_PWM_TIMER_DI_3     0x1
#define GP_WB_ENUM_GPIO_20_ALTERNATES_I2S_M_REF_CLK      0x2

/* gpio_21_alternates  */
#define GP_WB_ENUM_GPIO_21_ALTERNATES_PWM_TIMER_DI_0     0x4
#define GP_WB_ENUM_GPIO_21_ALTERNATES_SPI_SL_MOSI        0x5
#define GP_WB_ENUM_GPIO_21_ALTERNATES_SWJDP_TDI          0x6
#define GP_WB_ENUM_GPIO_21_ALTERNATES_UART_0_RX          0x7
#define GP_WB_ENUM_GPIO_21_ALTERNATES_I2S_M_SCK_IN       0x8

/* gpio_22_alternates  */
#define GP_WB_ENUM_GPIO_22_ALTERNATES_ASP_DATA           0x0
#define GP_WB_ENUM_GPIO_22_ALTERNATES_PWM_TIMER_DI_1     0x2
#define GP_WB_ENUM_GPIO_22_ALTERNATES_SPI_SL_SCLK        0x3
#define GP_WB_ENUM_GPIO_22_ALTERNATES_SWJDP_SWCLK_TCK    0x4
#define GP_WB_ENUM_GPIO_22_ALTERNATES_UART_0_RX          0x5
#define GP_WB_ENUM_GPIO_22_ALTERNATES_SPI_M_MISO         0x6
#define GP_WB_ENUM_GPIO_22_ALTERNATES_I2S_M_SDI          0x7
#define GP_WB_ENUM_GPIO_22_ALTERNATES_I2S_M_WS_IN        0x8
#define GP_WB_ENUM_GPIO_22_ALTERNATES_I2S_M_REF_CLK      0x9

#endif //GP_WB_ENUM_H
