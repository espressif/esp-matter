/*
 *
 * HIGH SPEED LOW LEVEL DISPLAY FUNCTIONS USING DIRECT TRANSFER MODE
 *
*/

#ifndef _TFTSPI_H_
#define _TFTSPI_H_

#include "tftspi.h"
#include "spi_master_lobo.h"
#include "sdkconfig.h"
#include "stmpe610.h"

#define TOUCH_TYPE_NONE		0
#define TOUCH_TYPE_XPT2046	1
#define TOUCH_TYPE_STMPE610	2

#define TP_CALX_XPT2046		7472920
#define TP_CALY_XPT2046		122224794

#define TP_CALX_STMPE610	21368532
#define TP_CALY_STMPE610	11800144

// === Screen tft_orientation constants ===
#define PORTRAIT	0
#define LANDSCAPE	1
#define PORTRAIT_FLIP	2
#define LANDSCAPE_FLIP	3

#define DISP_TYPE_ILI9341	0
#define DISP_TYPE_ILI9488	1
#define DISP_TYPE_ST7789V	2
#define DISP_TYPE_ST7735	3
#define DISP_TYPE_ST7735R	4
#define DISP_TYPE_ST7735B	5



#if CONFIG_TFT_PREDEFINED_DISPLAY_TYPE == 1

// ** Set the correct configuration for ESP-WROVER-KIT v3
// --------------------------------------------------------
#define DEFAULT_DISP_TYPE           DISP_TYPE_ST7789V
#define DEFAULT_TFT_DISPLAY_WIDTH   240
#define DEFAULT_TFT_DISPLAY_HEIGHT  320
#define DISP_COLOR_BITS_24          0x66
#define DEFAULT_GAMMA_CURVE         0
#define DEFAULT_SPI_CLOCK           26000000
#define TFT_INVERT_ROTATION         0
#define TFT_INVERT_ROTATION1        1
#define TFT_RGB_BGR                 0x00

#define USE_TOUCH	TOUCH_TYPE_NONE

#define PIN_NUM_MISO 25		// SPI MISO
#define PIN_NUM_MOSI 23		// SPI MOSI
#define PIN_NUM_CLK  19		// SPI CLOCK pin
#define PIN_NUM_CS   22		// Display CS pin
#define PIN_NUM_DC   21		// Display command/data pin
#define PIN_NUM_TCS   0		// Touch screen CS pin

#define PIN_NUM_RST  18  	// GPIO used for RESET control
#define PIN_NUM_BCKL  5     // GPIO used for backlight control
#define PIN_BCKL_ON   0     // GPIO value for backlight ON
#define PIN_BCKL_OFF  1     // GPIO value for backlight OFF
// --------------------------------------------------------

#elif CONFIG_TFT_PREDEFINED_DISPLAY_TYPE == 2

// ** Set the correct configuration for Adafruit TFT Feather
// ---------------------------------------------------------
#define DEFAULT_DISP_TYPE   DISP_TYPE_ILI9341
#define DEFAULT_TFT_DISPLAY_WIDTH   240
#define DEFAULT_TFT_DISPLAY_HEIGHT  320
#define DISP_COLOR_BITS_24          0x66
#define DEFAULT_GAMMA_CURVE         0
#define DEFAULT_SPI_CLOCK           26000000
#define TFT_INVERT_ROTATION         0
#define TFT_INVERT_ROTATION1        0
#define TFT_RGB_BGR                 0x08

#define USE_TOUCH                   TOUCH_TYPE_STMPE610

#define PIN_NUM_MISO 19		// SPI MISO
#define PIN_NUM_MOSI 18		// SPI MOSI
#define PIN_NUM_CLK  5		// SPI CLOCK pin
#define PIN_NUM_CS   15		// Display CS pin
#define PIN_NUM_DC   33		// Display command/data pin
#define PIN_NUM_TCS  32		// Touch screen CS pin (NOT used if USE_TOUCH=0)

#define PIN_NUM_RST  0  	// GPIO used for RESET control (#16)
#define PIN_NUM_BCKL 0  	// GPIO used for backlight control
#define PIN_BCKL_ON  0  	// GPIO value for backlight ON
#define PIN_BCKL_OFF 1  	// GPIO value for backlight OFF
// ---------------------------------------------------------

#elif CONFIG_TFT_PREDEFINED_DISPLAY_TYPE == 3

// ** Set the correct configuration for M5Stack TFT
// ---------------------------------------------------------
#define DEFAULT_DISP_TYPE   DISP_TYPE_ILI9341
#define DEFAULT_TFT_DISPLAY_WIDTH   320
#define DEFAULT_TFT_DISPLAY_HEIGHT  240
#define DISP_COLOR_BITS_24          0x66
#define DEFAULT_GAMMA_CURVE         0
#define DEFAULT_SPI_CLOCK           26000000
#define TFT_INVERT_ROTATION         0
#define TFT_INVERT_ROTATION1        0
#define TFT_INVERT_ROTATION2        1   // Adapte M5Stack TFT
#define TFT_RGB_BGR                 0x08

#define USE_TOUCH                   TOUCH_TYPE_NONE

#define PIN_NUM_MISO 19		// SPI MISO
#define PIN_NUM_MOSI 23		// SPI MOSI
#define PIN_NUM_CLK  18		// SPI CLOCK pin
#define PIN_NUM_CS   14		// Display CS pin
#define PIN_NUM_DC   27		// Display command/data pin
#define PIN_NUM_TCS  0		// Touch screen CS pin (NOT used if USE_TOUCH=0)

#define PIN_NUM_RST  33  	// GPIO used for RESET control (#16)
#define PIN_NUM_BCKL 32  	// GPIO used for backlight control
#define PIN_BCKL_ON  1  	// GPIO value for backlight ON
#define PIN_BCKL_OFF 0  	// GPIO value for backlight OFF
// ---------------------------------------------------------

#elif CONFIG_TFT_PREDEFINED_DISPLAY_TYPE == 4

// ** Set the correct configuration for ESP-WROVER-KIT v4.1
// --------------------------------------------------------
#define DEFAULT_DISP_TYPE           DISP_TYPE_ILI9341
#define DEFAULT_TFT_DISPLAY_WIDTH   240
#define DEFAULT_TFT_DISPLAY_HEIGHT  320
#define DISP_COLOR_BITS_24          0x66
#define DEFAULT_GAMMA_CURVE         0
#define DEFAULT_SPI_CLOCK           26000000
#define TFT_INVERT_ROTATION         0
#define TFT_INVERT_ROTATION1        0
#define TFT_INVERT_ROTATION2        0
#define TFT_RGB_BGR                 0x08

#define USE_TOUCH   TOUCH_TYPE_NONE

#define PIN_NUM_MISO 25     // SPI MISO
#define PIN_NUM_MOSI 23     // SPI MOSI
#define PIN_NUM_CLK  19     // SPI CLOCK pin
#define PIN_NUM_CS   22     // Display CS pin
#define PIN_NUM_DC   21     // Display command/data pin
#define PIN_NUM_TCS   0     // Touch screen CS pin

#define PIN_NUM_RST  18     // GPIO used for RESET control
#define PIN_NUM_BCKL  5     // GPIO used for backlight control
#define PIN_BCKL_ON   0     // GPIO value for backlight ON
#define PIN_BCKL_OFF  1     // GPIO value for backlight OFF
// --------------------------------------------------------
#elif CONFIG_TFT_PREDEFINED_DISPLAY_TYPE == 5
//CONFIG FOR TTGO T-DISPLAY
#define DEFAULT_DISP_TYPE           DISP_TYPE_ST7789V
#define DEFAULT_TFT_DISPLAY_WIDTH   135
#define DEFAULT_TFT_DISPLAY_HEIGHT  240

//Need to be defined together so they can be swapped for x;y when rotating
#define TFT_STATIC_WIDTH_OFFSET 53
#define TFT_STATIC_HEIGHT_OFFSET 40

#define DISP_COLOR_BITS_24          0x66
#define DEFAULT_GAMMA_CURVE         0
#define DEFAULT_SPI_CLOCK           20000000
#define TFT_INVERT_ROTATION         0
#define TFT_INVERT_ROTATION1        1
#define TFT_RGB_BGR                 0x00
//To be used by user application for initialization
#define TFT_START_COLORS_INVERTED

#define USE_TOUCH	TOUCH_TYPE_NONE

#define PIN_NUM_MISO 0		// SPI MISO
#define PIN_NUM_MOSI 19		// SPI MOSI
#define PIN_NUM_CLK  18		// SPI CLOCK pin
#define PIN_NUM_CS   5		// Display CS pin
#define PIN_NUM_DC   16		// Display command/data pin
#define PIN_NUM_TCS   0		// Touch screen CS pin

#define PIN_NUM_RST  23  	// GPIO used for RESET control
#define PIN_NUM_BCKL  4     // GPIO used for backlight control
#define PIN_BCKL_ON   1     // GPIO value for backlight ON
#define PIN_BCKL_OFF  0     // GPIO value for backlight OFF
//END TTGO T_DISPLAY

#elif CONFIG_TFT_PREDEFINED_DISPLAY_TYPE == 6
//CONFIG FOR TTGO T-WRISTBAND
#define DEFAULT_DISP_TYPE           DISP_TYPE_ST7735R
#define DEFAULT_TFT_DISPLAY_WIDTH   80
#define DEFAULT_TFT_DISPLAY_HEIGHT  160

//Need to be defined together so they can be swapped for x;y when rotating
#define TFT_STATIC_WIDTH_OFFSET 26
#define TFT_STATIC_HEIGHT_OFFSET 1

#define DISP_COLOR_BITS_24          0x66
#define DEFAULT_GAMMA_CURVE         0
#define DEFAULT_SPI_CLOCK           27000000
#define TFT_INVERT_ROTATION         0
#define TFT_INVERT_ROTATION1        1
#define TFT_RGB_BGR                 0x00
//To be used by user application for initialization
#define TFT_START_COLORS_INVERTED

#define USE_TOUCH       TOUCH_TYPE_NONE

#define PIN_NUM_MISO 0          // SPI MISO
#define PIN_NUM_MOSI 19         // SPI MOSI
#define PIN_NUM_CLK  18         // SPI CLOCK pin
#define PIN_NUM_CS   5          // Display CS pin
#define PIN_NUM_DC   23         // Display command/data pin
#define PIN_NUM_TCS   0         // Touch screen CS pin

#define PIN_NUM_RST  26         // GPIO used for RESET control
#define PIN_NUM_BCKL 27         // GPIO used for backlight control
#define PIN_BCKL_ON   1     // GPIO value for backlight ON
#define PIN_BCKL_OFF  0     // GPIO value for backlight OFF
//END TTGO T_DISPLAY
#else

// Configuration for other boards, set the correct values for the display used
//----------------------------------------------------------------------------
#define DISP_COLOR_BITS_24	0x66
//#define DISP_COLOR_BITS_16	0x55  // Do not use!

#define TFT_INVERT_ROTATION 0
#define TFT_INVERT_ROTATION1 CONFIG_TFT_INVERT_ROTATION1

// ################################################
// ### SET TO 0X00 FOR DISPLAYS WITH RGB MATRIX ###
// ### SET TO 0X08 FOR DISPLAYS WITH BGR MATRIX ###
// ### For ESP-WROWER-KIT set to 0x00           ###
// ################################################

#if CONFIG_TFT_RGB_BGR
#define TFT_RGB_BGR 0x00
#else
#define TFT_RGB_BGR 0x08
#endif

// ##############################################################
// ### Define ESP32 SPI pins to which the display is attached ###
// ##############################################################

// The pins configured here are the native spi pins for HSPI interface
// Any other valid pin combination can be used

#define PIN_NUM_MISO CONFIG_TFT_PIN_NUM_MISO
#define PIN_NUM_MOSI CONFIG_TFT_PIN_NUM_MOSI
#define PIN_NUM_CLK CONFIG_TFT_PIN_NUM_CLK
#define PIN_NUM_CS CONFIG_TFT_PIN_NUM_CS
#define PIN_NUM_DC CONFIG_TFT_PIN_NUM_DC
#define PIN_NUM_TCS CONFIG_TFT_PIN_NUM_TCS
#define PIN_NUM_RST CONFIG_TFT_PIN_NUM_RST
#define PIN_NUM_BCKL CONFIG_TFT_PIN_NUM_BCKL

#define PIN_BCKL_ON  0  	// GPIO value for backlight ON
#define PIN_BCKL_OFF 1  	// GPIO value for backlight OFF
// --------------------------------------------------------------

#define USE_TOUCH CONFIG_TFT_TOUCH_CONTROLLER

// #######################################################################
// Default display width (smaller dimension) and height (larger dimension)
// #######################################################################
#define DEFAULT_TFT_DISPLAY_WIDTH  CONFIG_TFT_DISPLAY_WIDTH
#define DEFAULT_TFT_DISPLAY_HEIGHT CONFIG_TFT_DISPLAY_HEIGHT
// #######################################################################

#define DEFAULT_GAMMA_CURVE 0
#define DEFAULT_SPI_CLOCK   26000000

#if defined(CONFIG_TFT_DISPLAY_CONTROLLER_MODEL)
#define DEFAULT_DISP_TYPE CONFIG_TFT_DISPLAY_CONTROLLER_MODEL
#else
#define DEFAULT_DISP_TYPE   DISP_TYPE_ILI9341
#endif
//----------------------------------------------------------------------------

#endif  // CONFIG_PREDEFINED_DISPLAY_TYPE

// Define offset generation, or ignore offsets if none are needed
#ifdef TFT_STATIC_WIDTH_OFFSET
#define TFT_STATIC_X_OFFSET (tft_orientation & 1 ? TFT_STATIC_HEIGHT_OFFSET : TFT_STATIC_WIDTH_OFFSET)
#define TFT_STATIC_Y_OFFSET (tft_orientation & 1 ? TFT_STATIC_WIDTH_OFFSET : TFT_STATIC_HEIGHT_OFFSET)
#else
#define TFT_STATIC_WIDTH_OFFSET 0
#define TFT_STATIC_X_OFFSET 0
#define TFT_STATIC_HEIGHT_OFFSET 0
#define TFT_STATIC_Y_OFFSET 0
#endif


// ##############################################################
// #### Global variables                                     ####
// ##############################################################

// ==== Converts colors to grayscale if 1 =======================
extern uint8_t tft_gray_scale;

// ==== Spi clock for reading data from display memory in Hz ====
extern uint32_t tft_max_rdclock;

// ==== Display dimensions in pixels ============================
extern int tft_width;
extern int tft_height;

// ==== Display type, DISP_TYPE_ILI9488 or DISP_TYPE_ILI9341 ====
extern uint8_t tft_disp_type;

// ==== Spi device handles for display and touch screen =========
extern spi_lobo_device_handle_t tft_disp_spi;
extern spi_lobo_device_handle_t tft_ts_spi;

// ##############################################################

// 24-bit color type structure
typedef struct __attribute__((__packed__)) {
//typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} color_t ;

// ==== Display commands constants ====
#define TFT_INVOFF     0x20
#define TFT_INVONN     0x21
#define TFT_DISPOFF    0x28
#define TFT_DISPON     0x29
#define TFT_MADCTL	   0x36
#define TFT_PTLAR 	   0x30
#define TFT_ENTRYM 	   0xB7

#define TFT_CMD_NOP			0x00
#define TFT_CMD_SWRESET		0x01
#define TFT_CMD_RDDID		0x04
#define TFT_CMD_RDDST		0x09

#define TFT_CMD_SLPIN		0x10
#define TFT_CMD_SLPOUT		0x11
#define TFT_CMD_PTLON		0x12
#define TFT_CMD_NORON		0x13

#define TFT_CMD_RDMODE		0x0A
#define TFT_CMD_RDMADCTL	0x0B
#define TFT_CMD_RDPIXFMT	0x0C
#define TFT_CMD_RDIMGFMT	0x0D
#define TFT_CMD_RDSELFDIAG  0x0F

#define TFT_CMD_GAMMASET	0x26

#define TFT_CMD_FRMCTR1		0xB1
#define TFT_CMD_FRMCTR2		0xB2
#define TFT_CMD_FRMCTR3		0xB3
#define TFT_CMD_INVCTR		0xB4
#define TFT_CMD_DFUNCTR		0xB6

#define TFT_CMD_PWCTR1		0xC0
#define TFT_CMD_PWCTR2		0xC1
#define TFT_CMD_PWCTR3		0xC2
#define TFT_CMD_PWCTR4		0xC3
#define TFT_CMD_PWCTR5		0xC4
#define TFT_CMD_VMCTR1		0xC5
#define TFT_CMD_VMCTR2		0xC7

#define TFT_CMD_RDID1		0xDA
#define TFT_CMD_RDID2		0xDB
#define TFT_CMD_RDID3		0xDC
#define TFT_CMD_RDID4		0xDD

#define TFT_CMD_GMCTRP1		0xE0
#define TFT_CMD_GMCTRN1		0xE1

#define TFT_CMD_POWERA		0xCB
#define TFT_CMD_POWERB		0xCF
#define TFT_CMD_POWER_SEQ	0xED
#define TFT_CMD_DTCA		0xE8
#define TFT_CMD_DTCB		0xEA
#define TFT_CMD_PRC			0xF7
#define TFT_CMD_3GAMMA_EN	0xF2

#define ST_CMD_VCOMS       0xBB
#define ST_CMD_FRCTRL2      0xC6
#define ST_CMD_PWCTR1		0xD0

#define ST7735_FRMCTR1 0xB1
#define ST7735_FRMCTR2 0xB2
#define ST7735_FRMCTR3 0xB3
#define ST7735_INVCTR  0xB4
#define ST7735_DISSET5 0xB6

#define ST7735_PWCTR1  0xC0
#define ST7735_PWCTR2  0xC1
#define ST7735_PWCTR3  0xC2
#define ST7735_PWCTR4  0xC3
#define ST7735_PWCTR5  0xC4
#define ST7735_VMCTR1  0xC5

#define ST7735_RDID1   0xDA
#define ST7735_RDID2   0xDB
#define ST7735_RDID3   0xDC
#define ST7735_RDID4   0xDD
#define ST7735_NOP     0x00
#define ST7735_SWRESET 0x01
#define ST7735_RDDID   0x04
#define ST7735_RDDST   0x09

#define ST7735_SLPIN   0x10
#define ST7735_SLPOUT  0x11
#define ST7735_PTLON   0x12
#define ST7735_NORON   0x13
#define ST7735_PWCTR6  0xFC
#define ST7735_GMCTRP1 0xE0
#define ST7735_GMCTRN1 0xE1

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_MH  0x04

#define TFT_CASET		0x2A
#define TFT_PASET		0x2B
#define TFT_RAMWR		0x2C
#define TFT_RAMRD		0x2E
#define TFT_CMD_PIXFMT	0x3A

#define TFT_CMD_DELAY	0x80


// Initialization sequence for ILI7749
// ====================================
static const uint8_t ST7789V_init[] = {
#if PIN_NUM_RST
  15,                   					        // 15 commands in list
#else
  16,                   					        // 16 commands in list
  TFT_CMD_SWRESET, TFT_CMD_DELAY,					//  1: Software reset, no args, w/delay
  200,												//     200 ms delay
#endif
  TFT_CMD_FRMCTR2, 5, 0x0c, 0x0c, 0x00, 0x33, 0x33,
  TFT_ENTRYM, 1, 0x45,
  ST_CMD_VCOMS, 1, 0x2B,
  TFT_CMD_PWCTR1, 1, 0x2C,
  TFT_CMD_PWCTR3, 2, 0x01, 0xff,
  TFT_CMD_PWCTR4, 1, 0x11,
  TFT_CMD_PWCTR5, 1, 0x20,
  ST_CMD_FRCTRL2, 1, 0x0f,
  ST_CMD_PWCTR1, 2, 0xA4, 0xA1,
  TFT_CMD_GMCTRP1, 14, 0xD0, 0x00, 0x05, 0x0E, 0x15, 0x0D, 0x37, 0x43, 0x47, 0x09, 0x15, 0x12, 0x16, 0x19,
  TFT_CMD_GMCTRN1, 14, 0xD0, 0x00, 0x05, 0x0D, 0x0C, 0x06, 0x2D, 0x44, 0x40, 0x0E, 0x1C, 0x18, 0x16, 0x19,
  TFT_MADCTL, 1, (MADCTL_MX | TFT_RGB_BGR),			// Memory Access Control (tft_orientation)
  TFT_CMD_PIXFMT, 1, DISP_COLOR_BITS_24,            // *** INTERFACE PIXEL FORMAT: 0x66 -> 18 bit; 0x55 -> 16 bit
  TFT_CMD_SLPOUT, TFT_CMD_DELAY, 120,				//  Sleep out,	//  120 ms delay
  TFT_DISPON, TFT_CMD_DELAY, 120,
};

// Initialization sequence for ILI7341
// ====================================
static const uint8_t ILI9341_init[] = {
#if PIN_NUM_RST
  23,                   					        // 24 commands in list
#else
  24,                   					        // 24 commands in list
  TFT_CMD_SWRESET, TFT_CMD_DELAY,					//  1: Software reset, no args, w/delay
  250,												//     200 ms delay
#endif
  TFT_CMD_POWERA, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
  TFT_CMD_POWERB, 3, 0x00, 0XC1, 0X30,
  0xEF, 3, 0x03, 0x80, 0x02,
  TFT_CMD_DTCA, 3, 0x85, 0x00, 0x78,
  TFT_CMD_DTCB, 2, 0x00, 0x00,
  TFT_CMD_POWER_SEQ, 4, 0x64, 0x03, 0X12, 0X81,
  TFT_CMD_PRC, 1, 0x20,
  TFT_CMD_PWCTR1, 1, 0x23,							//Power control VRH[5:0]
  TFT_CMD_PWCTR2, 1, 0x10,							//Power control SAP[2:0];BT[3:0]
  TFT_CMD_VMCTR1, 2, 0x3e, 0x28,					//VCM control
  TFT_CMD_VMCTR2, 1, 0x86,							//VCM control2
  TFT_MADCTL, 1,									// Memory Access Control (tft_orientation)
  (MADCTL_MX | TFT_RGB_BGR),
  // *** INTERFACE PIXEL FORMAT: 0x66 -> 18 bit; 0x55 -> 16 bit
  TFT_CMD_PIXFMT, 1, DISP_COLOR_BITS_24,
  TFT_INVOFF, 0,
  TFT_CMD_FRMCTR1, 2, 0x00, 0x18,
  TFT_CMD_DFUNCTR, 4, 0x08, 0x82, 0x27, 0x00,		// Display Function Control
  TFT_PTLAR, 4, 0x00, 0x00, 0x01, 0x3F,
  TFT_CMD_3GAMMA_EN, 1, 0x00,						// 3Gamma Function: Disable (0x02), Enable (0x03)
  TFT_CMD_GAMMASET, 1, 0x01,						//Gamma curve selected (0x01, 0x02, 0x04, 0x08)
  TFT_CMD_GMCTRP1, 15,   							//Positive Gamma Correction
  0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
  TFT_CMD_GMCTRN1, 15,   							//Negative Gamma Correction
  0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
  TFT_CMD_SLPOUT, TFT_CMD_DELAY,					//  Sleep out
  200,			 									//  120 ms delay
  TFT_DISPON, TFT_CMD_DELAY, 200,
};

// Initialization sequence for ILI9488
// ====================================
static const uint8_t ILI9488_init[] = {
#if PIN_NUM_RST
  17,                   					        // 17 commands in list
#else
  18,                   					        // 18 commands in list
  TFT_CMD_SWRESET, TFT_CMD_DELAY,					//  1: Software reset, no args, w/delay
  200,												//     200 ms delay
#endif
  0xE0, 15, 0x00, 0x03, 0x09, 0x08, 0x16, 0x0A, 0x3F, 0x78, 0x4C, 0x09, 0x0A, 0x08, 0x16, 0x1A, 0x0F,
  0xE1, 15,	0x00, 0x16, 0x19, 0x03, 0x0F, 0x05, 0x32, 0x45, 0x46, 0x04, 0x0E, 0x0D, 0x35, 0x37, 0x0F,
  0xC0, 2,   //Power Control 1
	0x17,    //Vreg1out
	0x15,    //Verg2out

  0xC1, 1,   //Power Control 2
	0x41,    //VGH,VGL

  0xC5, 3,   //Power Control 3
	0x00,
	0x12,    //Vcom
	0x80,

#if TFT_INVERT_ROTATION
  TFT_MADCTL, 1, (MADCTL_MV | TFT_RGB_BGR),			// Memory Access Control (tft_orientation), set to portrait
#else
  TFT_MADCTL, 1, (MADCTL_MX | TFT_RGB_BGR),			// Memory Access Control (tft_orientation), set to portrait
#endif

  // *** INTERFACE PIXEL FORMAT: 0x66 -> 18 bit;
  TFT_CMD_PIXFMT, 1, DISP_COLOR_BITS_24,

  0xB0, 1,   // Interface Mode Control
	0x00,    // 0x80: SDO NOT USE; 0x00 USE SDO

  0xB1, 1,   //Frame rate
	0xA0,    //60Hz

  0xB4, 1,   //Display Inversion Control
	0x02,    //2-dot

  0xB6, 2,   //Display Function Control  RGB/MCU Interface Control
	0x02,    //MCU
	0x02,    //Source,Gate scan direction

  0xE9, 1,   // Set Image Function
	0x00,    // Disable 24 bit data

  0x53, 1,   // Write CTRL Display Value
	0x28,    // BCTRL && DD on

  0x51, 1,   // Write Display Brightness Value
	0x7F,    //

  0xF7, 4,   // Adjust Control
	0xA9,
	0x51,
	0x2C,
	0x02,    // D7 stream, loose


  0x11, TFT_CMD_DELAY,  //Exit Sleep
    120,
  0x29, 0,      //Display on

};


// Initialization commands for 7735B screens
// ------------------------------------
static const uint8_t STP7735_init[] = {
#if PIN_NUM_RST
  16,                       // 17 commands in list
#else
  17,						// 18 commands in list:
  ST7735_SLPOUT,   TFT_CMD_DELAY,	//  2: Out of sleep mode, no args, w/delay
  255,			           			//     255 = 500 ms delay
#endif
  TFT_CMD_PIXFMT, 1+TFT_CMD_DELAY,	//  3: Set color mode, 1 arg + delay:
  0x06, 							//     18-bit color 6-6-6 color format
  10,	          					//     10 ms delay
  ST7735_FRMCTR1, 3+TFT_CMD_DELAY,	//  4: Frame rate control, 3 args + delay:
  0x00,						//     fastest refresh
  0x06,						//     6 lines front porch
  0x03,						//     3 lines back porch
  10,						//     10 ms delay
  TFT_MADCTL , 1      ,		//  5: Memory access ctrl (directions), 1 arg:
  0x08,						//     Row addr/col addr, bottom to top refresh
  ST7735_DISSET5, 2      ,	//  6: Display settings #5, 2 args, no delay:
  0x15,						//     1 clk cycle nonoverlap, 2 cycle gate
  // rise, 3 cycle osc equalize
  0x02,						//     Fix on VTL
  ST7735_INVCTR , 1      ,	//  7: Display inversion control, 1 arg:
  0x0,						//     Line inversion
  ST7735_PWCTR1 , 2+TFT_CMD_DELAY,	//  8: Power control, 2 args + delay:
  0x02,						//     GVDD = 4.7V
  0x70,						//     1.0uA
  10,						//     10 ms delay
  ST7735_PWCTR2 , 1      ,	//  9: Power control, 1 arg, no delay:
  0x05,						//     VGH = 14.7V, VGL = -7.35V
  ST7735_PWCTR3 , 2      ,	// 10: Power control, 2 args, no delay:
  0x01,						//     Opamp current small
  0x02,						//     Boost frequency
  ST7735_VMCTR1 , 2+TFT_CMD_DELAY,	// 11: Power control, 2 args + delay:
  0x3C,						//     VCOMH = 4V
  0x38,						//     VCOML = -1.1V
  10,						//     10 ms delay
  ST7735_PWCTR6 , 2      ,	// 12: Power control, 2 args, no delay:
  0x11, 0x15,
  ST7735_GMCTRP1,16      ,	// 13: Magical unicorn dust, 16 args, no delay:
  0x09, 0x16, 0x09, 0x20,	//     (seriously though, not sure what
  0x21, 0x1B, 0x13, 0x19,	//      these config values represent)
  0x17, 0x15, 0x1E, 0x2B,
  0x04, 0x05, 0x02, 0x0E,
  ST7735_GMCTRN1,16+TFT_CMD_DELAY,	// 14: Sparkles and rainbows, 16 args + delay:
  0x0B, 0x14, 0x08, 0x1E,	//     (ditto)
  0x22, 0x1D, 0x18, 0x1E,
  0x1B, 0x1A, 0x24, 0x2B,
  0x06, 0x06, 0x02, 0x0F,
  10,						//     10 ms delay
  TFT_CASET  , 4      , 	// 15: Column addr set, 4 args, no delay:
  0x00, 0x02,				//     XSTART = 2
  0x00, 0x81,				//     XEND = 129
  TFT_PASET  , 4      , 	// 16: Row addr set, 4 args, no delay:
  0x00, 0x02,				//     XSTART = 1
  0x00, 0x81,				//     XEND = 160
  ST7735_NORON  ,   TFT_CMD_DELAY,	// 17: Normal display on, no args, w/delay
  10,						//     10 ms delay
  TFT_DISPON ,   TFT_CMD_DELAY,  	// 18: Main screen turn on, no args, w/delay
  255						//     255 = 500 ms delay
};

// Init for 7735R, part 1 (red or green tab)
// --------------------------------------
static const uint8_t  STP7735R_init[] = {
#if PIN_NUM_RST
  14,                       // 14 commands in list
#else
  15,						// 15 commands in list:
  ST7735_SWRESET,   TFT_CMD_DELAY,	//  1: Software reset, 0 args, w/delay
  150,						//     150 ms delay
#endif
  ST7735_SLPOUT ,   TFT_CMD_DELAY,	//  2: Out of sleep mode, 0 args, w/delay
  255,						//     500 ms delay
  ST7735_FRMCTR1, 3      ,	//  3: Frame rate ctrl - normal mode, 3 args:
  0x01, 0x2C, 0x2D,			//     Rate = fosc/(1x2+40) * (LINE+2C+2D)
  ST7735_FRMCTR2, 3      ,	//  4: Frame rate control - idle mode, 3 args:
  0x01, 0x2C, 0x2D,			//     Rate = fosc/(1x2+40) * (LINE+2C+2D)
  ST7735_FRMCTR3, 6      ,	//  5: Frame rate ctrl - partial mode, 6 args:
  0x01, 0x2C, 0x2D,			//     Dot inversion mode
  0x01, 0x2C, 0x2D,			//     Line inversion mode
  ST7735_INVCTR , 1      ,	//  6: Display inversion ctrl, 1 arg, no delay:
  0x07,						//     No inversion
  ST7735_PWCTR1 , 3      ,	//  7: Power control, 3 args, no delay:
  0xA2,
  0x02,						//     -4.6V
  0x84,						//     AUTO mode
  ST7735_PWCTR2 , 1      ,	//  8: Power control, 1 arg, no delay:
  0xC5,						//     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
  ST7735_PWCTR3 , 2      ,	//  9: Power control, 2 args, no delay:
  0x0A,						//     Opamp current small
  0x00,						//     Boost frequency
  ST7735_PWCTR4 , 2      ,	// 10: Power control, 2 args, no delay:
  0x8A,						//     BCLK/2, Opamp current small & Medium low
  0x2A,
  ST7735_PWCTR5 , 2      ,	// 11: Power control, 2 args, no delay:
  0x8A, 0xEE,
  ST7735_VMCTR1 , 1      ,	// 12: Power control, 1 arg, no delay:
  0x0E,
  TFT_INVOFF , 0      ,		// 13: Don't invert display, no args, no delay
  TFT_MADCTL , 1      ,		// 14: Memory access control (directions), 1 arg:
  0xC0,						//     row addr/col addr, bottom to top refresh, RGB order
  TFT_CMD_PIXFMT , 1+TFT_CMD_DELAY,	//  15: Set color mode, 1 arg + delay:
  0x06,								//      18-bit color 6-6-6 color format
  10						//     10 ms delay
};

// Init for 7735R, part 2 (green tab only)
// ---------------------------------------
static const uint8_t Rcmd2green[] = {
  2,						//  2 commands in list:
  TFT_CASET  , 4      ,		//  1: Column addr set, 4 args, no delay:
  0x00, 0x02,				//     XSTART = 0
  0x00, 0x7F+0x02,			//     XEND = 129
  TFT_PASET  , 4      ,	    //  2: Row addr set, 4 args, no delay:
  0x00, 0x01,				//     XSTART = 0
  0x00, 0x9F+0x01			//     XEND = 160
};

// Init for 7735R, part 2 (red tab only)
// -------------------------------------
static const uint8_t Rcmd2red[] = {
  2,						//  2 commands in list:
  TFT_CASET  , 4      ,	    //  1: Column addr set, 4 args, no delay:
  0x00, 0x00,				//     XSTART = 0
  0x00, 0x7F,				//     XEND = 127
  TFT_PASET  , 4      ,	    //  2: Row addr set, 4 args, no delay:
  0x00, 0x00,				//     XSTART = 0
  0x00, 0x9F				//     XEND = 159
};

// Init for 7735R, part 3 (red or green tab)
// -----------------------------------------
static const uint8_t Rcmd3[] = {
  4,						//  4 commands in list:
  ST7735_GMCTRP1, 16      ,	//  1: Magical unicorn dust, 16 args, no delay:
  0x02, 0x1c, 0x07, 0x12,
  0x37, 0x32, 0x29, 0x2d,
  0x29, 0x25, 0x2B, 0x39,
  0x00, 0x01, 0x03, 0x10,
  ST7735_GMCTRN1, 16      ,	//  2: Sparkles and rainbows, 16 args, no delay:
  0x03, 0x1d, 0x07, 0x06,
  0x2E, 0x2C, 0x29, 0x2D,
  0x2E, 0x2E, 0x37, 0x3F,
  0x00, 0x00, 0x02, 0x10,
  ST7735_NORON  ,    TFT_CMD_DELAY,	//  3: Normal display on, no args, w/delay
  10,						//     10 ms delay
  TFT_DISPON ,    TFT_CMD_DELAY,	//  4: Main screen turn on, no args w/delay
  100						//     100 ms delay
};


// ==== Public functions =========================================================

// == Low level functions; usually not used directly ==
esp_err_t wait_trans_finish(uint8_t free_line);
void disp_spi_transfer_cmd(int8_t cmd);
void disp_spi_transfer_cmd_data(int8_t cmd, uint8_t *data, uint32_t len);
void drawPixel(int16_t x, int16_t y, color_t color, uint8_t sel);
void send_data(int x1, int y1, int x2, int y2, uint32_t len, color_t *buf);
void TFT_pushColorRep(int x1, int y1, int x2, int y2, color_t data, uint32_t len);
int read_data(int x1, int y1, int x2, int y2, int len, uint8_t *buf, uint8_t set_sp);
color_t readPixel(int16_t x, int16_t y);
int touch_get_data(uint8_t type);


// Deactivate display's CS line
//========================
esp_err_t disp_deselect();

// Activate display's CS line and configure SPI interface if necessary
//======================
esp_err_t disp_select();


// Find maximum spi clock for successful read from display RAM
// ** Must be used AFTER the display is initialized **
//======================
uint32_t find_rd_speed();


// Change the screen rotation.
// Input: m new rotation value (0 to 3)
//=================================
void _tft_setRotation(uint8_t rot);

// Initialize all pins used by display driver
// ** MUST be executed before SPI interface initialization
//=================
void TFT_PinsInit();

// Perform display initialization sequence
// Sets tft_orientation to landscape; clears the screen
// * All pins must be configured
// * SPI interface must already be setup
// * 'tft_disp_type', 'COLOR_BITS', 'tft_width', 'tft_height' variables must be set
//======================
void TFT_display_init();

//===================
void stmpe610_Init();

//============================================================
int stmpe610_get_touch(uint16_t *x, uint16_t *y, uint16_t *z);

//========================
uint32_t stmpe610_getID();

// ===============================================================================

#endif
