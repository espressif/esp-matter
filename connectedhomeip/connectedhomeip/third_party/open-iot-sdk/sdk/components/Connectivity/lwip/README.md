# lwIP

All the sources belong to the [lwIP project]](https://savannah.nongnu.org/projects/lwip/).

The library is an implementation of the `iotsdk-ip-network-api`. To compile you must provide a lwip_sys port for your platform.
Such a library is provided in the sdk as `lwip-cmsis-sys` inside the `lwip-cmsis-port` component.

To enable `lwip-cmsis-port`, it is required to link the target `lwipcore` to it:

```CMake
target_link_libraries(lwipcore
    PRIVATE
        lwip-cmsis-port
)
```

Otherwise to use another port, you must link the `lwipcore` library with your port.

## License

The lwIP project is licenced under the BSD license.
