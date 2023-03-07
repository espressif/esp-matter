## BLE Proximity Profile Tera Term Script
This application demonstrates how to use the RS9116W BLE proximity profile with AT commands over UART. A mobile device running the [Silabs EFR Connect app](https://www.silabs.com/developers/efr-connect-mobile-app) is required for the demonstration.

Before continuing, ensure the RS9116 EVK is plugged into your computer and Tera Term is connected as described in [Getting Started with PC using AT Commands](http://docs.silabs.com/rs9116-wiseconnect/latest/wifibt-wc-getting-started-with-pc/). 

**STEP 1.** Reset the RS9116 EVK.

**STEP 2.** In the Tera Term menu, select `Control->Macro`.
![Tera Term Macro](./resources/tera-term-macro.png)
	
**STEP 3.** Navigate to the folder `<SDK>/examples/at_commands/teraterm` containing example scripts and select the file `ble_proximity_profile.ttl`.
![Tera Term script to be selected](./resources/tera-term-scripts.png)

**STEP 4.** After running the `ble_proximity_profile.ttl` script, a pop-up appears. Select OK to continue.
![Application Start pop up](./resources/starting-popup-message-1.png)

**STEP 5.** The autobaud process runs and shows a pop-up with the text "Firmware Loading Done". Select OK to continue.
![Opermode command success](./resources/firmware-update-done-2.png)

**STEP 6.** The command sequence executes in turn, eventually putting the RS9116W into an advertising state.
![Band input prompt](./resources/advertising-state-3.png)

**STEP 7.** Using the [Silabs EFR Connect](#using-the-efr-connect-app) app, scan for and connect to the RS9116W
![Init command success](./resources/remote-device-connected-4.png)

**STEP 8.** After a successful connection, the script waits for the 'Alert Level' to be set by the EFR Connect app. The alert level is used for proximity alert indication.
![IPConfig Success](./resources/alert-level-selection-5.png)

**STEP 9.** Using the EFR Connect app, the alert level can be set to: no alert, mild alert, or high alert. See [Using the EFR Connect App](#using-the-efr-connect-app).
![IPConfig Success](./resources/selected-alert-level-6.png)

**STEP 10.** After the alert level is set, the script runs in a loop. If the RS9116W moves too far from the mobile device running the EFR Connect app, an alert shows. The RSSI threshold configured in the script is `-60 dBm`. In the following example, the RSSI is `-52` which is higher than the threshold. In this case, the alert shows as `0x00` which means no alert.
![Security mode input prompt](./resources/no-alert-7.png)

**STEP 11.** In the following example, the RSSI is `-62` which is lower than the threshold. In this case, the alert shows as `0x01` causing a mild alert to be generated.
![Security mode input prompt](./resources/mild-alert-8.png)

## Using the EFR Connect App
The [Silabs EFR Connect](https://www.silabs.com/developers/efr-connect-mobile-app) mobile app can be used to connect to the RS9116W.

**STEP 1.**  Open the 'EFR Connect' app, select 'Browser' and scan for the RS9116W device.
![waiting for station to connect](./resources/opening-connect-app-1.png)

**STEP 2.** Select 'Connect' to connect with the RS9116W.
![waiting for firmware upgrade](./resources/scanning-devices-2.png)

**STEP 3.**  Select the 'Link Loss' service. 
![waiting for firmware upgrade](./resources/link-loss-service-3.png)

**STEP 4.** The 'Alert Level' characteristic shows.
![waiting for firmware upgrade](./resources/alert-char-service-4.png)

**STEP 5.** Select the desired alert level.
![webpage](./resources/remote-device-alert-selection-5.png)
![webpage](./resources/remote-device-mild-alert-selection-6.png)

**STEP 6.**  In this example, a mild alert is selected.
![Upgrade in progress](./resources/remote-device-read-mild-alert-7.png)

**STEP 7.**  As the RSSI increases and decreases, the alert level changes to indicate whether the distance between the RS9116W and mobile device running the EFR Connect app is exceeded.
![Upgrade in progress](./resources/remote-device-read-no-alert-8.png)