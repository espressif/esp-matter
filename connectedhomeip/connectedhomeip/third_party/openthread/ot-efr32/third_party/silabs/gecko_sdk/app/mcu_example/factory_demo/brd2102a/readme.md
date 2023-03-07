# Factory Demo

This example project demonstrates a wide range of features of the EFM32TG11 MCU and
the SLSTK3301A Starter Kit.

This is an outdated example that doesn't use the new modules provided with Studio v5.
This example should not be used as a starting point for a new project. The sample
applications that should be used as reference and starting point are located in the
folder under 'app/common/example'.

Browse different app screens and interact with them using the push buttons BTN0 and BTN1.
- Press the push button BTN0 to change screen
- Some of the app screens will show Gecko symbol in the upper right corner of the screen.
  To trigger the actions for this app, press the push button BTN1.

**IMPORTANT**: The touch slider is calibrated during startup, i.e., when the kit is powered,
reset or recovering from a backup event. To ensure that the calibration is successful
and that the touch slider will work as expected, do not touch the slider while the calibration is in progress. The calibration process is
completed when data is visible on the display.

The available app screens include:
- Elapsed time app
  The runtime since last reset is shown in the upper part of the screen.

- Si7210 Hall-Effect Sensor app
  This app shows the magnitude and polarity of the magnetic field measured by the Si7210
  hall-effect sensor on the STK3301A.

- Si7021 Relative Humidity and Temperature Sensor app
  This app shows the temperature and relative humidity measured by the Si7021 relative
  humidity and temperature sensor on the STK3301A. Press the push button BTN1 to toggle
  Centigrade or Fahrenheit scale for the temperature.

- Capacitive Touch app
  This app shows the position and force the user is applying to the touch slider.

- Low Energy Inductive Sense (LC Sense) and True Random Number Generator (TRNG) app
  This app counts the number of times the inductive sense element connected to the
  EFM32TG11's LESENSE peripheral has been triggered. The sensor is triggered when a
  metallic object comes close to the LC Sense inductor. The counter and LEDs are active
  even when the LC Sense app is not shown on the screen. Press the push button BTN1 to
  reset the counter.

  This app also demonstrates the EFM32TG11's NIST-certified true random number generator
  (TRNG). The TRNG generates a 128-bit random number every time the LC sensor is triggered,
  and this number is used to display random pattern on the upper left LCD array.

- Backup Power Domain (BU Power) app
  This app demonstrates the EFM32TG11s ability to use a backup power source when its main
  power source is unavailable. The BU Power app screen displays the following metrics:
  - Voltage of the backup capacitor on the STK3301A (BUVDD)
  - Charge and discharge status
  - Voltage of the main power supply (AVDD)
  With a properly charged backup capacitor, the EFM32TG11 will retain the selected app,
  as well as keep the runtime timer running even when the main power supply is removed.
  To charge the backup capacitor, press the push button BTN1. The capacitor will keep on
  charging even if the user changes to another app.
  Press the push button BTN1 once more to stop charging.
  The user can now disconnect the main power supply for 10-30 minutes, depending on the
  charge level of the backup capacitor, without losing the app settings.
  To properly recover from a power outage, supply power using the "BAT"
  setting on the power switch, since the STK3301A board controller might issue a device reset
  when re-connecting a power supply using the "AEM" or "USB" settings on the power switch.

  To discharge the capacitor, press the push button BTN1 once more.
  Allow 30-60 seconds to fully discharge the capacitor, depending on the charge level of the capacitor.
  The BUVDD voltage will read 0 Volts when discharging, regardless of the actual
  voltage of the backup capacitor. Press the push button BTN1 once more to stop the
  discharge process.

*Board*:  Silicon Labs SLSTK3301A Starter Kit      
*Device*: EFM32TG11B520F128GM80