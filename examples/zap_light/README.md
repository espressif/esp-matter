# Zap Light

This example creates a Color Temperature Light device using the Zap
data model instead of the ESP Matter data model.

See the [docs](https://docs.espressif.com/projects/esp-matter/en/main/esp32/developing.html) for more information about building and flashing the firmware.

## 1. Additional Environment Setup

### 1.1 Customization

For customizing the 'device', the zap-tool can be used to create the
`.zap` file, if you don't already have one. An existing .zap file can
also be edited.

```
cd esp-matter/connectedhomeip/connectedhomeip
./scripts/tools/zap/run_zaptool.sh <optional_existing_zap_file>
```

-   If this command fails, run it again.
-   Once the customization is done, click on 'save' to save the .zap
    file.

The other zap-generated files can be generated using the generate
command:

```
./scripts/tools/zap/generate.py /path/to/<saved>.zap -o /path/to/<output_folder>
```

Now the files in zap_light/main/zap-generated can be replaced with the
new generated files.

## 2. Post Commissioning Setup

No additional setup is required.

## 3. Device Performance

### 3.1 Memory usage

The following is the Memory and Flash Usage.

-   `Bootup` == Device just finished booting up. Device is not
    commissionined or connected to wifi yet.
-   `After Commissioning` == Device is conneted to wifi and is also
    commissioned and is rebooted.
-   device used: esp32c3_devkit_m
-   tested on:
    [6a244a7](https://github.com/espressif/esp-matter/commit/6a244a7b1e5c70b0aa1bf57254f19718b0755d95)
    (2022-06-16)

|                         | Bootup | After Commissioning |
|:-                       |:-:     |:-:                  |
|**Free Internal Memory** |121KB   |118KB                |

**Flash Usage**: Firmware binary size: 1.24MB

This should give you a good idea about the amount of free memory that is
available for you to run your application's code.

Applications that do not require BLE post commissioning, can disable it using app_ble_disable() once commissioning is complete. It is not done explicitly because of a known issue with esp32c3 and will be fixed with the next IDF release (v4.4.2).

## A2 Appendix FAQs

### A2.1 Zaptool is not working

The run_zaptool.py command is failing:

-   Check that the connectedhomeip submodule is updated.
-   Revert any modifications in any of te files in the connectedhomeip
    submodule, or any submodules in connectedhomeip, and try again.
-   If you are still facing issues, reproduce the issue on the default
    example for the device and then raise an [issue](https://github.com/espressif/esp-matter/issues).
    Make sure to share these:
    -   The complete logs for the command.
    -   The esp-matter and esp-idf branch you are using.

### A2.2 Missing files in zap-generated

Some files are not generated on running the generate command
(generate.py):

-   This can happen depending on your zap configuration, and it should
    be okay to use the file which was already present in zap-generated.
-   We have observed that af-gen-event.h does not get generated on
    running the generate command.
-   If the file is missing from the zap-generated folder, there might be
    other compilation errors of the file not being found.
