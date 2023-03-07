# NVM3 Bare Metal

This example application demonstrates the use of Third Generation Non-Volatile Memory (NVM3) data storage. The application uses command line interface to execute operations on NVM3 which are given by user.

Users can give the following commands:

* Write - Writes into an existing or new file
* Read - Rest the contents of an existing file
* Delete - Deletes the contents of an existing file

This application can save 10 files. Additionally, the number of writes and deletes are saved in the counter objects in the `nvm3_app.c` file.

## Requirements

Silicon Labs board with MPU module.

## Resources

* [AN1135: Using Third Generation Non-Volatile Memory (NVM3) Data Storage](https://www.silabs.com/documents/public/application-notes/an1135-using-third-generation-nonvolatile-memory.pdf)