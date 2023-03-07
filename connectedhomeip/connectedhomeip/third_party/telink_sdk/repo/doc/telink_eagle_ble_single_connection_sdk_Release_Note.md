## V3.1.0

### Features
* SDK version：telink_eagle_ble_single_connection_sdk_v3.1.0.
* Driver version:  B91_Driver_Demo V1.2.0-Beta.
* This version sdk support B91 A0/A1 chip.
* Add suspend sleep mode and long sleep mode.
* Add more feature demo in B91_feature.
  +  EMI/STUCK_KEY/IR/IR Learn/OTA/Audio solution integration. 
* Add Low Battery Check demo.
* Add Public Key Engine(PKE) Standard Performance acceleration module for ECC.
* Demo illustration.
  +  BLE slave demo：B91_ble_sample 
  +  BLE feature demo：B91_feature
  +  BLE module demo：B91_module 


### Bug Fixes
* Fix flash_write_page error operating across the page.


### BREAKING CHANGES
* ATT write callback should add unsigned short connHandle.



### Features
* SDK 版本：telink_eagle_ble_single_connection_sdk_v3.1.0.
* Driver 版本:  B91_Driver_Demo V1.2.0-Beta.
* 此版本SDK支持B91 A0/A1芯片.
* 增加suspend睡眠模式和长睡眠模式.
* 增加测试Demo在B91_feature工程.
  +  EMI/STUCK_KEY/IR/IR_Learn/OTA/Audio solution integration. 
* 增加低压检测功能.
* 增加PKE性能加速硬件模块用于ECC.
* Demo illustration.
  +  BLE slave demo：B91_ble_sample 
  +  BLE feature demo：B91_feature
  +  BLE module demo：B91_module 


### Bug Fixes
* 修复flash_write_page跨页写操作的错误.


### BREAKING CHANGES
* ATT写回调函数需要传入connHandle变量.