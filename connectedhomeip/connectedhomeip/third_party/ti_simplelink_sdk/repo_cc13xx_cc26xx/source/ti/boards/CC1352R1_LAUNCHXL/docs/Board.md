# SimpleLink&trade; CC1352R1 LaunchPad&trade; Settings & Resources

The [__SimpleLink CC1352R1 LaunchPad__][launchpad] contains a
[__CC1352R1F3RGZR__][device] device.


## Jumper Settings

* Close the __`LEDs`__ jumper to enable the on-board LEDs.
* Close the __`RXD<<`__ and __`TXD>>`__ jumpers to enable UART via
the XDS110 on-board USB debugger.


## SysConfig Board File

The [CC1352R1_LAUNCHXL.syscfg.json](../.meta/CC1352R1_LAUNCHXL.syscfg.json)
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

  * The BoostPack's `DIN` and `DOUT` signal pins are not compatible with this
    LaunchPad.
    Use one of the following modification(s) to enable the CC3200 Audio
    BoosterPack's usage with the __i2secho__ example.
  * Bend down the two pins below `DIO30`. Additionally bend down `DIO28-30`.
    Be sure that the bent pin(s) do not make contact with the IC or any other
    component, bend them enough to make sure they don't connect to the CC3200
    Audio BoosterPack.
  * Attach the CC3200 Audio BoosterPack to the CC1352R1_LAUNCHXL
  * Run jumper wires between the following pins on the CC3200 Audio BoosterPack:
    - DIN: `P3.3` and `P3.9`
    - DOUT: `P3.4` and `P3.10`
  * See [Audio BP User Guide][cc3200audboost-user-guide], Figure 2-1 for
    information on where these pins are located.
  * If you are using the `LPTSTK-CC1352R1` which is based on the
    `CC1352R1_LAUNCHXL` the following pins must also be jumpered:
    - BCLK: `P3.6` and `P3.8`
    - DIN: `P3.3` and `P2.2`. Use the instead of `P3.4` on LPSTK devices
  * See below for a diagram.

![CC3200 Audio BoosterPack Jumper Wire Diagram](images/cc3200audboost_cc13x2_jumpers_annotated.png)

[device]: http://www.ti.com/product/CC1352R
[launchpad]: http://www.ti.com/tool/LAUNCHXL-CC1352R1
[boostxl-sharp128]: http://www.ti.com/tool/boostxl-sharp128
[bp-bassensorsmkii]: http://www.ti.com/tool/bp-bassensorsmkii
[cc3200audboost]: http://www.ti.com/tool/CC3200AUDBOOST
[cc3200audboost-user-guide]: http://www.ti.com/lit/pdf/swru383
