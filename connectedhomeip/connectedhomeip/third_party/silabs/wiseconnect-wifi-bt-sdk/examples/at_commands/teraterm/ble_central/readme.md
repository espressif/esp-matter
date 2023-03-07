## BLE Central (Main) Mode Tera Term Script
This application demonstrates how to connect with a remote BLE device in BLE central mode.

Before continuing, ensure the RS9116 EVK is plugged into your computer and Tera Term is connected as described in [Getting Started with PC using AT Commands](http://docs.silabs.com/rs9116-wiseconnect/2.4/wifibt-wc-getting-started-with-pc/). 

To run this example the user will be required to enter the Address type (0- for Public and 1- for random) of the remote device and remote device's BD address(XX-XX-XX-XX-XX-XX).

User needs to have BLE Peripheral. `"EFR Connect"` Mobile App some other Apps or devices of user's choice can be used for this. Refer the "Configuring Remote device as BLE Slave" section to configure the "EFR Connect" as "BLE Advertiser".

**STEP 1.** Reset the RS9116 EVK.

**STEP 2.** In the Tera Term menu select `Control->Macro`.

![TeraTerm Macro](./resources/tera-term-macro.png)
	
**STEP 3.** Navigate to the folder `<SDK>/examples/at_commands/teraterm/ble_central` and select the file `ble_central.ttl`.

![Tera Term Script to be loaded](./resources/tera-term-scripts-ble-central.png)

**STEP 4.** The script will perform the following command sequence: 

1. Opermode
2. Setlocalname
3. Get local address

**STEP 5.** After the "scan" command Tera Term will begin outputting advertising reports.

![BLE Central Prompt](./resources/ble-central-prompt-scan-enabled.png)


**STEP 6.** Advertise reports  come up with "Address" "RSSI" , "Adv data length" & "Adv data" .

![BLE Central scan disable prompt](./resources/ble-central-mode-prompt-scan-disable.png)

**STEP 7.** Enter the address type user needs to enter the Address type(i.e. 0 - Public address and 1 - Random address)  as given below , we share here "Random Address".

![Enter address type](./resources/ble-central-mode-prompt-enter-the-address-type.png)

**STEP 8.** After address type selection user is prompted to enter the Remote BLE Device Address to connect to the Silabs BLE Central.

![Connect to BLE central](./resources/ble-central-mode-prompt-connect-to-the-ble-central.png)

**STEP 9.** After entering the Remote BLE Device Address Silabs BLE Central device will initiate connection with the Remote BLE Device.

![BLE Device Connected](./resources/ble-central-mode-prompt-ble-device-connected.png)

**STEP 10.** When Silabs BLE Central device is connected to the Remote device, the Silicon Labs BLE device will query the profiles  from the Remote device.

**STEP 11.** Next it will ask the input to enter the "Handle" to read the value on Remote device.

![Handle](./resources/ble-central-mode-handle-to-read-value.png)

**STEP 12.** When we enter the "Handle" as input to read the value on Remote device, it will read the value.
![Handle](./resources/ble-central-mode-handle.png)

## Configuring Remote device as BLE Peripheral (Slave)

**STEP 1.** Open "EFR Connect" App in the Mobile device.

![Open EFR Connnect App](./resources/efr-connect-app-open-1.png)

**STEP 2.** Go to the "GATT Configurator" to create the GATT server with services.

![Open GATT Configurator](./resources/gatt-configurator-open-2.png)

**STEP 3.** Create the GATT server and add services to that server.

![Create Gatt Server](./resources/create-server-3.png)

**STEP 4.** Click on the created Gatt Server and Add the services using the "Add Service" button.

![Click on created Gatt Server](./resources/add-service-4.png)

**STEP 5.** Once you click on the "Add service" button, we will get the window to add the service. Once you add the service, enable the "Add mandatory service requirements" and  save the configuration.

![Add Service](./resources/heart-rate-service-adding-5.png)

**STEP 6.** After saving the configuration, you can see the added service under the "GATT Server".

![See the added service unser gatt server](./resources/service-added-checking-6.png)

**STEP 7.** Now enable the created GATT server.

![Enable the created Gatt server](./resources/enable-gatt-server-7.png)

**STEP 8.** Come back to "home page" of EFR Connect app and open "Advertiser" and create the "Advertiser".

![open the advertiser](./resources/open-advertiser-create-advertiser-8.png)

**STEP 9.** Open the created "Advertiser" add the required configuration, like "Addvertising Data", "Scan Response Data" etc, then save the configuration.

![Adding configuration in advertiser](./resources/open-created-advertiser-set-advert-name-9.png)

**STEP 10.** Click on the created "Advertiser" and see the added configuartion like "Advertising Type", "Flags" etc.

![Checking Advertiser configuration](./resources/exapand-advertiser-10.png)

**STEP 11.** Now enable the created "Advertiser" to advertise the device, this device should now be visible to BLE scanners.

![Enable the Advertiser](./resources/enable-advertiser-11.png)