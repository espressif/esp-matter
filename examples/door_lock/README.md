# Doorlock

This example creates a Doorlock device using the ESP Matter data model. It also demonstrates integration with Aliro over NFC feature.

See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) for more information about building and flashing the firmware.

## 1. Additional Environment Setup

No additional setup is required.

## 2. Post Commissioning Setup

No additional setup is required.

## 3. Time Synchronization

The example enables SNTP-backed Time Synchronization and exposes the Time Zone feature on the Root Node.

## 4. Aliro over NFC Feature

### Hardware Required

- [M5Stack NanoC6](https://docs.m5stack.com/en/core/M5NanoC6) or [M5Stack NanoH2](https://docs.m5stack.com/en/core/NanoH2)
- [M5Unit-NFC](https://docs.m5stack.com/en/unit/Unit_NFC)

### Build

```
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.nanoh2_aliro_nfc" set-target esp32h2 build
```

or

```
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.nanoc6_aliro_nfc" set-target esp32c6 build
```

### Test with Apple Home

After commissioning with the Apple Home app, the Home app automatically adds a key to Apple Wallet, which can be used to unlock the door.

|                                     Add Home Key                                     |                                     Unlock the door                                     |
| :----------------------------------------------------------------------------------: | :-------------------------------------------------------------------------------------: |
| <img src="./img/home_key_add.jpeg" alt="Home Key added to Apple Wallet" width="300"> | <img src="./img/unlock_page.jpeg" alt="Unlocking the door with Apple Home" width="300"> |

## 5. Local Controls and Status

Press the boot button (front button on M5Stack Nano) to toggle the lock state. The RGB LED indicates the current state:

- Red: locked
- Green: unlocked or unlatched
- Amber: locking or unlocking
- Off: unknown state

## 6. Diagnostic Console

The door-lock debug command is available under `matter esp dl`:

```text
matter esp dl status
matter esp dl users
matter esp dl pin-unlock <PIN>
```

- `status` prints aggregate user and credential capacity.
- `users` prints user-to-credential relationships without credential data.
- `pin-unlock` validates that the PIN belongs to an enabled user and requests a local keypad unlock.
