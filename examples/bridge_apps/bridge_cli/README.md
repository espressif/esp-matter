
# Bridge Command Line

This example demonstrates a Bridge Command Line example that can manage virtual bridged devices with device console for bridge tests.

See the [docs](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) for more information about building and flashing the firmware.

## 1. Additional Environment Setup

No additional setup is required.

## 2. Post Commissioning Setup

Here are some console command to manage the virtual bridged devices.

- List supported bridged device types

```
matter esp bridge support
```

- Add bridged device with specific device type

```
matter esp bridge add <parent_endpoint_id> <device_type_id>
```

- Remove bridged device on an endpoint

```
matter esp bridge remove <endpoint>
```

- List all the bridged devices

```
matter esp bridge list
```

- Reset the Bridge, clear all the bridged endpoints and factory-reset

```
matter esp bridge reset
```
