//  Copyright 2015 Silicon Laboratories, Inc.                               *80*

// -- GPIO access for nSEL signals
#define GPIO_PxCLR_BASE                              (GPIO_PACLR_ADDR)
#define GPIO_PxSET_BASE                              (GPIO_PASET_ADDR)
#define GPIO_PxOUT_BASE                              (GPIO_PAOUT_ADDR)
#define GPIO_PxCFG_BASE                              (GPIO_PACFGL_ADDR)
// Each port is offset from the previous port by the same amount
#define GPIO_Px_OFFSET                               (GPIO_PBCFGL_ADDR \
                                                      - GPIO_PACFGL_ADDR)

// Page 0 register
#define CS5463_CONFIG_REG                            0
#define CS5463_CURRENTDCOFFSET_REG                   1
#define CS5463_CURRENTGAIN_REG                       2
#define CS5463_VOLTAGEDCOFFSET_REG                   3
#define CS5463_VOLTAGEGAIN_REG                       4
#define CS5463_CYCLECOUNT_REG                        5
#define CS5463_PULSERATEE_REG                        6
#define CS5463_INSTCURRENT_REG                       7
#define CS5463_INSTVOLTAGE_REG                       8
#define CS5463_INSTPOWER_REG                         9
#define CS5463_ACTIVEPOWER_REG                       10
#define CS5463_RMSCURRENT_REG                        11
#define CS5463_RMSVOLTAGE_REG                        12
#define CS5463_EPSILON_REG                           13
#define CS5463_POWEROFFSET_REG                       14
#define CS5463_STATUS_REG                            15
#define CS5463_IACOFF_REG                            16
#define CS5463_VACOFF_REG                            17
#define CS5463_MODE_REG                              18
#define CS5463_TEMPERATURE_REG                       19
#define CS5463_REACTIVEPOWERAVG_REG                  20
#define CS5463_INSTREACTIVEPOWER_REG                 21
#define CS5463_PEAKCURRENT_REG                       22
#define CS5463_PEAKVOLTAGE_REG                       23
#define CS5463_REACTIVEPOWERTRIANGLE_REG             24
#define CS5463_POWERFACTOR_REG                       25
#define CS5463_MASK_REG                              26
#define CS5463_APPARENTPOWER_REG                     27
#define CS5463_CTRL_REG                              28
#define CS5463_HARMONICACTIVEPOWER_REG               29
#define CS5463_FUNDACTIVEPOWER_REG                   30
#define CS5463_FUNDREACTIVEPOWER_REG                 31
#define CS5463_PAGE_NUM                              31

// Page 1 register
#define CS5463_PULSEWIDTH                            0
#define CS5463_NO_LOAD_THRES                         1
#define CS5463_TEMP_GAIN                             2
#define CS5463_TEMP_OFFSET                           3

// read write bit for register access
#define CS5463_WRITE                                 1
#define CS5463_READ                                  0

// bit location
#define CS5463_WR_BIT                                6
#define CS5463_REG_ADDR_BIT                          1

#define PAGE_0                                       0
#define PAGE_1                                       1
#define PAGE_3                                       3

#define VOLTAGE_SCALE                                422 // full range is 250mV,
// and the voltage is
// scaled down by 1690 in HW
// so the scale constant = 1690 * 0.250
#define CURRENT_SCALE                                50 // full range is 50mV,
// and the current
// to voltage is 1A:1mV,
// so the scale constant = 1000 * 0.050

// bit location of configuration register
#define PC_BIT                                       17
#define IGAIN_BIT                                    16
#define EWA_BIT                                      15
#define IMODE_BIT                                    12
#define IINV_BIT                                     11
#define ICPU_BIT                                     4
#define CDIV_BIT                                     0

// default bit value for power meter applicaiton
#define PC                                           (0 << PC_BIT) // phase
// compensation,
// 7 bit
// value
#define IGAIN                                        (1 << IGAIN_BIT) // to set
// I scale
// to
// 100mVpp
#define EWA                                          (0 << EWA_BIT) // 0 =
// Normal
// outputs
// (default)
// 1 = Only the pull-down device
//    of the #E1 and #E2 pins are active

// IMODE, IINV Interrupt configuration bits.
// Select the desired pin behavior for indication of an interrupt.
// 00 = Active-low level (default)
// 01 = Active-high level
// 10 = High-to-low pulse
// 11 = Low-to-high pulse
#define IMODE                                                      (0 \
                                                                    << IMODE_BIT)
#define IINV                                                       (0 \
                                                                    << IINV_BIT)

// iCPU Inverts the CPUCLK clock.
// 0 = Normal operation (default)
// 1 = Minimize noise when CPUCLK is driving rising edge logic
#define ICPU                                                       (0 \
                                                                    << ICPU_BIT)

// Clock divider. A 4-bit binary number used to divide the value of
// MCLK to generate the internal clock DCLK. The internal clock frequency
// is DCLK = MCLK/K. The value of K can range between 1 and 16. Note that
// a value of 0000 will set K to 16 (not zero). K = 1 at reset
#define CDIV                                                       (1 \
                                                                    << CDIV_BIT)

// status register definition
#define CS5463_ERRORBIT_VSAG                                       BIT32(10)
#define CS5463_ERRORBIT_EOR                                        BIT32(12)
#define CS5463_ERRORBIT_VROR                                       BIT32(13)
#define CS5463_ERRORBIT_IROR                                       BIT32(14)
#define CS5463_ERRORBIT_VOR                                        BIT32(16)
#define CS5463_ERRORBIT_IOR                                        BIT32(17)
#define CS5463_STATUS_CONVREADY                                    BIT32(20)
#define CS5463_STATUS_DATAREADY                                    BIT32(23)

// over current and over heat threshold
#define CS5463_OVER_CURRENT_THRESHOLD                              14500
#define CS5463_OVER_HEAT_THRESHOLD                                 5000

#define WRITE_BUFFER_SIZE                                          4

// commands instructions
#define CS5463_CMD_START_CONV_SINGLE                               0xE0
#define CS5463_CMD_START_CONV_CONTINUOUS                           0xE8
#define CS5463_CMD_SYNC0                                           0xFE
#define CS5463_CMD_SYNC1                                           0xFF
#define CS5463_CMD_POWERUP_HALT                                    0xA0
#define CS5463_CMD_POWERDOWN_RESET                                 0x80
#define CS5463_CMD_POWERDOWN_STANDBY                               0x88
#define CS5463_CMD_POWERDOWN_SLEEP                                 0x90
#define CS5463_CMD_CALIBRATION_CURRENT_DC_OFFSET                   0xC9
#define CS5463_CMD_CALIBRATION_CURRENT_DC_GAIN                     0xCA
#define CS5463_CMD_CALIBRATION_CURRENT_AC_OFFSET                   0xCD
#define CS5463_CMD_CALIBRATION_CURRENT_AC_GAIN                     0xCE
#define CS5463_CMD_CALIBRATION_VOLTAGE_DC_OFFSET                   0xD1
#define CS5463_CMD_CALIBRATION_VOLTAGE_DC_GAIN                     0xD2
#define CS5463_CMD_CALIBRATION_VOLTAGE_AC_OFFSET                   0xD5
#define CS5463_CMD_CALIBRATION_VOLTAGE_AC_GAIN                     0xD6
#define CS5463_CMD_CALIBRATION_CURRENTVOLTAGE_DC_OFFSET            0xD9
#define CS5463_CMD_CALIBRATION_CURRENTVOLTAGE_DC_GAIN              0xDA
#define CS5463_CMD_CALIBRATION_CURRENTVOLTAGE_AC_OFFSET            0xDD
#define CS5463_CMD_CALIBRATION_CURRENTVOLTAGE                      AC_GAIN \
  0xDE

// calibrate options
#define CS5463_CALIBRATE_ALL                                       0
#define CS5463_CALIBRATE_DC                                        1
#define CS5463_CALIBRATE_AC                                        2

#define CS5463_INIT_DELAY_MS                                       1000
#define CS5463_READ_INTERVAL_MS                                    2000
#define CS5463_CALIBRATE_INTERVAL_MS                               5000
#define CS5463_MEASUREMENT_DELAY_MS                                2000

#define CS5463_OVER_CURRENT_TO_NORMAL                              0
#define CS5463_NORMAL_TO_OVER_CURRENT                              1
#define CS5463_OVER_HEAT_TO_NORMAL                                 0
#define CS5463_NORMAL_TO_OVER_HEAT                                 1

#define CONVERT_NEG_24_BIT_TO_NEG_32_BIT(x) ((x) | 0xFF000000)

#define ADC_REGISTER_SIGN_BIT                                      23
