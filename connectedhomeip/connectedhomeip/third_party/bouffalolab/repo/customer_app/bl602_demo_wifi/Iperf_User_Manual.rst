==============
iperf测试准备
==============
1. Window PC安装iperf工具：

\ `Iperf下载链接 <https://iperf.fr/iperf-download.php#windows>`__\ ，（下载的2.0.9版本）下载完之后解压得到\ ``iperf-2.0.9-win64``\文件，使用快捷键\ ``WIN + R``\ ，启动运行窗口，输入\ ``cmd``\，点击确定按钮，进入\ ``iperf``\工具所在的目录（本示例\ ``iperf``\工具放在c盘的根目录下）。

.. figure:: picture/image1.png
   :align: center

   解压后的文件

.. figure:: picture/image2.png
   :align: center

   cmd界面

2. PC与路由器通过有线连接

3. 烧录：烧录前硬件模块的相关引脚连接如下图所示，其中图一是模块的正面图，其标号1处用跳线帽短接，标号2处将左边两根排针短接，标号3处将上面的两根排针短接；图二是模块的背面图，烧录时将IO8和HI两根排针短接，烧录完成后将IO8和LOW两根排针短接并重新上电。

.. figure:: picture/image3.png
   :align: center

   模块正面

.. figure:: picture/image4.png
   :align: center

   模块背面

完成硬件连接后，打开烧写工具\ ``Bouffalo Lab Dev Cube``\中的\ ``BLFlashEnv.exe``\，\ ``chip type``\选择\ ``BL602/604``\，打开后设置界面参数，配置完后点击\ ``Download``\，配置及下载完成效果如下图所示：

.. figure:: picture/image5.png
   :align: center

   烧写工具界面


4. 串口工具\ ``putty``\的使用：

\ `下载链接 <https://www.chiark.greenend.org.uk/~sgtatham/putty/latest.html>`__\

.. figure:: picture/image7.png
   :align: center

   putty下载界面

将BL602模组用串口线与电脑连接，右击我的电脑->管理->设备管理器->端口，查看端口号，一般选择较小的端口号用来配置\ ``putty``\。

.. figure:: picture/image8.png
   :align: center

   查看串口号

打开\ ``putty``\工具，设置对应的端口号，波特率设定为2000000 bps。在putty中输入“reboot”命令重启模块（IO8和LOW两根排针短接)。

.. figure:: picture/image9.png
   :align: center

   putty配置

==================
ipu/UDP Tx测试
==================
bl602作为\ ``client``\，PC作为\ ``server``\

1. router ssid: \ ``bl_test_008``\，passwd: \ ``12345678``\
2. 在PC 的cmd界面运行命令：\ ``$iperf.exe -s -u -i 1``\

.. figure:: picture/image10.png
   :align: center

   PC端Iperf开启sever模式

3. 在putty中运行命令：

   - \ ``#wifi_sta_connect bl_test_008 12345678``\   (连接成功后会获取IP地址)

   .. figure:: picture/image11.png
      :align: center

      模块成功连接WiFi


   - \ ``#ipu 192.168.8.101``\  (192.168.8.101是PC的IP地址)

   .. figure:: picture/image12.png
      :align: center

      模块开启ipu

   .. figure:: picture/image13.png
      :align: center

      Sever端数据


===============
ipc/TCP Tx测试
===============
bl602作为\ ``client``\，PC作为\ ``server``\

1. router ssid: \ ``bl_test_008``\，passwd: \ ``12345678``\
2. PC运行命令： \ ``$iperf -s -i 1``\
3. 启动bl602模组，运行命令：

   - \ ``#wifi_sta_connect bl_test_008 12345678``\   (连接成功后会获取IP地址)
   - \ ``#ipc 192.168.8.101``\  (192.168.8.101是PC的IP地址)

===================
ips/TCP Rx测试
===================
bl602作为\ ``server``\，PC作为\ ``client``\

1. router ssid: \ ``bl_test_008``\，passwd: \ ``12345678``\
2. 启动\ ``bl602``\模组，运行命令：

   - \ ``#wifi_sta_connect bl_test_008 12345678``\   (连接成功后会获取IP地址)
   - \ ``#ips``\
3. PC运行命令：\ ``$iperf.exe -c 192.168.8.100 -t 360 -i 1``\   (192.168.8.100是模组的IP地址)

=====================
ipus/UDP Rx测试
=====================
bl602作为\ ``server``\，PC作为\ ``client``\

1. router ssid: \ ``bl_test_008``\，passwd: \ ``12345678``\
2. 启动\ ``bl602``\模组，运行命令：

   - \ ``#wifi_sta_connect bl_test_008 12345678``\   (连接成功后会获取IP地址)
   - \ ``#ipus``\
3. PC运行命令：\ ``$iperf.exe -u -c 192.168.8.100 -t 360 -i 1``\   (192.168.8.100是模组的IP地址)