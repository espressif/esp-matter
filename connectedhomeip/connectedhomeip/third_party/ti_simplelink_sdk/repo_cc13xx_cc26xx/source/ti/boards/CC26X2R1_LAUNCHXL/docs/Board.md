# SimpleLink&trade; CC2652R1 LaunchPad&trade; Settings & Resources

The [__SimpleLink CC2652R1 LaunchPad__][launchpad] contains a
[__CC2652F128__][device] device.


## Jumper Settings

* Close the __`LEDs`__ jumper to enable the on-board LEDs.
* Close the __`RXD<<`__ and __`TXD>>`__ jumpers to enable UART via
the XDS110 on-board USB debugger.


## SysConfig Board File

The [CC26X2R1_LAUNCHXL.syscfg.json](../.meta/CC26X2R1_LAUNCHXL.syscfg.json)
is a handcrafted file used by SysConfig. It describes the physical pin-out
and components on the LaunchPad.


## Driver Examples Resources

Examples utilize SysConfig to generate software configurations into
the __ti_drivers_config.c__ and __ti_drivers_config.h__ files. The SysConfig
user interface can be utilized to determine pins and resources used.
Information on pins and resources used is also present in both generated files.


## TI BoosterPacks&trade;

The following BoosterPack(s) are used with some driver examples.

#### [__BOOSTXL-SHARP128 LCD & SD Card BoosterPack__][boostxl-sharp128]
  * No modifications are needed.

#### [__BP-BASSENSORSMKII BoosterPack__][bp-bassensorsmkii]
  * No modifications are needed.

#### [__CC3200 Audio BoosterPack__][cc3200audboost]
  * No modifications are needed.


[device]: http://www.ti.com/product/CC2652R
[launchpad]: http://www.ti.com/tool/LAUNCHXL-CC26X2R1
[boostxl-sharp128]: http://www.ti.com/tool/boostxl-sharp128
[bp-bassensorsmkii]: http://www.ti.com/tool/bp-bassensorsmkii
[cc3200audboost]: http://www.ti.com/tool/CC3200AUDBOOST
