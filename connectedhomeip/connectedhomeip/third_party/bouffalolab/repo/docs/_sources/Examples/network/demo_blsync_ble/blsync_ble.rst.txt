.. _blsync-ble-index:

BLSYNC-BLE
==================

总览
------

本示例主要介绍如何使用ble进行wifi配网。

APP使用步骤
-------------

- 编译 ``customer_app/sdk_app_ble_sync`` 工程并下载工程固件；
- 固件上电运行会自动开启ble广播，等待手机APP连接配网，如下所示；

    .. figure:: imgs/image1.png
       :alt: 

- 打开手机APP搜索蓝牙设备，搜索到设备名“BL602-BLE-DEV”；

    .. figure:: imgs/image2.png
       :alt: 

- 点击连接设备后，点击APP中的扫描，等待数秒后APP会显示开发板扫描到的wifi设备列表；

    .. figure:: imgs/image3.png
       :alt: 

    .. figure:: imgs/image4.png
       :alt: 

- 用户可以通过扫描出来的设备列表对进行需要配网的wifi进行连接；

    .. figure:: imgs/image5.png
       :alt: 

- 当用户确定配网完成时，不需要再使用配网功能，可以使用“blsync_ble_stop”命令将其关闭。

    .. figure:: imgs/image6.png
       :alt: 

微信小程序使用步骤
---------------------

- 编译 ``customer_app/sdk_app_ble_sync`` 工程并下载工程固件；
- 固件上电运行会自动开启ble广播，等待手机APP连接配网，如下所示；

    .. figure:: imgs/image1.png
       :alt: 

- 打开微信小程序搜索蓝牙设备，搜索到设备名“BL602-BLE-DEV”；

    .. figure:: imgs/image7.png
       :alt: 

- 点击“BL602-BLE-DEV”连接设备，连接上设备BLE后会获取到BLE的服务，点击第一个服务，再选择“写通知“；

    .. figure:: imgs/image8.png
       :alt: 

- 点击小程序中的”点击配网“，小程序会回显获取到的wifi列表，；

    .. figure:: imgs/image9.png
       :alt: 

- 用户可以通过扫描出来的设备列表对进行需要配网的wifi进行连接，点击需要连接的wifi名称；

    .. figure:: imgs/image10.png
       :alt: 

- 接着在输入框输入wifi密码，点击”发送密码“，即可连接wifi；

    .. figure:: imgs/image11.png
       :alt: 

- 点击小程序中的”获取状态“按钮，获取wifi当前的连接状态；

    .. figure:: imgs/image12.png
       :alt: 

- 当前已经连接wifi，则会显示”已经连接“并弹出板子的ip地址等信息；

    .. figure:: imgs/image13.png
       :alt: 

- 点击”断开wifi“按钮，即可断开wifi，再次点击”获取状态“按钮可以获取当前wifi已经断开；

    .. figure:: imgs/image14.png
       :alt: 

- 当用户确定配网完成时，不需要再使用配网功能，可以使用“blsync_ble_stop”命令将其关闭。

    .. figure:: imgs/image6.png
       :alt: 
