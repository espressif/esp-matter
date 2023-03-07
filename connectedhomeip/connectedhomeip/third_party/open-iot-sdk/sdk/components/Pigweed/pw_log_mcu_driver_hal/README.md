# Pigweed log MCU-Driver-HAL

[Pigweed log](https://pigweed.dev/pw_log/) backend using
[MCU-Driver-HAL](https://gitlab.arm.com/iot/open-iot-sdk/mcu-driver-hal/mcu-driver-hal).

The backend output logs through one of the serial port. It is optimized for
speed over size.

# Usage

Applications must link against `pw_log` and `pw_log_mcu_driver_hal` to use this
backend. Another library is required for [multithreading](#multithreading)
support.

## Initialization

Before any use of the logger, the application must pass the serial port to use
to the logger: `pw_log_mdh_init(&serial)`

## Multithreading

The serial port can be protected from concurrent access using an RTOS mutex.
Definitions of the locking feature are present in a separate library that the
application must link against:
- `pw_log_mdh_null_lock`: No mutex is used. This is useful for bare metal
  targets.
- `pw_log_mdh_cmsis_rtos_lock`: A CMSIS-RTOS mutex is used to protect the
  serial from concurrent access.

It is possible to provide a different locking library for this log backend. It
must define the following functions:
- `void _pw_log_init_lock()`
- `void _pw_log_lock()`
- `void _pw_log_unlock()`

## Config

Configuration is made using a CMake library. Configuration options are
available from [config.h](public/config.h).

1. Create the config library in the build files: `add_library(<config_name>
   INTERFACE)`. The library must be an interface as the configuration should be
   visible to all modules linking against the log library.
2. Add the compile definitions to the library:
   `target_compile_definitions(<config name> INTERFACE [defines to override...])`.
3. Bind the configuration with the backend:
   `pw_set_module_config(pw_log_mcu_driver_hal_CONFIG <config name>)`.

## License

This backend is licensed under the Apache 2.0 license.
