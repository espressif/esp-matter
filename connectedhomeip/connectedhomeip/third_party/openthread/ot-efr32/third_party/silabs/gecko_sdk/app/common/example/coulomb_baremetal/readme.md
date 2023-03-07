# Coulomb Counter Bare Metal Application

This example application shows the use of coulomb counter. See [AN1188: EFP01 Coulomb Counting](https://www.silabs.com/documents/public/application-notes/an1188-efp01-coulomb-counting.pdf) for more details about coulomb counter.

The example demonstrates how to read coulomb counter total charge and store it in NVM3. This can be done in two different ways:

1. By periodically (every second) polling total coulombs consumed and storing it in a NVM3 object
2. By using CLI commands:

	* **coulomb_get**: Return coulombs consumed since last boot.
	* **coulomb_calibrate**: Calibrate coulomb counter.
	* **coulomb_get_total**: Return coulombs consumed over device's lifetime.
	* **coulomb_update_total**: Read coulomb counters and update total value.
	* **coulomb_reset_total**: Reset total counter to zero.

## Requirements

Silicon Labs board with analog pins and UART.