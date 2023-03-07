# Factory Demo

This example project demonstrates a wide range of features of the EFM32GG11 MCU and the
SLSTK3701A Starter Kit.

This is an outdated example that doesn't use the new modules provided with Studio v5.
This example should not be used as a starting point for a new project. The sample
applications that should be used as reference and starting point are located in the
folder under 'app/common/example'.

Browse different app screens and interact with them using the touch slider.
- Swipe the slider left or right to change screen
- Some of the app screens will show virtual buttons in the lower left and/or lower
  right corner of the screen. To trigger the buttons, tap the left or right end of
  the slider, respectively. Whenever the phrase "click the ACTION button" is mentioned
  later in this file, it should be understood as tapping the end of the touch slider
  corresponding to the ACTION button label on the lower left or lower right corner of
  the screen.

**IMPORTANT**: The touch slider is calibrated during startup, i.e., when the kit is powered,
reset or recovering from a backup event. To ensure that the calibration is successful
and that the touch slider will work as expected, do not
touch the slider while the calibration is in progress. The calibration process is
completed when data is visible on the display.

The runtime since last reset is shown in the top right corner of the screen of all apps.

The available app screens include:
- RGB LED app
  this app lets the user change the color and light intensity of each of the two RGB LEDs
  on the STK3701A. Click the "Settings" button to enter change settings mode. Change the
  value of the highlighted setting by swiping the slider. Click the "Next" button to
  cycle through the available settings. Click the "Exit" button to exit the settings mode.

- Si7210 Hall-Effect Sensor app
  This app shows the magnitude and polarity of the magnetic field measured by the Si7210
  hall-effect sensor on the STK3701A.

- Si7021 Relative Humidity and Temperature Sensor app
  This app shows the temperature and relative humidity measured by the Si7021 relative
  humidity and temperature sensor on the STK3701A. Click the "C/F" button to toggle
  Centigrade or Fahrenheit scale for the temperature.

- Capacitive Touch app
  This app shows the position and force the user is applying to the touch slider, as well
  as the duration of the last touch event. Click "Swipe lock" to prevent changing app
  screen while using the app. Click "Unlock" to allow changing app screen again.

- Low Energy Inductive Sense (LC Sense) and True Random Number Generator (TRNG) app
  This app counts the number of times the inductive sense element connected to the
  EFM32GG11's LESENSE peripheral has been triggered. The sensor is triggered when a
  metallic object comes close to the LC Sense inductor. The counter is active even when
  the LC Sense app is not shown on the screen. Click the "Clr Count" button to reset the
  counter.

  This app also demonstrates the EFM32GG11's NIST-certified true random number generator
  (TRNG). The TRNG generates a 128-bit random number every time the LC sensor is triggered,
  and this number is used to randomize the RGB LEDs' colors.

- Backup Power Domain (BU Power) app
  This app demonstrates the EFM32GG11s ability to use a backup power source when its main
  power source is unavailable. The BU Power app screen displays the following metrics:
  - Voltage of the backup battery on the STK3701A (BUVDD)
  - Graphical display of the BUVDD voltage over the last 90 seconds
  - Charge status
  - Voltage of the main power supply (AVDD)
  - Number of backup power (BU) events encountered
  - Duration of the last BU event
  - Total duration of all BU events
  With a properly charged backup battery, the EFM32GG11 will retain the RGB LED settings
  and the selected app, as well as keep the runtime timer running even when the main power
  supply is removed.
  To charge the battery, click the "Charge On" button. The battery will keep on charging
  even if the user changes to another app. Keep charging until the status reads "BU Ready".
  Click the "Charge On" button once more to stop charging.
  The user can now disconnect the main power supply for 10-30 minutes, depending on the
  charge level of the backup battery, without losing the app settings. Note, however, that
  to properly recover from a power outage, the user should supply power using the "BAT"
  setting on the power switch, since the STK3701A board controller might issue a device reset
  when re-connecting a power supply using the "AEM" or "USB" settings on the power switch.
  
  To discharge the battery, click the "Discharge" button. Allow 30-60
  seconds to fully discharge the battery, depending on the charge level of the battery.
  The BUVDD voltage will read 0.00 Volts when discharging, regardless of the actual
  voltage of the backup battery. Click the "Discharge" button once more to stop the
  discharge process. You can also click the "Charge On" button when discharging to
  switch directly to recharging the backup battery.

*Board*:  Silicon Labs SLSTK3701A Starter Kit  
*Device*: EFM32GG11B820F2048GL192