## Example Summary

This example introduce the MQTT Client library API and usage, as well as 

## Peripherals Exercised

* The board LEDs are used for status indication. To distinguish similar indications, the user needs to be aware of the executed procedure.   
The following table lists all options.

<table>
  <tr>
    <th>LED indication</th>
    <th>Led Color for CC3220 </th>
    <th>Led Color for CC3235 </th>
    <th>Interpretation</th>
  </tr>
  <tr>
    <td>Solidly on</td>
    <td>Green</td>
    <td>Green</td>
    <td>Indicate Simplelink is properly up - Every Reset / Initialize</td>
  </tr>
  <tr>
    <td>Blinking</td>
    <td>Red</td>
    <td>Blue</td>
    <td>Device is trying to connect to AP - Every Reset / Initialize</td>
  </tr>
  <tr>
    <td>Solidly off</td>
    <td>All</td>
    <td>All</td>
    <td>Device connected and working - Only after connection</td>
  </tr>
  <tr>
    <td>Toggling (Solidly on/off)</td>
    <td>Red</td>
    <td>Blue</td>
    <td>Publish message received in cc32xx/ToggleLED1 topic</td>
  </tr>
  <tr>
    <td>Toggling (Solidly on/off)</td>
    <td>Yellow</td>
    <td>Red</td>
    <td>Publish message received in cc32xx/ToggleLED2 topic</td>
  </tr>
  <tr>
    <td>Toggling (Solidly on/off)</td>
    <td>Green</td>
    <td>Green</td>
    <td>Publish message received in cc32xx/ToggleLED3 topic</td>
  </tr>
</table>

## Example Usage

* Access Point (AP) Configuration
	- AP information is set in 'network\_if.h' file.

* Remote Broker Configuration

	- Broker parameters can be configured in mqttConnParams parameter which can be found in 'mqtt\_client\_app.c'
	- The broker parameters are:
		- Connection types and security options
			- IPv4 connection
			- IPv6 connection
			- URL connection
			- Secure connection
			- skip domain name verification in secure connection
			- skip certificate catalog verification in secure connection
		- Server Address: URL or IP
    	- Port number of MQTT server
    	- Method to tcp secured socket
    	- Cipher to tcp secured socket
    	- Number of files for secure transfer
    	- The secure Files  

* Secured socket  
	In order to activate the secured example, SECURE\_MQTT must be defined in 'mqtt\_client\_app.c' file  ( certificates should be programmed ).

* Client Authentication  
	In order to activate the Client authentication by the server, the clientID and password must be defined in mqttClientParams located in 'mqtt_client_app.c'.
  
* Topics Configuration
	- The topics can be set by calling MQTT_IF_Subscribe with the appropriate parameters. More details in 'mqtt_if.h'.
	- The subscription topics can be set in the **SUBSCRIPTION\_TOPICX** definitions
	- The Client is subscribe to the following default topics  
		**"Broker/To/cc32xx"**  
		**"cc32xx/ToggleLEDCmdL1"**  
		**"cc32xx/ToggleLEDCmdL2"**  
		**"cc32xx/ToggleLEDCmdL3"**  
		**"cc32xx/OTA"** (See details below in the OTA section)  
	- To publish to topics the user can call MQTT_IF_Publish with the appropriate parameters. More details in 'mqtt_if.h'.
	- The Client publish the following default topic “cc32xx/ToggleLED1” - the topic will be published by pressing SW2 on the board
	
* Build the project and flash it by using the Uniflash tool for cc32xx, or equivalently, run debug session on the IDE of your choice.

* Open a serial port session (e.g. 'HyperTerminal','puTTY', 'Tera Term' etc.) to the appropriate COM port - listed as 'User UART'.  
The COM port can be determined via Device Manager in Windows or via `ls /dev/tty*` in Linux.

	The connection should have the following connection settings:

    	Baud-rate:    115200
	    Data bits:         8
	    Stop bits:         1
	    Parity:         None
	    Flow Control:   None


* Run the example by pressing the reset button or by running debug session through your IDE.  
 `Green LED` turns ON to indicate the Application initialization is complete 

* Once the application has completed it's initialization and the network processor is up,  
  the application banner would be displayed, showing version details:

        ============================================
           [GEN::TRACE] MQTT client Example Ver: 2.0.0
        ============================================

         [GEN::TRACE] CHIP: 0x30000019
         [GEN::TRACE] MAC:  2.0.0.0
         [GEN::TRACE] PHY:  2.2.0.7
         [GEN::TRACE] NWP:  3.15.0.1
         [GEN::TRACE] ROM:  0
         [GEN::TRACE] HOST: 3.0.1.65
         [GEN::TRACE] MAC address: 04:a3:16:45:89:8e

        ============================================

* At this point `Board_LED0` will blink until the device will connect to the hard coded AP.  
	* In case connection to the hard coded SSID AP fails, user will be requested to fill the SSID of an open AP it wishes to connect to.
  	* If no AP is available or connection failed, the example will prompt the user to enter the SSID of an open AP.
	* Once the connection success all LEDs turn off.

* Special handling
	- In case the client will disconnect (for any reason) from the remote broker, the MQTT will be restarted.   
	The user can change this behavior by removing the mq_send call for APP_MQTT_DEINIT in the MQTT_EVENT_SERVER_DISCONNECT event for the MQTT_EventCallback.

## Application Design Details

This is an MQTT Client application used to demonstrate the client side of the MQTT protocol. This application uses our MQTT module (mqtt_if.c) as an abstraction layer to our internal MQTT library to make it easier for developers to use MQTT. 

The application starts by performing all the necessary initializations for the peripherals (e.g. GPIO, SPI, UART and timer).  Once that is done the NWP is initialized and the application attempts to connect to an AP using the credentials the user configured in network_if.c. 

At this point everything is initialized except the MQTT client and the device is connected to an AP. In case the connection to the AP fails, the user will be requested to fill the SSID of an open AP to connect to. Next the application calls 'MQTT\_IF\_Init()' to initialize the MQTT module. By default the module will create an internal thread of stack size 2048 and priority 2. These parameters can be modified by changing mqttInitParams in 'mqtt\_client\_app.c'. This thread handles all the events invoked by the internal MQTT library and notifies the user of such events through the MQTT_EventCallback and specific topic callbacks they register when subscribing. 

Then 'MQTT\_IF\_Connect' is called to connect to the broker the user configured in the mqttConnParams structure in 'mqtt\_client\_app.c'. Additionally, it will configure the client parameters using the mqttClientParams structure and register the MQTT\_EventCallback to the MQTT module. If the  'MQTT\_IF\_Connect' call is successful the user will get a CONNACK event indication over UART to show the client is connected to the MQTT broker. 

The application proceeds to subscribe to 4 default topics for illustration purposes and registers 4 individual topic callbacks that are defined in 'mqtt\_client\_app.c'. 

Now the client can receive publish messages from the broker.
In this example the topics in the left will toggle the LEDs in the right
     
							"cc32xx/ToggleLEDCmdL1" <-------------> toggle LED0  
							"cc32xx/ToggleLEDCmdL2" <-------------> toggle LED1  
							"cc32xx/ToggleLEDCmdL3" <-------------> toggle LED2   


The user can invoke more commands by pressing the push buttons on the CC32xx launchpad device: 

* When pressing push button 0 - SW2, The device will publish the message that include the topic and data which is hard coded in 'mqtt\_client\_app.c' by invoking 'MQTT\_IF\_Publish' command.

* Push button 1 - SW3 has multiple functionalities. The first time it's pressed the device will disconnect from the broker and the user will see a MQTT_EVENT_CLIENT_DISCONNECT event from the MQTT_EventCallback. Every time there is a short press on the button it will toggle the connection to the MQTT broker. If the user does a long press the application will de-initialize the MQTT module by destroying the internal MQTT instance as well as freeing any module resources that were allocated. 

## Adding in Over-the-Air (OTA) Update Functionality using the OTA Interface Module
Over-the-Air update functionality allows users to update their devices without physically connecting to them. 

The new OTA Interface Module (OTA_IF) simplifies the integration and usage of OTA in any application. 
The usage of OTA_IF which is demonstrated as an add-on within the MQTT Client application replaces the previous Local and Cloud OTA examples.
This readme assumes you are familiar with the following documents:<br>
* ["Over-The-Air Application Report"](https://www.ti.com/lit/pdf/swra510) - explaining cloud OTA<br>
* [Local OTA README from SDK 5.20 or earlier](https://dev.ti.com/tirex/explore/node?a=fc2e6sr__5.20.00.06&node=AHQEITy7hJ3KKFnYBLkIag__fc2e6sr__5.20.00.06) - explaining local OTA  <br>
* [SimpleLink Academy](https://dev.ti.com/tirex/explore/node?node=ACE5ggZRybEJNZUcnzc3ww__fc2e6sr__LATEST) - OTA Training <br>
 
 
Note: The OTA capability is disabled by default. Please enable the required method in the app's "ota\_settings.h" (see "Enable The OTA\_IF" section below). To enable the launchpad's button to trigger the primary OTA method also set OTA\_DEFAULT\_METHOD (see in mqtt\_client\_app.c") to the required trigger.

### What is OTA\_IF
The OTA\_IF is a wrapper on top of the legacy "ota.a" lib. The OTA_IF module is available as part of the mqtt_client application (see under ifmod/). 

The OTA\_IF was planned to ease the integration of OTA to any existing application. It is designed for RTOS environment only (non RTOS users will use the OTA library as before). The OTA\_IF presents a simple interface for the main application to poll, download and install an OTA update (see API in ifmod/ota\_if.h).

The OTA\_IF currently supports 4 use cases:

1. <b>Cloud OTA (FULL)</b> - Download from an CDN server (such as Github or Dropbox). This use case starts by connecting to the CDN server to get a download link (file server's URL). Once the link is received, the download of the OTA (tar) image will be started automatically by sending HTTP Get Request to the file server. This method supports up to 2 callbacks to 2 different servers (primary and backup) <br>API: **OTA\_IF\_downloadImageByCloudVendor()** (see ifmod/ota\_if.h)<br>Enabled With: **CLOUD\_OTA\_SUPPORT** (see ota\_settings.h)<br>
This method requires Internet Level Connection.<br>

2. <b>Cloud OTA (Download from File Server)</b> - in this case the user will get the URL of the OTA (tar) image using other method and directly download the file (using HTTP Get Request).<br>API: **OTA\_IF\_downloadImageByFileURL()** (see ifmod/ota\_if.h)<br>Enabled With: **CLOUD\_OTA\_SUPPORT** (see ota_settings.h)<br>
This method requires Internet Level Connection.<br>

3. <b>Local OTA</b> - using the SimpleLink internal HTTP server to enable connection from a PC or a mobile device connected to the same local network. The PC/Mobile device will send the OTA (tar) image in a HTTP Post request.<br>API: **OTA\_IF\_uploadImage()** (see ifmod/ota\_if.h)<br>Enabled With: **LOCAL\_OTA\_SUPPORT** (see ota_settings.h)
This method requires local network connection (it can work in AP mode or in a station connected to local AP, even without internet access).<br>
Note: In the mqtt\_client example the OTA is enabled only after connecting (SimpleLink as a station) to a router with internet connection. <br>   

4. <b>Internal Update</b> - in this case the update starts when the OTA (tar) image is already located in the SimpleLink file-system. The OTA\_IF will be used to read the image and install the content.<br>API: **OTA\_IF\_readImage()** (see ifmod/ota\_if.h)<br>Enabled With: **INTERNAL\_UPDATE\_SUPPORT** (see ota_settings.h)
This method can work without any Wi-Fi connection.<br>
Note: In the mqtt\_client example the OTA is enabled only after connecting (SimpleLink as a station) to a router with internet connection. <br>

Combinations of the above methods are allowed, e.g. using Local OTA as backup method in case the Cloud OTA fails.

The OTA\_IF uses a dedicate thread context to load the image and process its content (i.e. write the separate files  to the file system).  It will report the completion of the loading process (**OTA\_NOTIF\_IMAGE\_DOWNLOADED**) in the user registered callback (registred in **OTA\_IF\_init()**). Other async indications such as errors or PENDING_COMMIT state will also be reported through this callback. The full list of events can be found in "ota_if.h" (see otaNotif_e).

During the download process, the device can be operational (the OTA will run in the background). Once the **OTA\_NOTIF\_IMAGE\_DOWNLOADED** indication is received, the user can choose when to call **OTA\_IF\_install()**  to reset the MCU and enable the new update.

Calling the **OTA\_IF\_init** should be called as part of the boot sequence. It will allocate resources needed for the OTA and will check if the image is pending commit. In case of receiving the **OTA\_NOTIF\_IMAGE\_PENDING\_COMMIT**, the **OTA\_IF\_commit()** must be called when the new image is verified.

The OTA\_IF still requires the legacy OTA library (**"ota.a"**). The OTA library should be built as "**OTA\_FILE\_DOWNLOAD**" (in otauser.h). This means that all the Cloud Vendor code will be compiled out and that all the other settings of "otauser.h" will be ignored (See "Configure user accounts" for  details about the new configuration method). The cloud OTA implementation requires the SDK's HTTP Client and JSON libraries.

The OTA\_IF in fact is a component which is composed of the following files: <br>
* ota\_if.h - simplified OTA API definition<br>
* ota\_if.c - the new OTA engine (see "How it works" for more details)<br>
* ota\_vendors.h - API for the *getDownloadLink()* callbacks, currently GITHUB and DROPBOX are supported. See "How to support another server" for more details.<br>
* ota\_vendor\_github.c - *getDoanloadLink()* implementation for GITHUB<br>
* ota\_vendor\_dropbox.c - *getDoanloadLink()* implementation for DROPBOX<br>

  ***************************************************************************************************
	IMPORTANT NOTE: The GITHUB and DROPBOX code is just an example. TI is not responsible for 
	changes of the OTA vendor API or certificates that may impact the access to the cloud servers.
    When planning to use these servers, TI recommends using an altenrate method that can be
	activated in case of issues (this can be a using second OTA vendor and/or local OTA).
  ***************************************************************************************************

### The "ifmod/" folder
The "ifmod" folder contains all the "interface modules" required by the main application. Those are close modules that a user typically won't need to change that are meant to simplify the use and the integration of common feature (e.g. provisioning through the WIFI\_IF, OTA update through the OTA\_IF, MQTT connection through the MQTT\_IF etc). 

The folder contains all the code required for the OTA functionality.

In additions, the "ifmod/" contains other interface modules that are used by the MQTT example: WIFI\_IF, MQTT\_IF, UART\_IF and UTILS\_IF. Those are general interfaces that are not related to the OTA functionality directly. 
If the entire "ifmod" folder is copied to a new project, any unused module may be excluded from the build (or you can trust the linker to do the work (dead code will typically be removed by the linker).


### Enable the OTA\_IF in the mqtt\_client example
The following are instructions for quickly enabling the OTA in the mqtt_client example (OTA is disabled by default):<br>
1. In "ota\_settings.h", enable one or more of the OTA methods by setting the following values to "1":<br>
    `#define CLOUD_OTA_SUPPORT                   (0)` <br>
	`#define LOCAL_OTA_SUPPORT                   (0)` <br>
	`#define INTERNAL_UPDATE_SUPPORT               (0)` <br>
See more details in "Configure user accounts".<br><br>
2.  For cloud OTA, enable the example user parameters (example repository), by un-commenting the following line in "ota\_settings.h":<br>
    `//#define USE_TI_EXAMPLE` <br><br>
3. In "mqtt\_client\_app.c" un-comment one of the following:<br>
	`//#define OTA_DEFAULT_METHOD              StartCloudOTA`<br>
	`//#define OTA_DEFAULT_METHOD              StartLocalOTA`<br>
	`//#define OTA_DEFAULT_METHOD              StartInternalUpdate`<br>
This will set the default method that will get triggered when pressing the left button (one the device is connected). This should correspond to the enabled OTA methods in "ota\_settings.h".


### Enable the OTA\_IF
The following are instructions for including OTA in an existing example:

1. Add the following SDK's libraries to the project linker's files:<br>
1.1 For all uses cases: "ota.a" (under <SDK\>/ti/net/ota/) and "json\_[release|debug].a" (under <SDK\>/ti/utils/json/)<br>
1.2 When using cloud ota use case, add:  "httpclient\_[release|debug].a" (under <SDK\>/ti/net/http/http\_lib\_for\_OTA/) - this library is the same as the original httpclient but uses a larger internal buffer (HTTPClient\_BUF\_LEN is set to 512 due to the Dropbox server's requirements for large HTTP header) <br>

2. Copy the entire "ifmod/" folder from the mqtt_client example to target project.
3. Copy the ota_settings.h to the target application, enable OTA (it is disabled by default) and update user configuration (see "Configure user accounts")   
4. Copy user files and use them (or modification) when creating the flash image:
	* www/ - contain files (html, js, css,etc) need by the HTTP Server for Local OTA
	* RootCACerts.pem and digicert_high_assurance_ca.der are required for the connecting to GITHUB and DROPBOX as part of the cloud OTA
 
 


### Configure user accounts
"ota\_settings.h" is a new header in the application that holds the user settings (enabling/disabling of OTA methods and OTA vendors as well as user's account parameter). The configuration is for the application only. The library does not require a re-build when user's account get changed (as long as the library is configured to OTA\_FILE\_DOWNLOAD.

Several methods (CLOUD\_OTA\_SUPPORT, LOCAL\_OTA\_SUPPORT and\or INTERNAL\_UPDATE\_SUPPORT) can be enabled together. Note: by default all the methods are disabled. User must enable at least one method to enable the OTA_IF API.
 
Several cloud vendors (such as OTA\_VENDOR\_GITHUB\_SUPPORT and OTA\_VENDOR\_DROPBOX\_SUPPORT) can also be configured together.

**NOTE 1:** Servers' certificates are not defined here but in the ota\_vendor\_...c implementation. The required certificates for GITHUB and DROPBOX are provided with the example (as user files for the image.syscfg). The certificates must be installed in the file system before trying to connect to the cloud vendors.
 
**NOTE 2:** Since both Dropbox and Github are about to replace their certificate soon (fall 2021), the current code support both the legacy certificate (digicert high assurance ev root ca) and new certificate (digicert global root ca). For CC3230/5 it is done using pem file that contains both root certificates. This will work on the first attempt as the NWP can choose the right certificate from the file. For CC3220, only the first certificate of the PEM file is being used (the new certificate). In case the connection will fail (i.e. until the certificate change at the server side), the module will print the error message but will automatically tries the legacy certificate (provided as as separate der file). This behavior is implemented in the OTA Vendor specific code (ota\_vendor\_github.c and ota\_vendor\_dropbox.c).    

#### DROPBOX parameters
**DROPBOX\_USER\_TOKEN** - Dropbox Application identifier. Only "No-Expiration" tokens are currently supported.<br>
**DROPBOX\_USER\_PATH** - relative path to the folder (within the Dropbox application) that contains the OTA (tar) files.<br>

#### GITHUB parameters
**GITHUB\_USER\_NAME** - GitHub user account name.<br>
**GITHUB\_USER\_REPO** - GitHub repository name<br>
**GITHUB\_USER\_PATH** - relative path to the folder (within the GitHub repository) that contains the OTA (tar) files. <br>
**GITHUB\_USER\_TOKEN\_B64** - GitHub Personal Access Token (defined per user) - converted to base64.<Br> 
**Note**: The Personal Access Token must be converted to base64 format. To generate the base64 format, use the following bash command (any other base64 converter is acceptable): 
** "*echo <personal-access-token> | base64*"** <br>
e.g.: "*echo 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 | base64*"<br>
**Important**: make sure the token is defined with no "No Expiration" and that it enables read-only access (no "scope" should be enabled). If needed enable a second Personal Access with write-access. The reason is that the token can be retrieved from the image.
 

### Support different Cloud Vendor
For cloud OTA TI provides 2 examples of cloud vendors: GITHUB and DROPBOX (implemented in  ota\_vendor\_github.c and ota\_vendor\_dropbox.c). 
Both examples implement the following callback:<br>
int16\_t (*getDownloadLink)(FileServerParams_t *pServerParams)

The callback doesn't get any input, but is based on the configuration in "ota\_settings.h" to connect to the user account on the specific OTA server. The callback return status code (0 upon success) and a link (URL) to a file server to download the OTA (tar) file (see the output structure: pServerParams).
If all goes well, the OTA\_IF will download the tar file and continue to install the files.

If a user needs to connect to another server (i.e. not GITHUB or DROPBOX) - it may refer to the 2 examples and update them as needed (as well as the ota\_vendors.h file).
By doing so he will be able to use the *OTA\_IF\_downloadImageByCloudVendor()*, e.g:<br>

    OTA_IF_downloadImageByCloudVendor(OTA_NEWSERVER_getDownloadLink, OTA_GITHUB_getDownloadLink, 0); 
	// i.e. Try the new server as first and use GITHUB as a backup server

A user can get the Download link in any other way and simply use `OTA_IF_downloadImageByFileURL(FileServerParams_t *pFileServerParams, uint32_t flags)` with the required download URL. 


### OTA in the MQTT Client Application
Note: The OTA capability is disabled by default. Please enable the required method(s) in the app's "ota\_settings.h".

The OTA interface module creates a separate thread used for polling CDN servers, downloading and extracting new updates, and lastly installing them. The OTA_IF API exposes functions that schedule tasks for this thread. After each task completes, a registered callback function is called.

In mqtt\_client\_app.c, navigate to mainThread. Find the call to OTA\_IF\_init(). This function creates the OTA thread that will be running in the background and used to perform operations for the OTA process. Notice how it registers the callback function OtaCallback. Next, scroll down to the MQTT\_IF\_SUBSCRIBE calls. There is one new hooks here (see the "cc32xx/OTA" topic registration with the StartOTA() callback). According to the MQTT payload an external MQTT user can set the required method to be enabled ("cloud", "local" or "internal" - see in StartOta()).

Additional method for triggering the OTA is through the "publish" button (button 0 / SW2) on the launchpad. In addition to publishing an MQTT message, the handler will also trigger the StartOTA() where a default OTA method will be used (by default no method is defined. The OTA\_DEFAULT\_METHOD macro (see in "mqtt\_client\_app.c") needs to be set to one of: StartCloudOTA / StartLocalOTA / StartInternalUpdate. 

Note: The Internal Update requires having a tar file in the file system (by default under "OtaImages/" folder - see the StartInternalUpdate\(\) in "mqtt\_client\_app.c"). A pre-built example tar files can be found in the project directory in the SDK under userFiles). 

Next, navigate to StartCloudOta, which calls OTA\_IF\_downloadImageByCloudVendor. This function makes calls to two different vendor functions. These functions can be replaced with any user-created function, as long as they follow the same function prototype and return behavior. If you open ota\_vendor\_dropbox.c or ota\_vendor\_github.c, you will see the implementation for these example vendor functions. StartLocalOta calls OTA_IF_uploadImage, which sets up the HTTP server and prepares to receive a .tar file. You can add extra security parameters here.

Next, navigate to OtaCallback. This is the callback function that is called for OTA messages. OTA\_NOTIF\_IMAGE\_DOWNLOADED is received when the image has finished downloading, and when this occurs, the function tells the main thread to DEINIT MQTT and prepare to install the new update and restart the device. In mainThread, this queued message causes the code to break out of the infinite while loop, de-initialize MQTT, and install the new OTA image. This will also reset the MCU.

After the MCU resets, the image needs to be committed. In OtaCallback, OTA\_NOTIF\_IMAGE\_PENDING\_COMMIT is received. After ensuring the device is properly connected to the internet, OTA\_IF\_commit is called, and the new image is committed successfully.

## References

[MQTT Org - MQTT Home page](http://mqtt.org/documentation)  
[MQTT v3.1.1 specification](http://docs.oasis-open.org/mqtt/mqtt/v3.1.1/os/mqtt-v3.1.1-os.html)  
[MQTT v3.1 specification](http://www.ibm.com/developerworks/webservices/library/ws-mqtt/index.html)  

For further information please refer to the user programmers guide: [CC3X20 Programmer's Guide](http://www.ti.com/lit/swru455)
