/***************************************************************************//**
* \file cy_capsense.h
* \version 3.0
*
* \brief
* This file includes all the header files of the CapSense middleware.
*
********************************************************************************
* \copyright
* Copyright 2018-2020, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

/**
********************************************************************************
* \mainpage CapSense Middleware Library 3.0 (Alpha)
********************************************************************************
*
* Alpha Release
* The support for the PSoC 4100S Max family on ModusToolbox is currently 
* at the Alpha level. Features may change without notice.
* Contact <a href="https://www.cypress.com/support">
* <b>Cypress Support</b></a> for additional details.
*
* \note This documentation includes only the new version to support 
* fifth CapSense generation devices with MSCv3 HW blocks. 
* For detail, contact Cypress support team.
*
* CapSense is a Cypress capacitive sensing solution. Capacitive sensing can be
* used in a variety of applications and products where conventional mechanical
* buttons can be replaced with sleek human interfaces to transform the way
* users interact with electronic systems. These include home appliances, and
* automotive, IoT, and industrial applications. CapSense supports multiple
* interfaces (widgets) using both CSX and CSD sensing methods with robust
* performance.
*
* CapSense has become a popular technology to replace conventional
* mechanical- and optical-based user interfaces. There are fewer parts
* involved, which saves cost and increases reliability with no wear-and-tear.
* The main advantages of CapSense compared with other solutions are:
* robust performance in harsh environmental conditions and rejection of a
* wide range of external noise sources.
*
* Use CapSense for:
* * Touch and gesture detection for various interfaces
* * Proximity detection for innovative user experiences and low-power
*   optimization
* * Contactless liquid-level sensing in a variety of applications
* * Touch-free operations in hazardous materials
*
********************************************************************************
* \section section_capsense_general General Description
********************************************************************************
*
* The MSCv3 HW block enables multiple sensing capabilities on PSoC devices
* including the self-cap and mutual-cap capacitive touch sensing solutions,
* inductive sensing, impedance measurement, and other features.
* The MSC driver is a low-level
* peripheral driver, a wrapper to access the MSCv3 HW block.
* Middleware access available MSC HW blocks through the MSC Driver.
*
* The MSCv3 HW block can support only one function at a time. However, all
* supported functionality (like CapSense, IndSense, etc.) can be
* time-multiplexed in a design. I.e. you can save the existing state
* of the CapSense middleware, restore the state of the IndSense middleware, perform
* IndSense scans, and then switch back to the CapSense functionality.
* For detail and code examples, refer to the description of the
* Cy_CapSense_Save() and Cy_CapSense_Restore() functions.
*
* \image html CAPSENSE_SOLUTION_MSC_V3.png "CapSense Solution" width=800px
* \image latex CAPSENSE_SOLUTION_MSC_V3.png
*
* This section describes only CapSense middleware. Refer to the corresponding
* sections for documentation of other middleware supported by the MSCv3 HW block.
*
* A CapSense solution includes:
* * The CapSense Configurator to create
*   and configure CapSense widgets. It can be launched in ModusToolbox
*   from the CapSense superblock personality and in Stand-alone mode.
*   It contains a separate document about how to create and
*   configure widgets, parameters and algorithm descriptions.
* * API to control the design from the application program. This documentation
*   describes API with code snippets of how to use them.
* * The CapSense Tuner tool for real-time tuning, testing, and debugging,
*   for easy and smooth designing of human interfaces on customer products.
*   The Tuner tool communicates with a device through a HW bridge and
*   communication drivers (EzI2C, UART, etc.) and allows to monitor
*   widget statuses, sensor signals, detected touch positions, gestures, etc.
* The application program does not need to interact with the CSD driver
* and/or other drivers such as GPIO, SysClk directly. All of that is
* configured and managed by middleware.
*
* Include cy_capsense.h to get access to all functions and other declarations
* in this library. If you are using the ModusToolbox CapSense Configurator tool,
* you can include cycfg_capsense.h only.
*
* \subsection subsection_capsense_features Features
*
* * Offers best-in-class signal-to-noise ratio (SNR)
* * Supports Self-Capacitance (CSD RM) and Mutual-Capacitance (CSX RM)
*   ratio metric sensing methods
* * Supports various Widgets, such as Buttons, Matrix Buttons, Sliders,
*   Touchpads, and Proximity Sensors
* * Provides ultra-low power consumption and liquid-tolerant capacitive
*   sensing technology
* * Contains the integrated graphical CapSense Tuner tool for real-time tuning,
*   testing, and debugging
* * Provides superior immunity against external noise and low-radiated
*   emission
* * Offers best-in-class liquid tolerance
* * Supports one-finger and two-finger gestures
*
********************************************************************************
* \section group_capsense_quick_start Quick Start Guide
********************************************************************************
*
* CapSense middleware can be used in various Development Environments
* such as ModusToolbox, MBED, etc. Refer to the \ref section_capsense_toolchain.
* The quickest way to get started is using the Code Examples. Cypress
* Semiconductor continuously extends its portfolio of the code examples at the
* <a href="http://www.cypress.com"><b>Cypress Semiconductor website</b></a>
* and at the <a href="https://github.com/cypresssemiconductorco">
* <b>Cypress Semiconductor GitHub</b></a>.
*
* \subsection group_capsense_quick_start_modus ModusToolbox Quick Start Guide
*
* This quick start guide assumes that the environment is configured to use the
* PSoC 4 Peripheral Driver Library(psoc4pdl) for development and the
* PSoC 4 Peripheral Driver Library(psoc4pdl) is included in the project.
* It also assumes the
* <a href="https://www.cypress.com/ModusToolboxDeviceConfig">
* <b>ModusToolbox Device Configurator Tool</b></a>,
* <a href="https://www.cypress.com/ModusToolboxCapSenseConfig">
* <b>ModusToolbox CapSense Configurator Tool</b></a>, and
* <a href="https://www.cypress.com/ModusToolboxCapSenseTuner">
* <b>ModusToolbox CapSense Tuner Tool</b></a>
* are installed on your machine.
*
* \note Ensure to set up the device power voltages correctly
* to the proper operation of the device power domains. The Setup is
* on the System Tab of the Device Configurator. Enable the Power check-box
* and set up the voltages as they are red-outlined in the picture below.
*
* \image html check_power.png "Power Setup" width=800px
* \image latex check_power.png
*
* \subsection group_capsense_quick_start_mbed MBED OS Quick Start Guide
*
* You can immediately start with the following MBED OS code example available
* at the <a href="https://github.com/cypresssemiconductorco">
* <b> Cypress Semiconductor GitHub</b></a>:
* * <a href="https://github.com/cypresssemiconductorco/mbed-os-example-capsense">
*   <b>CapSense buttons and slider for PSoC 4AS4 MCU with Mbed OS</b></a>
*
* If you are doing your own project, remember to include cycfg.h file:
* \snippet capsense/snippet/main.c snippet_Cy_CapSense_Resource_Include
*
* and call the resource initialization functions in main() at the beginning:
* \snippet capsense/snippet/main.c snippet_Cy_CapSense_Resource_Initialization
*
********************************************************************************
* \section section_capsense_configuration Summary of Application Programming Interface (API)
********************************************************************************
*
* CapSense operates on the top of the CapSense MSC (Multi-Sensor Converter) driver.
* Refer to the PDL API Reference Manual.
*
* This document provides descriptions of the functions in the CapSense
* middleware library, and descriptions of the data structures (register map)
* used by the middleware library.
*
* The Application Programming Interface (API) routines allow controlling and
* executing specific tasks using the CapSense middleware. The CapSense API
* is described in the following sections:
* * \ref group_capsense_high_level
* * \ref group_capsense_low_level
* * \ref group_capsense_data_structure
* * \ref group_capsense_enums
* * \ref group_capsense_macros
* * \ref group_capsense_callbacks
*
********************************************************************************
* \section section_capsense_toolchain Supported Software and Tools
********************************************************************************
*
* This version of the CapSense Middleware was validated for compatibility 
* with the following Software and Tools:
* <table class="doxtable">
*   <tr>
*     <th>Software and Tools</th>
*     <th>Version</th>
*   </tr>
*   <tr>
*     <td>ModusToolbox Software Environment</td>
*     <td>2.2</td>
*   </tr>
*   <tr>
*     <td>- ModusToolbox Device Configurator</td>
*     <td>2.20</td>
*   </tr>
*   <tr>
*     <td>- ModusToolbox CapSense Superblock Personality for PSoC4AS4 devices in the Device Configurator</td>
*     <td>1.0</td>
*   </tr>
*   <tr>
*     <td>- ModusToolbox MSC Personality for PSoC4AS4 devices in the Device Configurator</td>
*     <td>1.0</td>
*   </tr>
*   <tr>
*     <td>- ModusToolbox CSD Personality for PSoC6 devices in the Device Configurator</td>
*     <td>2.0</td>
*   </tr>
*   <tr>
*     <td>- ModusToolbox CapSense Configurator tool</td>
*     <td>3.20</td>
*   </tr>
*   <tr>
*     <td>- ModusToolbox CapSense Tuner tool</td>
*     <td>3.20</td>
*   </tr>
*   <tr>
*     <td>CAT1 Peripheral Driver Library (PDL)</td>
*     <td>1.5.0</td>
*   </tr>
*   <tr>
*     <td>CAT2 Peripheral Driver Library (PDL)</td>
*     <td>1.0.0</td>
*   </tr>
*   <tr>
*     <td>GCC Compiler</td>
*     <td>9.3.1</td>
*   </tr>
*   <tr>
*     <td>IAR Compiler</td>
*     <td>8.42.2</td>
*   </tr>
*   <tr>
*     <td>Arm Compiler 6 <sup><b>(Note 1)</b></sup></td>
*     <td>6.13</td>
*   </tr>
*   <tr>
*     <td>MBED OS (only for PSoC6)</td>
*     <td>5.15.1</td>
*   </tr>
*   <tr>
*     <td>FreeRTOS</td>
*     <td>10.3.1</td>
*   </tr>
* </table>
*
* <b>Note 1</b> The CapSense middleware includes the pre-compiled libraries for
* Arm Compiler 6. They are built with the following options to be compatible
* with ModusToolbox and MBED:
*
* * -fshort-enums - Set the size of an enumeration type to the smallest
*                   data type that can hold all enumerator values
* * -fshort-wchar - Set the size of wchar_t to 2 bytes
*
* To operate in custom environments with Arm Compiler 6, apply
* the above mentioned build options.
*
********************************************************************************
* \section section_capsense_update Update to Newer Versions
********************************************************************************
* Refer to the \ref section_capsense_changelog to learn about the design impact
* of the newer version. Set up your environment in accordance with
* \ref section_capsense_toolchain.
*
* Ensure:
* * The specified version of the ModusToolbox Device Configurator and
*   the MSC personality are used to re-generate the device configuration.
* * The specified version of the ModusToolbox CapSense Configurator is used
*   to re-generate the middleware configuration.
* * The toolchains are set up properly for your environment per the settings
*   outlined in the Supported Software and Tools.
* * The project is re-built once the toolchains are configured and the
*   configuration is completed.
*
* You might need to re-generate the configuration structures for either the
* device initialization code or the middleware initialization code.
* * Launch the ModusToolbox Device Configurator and perform the File->Save command
*   to re-generate the device initialization code.
* * From the ModusToolbox Device Configurator, launch the
*   ModusToolbox CapSense Configurator and perform the File->Save command to
*   re-generate the middleware initialization code.
*
********************************************************************************
* \section section_capsense_memory_usage Memory Usage
********************************************************************************
*
* The CapSense middleware Flash and RAM memory consumption varies:
* * marginally - depending on the compiler and device
* * significantly - depending on the project CapSense configuration and
*   number of APIs called by the application program.
*
* Memory consumption for any custom
* design/configuration can be determined by analyzing a *.map file
* generated by the compiler.
*
********************************************************************************
* \section section_capsense_misra MISRA-C Compliance
********************************************************************************
*
* <table class="doxtable">
*   <tr>
*     <th>MISRA Rule</th>
*     <th>Rule Class (Required/Advisory)</th>
*     <th>Rule Description</th>
*     <th>Description of Deviation(s)</th>
*   </tr>
*   <tr>
*     <td>11.4</td>
*     <td>A</td>
*     <td>A conversion should not be performed between a pointer to object
*         and an integer type.</td>
*     <td>Such conversion is performed with CapSense context in two cases:
*         interrupt handler and DeepSleepCallback function.
*         Both cases are verified on correct operation.</td>
*   </tr>
*   <tr>
*     <td>12.13</td>
*     <td>A</td>
*     <td>The increment (++) and decrement (--) operators should not be mixed
*         with other operators in an expression.</td>
*     <td>These violations are reported for the GCC ARM optimized form of
*         the "for" loop that have the following syntax:
*         for(index = COUNT; index --> 0u;)
*         It is used to improve performance.</td>
*   </tr>
*   <tr>
*     <td>1.2</td>
*     <td rowspan=2> R</td>
*     <td rowspan=2> Constant: Dereference of NULL pointer.</td>
*     <td rowspan=2> These violations are reported as a result of using of offset macros
*         of CSD Driver with corresponding documented violation 20.6. Refer
*         to CSD and MSC Driver API Ref Guide.</td>
*   </tr>
*   <tr>
*     <td>20.3</td>
*   </tr>
* </table>
*
********************************************************************************
* \section section_capsense_errata Errata
********************************************************************************
*
* This section lists the known problems with the CapSense middleware.
*
* <table class="doxtable">
*   <tr><th>ID</th><th>Known Issue</th><th>Workaround</th></tr>
*   <tr>
*     <td>319100</td>
*     <td>
*         GPIO simultaneous operation with unrestricted strength and frequency
*         creates noise that can affect CapSense operation.
*     </td>
*     <td>
*         For detail, refer to the errata section of the device datasheet.
*     </td>
*   </tr>
*   <tr>
*     <td>3159</td>
*     <td>
*         Scanning a sensor with low capacitance (about 8pF and less) with low
*         frequency (around 300kHz and less) might lead to raw count variation
*         from scan to scan.
*     </td>
*     <td>
*         There are several possible workarounds:
*         1. Increase the Scan resolution.
*         2. Increase the Sense clock frequency. For the best results, perform
*            scanning with as high as possible Sense clock frequency.
*         3. If shield is required for a design, enable the shield tank (Csh)
*            capacitor.
*         4. Increase the sensor capacitance by changing its layout or introduce
*            extra capacitor between the sensor pin and ground.
*         5. Increase number of Fine initialization cycles. Open the cycfg_capsense.c
*            file and modify the .csdFineInitTime field of the cy_capsense_commonConfig
*            structure.
*         6. Increase the CSD init switch resistance. Open the cycfg_capsernse.c file
*            and update the .csdInitSwRes field of the cy_capsense_commonConfig structure
*            with the CY_CAPSENSE_INIT_SW_RES_HIGH value.
*     </td>
*   </tr>
*   <tr>
*     <td>3191</td>
*     <td>
*         Channel 0 is scanned with the incorrect settings when all of the following
*         conditions are met:
*         1. Multi-frequency scan is enabled.
*         2. The Cy_CapSense_CSDSetupWidgetExt() function called only once.
*         3. The Cy_CapSense_CSDScanExt() function called multiple times.
*     </td>
*     <td>
*         Call the Cy_CapSense_CSDSetupWidgetExt() function before each call of
*         the Cy_CapSense_CSDScanExt() function.
*     </td>
*   </tr>
* </table>
*
********************************************************************************
* \section section_capsense_changelog Changelog
********************************************************************************
*
* <table class="doxtable">
*   <tr><th>Version</th><th>Changes</th><th>Reason for Change</th></tr>
*   <tr>
*     <td rowspan="3">3.0</td>
*     <td>Added fifth CapSense generation device support</td>
*     <td>New CapSense platform covering</td>
*   </tr>
*   <tr>
*     <td>Removed usage of deprecated return error and status codes,
*         cy_status enum was replaced with the cy_capsense_status_t variable</td>
*     <td>Defect fixing</td>
*   </tr>
*   <tr>
*     <td>Removed usage of deprecated types, such as uint32 and uint16</td>
*     <td>Defect fixing</td>
*   </tr>
*   <tr>
*     <td rowspan="9">2.10</td>
*     <td>Added Built-in Self-test (BIST) library</td>
*     <td>Support Class B (IEC-60730), safety integrity-level compliant design</td>
*   </tr>
*   <tr>
*     <td>Improved the Csh and Cmod coarse initialization functionality.</td>
*     <td>Feature enhancement</td>
*   </tr>
*   <tr>
*     <td>Improved the shield performance when Csh is enabled</td>
*     <td>Feature enhancement</td>
*   </tr>
*   <tr>
*     <td>Fixed Cy_CapSense_ScanExt() operation</td>
*     <td>Defect fixing</td>
*   </tr>
*   <tr>
*     <td>Fixed the bug in the Cy_CapSense_SetPinState() function</td>
*     <td>Defect fixing</td>
*   </tr>
*   <tr>
*     <td>Optimized software watch-dog values used in monitoring CapSense
*         scanning duration</td>
*     <td>User experience improvement</td>
*   </tr>
*   <tr>
*     <td>Improved IDAC auto-calibration</td>
*     <td>Operation accuracy increasing</td>
*   </tr>
*   <tr>
*     <td>Added the following functions:
*         * Cy_CapSense_GetParam()
*         * Cy_CapSense_SetParam()
*         * Cy_CapSense_GetCRC()
*     </td>
*     <td>Feature enhancement</td>
*   </tr>
*   <tr>
*     <td>Changed the type of context argument to const in the following
*         functions:
*         * Cy_CapSense_CSDConnectSns()
*         * Cy_CapSense_CSXConnectRx()
*         * Cy_CapSense_CSXConnectTx()
*         * Cy_CapSense_CSXDisconnectRx()
*         * Cy_CapSense_CSXDisconnectTx()
*         * Cy_CapSense_SetPinState()
*     </td>
*     <td>Defect fixing</td>
*   </tr>
*   <tr>
*     <td rowspan="6">2.0</td>
*     <td>Added memory usage section to the CapSense API Ref Guide</td>
*     <td>User experience improvement</td>
*   </tr>
*   <tr>
*     <td>Updated documentation</td>
*     <td>User experience improvement</td>
*   </tr>
*   <tr>
*     <td>Added the errata section to the CapSense API Ref Guide</td>
*     <td>User experience improvement</td>
*   </tr>
*   <tr>
*     <td>CapSense MW sources are enclosed with the conditional compilation to
*         ensure a successful compilation for non-CapSense-capable devices</td>
*     <td>Fixing a compilation error for non CapSense-capable devices</td>
*   </tr>
*   <tr>
*     <td>Optimized flash memory consumption based on user's configuration</td>
*     <td>Flash foot-print optimization</td>
*   </tr>
*   <tr>
*     <td>Renamed function Cy_CapSense_CheckCommandIntegrity() to
*         Cy_CapSense_CheckTunerCmdIntegrity()</td>
*     <td>User experience improvement</td>
*   </tr>
*   <tr>
*     <td rowspan="2">1.20</td>
*     <td>Added Arm Compiler 6 support</td>
*     <td>Feature enhancement</td>
*   </tr>
*   <tr>
*     <td>Changed the hierarchy of the binary files folders</td>
*     <td>MBED OS compatibility</td>
*   </tr>
*   <tr>
*     <td rowspan="6">1.1</td>
*     <td>
*         The following functions made obsolete:
*         * Cy_CapSense_CSDSetupWidget()
*         * Cy_CapSense_CSDSetupWidgetExt()
*         * Cy_CapSense_CSDScan()
*         * Cy_CapSense_CSDScanExt()
*         * Cy_CapSense_CSDCalibrateWidget()
*         * Cy_CapSense_CSXSetupWidget()
*         * Cy_CapSense_CSXSetupWidgetExt()
*         * Cy_CapSense_CSXScan()
*         * Cy_CapSense_CSXScanExt()
*         * Cy_CapSense_CSXCalibrateWidget()
*
*         Two simple functions introduced to replace the listed above functions:
*         * Cy_CapSense_SetupWidgetExt()
*         * Cy_CapSense_ScanExt()
*     </td>
*     <td>User experience improvement</td>
*   </tr>
*   <tr>
*     <td>Fixed the shield operation when Csh is disabled</td>
*     <td>Defect fixing</td>
*   </tr>
*   <tr>
*     <td>Fixed the implementation of the position filtering for the Radial Slider widget</td>
*     <td>Defect fixing</td>
*   </tr>
*   <tr>
*     <td>Added restoring hardware to its default state in the Cy_CapSense_DeInit() implementation</td>
*     <td>Defect fixing</td>
*   </tr>
*   <tr>
*     <td>Added the capability to enable the shield electrode without dedicated electrodes</td>
*     <td>Feature enhancement</td>
*   </tr>
*   <tr>
*     <td>Added support of a protocol-agnostic tuner interface (UART, SPI, etc.)</td>
*     <td>Feature enhancement</td>
*   </tr>
*   <tr>
*     <td>1.0</td>
*     <td>
*         The initial version
*     </td>
*     <td></td>
*   </tr>
* </table>
*
********************************************************************************
* \section section_capsense_more_information More Information
********************************************************************************
*
* Important information about the CapSense-technology overview, appropriate
* Cypress device for the design, CapSense system and sensor design guidelines,
* different interfaces and tuning guidelines necessary for a successful design
* of a CapSense system is available in the Getting Started with CapSense
* document and the product-specific CapSense design guide. Cypress highly
* recommends starting with these documents. They can be found on the
* Cypress web site at www.cypress.com.
*
* For more information, refer to the following documents:
*
* * <a href="https://www.cypress.com/products/modustoolbox-software-environment">
*   <b>ModusToolbox Software Environment, Quick Start Guide, Documentation,
*   and Videos</b></a>
*
* * <a href="https://github.com/cypresssemiconductorco"><b>
*   Cypress Semiconductor GitHub</b></a>
*
* * <a href="https://github.com/cypresssemiconductorco/mbed-os-example-capsense">
*   <b>CapSense buttons and slider for PSoC 6 MCU with Mbed OS</b></a>
*
* * <a href="https://www.cypress.com/documentation/code-examples/ce218136-psoc-6-mcu-e-ink-display-capsense-rtos">
*   <b>CapSense Middleware Code Example for FreeRTOS</b></a>
*
* * <a href="https://www.cypress.com/ModusToolboxCapSenseConfig"><b>ModusToolbox
*   CapSense Configurator Tool Guide</b></a>
*
* * <a href="https://www.cypress.com/ModusToolboxCapSenseTuner"><b>ModusToolbox
*   CapSense Tuner Tool Guide</b></a>
*
* * <a href="https://www.cypress.com/ModusToolboxDeviceConfig"><b>ModusToolbox
*   Device Configurator Tool Guide</b></a>
*
* * <a href="https://www.cypress.com/documentation/application-notes/an85951-psoc-4-and-psoc-6-mcu-capsense-design-guide">
*   <b>CapSense Design Guide</b></a>
*
* * <a href="https://cypresssemiconductorco.github.io/psoc6pdl/pdl_api_reference_manual/html/index.html">
*   <b>CAT1 PDL API Reference</b></a>
*
* * <a href="https://cypresssemiconductorco.github.io/psoc4pdl/pdl_api_reference_manual/html/index.html">
*   <b>CAT2 PDL API Reference</b></a>
*
* * <a href="http://www.cypress.com/an210781"><b>AN210781 Getting Started with
*   PSoC 6 MCU with Bluetooth Low Energy (BLE) Connectivity</b></a>
*
* * <a href="https://www.cypress.com/documentation/technical-reference-manuals/psoc-6-mcu-psoc-63-ble-architecture-technical-reference">
*   <b>PSoC 6 Technical Reference Manual</b></a>
*
* * <a href="http://www.cypress.com/ds218787">
*   <b>PSoC 63 with BLE Datasheet Programmable System-on-Chip datasheet</b></a>
*
* * <a href="http://www.cypress.com"><b>Cypress Semiconductor</b></a>
*
* \note
* The links to another software component's documentation (middleware and PDL)
* point to GitHub to the latest available version of the software.
* To get documentation of the specified version, download from GitHub and unzip
* the component archive. The documentation is available in the <i>docs</i> folder.
*
* \defgroup group_capsense_high_level               High-level Functions
*
* \defgroup group_capsense_low_level                Low-level Functions
*
* \defgroup group_capsense_data_structure           CapSense Data Structure
*
* \defgroup group_capsense_structures               CapSense Structures
* \ingroup group_capsense_data_structure
* \brief The CapSense structures.
*
* \defgroup group_capsense_gesture_structures       Gesture Structures
* \ingroup group_capsense_data_structure
* \brief The Gesture-related structures.
*
* \defgroup group_capsense_enums                    Enumerated Types
*
* \defgroup group_capsense_macros                   Macros
*
* \defgroup group_capsense_macros_general           General Macros
*   \ingroup group_capsense_macros
*   \brief General macros
* \defgroup group_capsense_macros_settings          Settings Macros
*   \ingroup group_capsense_macros
*   \brief Settings macros
* \defgroup group_capsense_macros_pin               Pin-related Macros
*   \ingroup group_capsense_macros
*   \brief Pin-related macros
* \defgroup group_capsense_macros_process           Processing Macros
*   \ingroup group_capsense_macros
*   \brief Processing macros
* \defgroup group_capsense_macros_touch             Touch-related Macros
*   \ingroup group_capsense_macros
*   \brief Touch-related macros
* \defgroup group_capsense_macros_gesture           Gesture Macros
*   \ingroup group_capsense_macros
*   \brief Gesture macros
* \defgroup group_capsense_macros_miscellaneous     Miscellaneous Macros
*   \ingroup group_capsense_macros
*   \brief Miscellaneous macros
*
* \defgroup group_capsense_callbacks                Callbacks
*
* \cond SECTION_CAPSENSE_INTERNAL
* \defgroup group_capsense_internal                 Internal Functions
* \endcond
*
**/

/******************************************************************************/
/** \addtogroup group_capsense_high_level
* \{
*
* High-level functions represent the highest abstraction layer of the
* CapSense middleware.
*
* These functions perform tasks such as scanning, data processing, data
* reporting and tuning interfaces. When performing a task, different
* initialization is required based on a sensing method or type of
* widgets is automatically handled by these functions. Therefore, these
* functions are sensing methods, features, and widget type agnostics.
*
* All the tasks required to implement a sensing system can be fulfilled
* by the high-level functions. But, there is a set of
* \ref group_capsense_low_level that provides access to lower level
* and specific tasks. If a design requires access to low-level tasks,
* these functions can be used. The functions related to a given sensing
* methods are not available if the corresponding method is disabled.
*
* \} */

/******************************************************************************/
/** \addtogroup group_capsense_low_level
* \{
*
* The Low-level functions represent the lower layer of abstraction in
* support of \ref group_capsense_high_level.
*
* These functions also enable implementation of special case designs
* requiring performance optimization and non-typical functionalities.
*
* All functions are general to all sensing methods. Some of the
* functions detect the sensing method used by the widget and execute
* tasks as appropriate.
*
* \} */

/******************************************************************************/
/** \addtogroup group_capsense_macros
* \{
*
* Specifies constants used in CapSense middleware.
*
* \} */

/******************************************************************************/
/** \addtogroup group_capsense_enums
* \{
*
* Documents CapSense related enumerated types.
*
* \} */

/******************************************************************************/
/** \cond SECTION_CAPSENSE_INTERNAL */
/** \addtogroup group_capsense_internal
* \{
*
* The section documents CapSense related internal function.
*
* These function should not be used in the application program.
*
* \} \endcond */

/******************************************************************************/
/** \addtogroup group_capsense_data_structure
* \{
*
* The CapSense Data Structure organizes configuration parameters, input, and
* output data shared among different FW modules within the CapSense.
*
* The key responsibilities of the Data Structure are as follows:
* * The Data Structure is the only data container in the CapSense middleware.
* * It serves as storage for the configuration and the output data.
* * All CapSense modules use the data structure for the communication
*   and data exchange.
*
* The CapSense Data Structure is a composite of several smaller structures
* (for global / common data, widget data, sensor data, and pin data).
* Furthermore, the data is split between RAM and Flash to achieve a
* reasonable balance between resources consumption and configuration / tuning
* flexibility at runtime and compile time. A graphical representation of
* the CapSense Data Structure is shown below.
*
* Note that figure below shows a sample representation and documents the
* high-level design of the data structure, it does not include all the
* parameters and elements in each object.
*
* \image html capsense_ds.png "CapSense Data Structure" width=800px
* \image latex capsense_ds.png
*
* CapSense Data Structure does not perform error checking on the data
* written to CapSense Data Structure. It is the responsibility of application
* program to ensure register map rule are not violated while
* modifying the value of data field in CapSense Data Structure.
*
* \} */

/******************************************************************************/
/** \addtogroup group_capsense_callbacks
* \{
*
* Callbacks allow the user to execute Custom code called from the CapSense
* middleware when an event occurs. CapSense supports several callbacks.
*
* <table>
*   <tr>
*     <th>#</th>
*     <th >Callback Name</th>
*     <th>Associated Register</th>
*     <th>Callback Function Prototype</th>
*     <th>Description</th>
*   </tr>
*   <tr>
*     <td>1</td>
*     <td>Start Sample</td>
*     <td>ptrSSCallback</td>
*     <td>\ref cy_capsense_callback_t</td>
*     <td>This is called before each sensor scan triggering. Such a callback
*         can be used to implement user-specific use cases like changing scan
*         parameters depending on whether a sensor is going to be scanned.</td>
*   </tr>
*   <tr>
*     <td>2</td>
*     <td>End Of Scan</td>
*     <td>ptrEOSCallback</td>
*     <td>\ref cy_capsense_callback_t</td>
*     <td>This is called after sensor scan completion and there is no other
*         sensor in the queue to be scanned.</td>
*   </tr>
*   <tr>
*     <td>3</td>
*     <td>Tuner Send Callback</td>
*     <td>ptrTunerSendCallback</td>
*     <td>\ref cy_capsense_tuner_send_callback_t</td>
*     <td>This is called by the Cy_CapSense_RunTuner() function to establish
*         synchronous communication with the Tuner tool.</td>
*   </tr>
*   <tr>
*     <td>4</td>
*     <td>Tuner Receive Callback</td>
*     <td>ptrTunerReceiveCallback</td>
*     <td>\ref cy_capsense_tuner_receive_callback_t</td>
*     <td>This is called by the Cy_CapSense_RunTuner() function to establish
*         synchronous communication with the Tuner tool.</td>
*   </tr>
*   <tr>
*     <td>5</td>
*     <td>CapSense Data Structure Initialization Callback</td>
*     <td>ptrEODsInitCallback</td>
*     <td>\ref cy_capsense_ds_init_callback_t</td>
*     <td>This is called by the Cy_CapSense_Enable() function after CapSense
*         Data Structure initialization complete and before launching
*         the first initialization scan. Using this callback is not
*         recommended. It is used only to implement only user's
*         specific use cases (while changing the CapSense
*         default configuration). The callback is available for Fifth
*         Generation CapSense devices.</td>
*   </tr>
* </table>
*
* \note
* Callbacks 1 and 2 are called by the Cy_CapSense_InterruptHandler()
* function and lengthen this function execution. Usually,
* Cy_CapSense_InterruptHandler() is called inside the ISR.
* Then these callbacks also lengthen the ISR execution.
*
* All callbacks can be registered by direct assignment of the function
* pointers to the corresponding CapSense Data Structure field after call of
* the Cy_CapSense_Init() function as follows:
* * Fifth Generation CapSense devices:
*   <tt>context-\>ptrInternalContext-\>\<Associated Register\> = \&CallbackFunction;</tt>
* * Fourth Generation CapSense devices:
*   <tt>context-\>ptrCommonContext-\>\<Associated Register\> = \&CallbackFunction;</tt>
*
* Callbacks 1 and 2 can be registered / unregistered using
* the Cy_CapSense_RegisterCallback() and Cy_CapSense_UnRegisterCallback()
* functions.
*
* \} */


#if !defined(CY_CAPSENSE_H)
#define CY_CAPSENSE_H

#include "cy_device_headers.h"
#include "cy_capsense_common.h"
#include "cy_capsense_centroid.h"
#include "cy_capsense_control.h"
#include "cy_capsense_filter.h"
#include "cy_capsense_lib.h"
#include "cy_capsense_gesture_lib.h"
#include "cy_capsense_processing.h"
#include "cy_capsense_structure.h"
#include "cy_capsense_tuner.h"
#if (CY_CAPSENSE_PLATFORM_BLOCK_CSDV2)
    #include "cy_capsense_csd_v2.h"
    #include "cy_capsense_csx_v2.h"
    #include "cy_capsense_sensing_v2.h"
    #include "cy_capsense_selftest.h"
#else /* (CY_CAPSENSE_PLATFORM_BLOCK_MSCV3) */
    #include "cy_capsense_generator_v3.h"
    #include "cy_capsense_sensing_v3.h"
    #include "cy_capsense_sm_base_full_wave_v3.h"
#endif

#endif /* CY_CAPSENSE_H */


/* [] END OF FILE */
