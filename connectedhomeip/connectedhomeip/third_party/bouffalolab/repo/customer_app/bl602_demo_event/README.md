# Current 测试 

如何使用此例程请参考目录app/bl602_demo_event下的[BL602模组基本功耗测量方法](BL602模组基本功耗测量方法.pdf)。

# Iperf测试

如何使用此例程请参考目录app/bl602_demo_event下的[Iperf_User_Manual](Iperf_User_Manual.pdf)。

# Ble编译脚本说明
genblecontroller: build Wi-Fi and ble controller. Using uart hci cmd to communicate with ble controller. 
genblehogp:       build Wi-Fi and BLE. BLE as slave, and enable HOGP service.
genblem0s1:       build Wi-Fi and BLE. 1 BLE connection is supported, BL602 can only be slave in this connection.
genblem0s1s:      build Wi-Fi and BLE. based on genblem0s1, add BLE scan feature.
genblemesh:       build Wi-Fi and BLE mesh. mesh application without mesh model code.
genblemeshmodel:  build Wi-Fi and BLE mesh. mesh application with mesh model code.
genromap:         build Wi-Fi and BLE. BLE support all roles, 2 BLE connection is supported, and enable tp service.

