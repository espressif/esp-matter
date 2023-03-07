# SimpleLink&trade; LPSTK_CC1352R LaunchPad&trade; Settings & Resources

The [__SimpleLink LPSTK_CC1352R LaunchPad__][launchpad] contains a
[__CC1352R1F3RGZR__][device] device and several on-board sensors:

- The [__HDC2080__](http://www.ti.com/product/HDC2080) is an integrated humidity and temperature sensor that provides high accuracy measurements with very low power consumption, in a small DFN package
- The [__DRV5055__](http://www.ti.com/product/DRV5055) is a high accuracy 3.3 V or 5 V ratio-metric bipolar hall effect sensor that responds proportionally to magnetic flux density. The device can be used for accurate position sensing in a  wide range of applications.
- The [__ADXL362__](https://www.analog.com/en/products/adxl362.html) is an ultralow power, 3-axis MEMS accelerometer that consumes less than 2 μA at a 100 Hz output data rate and 270 nA when in motion triggered wake-up mode,
- The [__OPT3001__](http://www.ti.com/product/OPT3001) is a sensor that measures the intensity of visible light. The spectral response of the sensor tightly matches the photopic response of the human eye and includes significant infrared rejection.


## Jumper Settings

* No modifications are needed.

## SysConfig Board File

The [LPSTK_CC1352R.syscfg.json](../.meta/LPSTK_CC1352R.syscfg.json)
is a handcrafted file used by SysConfig. It describes the physical pin-out
and components on the LaunchPad.


## Driver Examples Resources

Examples utilize SysConfig to generate software configurations into
the __ti_drivers_config.c__ and __ti_drivers_config.h__ files. The SysConfig
user interface can be utilized to determine pins and resources used.
Information on pins and resources used is also present in both generated files.


## TI BoosterPacks&trade;

The following BoosterPack(s) are used with some driver examples.

#### [__BOOSTXL-BASSENSORS BoosterPack__][boostxl-bassensors]
  * No modifications are needed.

#### [__BP-BASSENSORS MKII BoosterPack__][bp-bassensorsmkii]
  * No modifications are needed.

[device]: https://www.ti.com/product/CC1352R
[launchpad]: https://www.ti.com/tool/LPSTK-CC1352R
[boostxl-bassensors]: https://www.ti.com/tool/BOOSTXL-BASSENSORS
[bp-bassensorsmkii]: https://www.ti.com/tool/BP-BASSENSORSMKII
