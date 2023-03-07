# TEMPDRV Micrium OS kernel application


This example project demonstrates use of the temperature sensor driver 
in a Micrium OS task.


The initial temperature is read, and upper and lower limits are calculated based
on TEMPERATURE\_BAND\_C. If the temperature passes outside the allowable range, 
LED0 (low limit) or LED1 (high limit) is illuminated. The LED will stay lit until
the temperature returns to the acceptable range, +/- HYSTERESIS\_C.


Information is also output on the VCOM serial port. 

