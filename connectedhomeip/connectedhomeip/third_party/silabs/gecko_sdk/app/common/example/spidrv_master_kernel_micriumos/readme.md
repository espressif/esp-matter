# SPI Primary Micrium OS Application


This example project demonstrates use of the Serial Peripheral Interface in primary mode 
in a Micrium OS task.


Once the primary application is installed on a starter kit, the kit can be connected 
via the 20-pint expansion header to a second kit which has the secondary application installed.


The expansion header pins must be connected as follows:   
Connect primary CS to secondary CS    
Connect primary SCLK to secondary SCLK   
Connect primary MOSI to secondary MOSI   
Connect primary MISO to secondary MISO


The user can then connect to the device via the VCOM serial connection. The primary and secondary
devices will periodically exchange data, logging the exchanges over VCOM.

