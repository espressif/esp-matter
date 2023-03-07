This project runs the application on EFM32 platform with SDIO interface in Baremetal environment only.
The folder projects_sdio_efm32 contains two folders
	|- projects_sdio_efm32
	
		|- tls_client
		|- sdio_project_efm32
		
tls_client 
==========
This folder contains application related files.

sdio_project_efm32
==================
This folder contains project configuration files and folders

To import the project to simplicity studio, please follow the below steps:
===========================================================================
1.import the tls_client project, navigate to the “File” menu and "click" on the Import option
2.click “More Import Options”
3.Select “Existing Projects Into Workspace”
4.Click Next
5.In the window that will open after doing so, click on the “Browse” button 
6.In the explorer window that will pop up after pressing on the “Browse” button, navigate to the examples/snippets/wlan/tls_client/projects_sdio_efm32/sdio_project_efm32 project directory.
7.and press "finish".
8.project gets imported to simplicity studio.

NOTE:  If the user imports another example project on the Simplicity Studio  without closing the previous project / previous example, then the user needs to close “project explorer” and import the project using the above steps. Once importing is completed – To view the imported example project in the “project explorer tab” click on “window” -> Show view -> Project explorer.