# Bridge CLI

This example manages virtual bridged devices and supports local Read, Write, and Invoke commands.

See the [ESP-Matter documentation](https://docs.espressif.com/projects/esp-matter/en/latest/esp32/developing.html) for build and flash instructions.

## Bridged devices

```text
matter esp bridge support
matter esp bridge add <parent-endpoint> <device-type>
matter esp bridge remove <endpoint>
matter esp bridge list
matter esp bridge reset
```

`reset` removes bridged endpoints and factory-resets the bridge.

## Local control

Commission the bridge before using local commands. Commands target the bridge node on the first available fabric.

```text
matter esp local read attr <endpoint> <cluster> <attribute>
matter esp local write attr <endpoint> <cluster> <attribute> -- <tagged-json>
matter esp local invoke <endpoint> <cluster> <command> -- <json-fields>
```

For an On/Off bridged device at endpoint 2, use:

```text
matter esp local read attr 2 6 0
matter esp local invoke 2 6 1 -- {}
matter esp local write attr 2 6 16385 -- "{\"0:U16\":10}"
```

Write values use `{"0:TYPE": value}`. Invoke accepts `{}` or fields such as `{"0:U16":42}`.
Put JSON after the final `--` delimiter and escape its double quotes.
