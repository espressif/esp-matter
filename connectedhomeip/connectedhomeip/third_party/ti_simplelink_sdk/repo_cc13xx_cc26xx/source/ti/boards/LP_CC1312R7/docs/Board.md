# SimpleLink&trade; CC1312R7 LaunchPad&trade; Settings & Resources

The [__SimpleLink CC1312R7 LaunchPad__][launchpad] contains a
[__CC1312R7__][device] device.


## Jumper Settings

* Close the __`LEDs`__ jumper to enable the on-board LEDs.
* Close the __`RXD<<`__ and __`TXD>>`__ jumpers to enable UART via
the XDS110 on-board USB debugger.


## SysConfig Board File

The [LP_CC1312R7.syscfg.json](../.meta/LP_CC1312R7.syscfg.json)
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

  * The BoostPack's `DIN` signal pin is not compatible with this LaunchPad.
    Use one of the following modification(s) to enable the CC3200 Audio
    BoosterPack's usage with the __i2secho__ example.
  * Bend down the pin between `DIO1` and `DIO30`. Be sure that the bent pin
    does not make contact with the IC or any other component, bend it enough to
    make sure it doesn't connect to the CC3200 Audio BoosterPack.
  * Attach the CC3200 Audio BoosterPack to the LP_CC1312R7
  * Connect a jumper wire between `P3.3` and `P3.9` on the CC3200 Audio BoosterPack.
  * See [Audio BP User Guide][cc3200audboost-user-guide], Figure 2-1 for
    information on where these pins are located.
  * See below for a diagram.

  ![CC3200 Audio BoosterPack Jumper Wire Diagram](images/cc3200audboost_cc13x2_jumpers_annotated.png)

[device]: http://www.ti.com/product/CC1312R7
[launchpad]: http://www.ti.com/tool/LP-CC1312R7
[boostxl-sharp128]: http://www.ti.com/tool/boostxl-sharp128
[bp-bassensorsmkii]: http://www.ti.com/tool/bp-bassensorsmkii
[cc3200audboost]: http://www.ti.com/tool/CC3200AUDBOOST
[cc3200audboost-user-guide]: http://www.ti.com/lit/pdf/swru383
