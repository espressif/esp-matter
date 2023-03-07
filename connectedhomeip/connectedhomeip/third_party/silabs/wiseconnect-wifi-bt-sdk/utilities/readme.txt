1. Description  of "ble_provisioning_apps" folder 
===========================================================================================================
   1) android_based_provisioning_app- 
      It consists of Silabs_connect.apk application installable on Android smart phone for testing WLAN BLE provisoing applcations in Snippets.  
   2) windows_based_provisioning_app - 
      It consists Widnows based application software to run on Windows PC for testing WLAN BLE provisoing applcations in Snippets.

2. Description of "Python" folder :
===========================================================================================================
   load_certificate.py : Script to load the Server/Client certificates and keys onto the module.
   load_certificate_with_inx.py : Script to load the Server/Client certificates along with index onto the module.

   USAGE :
   python load_certificate.py <certificate_type> <certificate_path>
   python load_certificate_with_inx.py <certificate_type> <certificate_index> <certificate_path>

3. Description of "usb_cdc" folder
===========================================================================================================
   rules : rules file for linux PC to recognize RS9116
   
4. Description of "advanced_logging" folder
===========================================================================================================
   "advanced_logs_generator.py", "log_decoder.py" are the python files required for the Logging enhancement support.

