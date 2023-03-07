
### TFT library for ESP32

---

**This library must be built with the esp-idf release/v4.0 branch.**

ESP-IDF 4.0 is currently in beta and instructions are found [here](
https://docs.espressif.com/projects/esp-idf/en/v4.0-beta1/get-started/index.html)
---

#### Features

* Full support for **ILI9341**, **ILI9488**, **ST7789V** and **ST7735** based TFT modules in 4-wire SPI mode.
* **18-bit (RGB)** color mode used
* **SPI displays oriented SPI driver library** based on *spi-master* driver
* Combined **DMA SPI** transfer mode and **direct SPI** for maximal speed
* **Grayscale mode** can be selected during runtime which converts all colors to gray scale
* SPI speeds up to **40 MHz** are tested and work without problems
* **Demo application** included which demonstrates most of the library features


* **Graphics drawing functions**:
  * **TFT_drawPixel**  Draw pixel at given x,y coordinates
  * **TFT_drawLine**  Draw line between two points
  * **TFT_drawFastVLine**, **TFT_drawFastHLine**  Draw vertical or horizontal line of given length
  * **TFT_drawLineByAngle**  Draw line on screen from (x,y) point at given angle
  * **TFT_drawRect**, **TFT_fillRect**  Draw rectangle on screen or fill given rectangular screen region with color
  * **TFT_drawRoundRect**, **TFT_fillRoundRect**  Draw rectangle on screen or fill given rectangular screen region with color with rounded corners
  * **TFT_drawCircle**, **TFT_fillCircle**  Draw or fill circle on screen
  * **TFT_drawEllipse**, **TFT_fillEllipse**  Draw or fill ellipse on screen
  * **TFT_drawTriangle**, **TFT_fillTriangle**  Draw or fill triangle on screen
  * **TFT_drawArc**  Draw circle arc on screen, from ~ to given angles, with given thickness. Can be outlined with different color
  * **TFT_drawPolygon**  Draw polygon on screen with given number of sides (3~60). Can be outlined with different color and rotated by given angle
* **Fonts**:
  * **fixed** width and proportional fonts are supported; 8 fonts embedded
  * unlimited number of **fonts from file**
  * **7-segment vector font** with variable width/height is included (only numbers and few characters)
  * Proportional fonts can be used in fixed width mode
  * Related functions:
    * **TFT_setFont**  Set current font from one of embeded fonts or font file
    * **TFT_getfontsize**  Returns current font height & width in pixels
    * **TFT_getfontheight**  Returns current font height in pixels
    * **set_7seg_font_atrib**  Set attributes for 7 segment vector font
    * **getFontCharacters**  Get all font's characters to buffer
* **String write function**:
  * **TFT_print**  Write text to display
    * Strings can be printed at **any angle**. Rotation of the displayed text depends on *tft_font_rotate* variable (0~360)
    * if *font_transparent* variable is set to 1, no background pixels will be printed
    * If the text does not fit the screen/window width it will be clipped ( if *text_wrap=0* ), or continued on next line ( if *text_wrap=1* )
    * Two special characters are allowed in strings: *\r* CR (0x0D), clears the display to EOL, *\n* LF (ox0A), continues to the new line, x=0
    * Special values can be entered for X position:
      * *CENTER*  centers the text
      * *RIGHT*   right justifies the text horizontaly
      * *LASTX*   continues from last X position; offset can be used: *LASTX+n*
    * Special values can be entered for Y:
      * *CENTER*  centers the text verticaly
      * *BOTTOM*  bottom justifies the text
      * *LASTY*   continues from last Y position; offset can be used: *LASTY+n*
  * **TFT_getStringWidth** Returns the string width in pixels based on current font characteristics. Useful for positioning strings on the screen
  * **TFT_clearStringRect** Fills the rectangle occupied by string with current background color
* **Images**:
  * **TFT_jpg_image**  Decodes and displays JPEG images
    * Limits:
      * Baseline only. Progressive and Lossless JPEG format are not supported
      * Image size: Up to 65520 x 65520 pixels
      * Color space: YCbCr three components only. Gray scale image is not supported
      * Sampling factor: 4:4:4, 4:2:2 or 4:2:0
    * Can display the image **from file** or **memory buffer**
    * Image can be **scaled** by factor 0 ~ 3  (1/1, 1/2, 1/4 or 1/8)
    * Image is displayed from X,Y position on screen/window:
      * X: image left position; constants CENTER & RIGHT can be used; *negative* value is accepted
      * Y: image top position;  constants CENTER & BOTTOM can be used; *negative* value is accepted
  * **TFT_bmp_image**  Decodes and displays BMP images
    * Only uncompressed RGB 24-bit with no color space information BMP images can be displayed
    * Can display the image **from file** or **memory buffer**
    * Image can be **scaled** by factor 0 ~ 7; if scale>0, image is scaled by factor 1/(scale+1)
    * Image is displayed from X,Y position on screen/window:
      * X: image left position; constants CENTER & RIGHT can be used; *negative* value is accepted
      * Y: image top position;  constants CENTER & BOTTOM can be used; *negative* value is accepted
* **Window functions**:
  * Drawing on screen can be limited to rectangular *window*, smaller than the full display dimensions
  * When defined, all graphics, text and image coordinates are translated to *window* coordinates
  * Related functions
    * **TFT_setclipwin**  Sets the *window* area coordinates
    * **TFT_resetclipwin**  Reset the *window* to full screen dimensions
    * **TFT_saveClipWin**  Save current *window* to temporary variable
    * **TFT_restoreClipWin**  Restore current *window* from temporary variable
    * **TFT_fillWindow**  Fill *window* area with color
* **Touch screen** supported (for now only **XPT2046** controllers)
  * **TFT_read_touch**  Detect if touched and return X,Y coordinates. **Raw** touch screen or **calibrated** values can be returned.
    * calibrated coordinates are adjusted for screen orientation
* **Read from display memory** supported
  * **TFT_readPixel**  Read pixel color value from display GRAM at given x,y coordinates
  * **TFT_readData**  Read color data from rectangular screen area
* **Other display functions**:
  * **TFT_fillScreen**  Fill the whole screen with color
  * **TFT_setRotation**  Set screen rotation; PORTRAIT, PORTRAIT_FLIP, LANDSCAPE and LANDSCAPE_FLIP are supported
  * **TFT_invertDisplay**  Set inverted/normal colors
  * **TFT_compare_colors**  Compare two color structures
  * **disp_select()**  Activate display's CS line
  * **disp_deselect()**  Deactivate display's CS line
  * **find_rd_speed()**  Find maximum spi clock for successful read from display RAM
  * **TFT_display_init()**  Perform display initialization sequence. Sets orientation to landscape; clears the screen. SPI interface must already be set up, *tft_disp_type*, *tft_width*, *tft_height* variables must be set.
  * **HSBtoRGB**  Converts the components of a color, as specified by the HSB model to an equivalent set of values for the default RGB model
  * **TFT_setGammaCurve()** Select one of 4 Gamma curves
* **compile_font_file**  Function which compiles font c source file to font file which can be used in *TFT_setFont()* function to select external font. Created file have the same name as source file and extension *.fnt*


* **Global variables**
  * **tft_orientation**  current screen orientation
  * **tft_font_rotate**  current font rotate angle (0~395)
  * **tft_font_transparent**  if not 0 draw fonts transparent
  * **tft_font_forceFixed**  if not zero force drawing proportional fonts with fixed width
  * **tft_text_wrap**  if not 0 wrap long text to the new line, else clip
  * **tft_fg**  current foreground color for fonts
  * **tft_bg**  current background for non transparent fonts
  * **tft_dispWin** current display clip window
  * **tft_angleOffset**  angle offset for arc, polygon and line by angle functions
  * **tft_image_debug**  print debug messages during image decode if set to 1
  * **tft_cfont**  Currently used font structure
  * **tft_x**  X position of the next character after TFT_print() function
  * **tft_y**  Y position of the next character after TFT_print() function
  * **tft_tp_calx**  touch screen X calibration constant
  * **tft_tp_caly**  touch screen Y calibration constant
  * **tft_gray_scale**  convert all colors to gray scale if set to 1
  * **tft_max_rdclock**  current spi clock for reading from display RAM
  * **tft_width** screen width (smaller dimension) in pixels
  * **tft_height** screen height (larger dimension) in pixels
  * **tft_disp_type**  current display type (DISP_TYPE_ILI9488 or DISP_TYPE_ILI9341)

---

Full functions **syntax and descriptions** can be found in *tft.h* and *tftspi.h* files.

Full **demo application**, well documented, is included, please **analyze it** to learn how to use the library functions.

---

#### Connecting the display

| ESP32 pin | Display module | Notes |
| - | - | - |
| Any output pin | MOSI | SPI input on Display module |
| Any pin | MISO | SPI output from Display module, optional |
| Any output pin | SCK | SPI clock input on Display module |
| Any output pin | CS  | SPI CS input on Display module |
| Any output pin | DC  | DC (data/command) input on Display module |
| Any output pin | TCS  | Touch pannel CS input (if touch panel is used) |
| Any output pin | RST  | **optional**, reset input of the display module, if not used **pullup the reset input** to Vcc |
| Any output pin | BL  | **optional**, backlight input of the display module, if not used connect to +3.3V (or +5V) |
| GND | GND  | Power supply ground |
| 3.3V or +5V | Vcc  | Power supply positive |

**Make sure the display module has 3.3V compatible interface, if not you must use level shifter!**

---

To run the demo, attach ILI9341, ILI9488 or ST7735 based display module to ESP32. Default pins used are:
* mosi: 23
* miso: 19
*  sck: 18
*   CS:  5 (display CS)
*   DC: 26 (display DC)
*  TCS: 25 (touch screen CS)

**Custom PINS can be defined in `idf.py menuconfig` in Components -> TFT Display menu**
---

#### Display Kits

Predefined display configurations are available that will set pins, display size, and inversion properly for the specified kit.

Access these through the `idf.py menuconfig` in Components->TFT Display

Configurations are available for:

    "ESP-WROVER-KIT v3 Display (ST7789V)"
    "ESP-WROVER-KIT v4.1 Display (ILI9341)"
    "Adafruit TFT Feather Display"
    "M5Stack TFT Display"

---

#### Other config notes

Touch screen can be enabled in Components -> TFT Display as well.

Using *idf.py menuconfig* **select tick rate 1000** ( → Component config → FreeRTOS → Tick rate (Hz) ) to get more accurate timings

---

#### Installing as Library
This repository is intended to be installable as a component library using the ESP-IDF 4.0 build system.

It is recommended though that you first follow the [demo instructions](#building-the-demo) below to build this repository as a standalone example to validate your hardware and have a basis to learn the library features. 

When you are ready to incorporate it into your existing project, it is recommended to:

```shell
mkdir -p externals
git submodule add https://github.com/jeremyjh/ESP32_TFT_library.git externals/ESP32_TFT_library
```

In your project's root CMakeLists.txt add the components folder to `EXTRA_COMPONENT_DIRS` - make sure this is before the project() config e.g.

```cmake
cmake_minimum_required(VERSION 3.5)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    set(EXTRA_COMPONENT_DIRS
            externals/ESP32_TFT_library/components)

project(hello-world)
```

In your own components/<my-component>/CMakeLists.txt or main/CMakeLists.txt add `tft` and `spiffs` (if you are using spiffs) as `REQUIRES` e.g.

```cmake
set(SOURCES tft_demo.c)
idf_component_register(
        SRCS ${SOURCES}
        INCLUDE_DIRS
          ${CMAKE_CURRENT_LIST_DIR}
          $ENV{IDF_PATH}/components
        REQUIRES
            tft
            spiffs
)
```

---

#### Building the Demo

Clone the repository to your esp folder (same level as esp-idf, as explained [in instructions](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html)).

`git clone https://github.com/jeremyjh/ESP32_TFT_library.git`

Execute `idf.py menuconfig` and configure your Serial flash config and other settings. Included *sdkconfig.defaults* sets some defaults to be used.

Navigate to **Components -> TFT Display** and set **display and pin** options or select a pre-defined display configuration for a kit.

To enable **Wifi** in the demo (recommended - gets time from NTP), select **TFT Display DEMO Configuration** from the top-level menu and select those options.

Make and flash the example.

`idf.py build && idf.py -p <PORT> flash monitor`

Deploy the SPIFFS image as below to make the image and font examples work.

---

#### Prepare **SPIFFS** image

*The demo uses some image and font files and it is necessary to flash the spiffs image.*

**To flash already prepared image to flash** execute:

`ESPPORT=<PORT> make copyfs`

---

You can also prepare different SFPIFFS **image** and flash it to ESP32.

The example partition file reserves 1MB for SPIFFS.
If you change the spiffs partition size, update the SPIFFS configuration as well.

`idf.py menuconfig`
Navigate to **Components -> TFT SPIFFS** and set **SPIFFS** options.

Files to be included on spiffs are already in **components/spiffs_image/image/** directory. You can add or remove the files you want to include.

Then execute:

`make makefs`

to create **spiffs image** in *build* directory **without flashing** to ESP32

Or execute:

`ESPPORT=<PORT> make flashfs`

to create **spiffs image** in *build* directory and **flash** it to ESP32

---


---

**Example output:**

```

I (0) cpu_start: App cpu up.
I (312) heap_init: Initializing. RAM available for dynamic allocation:
I (319) heap_init: At 3FFAE6E0 len 00001920 (6 KiB): DRAM
I (325) heap_init: At 3FFBB0B8 len 00024F48 (147 KiB): DRAM
I (331) heap_init: At 3FFE0440 len 00003BC0 (14 KiB): D/IRAM
I (338) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (344) heap_init: At 40091F94 len 0000E06C (56 KiB): IRAM
I (350) cpu_start: Pro cpu start user code
I (144) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.

==============================
TFT display DEMO, LoBo 09/2017
==============================

SPI: display device added to spi bus (2)
SPI: attached display device, speed=8000000
SPI: bus uses native pins: false
SPI: display init...
OK
SPI: Changed speed to 26666666

---------------------
Graphics demo started
---------------------
I (2815) [TFT Demo]: Time is not set yet. Connecting to WiFi and getting time over NTP.
I (2845) wifi: wifi firmware version: ee52423
I (2846) wifi: config NVS flash: enabled
I (2846) wifi: config nano formating: disabled
I (2846) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (2856) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (2890) wifi: Init dynamic tx buffer num: 32
I (2890) wifi: Init data frame dynamic rx buffer num: 32
I (2890) wifi: Init management frame dynamic rx buffer num: 32
I (2894) wifi: wifi driver task: 3ffc83d8, prio:23, stack:4096
I (2899) wifi: Init static rx buffer num: 10
I (2903) wifi: Init dynamic rx buffer num: 32
I (2907) wifi: Init rx ampdu len mblock:7
I (2911) wifi: Init lldesc rx ampdu entry mblock:4
I (2916) wifi: wifi power manager task: 0x3ffcd844 prio: 21 stack: 2560
I (2922) [TFT Demo]: Setting WiFi configuration SSID LoBoInternet...
I (2951) phy: phy_version: 359.0, e79c19d, Aug 31 2017, 17:06:07, 0, 0
I (2951) wifi: mode : sta (24:0a:c4:11:a4:0c)
I (3073) wifi: n:11 0, o:1 0, ap:255 255, sta:11 0, prof:1
I (3731) wifi: state: init -> auth (b0)
I (3734) wifi: state: auth -> assoc (0)
I (3738) wifi: state: assoc -> run (10)
I (3776) wifi: connected with LoBoInternet, channel 11
I (5827) event: ip: 192.168.0.21, mask: 255.255.255.0, gw: 192.168.0.1
I (5828) [TFT Demo]: Initializing SNTP
I (6331) [TFT Demo]: System time is set.
I (6331) wifi: state: run -> init (0)
I (6332) wifi: n:11 0, o:11 0, ap:255 255, sta:11 0, prof:1
I (6344) wifi: flush txq
I (6344) wifi: stop sw txq
I (6344) wifi: lmac stop hw txq
E (6344) wifi: esp_wifi_connect 836 wifi not start


I (8441) [SPIFFS]: Registering SPIFFS file system
I (8441) [SPIFFS]: Mounting SPIFFS files system
I (8441) [SPIFFS]: Start address: 0x280000; Size 1024 KB
I (8447) [SPIFFS]:   Work buffer: 2048 B
I (8451) [SPIFFS]:    FDS buffer: 384 B
I (8456) [SPIFFS]:    Cache size: 2048 B
I (8500) [SPIFFS]: Mounted

==========================================
Display: ILI9488: PORTRAIT 240,320 Color

     Clear screen time: 60 ms
Send color buffer time: 228 us (240 pixels)
       JPG Decode time: 287 ms
    BMP time, scale: 5: 422 ms
    BMP time, scale: 4: 431 ms
    BMP time, scale: 3: 430 ms
    BMP time, scale: 2: 434 ms
    BMP time, scale: 1: 442 ms
    BMP time, scale: 0: 335 ms

==========================================
Display: ILI9488: LANDSCAPE 320,240 Color

     Clear screen time: 57 ms
Send color buffer time: 301 us (320 pixels)
I (126333) event: station ip lost
       JPG Decode time: 286 ms
    BMP time, scale: 5: 422 ms
    BMP time, scale: 4: 431 ms
    BMP time, scale: 3: 433 ms
    BMP time, scale: 2: 435 ms
    BMP time, scale: 1: 444 ms
    BMP time, scale: 0: 260 ms

==========================================
Display: ILI9488: PORTRAIT FLIP 240,320 Color

     Clear screen time: 60 ms
Send color buffer time: 228 us (240 pixels)
       JPG Decode time: 287 ms
    BMP time, scale: 5: 420 ms
    BMP time, scale: 4: 430 ms
    BMP time, scale: 3: 429 ms
    BMP time, scale: 2: 436 ms
    BMP time, scale: 1: 446 ms
    BMP time, scale: 0: 338 ms

==========================================
Display: ILI9488: PORTRAIT FLIP 240,320 Color

     Clear screen time: 60 ms
Send color buffer time: 228 us (240 pixels)
       JPG Decode time: 287 ms
    BMP time, scale: 5: 420 ms
    BMP time, scale: 4: 430 ms
    BMP time, scale: 3: 429 ms
    BMP time, scale: 2: 436 ms
    BMP time, scale: 1: 446 ms
    BMP time, scale: 0: 338 ms


```

---

### Tested on

ESP32-WROVER-KIT v3, ST7789V controller, 240x320
![Tested on](https://raw.githubusercontent.com/loboris/MicroPython_ESP32_psRAM_LoBo/master/Documents/disp_wrower-kit.jpg)

2.4" 240x320 ILI9341 conroller with Touch panel from eBay
![Tested on](https://raw.githubusercontent.com/loboris/MicroPython_ESP32_psRAM_LoBo/master/Documents/disp_ili9341.jpg)

3.5" 320x480 ILI9844 controller with Touch panel from BuyDisplay
![Tested on](https://raw.githubusercontent.com/loboris/MicroPython_ESP32_psRAM_LoBo/master/Documents/disp_9488.jpg)

1.8" 128x160 ST7735 conroller from eBay
![Tested on](https://raw.githubusercontent.com/loboris/MicroPython_ESP32_psRAM_LoBo/master/Documents/disp_7735.jpg)

