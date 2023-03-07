.. _wifi-index:

WiFi
==================

总览
------

本示例主要介绍与wifi相关命令的功能。

使用步骤
----------------

- 编译 ``customer_app/bl602_demo_wifi`` 工程并下载对应的bin文件；
- 成功启动IoT Board之后在终端输入命令 ``stack_wifi`` 创建wifi任务。

.. figure:: imgs/image1.png
   :alt: 

功能
----------

- 连接wifi：

命令： ``wifi_sta_connect <ssid> <pwd>`` ，如在终端输入 ``wifi_sta_connect bl_test_005 12345678`` ，成功连接wifi会打印wifi和memory相关信息，如下图中可以看到wifi的IP，MASK等信息，memory剩余大小信息 。

.. figure:: imgs/image2.png
   :alt: 

- 断开wifi：

命令： ``wifi_sta_disconnect`` ，成功断开连接后会打印断开连接的log。

.. figure:: imgs/image3.png
   :alt: 

- 扫描wifi：

命令： ``wifi_scan`` ，扫描成功后会打印scan list，用户可以看到成功搜索的wifi信息。

.. figure:: imgs/image4.png
   :alt: 

- 开启ap模式：

命令： ``wifi_ap_start`` ，成功开启后打印ssid，pwd，channel等相关信息，并且可以在手机上搜索到相应名字的wifi。

.. figure:: imgs/image5.png
   :alt: 

- 关闭ap模式：

命令： ``wifi_ap_stop`` 。成功关闭ap模式后手机无法搜索到对应wifi。

.. figure:: imgs/image6.png
   :alt: 


