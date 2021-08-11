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

This need to be done before flashing the firmware. Note the mac address of the device.

RainMaker CLI:
```
$ cd $ESP_RMAKER_PATH/cli
$ rainmaker.py claim --addr 0x3E0000 $ESPPORT
```

### RainMaker User-Node Association

This need to be done after commissioning.

RainMaker CLI:
```
$ rainmaker.py test --addnode <node-id>
```

This will print the console command to be run on the device:
```
add-user <user-id> <secret-key>
```

Copy-paste this command on the device console.
