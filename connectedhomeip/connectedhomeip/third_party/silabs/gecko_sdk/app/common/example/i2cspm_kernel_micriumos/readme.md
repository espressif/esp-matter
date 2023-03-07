# I2C SPM Micrium OS Application


This example project shows how to use the I2C Simple Polled Master driver using the Si7021 Relative Humidity and Temperature Sensor in a Micrium OS Task.


The initial temperature is read, and upper and lower limits are calculated based on TEMPERATURE\_BAND\_C. When the current temperature reaches the upper limit, LED0 is turned on and LED1 is turned off. When the current temperature reaches the lower limit, LED1 is turned on and LED0 is turned off.


Information is also output on the VCOM serial port.

