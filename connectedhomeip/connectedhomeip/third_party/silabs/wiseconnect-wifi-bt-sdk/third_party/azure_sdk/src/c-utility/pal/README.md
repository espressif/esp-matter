This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

# azure-c-shared-utility/pal

This directory and its subfolders contain files for adapting various devices and operating
systems to the Azure C Shared Utility.

### Commonly used files
The directories 
* [azure-c-shared-utility/pal](https://github.com/Azure/azure-c-shared-utility/tree/master/pal) and
* [azure-c-shared-utility/pal/inc](https://github.com/Azure/azure-c-shared-utility/tree/master/pal/inc)

contain common files used by multiple devices and operating systems.

### Specialized files

The remainder of the directories under 
[azure-c-shared-utility/pal](https://github.com/Azure/azure-c-shared-utility/tree/master/pal)
such as
* [azure-c-shared-utility/pal/freertos](https://github.com/Azure/azure-c-shared-utility/tree/master/pal/freertos)
* [azure-c-shared-utility/pal/lwip](https://github.com/Azure/azure-c-shared-utility/tree/master/pal/lwip)
* [azure-c-shared-utility/pal/linux](https://github.com/Azure/azure-c-shared-utility/tree/master/pal/linux)

contain files whose use specific to an operating system or other component.

### Porting to new devices

Instructions for porting the Azure IoT C SDK to new devices are located
[here](https://github.com/Azure/azure-c-shared-utility/blob/master/devdoc/porting_guide.md).
