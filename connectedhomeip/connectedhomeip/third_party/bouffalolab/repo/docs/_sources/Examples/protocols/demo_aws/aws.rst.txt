.. _aws-index:

aws
==================

总览
------

本示例主要介绍如何使用aws。

使用步骤
-----------

- 在 ``customer_app/bl602_demo_ble_pds/bl602_demo_ble_pds/aws_iot_main.c`` 文件的头部有相关证书及配置，此处的配置只限于本示例演示使用（同时只能一处使用），用户需要根据实际情况修改文件开头的宏定义，下图是配置的一部分

    .. figure:: imgs/image1.png
       :alt: 

- 编译 ``customer_app/bl602_demo_event`` 工程并下载工程；
- 在终端输入开启wifi的命令： ``stack_wifi`` ，输入连接wifi的命令： ``wifi_sta_connect <name> <key>`` ，并确认wifi连接成功（如输入：wifi_sta_connect bl_test_005 12345678）；

    .. figure:: imgs/image2.png
       :alt: 

    .. figure:: imgs/image3.png
       :alt: 
- 在终端输入 ``aws`` 不断打印如下图所示log即表明连接成功。

    .. figure:: imgs/image4.png
       :alt: 

