# SimpleLink&trade; CC2652RSIP LaunchPad&trade; Settings & Resources

The [__SimpleLink CC2652RSIP LaunchPad__][launchpad] contains a
[__CC2652R1FSIP__][device] device.


## Jumper Settings

* Close the __`LEDs`__ jumper to enable the on-board LEDs.
* Close the __`RXD<<`__ and __`TXD>>`__ jumpers to enable UART via
the XDS110 on-board USB debugger.


## SysConfig Board File

The [LP_CC2652RSIP.syscfg.json](../.meta/LP_CC2652RSIP.syscfg.json)
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

[boostxl-sharp128]: http://www.ti.com/tool/boostxl-sharp128
[bp-bassensorsmkii]: http://www.ti.com/tool/bp-bassensorsmkii
[cc3200audboost]: http://www.ti.com/tool/CC3200AUDBOOST
