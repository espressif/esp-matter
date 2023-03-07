# OS config

The below Real Time Operating Systems (RTOSes) are available in the Open IoT SDK:
- [RTX](https://www2.keil.com/mdk5/cmsis/rtx)
- [FreeRTOS](https://www.freertos.org/)
- [ThreadX](https://azure.microsoft.com/en-us/services/rtos/#overview)

All supported RTOSes can be used via the [CMSIS-RTOS v2 API](https://www.keil.com/pack/doc/CMSIS/RTOS2/html/index.html).

FreeRTOS and ThreadX can be used via their own native APIs.

## RTX (via CMSIS-RTOS v2 API)

`IOTSDK_CMSIS_RTX=ON` option makes available both the CMSIS-RTOS v2 API and the required RTX components from CMSIS5.

These libraries are made available to CMake projects under the  names `cmsis-rtos-api` and `cmsis-rtx` and can be linked into your application as follows:

```
    target_link_libraries(my_app
        cmsis-rtos-api
        cmsis-rtx
    )
```

A full example application can be found at [examples/cmsis-rtos-api](../examples/cmsis-rtos-api).

## FreeRTOS (via CMSIS-RTOS v2 API)

`IOTSDK_FREERTOS=ON` should be passed with the `IOTSDK_CMSIS_RTOS_API=ON` flag. `freertos-cmsis-rtos` can then be linked in your application:

```
target_link_libraries(my_app freertos-cmsis-rtos)
```

A full example application can be found at [examples/cmsis-rtos-api](../examples/cmsis-rtos-api).

## FreeRTOS (via stand-alone kernel)

For vanilla FreeRTOS API support, set only `IOTSDK_FREERTOS=ON`. `freertos-kernel` can then be linked in your application:

```
target_link_libraries(my_app freertos-kernel)
```

You may also select a FreeRTOS heap implementation by linking with `freertos-kernel-heap-*` (\* is 1-5, [see for details](https://freertos.org/a00111.html)).

A full example application can be found at [examples/freertos-api](../examples/freertos-api).

## ThreadX (via CMSIS-RTOS v2 API)

To enable use of ThreadX with the CMSIS-RTOSv2 API, fetch both ThreadX and the ThreadX CDI Port by passing both `IOTSDK_THREADX=ON` and `IOTSDK_THREADX_CDI_PORT=ON`. Then, link `threadx-cdi-port` to your application:

```
target_link_libraries(my_app threadx-cdi-port)
```

A full example application can be found at [examples/cmsis-rtos-api](../examples/cmsis-rtos-api).

## ThreadX (via stand-alone kernel)

To fetch and use ThreadX, the flag `IOTSDK_THREADX=ON` should be passed. The `threadx` cmake target can then be linked in your application:

```
target_link_libraries(my_app threadx)
```

A full example application can be found at [examples/threadx-api](../examples/threadx-api).
