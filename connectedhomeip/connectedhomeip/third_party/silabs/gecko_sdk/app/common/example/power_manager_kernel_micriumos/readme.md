# Power Manager Kernel Application


This example project demonstrates use of the Power Manager module 
in an OS task.


To cycle through the desired energy modes, click Button 1.
 * 1 click -> EM1
 * 2 clicks -> EM2, etc.
Button 0 is used to confirm selection.


Once a mode is selected, the device will remain in that mode until the following conditions are met:
 * EM1, EM2: the sleep timer expires
 * EM3: a button is pushed
 * EM4: the device resets 

