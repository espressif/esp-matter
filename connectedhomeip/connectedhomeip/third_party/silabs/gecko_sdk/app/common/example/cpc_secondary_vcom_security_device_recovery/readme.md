# CPC Secondary VCOM

This sample application demonstrates how to recover a device if a the CPC keys has been lost and the unbind functionality has 
not been implemented. This sample does nothing else than allowing unbinding the device so it can be re-flashed and bind can be
performed to associate the key.

## Installation and Use

Flash the appliction and let it run. Once the init function has executed the device would be unbound and it will be possible to 
call the CPCd bind functionality.