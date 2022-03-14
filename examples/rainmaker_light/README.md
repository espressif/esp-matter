# RainMaker Light Example

## Building and Flashing the Firmware

See the [README.md](../../README.md) file for more information about building and flashing the firmware.

## What to expect in this example?

This example showcases integration of ESP RainMaker with a Matter light device. ESP RainMaker is an end-to-end solution offered by Espressif to enable remote control and monitoring for ESP32 based products without any configuration required in the Cloud. Please refer the ESP RainMaker documentation [here](https://rainmaker.espressif.com/docs/get-started.html) for more details.

Supported features:
 - Matter Commissioning
 - RainMaker Claiming and User-Node Association
 - On/Off, Brightness and Color (on ESP32-C3 for now) control over RainMaker app as well as Matter Controller
 - Updates through RainMaker reflected over Matter and vice versa

Make sure to follow these additional steps along with the steps in the top level README.

### Getting the Repositories

This only needs to be done once:
```
$ git clone --recursive https://github.com/espressif/esp-rainmaker.git
```
Setup the RainMaker CLI from here: https://rainmaker.espressif.com/docs/cli-setup.html

### Configuring the environment

This needs to be done everytime a new terminal is opened:
```
cd esp-matter/examples/rainmaker_light/

export ESP_RMAKER_PATH=/path/to/esp-rainmaker
```

### RainMaker Claiming

If self-claiming is not enabled/supported, this need to be done before flashing the firmware.

RainMaker CLI:
```
$ cd $ESP_RMAKER_PATH/cli
$ rainmaker.py claim --addr 0x3E0000 $ESPPORT
```

### RainMaker User-Node Association

This needs to be done after commissioning.

Check if the device already has user node association done, using the custom RainMaker cluster (cluster_id: 0xc00):
```
$ ./out/debug/chip-tool any read-by-id 0xc00 0x0 0x1 0x0
```
* If the above custom status attribute (attribute_id: 0x0) returns true, the association has already been done.
* If the attribute returns false, the association has not been done. And the below custom configuration command
(command_id: 0x0) can be used to do the association.


RainMaker CLI:

Get the details: This will print the user_id and secret_key (do not close this): 
```
$ rainmaker.py test --addnode <node-id>

>> add-user <user-id> <secret-key>
```

Prepare the command payload: Use the above details.
```
payload: <user_id>::<secret_key>
```

Now use the payload to run the RainMaker configuration command from chip-tool:
```
$ ./out/debug/chip-tool any command-by-id 0xc00 0x0 '"<user_id>::<secret_key>"' 0x1 0x0
```

The device/node should now be associated with the user.
