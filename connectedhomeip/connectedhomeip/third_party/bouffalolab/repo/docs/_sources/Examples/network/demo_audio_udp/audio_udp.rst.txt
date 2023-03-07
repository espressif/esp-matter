.. _audio_udp-index:

AUDIO UDP
=========

总览
------

本示例主要介绍使用手机wifi连接602开发板通过UDP传输语音播报收款

使用步骤
-----------

- 编译 ``customer_app/sdk_app_audio_udp`` 工程，勾选烧录软件中的romfs选项，
然后选中处理后的音频文件所在的文件夹并下载工程，如图所示：

    .. figure:: imgs/image1.png
       :alt: 

- 板子启动后会自动开启wifi,名称为“BL60X_uAP_827302”，使用手机连接该wifi，如图所示：

    .. figure:: imgs/image2.png
       :alt: 

- 打开微信小程序，IP地址设置为“192.168.169.1”，端口为“5002”，注意此时可能需要关闭数据流量，
输入需要发送播报的数字，点击“发送即可”，如图所示：

    .. figure:: imgs/image3.png
       :alt: 
