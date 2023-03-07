/***************************************************************************//**
 * @file
 * @brief Setup LCD for energy mode demo
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "lcd_setup.h"
#include "emodes.h"
#include "sl_simple_button_instances.h"
#include "sl_simple_button_btn0_config.h"
#include "sl_simple_button_btn1_config.h"
#include "sl_board_control.h"
#include "sl_atomic.h"
#include "glib.h"
#include "dmd.h"

/*******************************************************************************
 *******************************   DEFINES   ***********************************
 ******************************************************************************/

#define TEXT_MARGIN   5

/*******************************************************************************
 ***************************  LOCAL VARIABLES   ********************************
 ******************************************************************************/
typedef enum {
  EM_SELECT,
  DCDC_SELECT,
  OSC_SELECT,
  OP_SELECT
} selection_stage_t;

static energy_mode_t emode = { EM0, 0, WHILE, false };  // Default energy mode config is set to EM0, HFXO, (while)
static selection_stage_t stage = EM_SELECT;           // Selection stage.
static bool start_test = false;                       // Start selected energy mode test.
static GLIB_Context_t glib_context;                   // To initialize the glib context
static bool update_display = true;                    // Update display flag

/*******************************************************************************
 *********************   LOCAL FUNCTION PROTOTYPES   ***************************
 ******************************************************************************/

static void print_on_lcd(char* str, uint16_t line);
static void lcd_select_mode(void);
static void disable_before_test(void);

/*******************************************************************************
 **************************   LOCAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Function to print on lcd.
 ******************************************************************************/
static void print_on_lcd(char* str, uint16_t line)
{
  GLIB_drawStringOnLine(&glib_context,
                        str,
                        line,
                        GLIB_ALIGN_LEFT,
                        TEXT_MARGIN,
                        TEXT_MARGIN,
                        true);
  DMD_updateDisplay();
}

/***************************************************************************//**
 * Shows the selected eMode option on the LCD.
 ******************************************************************************/
static void lcd_select_mode(void)
{
  bool local_update_display;
  sl_atomic_load(local_update_display, update_display);
  if (!local_update_display) {
    return;
  }

  energy_mode_t local_emode;
  sl_atomic_load(local_emode.em, emode.em);
  sl_atomic_load(local_emode.dcdc, emode.dcdc);
  sl_atomic_load(local_emode.osc, emode.osc);
  sl_atomic_load(local_emode.op, emode.op);

  print_on_lcd(" ", 7 + stage); //erase '>' in last line

  if (stage == EM_SELECT) {
    switch (local_emode.em) {
      case EM0:
        print_on_lcd("> EM0 ", 8);
        break;
      case EM1:
        print_on_lcd("> EM1 ", 8);
        break;
      case EM2:
        print_on_lcd("> EM2 ", 8);
        break;
      case EM3:
        print_on_lcd("> EM3 ", 8);
        break;
#if !defined(_SILICON_LABS_32B_SERIES_1)
      case EM4:
        print_on_lcd("> EM4 ", 8);
        break;
#else // ! _SILICON_LABS_32B_SERIES_1
      case EM4H:
        print_on_lcd("> EM4H", 8);
        break;
      case EM4S:
        print_on_lcd("> EM4S", 8);
        break;
#endif
      default:
        EFM_ASSERT(false);
        break;
    }
  } else if (stage == DCDC_SELECT) {
    if (local_emode.dcdc) {
      print_on_lcd("> With DC/DC   ", 9);
    } else {
      print_on_lcd("> Without DC/DC", 9);
    }
  } else if (stage == OSC_SELECT) {
    if (local_emode.em <= 1) {
      switch ((em01_oscillator_enum_t)local_emode.osc) {
        case HFXO_:
          print_on_lcd(">  HFXO       ", 10);
          break;
#if defined(_SILICON_LABS_32B_SERIES_2)
        case FSRCO_20MHZ:
          print_on_lcd(">  FSRCO 20MHz", 10);
          break;
        case HFRCO_80MHZ:
          print_on_lcd(">  HFRCO 80MHz", 10);
          break;
        case HFRCO_38MHZ:
          print_on_lcd(">  HFRCO 38MHz", 10);
          break;
        case HFRCO_26MHZ:
          print_on_lcd(">  HFRCO 26MHz", 10);
          break;
        case HFRCO_1MHZ:
          print_on_lcd(">  HFRCO 1MHz ", 10);
          break;
#elif defined(_SILICON_LABS_32B_SERIES_1)
#if defined(_DEVINFO_HFRCOCAL16_MASK)
        case HFRCO_72MHZ:
          print_on_lcd(">  HFRCO 72MHz", 10);
          break;
#endif // _DEVINFO_HFRCOCAL16_MASK
#if defined(_DEVINFO_HFRCOCAL15_MASK)
        case HFRCO_64MHZ:
          print_on_lcd(">  HFRCO 64MHz", 10);
          break;
#endif // _DEVINFO_HFRCOCAL15_MASK
#if defined(_DEVINFO_HFRCOCAL14_MASK)
        case HFRCO_56MHZ:
          print_on_lcd(">  HFRCO 56MHz", 10);
          break;
#endif // _DEVINFO_HFRCOCAL14_MASK
#if defined(_DEVINFO_HFRCOCAL13_MASK)
        case HFRCO_48MHZ:
          print_on_lcd(">  HFRCO 48MHz", 10);
          break;
#endif // _DEVINFO_HFRCOCAL13_MASK
        case HFRCO_38MHZ:
          print_on_lcd(">  HFRCO 38MHz", 10);
          break;
        case HFRCO_26MHZ:
          print_on_lcd(">  HFRCO 26MHz", 10);
          break;
        case HFRCO_19MHZ:
          print_on_lcd(">  HFRCO 19MHz", 10);
          break;
        case HFRCO_4MHZ:
          print_on_lcd(">  HFRCO 4MHz ", 10);
          break;
        case HFRCO_1MHZ:
          print_on_lcd(">  HFRCO 1MHz ", 10);
          break;
#else // _SILICON_LABS_32B_SERIES_1
#if defined(CMU_HFRCOCTRL_BAND_28MHZ)
        case HFRCO_28MHZ:
          print_on_lcd(">  HFRCO 28MHz", 10);
          break;
#endif // CMU_HFRCOCTRL_BAND_28MHZ
        case HFRCO_21MHZ:
          print_on_lcd(">  HFRCO 21MHz", 10);
          break;
        case HFRCO_14MHZ:
          print_on_lcd(">  HFRCO 14MHz", 10);
          break;
        case HFRCO_11MHZ:
          print_on_lcd(">  HFRCO 11MHz", 10);
          break;
        case HFRCO_1MHZ:
          print_on_lcd(">  HFRCO 1MHz ", 10);
          break;
#endif
        default:
          EFM_ASSERT(false);
          break;
      }
    } else if (local_emode.em == 2) {
      switch ((em2_oscillator_enum_t)local_emode.osc) {
#if defined(RTCC_PRESENT)
        case EM2_LFXO_RTCC:
          print_on_lcd(">  RTCC LFXO  \n   (full RAM)", 10);
          break;
        case EM2_LFXO_RTCC_RAM_POWERDOWN:
          print_on_lcd(">  RTCC LFXO  \n   (16kB RAM)", 10);
          break;
        case EM2_LFRCO_RTCC:
          print_on_lcd(">  RTCC LFRCO \n   (full RAM)", 10);
          break;
        case EM2_LFRCO_RTCC_RAM_POWERDOWN:
          print_on_lcd(">  RTCC LFRCO \n   (16kB RAM)", 10);
          break;
#endif
#if defined(RTC_PRESENT)
        case EM2_LFRCO_RTC:
          print_on_lcd(">  RTC LFRCO  \n   (full RAM)", 10);
          break;
        case EM2_LFRCO_RTC_RAM_POWERDOWN:
          print_on_lcd(">  RTC LFRCO  \n   (16kB RAM)", 10);
          break;
#elif defined(SYSRTC_PRESENT)
        case EM2_LFRCO_SYSRTC:
          print_on_lcd(">  SYSRTC LFRCO\n   (full RAM)", 10);
          break;
        case EM2_LFRCO_SYSRTC_RAM_POWERDOWN:
          print_on_lcd(">  SYSRTC LFRCO\n   (16kB RAM)", 10);
          break;
#if !(defined(ZGM230SB27HGN) && ZGM230SB27HGN == 1)
        case EM2_LFXO_SYSRTC:
          print_on_lcd(">  SYSRTC LFXO\n   (full RAM)", 10);
          break;
        case EM2_LFXO_SYSRTC_RAM_POWERDOWN:
          print_on_lcd(">  SYSRTC LFXO\n   (16kB RAM)", 10);
          break;
#endif //!(defined(ZGM230SB27HGN) && ZGM230SB27HGN == 1)
#endif
        default:
          EFM_ASSERT(false);
          break;
      }
    } else if (local_emode.em == 3) {
      switch ((em3_oscillator_enum_t)local_emode.osc) {
        case EM3_ULFRCO:
          print_on_lcd(">  ULFRCO      \n   (full RAM)", 10);
          break;
        case EM3_ULFRCO_RAM_POWERDOWN:
          print_on_lcd(">  ULFRCO      \n   (16kB RAM)", 10);
          break;
#if defined(_SILICON_LABS_32B_SERIES_2)
        case EM3_ULFRCO_BURTC:
          print_on_lcd(">  BURTC ULFRCO\n   (full RAM)", 10);
          break;
        case EM3_ULFRCO_BURTC_RAM_POWERDOWN:
          print_on_lcd(">  BURTC ULFRCO\n   (16kB RAM)", 10);
          break;
#endif // _SILICON_LABS_32B_SERIES_2
#if defined(CRYOTIMER_PRESENT)
        case EM3_ULFRCO_CRYO:
          print_on_lcd(">  CRYO ULFRCO \n   (full RAM)", 10);
          break;
        case EM3_ULFRCO_CRYO_RAM_POWERDOWN:
          print_on_lcd(">  CRYO ULFRCO \n   (16kB RAM)", 10);
          break;
#endif // CRYOTIMER_PRESENT
        default:
          EFM_ASSERT(false);
          break;
      }
    } else if (local_emode.em == 4) {
      switch ((em4h_oscillator_enum_t)local_emode.osc) {
        case NONE:
          print_on_lcd("> no oscillator\n             ", 10);
          break;
#if defined(_SILICON_LABS_32B_SERIES_2)
        case EM4_LFRCO_BURTC:
          print_on_lcd(">  BURTC LFRCO \n             ", 10);
          break;
        case EM4_ULFRCO_BURTC:
          print_on_lcd(">  BURTC ULFRCO\n             ", 10);
          break;
#else // _SILICON_LABS_32B_SERIES_2
        case EM4H_LFXO_RTCC:
          print_on_lcd(">  RTCC LFXO   \n   (128b RAM)", 10);
          break;
#endif
#if defined(CRYOTIMER_PRESENT)
        case EM4H_ULFRCO_CRYO:
          print_on_lcd(">  CRYO ULFRCO \n   (128b RAM)", 10);
          break;
#endif // CRYOTIMER_PRESENT
        default:
          EFM_ASSERT(false);
          break;
      }
    }
  } else if (stage == OP_SELECT) {
    if (local_emode.em == EM0) {
      switch (local_emode.op) {
        case WHILE:
          print_on_lcd(">  (while loop) ", 11);
          break;
        case PRIME:
          print_on_lcd(">  (primes calc)", 11);
          break;
        case COREMARK:
          print_on_lcd(">  (CoreMark)   ", 11);
          break;
        default:
          EFM_ASSERT(false);
          break;
      }
    }
  }
  local_update_display = false;
  sl_atomic_store(update_display, local_update_display);
}

/***************************************************************************//**
 * Get ready to start the energy mode test. Turn off everything we do not need.
 ******************************************************************************/
static void disable_before_test(void)
{
  // Disable buttons.
  sl_button_disable(&sl_button_btn0);
  sl_button_disable(&sl_button_btn1);

  // Disabling the GPIO Pins used for Button.
  NVIC_DisableIRQ(GPIO_EVEN_IRQn);
  NVIC_DisableIRQ(GPIO_ODD_IRQn);
  GPIO_PinModeSet(SL_SIMPLE_BUTTON_BTN0_PORT, SL_SIMPLE_BUTTON_BTN0_PIN, gpioModeDisabled, 1);
  GPIO_PinModeSet(SL_SIMPLE_BUTTON_BTN1_PORT, SL_SIMPLE_BUTTON_BTN1_PIN, gpioModeDisabled, 1);

  // Clear LCD display.
  GLIB_clear(&glib_context);

  // Turn off the display
  DMD_sleep();

  // disable the LCD display.
  sl_board_disable_display();
}

/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * Function to initialize memory lcd.
 ******************************************************************************/
void lcd_init(void)
{
  uint32_t status;

  /* Enable the memory lcd */
  status = sl_board_enable_display();
  EFM_ASSERT(status == SL_STATUS_OK);

  /* Initialize the DMD support for memory lcd display */
  status = DMD_init(0);
  EFM_ASSERT(status == DMD_OK);

  /* Initialize the glib context */
  status = GLIB_contextInit(&glib_context);
  EFM_ASSERT(status == GLIB_OK);

  glib_context.backgroundColor = White;
  glib_context.foregroundColor = Black;

  /* Fill lcd with background color */
  GLIB_clear(&glib_context);

  /* Use Narrow font */
  GLIB_setFont(&glib_context, (GLIB_Font_t *) &GLIB_FontNarrow6x8);

  print_on_lcd("    ENERGY MODE\n"
               "CURRENT CONSUMPTION\n\n"
               "  Push BTN1 to\n  "
               "cycle options\n"
               "  Push BTN0 to\n"
               "  select/start test", 0);
}

/***************************************************************************//**
 * Ticking function.
 ******************************************************************************/
void emode_app_process_action(void)
{
  bool local_start_test;
  sl_atomic_load(local_start_test, start_test);
  if (local_start_test) {
    energy_mode_t local_emode;
    sl_atomic_load(local_emode.em, emode.em);
    sl_atomic_load(local_emode.dcdc, emode.dcdc);
    sl_atomic_load(local_emode.osc, emode.osc);
    sl_atomic_load(local_emode.op, emode.op);
    /* reconfigures chip to initial state */
    disable_before_test();
    /* Start the selected energy mode setup.
     * See files for "emodes" for more information.
     * Copy and paste function definition to replicate setup.*/
    start_emode_test(&local_emode);
    // Should not be reached.
    EFM_ASSERT(false);
  } else {
    /* Updates the lcd */
    lcd_select_mode();
  }
}

/***************************************************************************//**
 * Callback on button change.
 *
 * This function overrides a weak implementation defined in the simple_button
 * module. It is triggered when the user activates one of the buttons.
 *        BTN0 Selects the option and Starts selected test.
 *        BTN1 Cycles through the available options.
 ******************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if (sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    energy_mode_t local_emode;
    sl_atomic_load(local_emode.em, emode.em);
    sl_atomic_load(local_emode.dcdc, emode.dcdc);
    sl_atomic_load(local_emode.osc, emode.osc);
    sl_atomic_load(local_emode.op, emode.op);

    if (&sl_button_btn0 == handle) {
      // BTN0: Select/Start test.
      stage++;
      if (local_emode.em == 0 && stage == 4) {
        sl_atomic_store(start_test, true);
      }
      if (local_emode.em > 0 && stage == 3) {
        sl_atomic_store(start_test, true);
      }
#if defined(_SILICON_LABS_32B_SERIES_0)
      if (local_emode.em == 4 && stage == 2) {
        sl_atomic_store(start_test, true);
      }
#elif defined(_SILICON_LABS_32B_SERIES_1)
      if (local_emode.em == EM4S && stage == 2) {
        sl_atomic_store(start_test, true);
      }
#else
      if (local_emode.em == EM4 && stage == 3) {
        sl_atomic_store(start_test, true);
      }
#endif
    } else if (&sl_button_btn1 == handle) {
      // BTN1: cycle through options.
      if (stage == EM_SELECT) {
        local_emode.em = (energy_mode_enum_t)(((uint16_t)local_emode.em + 1) % (uint16_t)NUM_EMODES);
        sl_atomic_store(emode.em, local_emode.em);
      } else if (stage == DCDC_SELECT) {
#if defined(DCDC_PRESENT)
        local_emode.dcdc = !local_emode.dcdc;
        sl_atomic_store(emode.dcdc, local_emode.dcdc);
#endif // DCDC_PRESENT
      } else if (stage == OSC_SELECT) {
        if (local_emode.em <= 1) {
          local_emode.osc = (local_emode.osc + 1) % (uint16_t)NUM_EM01_OSCS;
        } else if (local_emode.em == 2) {
          local_emode.osc = (local_emode.osc + 1) % (uint16_t)NUM_EM2_OSCS;
        } else if (local_emode.em == 3) {
          local_emode.osc = (local_emode.osc + 1) % (uint16_t)NUM_EM3_OSCS;
        } else if (local_emode.em == 4) {
          local_emode.osc = (local_emode.osc + 1) % (uint16_t)NUM_EM4H_OSCS;
        }
        sl_atomic_store(emode.osc, local_emode.osc);
      } else {
        if (local_emode.em == EM0) {
          local_emode.op = (operation_enum_t)(((uint16_t)local_emode.op + 1) % (uint16_t)NUM_OPS);
          sl_atomic_store(emode.op, local_emode.op);
        }
      }
    }
    sl_atomic_store(update_display, true);
  }
}
