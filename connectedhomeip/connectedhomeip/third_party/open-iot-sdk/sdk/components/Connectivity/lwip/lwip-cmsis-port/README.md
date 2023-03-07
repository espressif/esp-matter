# lwIP CMSIS port

This library is a dependency of `lwip`.

To use this port your application must provide `user_lwipopts.h` which contains optional user overrides of lwIP options.
You can do this by adding an include directory to library `lwipopts`.

```
target_include_directories(lwipopts
    INTERFACE
        <PATH TO USER CONFIG>
)
```

You may use the template provided in `./include/user-config`. See `lwipopts.h` in the lwIP project for details on
options.

The `lwipcore` library requires a valid config to build. To use the configuration defined by this port,
the application must link `lwipcore` to the `lwip-cmsis-port`.

```CMake
target_link_libraries(lwipcore
    PRIVATE
        lwip-cmsis-port
)
```

Optionally, if you want to get tracing from lwIP you must provide a definition of `DEBUG_PRINT` in `lwipopts`
library. For example a `DEBUG_PRINT=printf` will allow you to use the `printf` from libc whose stdio is retargeted
to MCU-Driver-HAL's serial by `iotsdk-serial-retarget`. You may do this like so:

```
target_link_libraries(my-application
    PRIVATE
        iotsdk-serial-retarget
)

target_compile_definitions(lwipopts
    INTERFACE
        DEBUG_PRINT=printf
)
```

# EMAC Interface

The library requires an implementation of an MCU Driver HAL EMAC interface. The
EMAC interface is retrieved using the function:

```C
mdh_emac_t *mdh_emac_get_default_instance(void)
```

This function must be provided by the application.

# Packet latency on the input interface

Normally the TCP/IP thread priority is higher than the thread servicing the interface receiving network packets.
This optimises for heap memory consumption - any packets received are processed immediately to free up the buffer
as quickly as we can. This might mean that the network interface is in the meantime buffering up data.

To prioritize servicing the input thread, link the `lwip-cmsis-port` CMake target with
`lwip-cmsis-port-low-input-latency` as follows:

```CMake
target_link_libraries(lwip-cmsis-port PUBLIC lwip-cmsis-port-low-input-latency)
```

If you use this option it's important to make sure the heap is big enough to buffer up all the data it receives.
Dropped packets could be an indication of insufficent heap space.
Enable LWIP debug prints in your `user_lwipopts.h` as shown below to see if pbuf allocations fail.

```C
#define LWIP_DEBUG
#define PBUF_DEBUG LWIP_DBG_ON
```

## License

Files are licensed under the Apache 2.0 license.
