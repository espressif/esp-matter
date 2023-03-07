.. _wifi-index:

WiFi
==================

概述
------

本文档介绍BL_IoT SDK的WiFi联网功能。主要包括

- 基站模式（即 STA 模式或 Wi-Fi 客户端模式），此时 BL_IoT 连接到接入点 (AP)。
- AP 模式（即 Soft-AP 模式或接入点模式），此时基站连接到 BL_IoT。
- AP-STA 共存模式（BL_IoT 既是接入点，同时又作为基站连接到另外一个接入点）。
- 上述模式的各种安全模式（WPA、WPA2 及 WEP（AP模式不支持））。
- 扫描接入点（包括主动扫描及被动扫描）。
- 使用混杂模式监控 IEEE802.11 Wi-Fi 数据包。

应用实例
----------

BL_IoT的实例 ``bl602_demo_wifi`` 目录下包含了一个应用程序，该demo介绍了如何使用BL_IoT模组连接到AP等一系列wifi操作。该实例实现的主要思路如下：

.. figure:: imgs/image1.png
    :alt:

- 在主函数 ``bfl_main()`` 中创建一个 ``aos_loop_proc()`` 线程，在此线程中调用 ``aos_register_event_filter()`` 接口注册一个 ``EV_WIFI`` 事件的监听函数 ``event_cb_wifi_event()`` ；
- 首先用户在终端中输入 ``stack_wifi`` 命令后，在 ``cmd_stack_wifi()`` 函数中创建一个 ``wifi_main()`` 的线程，接着调用 ``aos_post_event()`` 接口发布 ``CODE_WIFI_ON_INIT_DONE`` 事件后， ``event_cb_wifi_event()`` 会被调用，并进入case ``CODE_WIFI_ON_INIT_DONE`` 分支从而调用 ``wifi_mgmr_start_background()`` 开启WiFi Manager，同时发布 ``CODE_WIFI_ON_MGMR_DONE`` 事件，进入case ``CODE_WIFI_ON_MGMR_DONE`` 分支调用 ``_connect_wifi()`` 函数，此函数先判断easyflash中是否设置了可连接的ssid，如果设置了则会自动进行wifi连接，没有设置则需手动输命令设置；
- 如用户在终端中输入 ``wifi_sta_connect`` 命令，会调用相应的api实现。如需实现wifi相关的其他功能可通过调用对应api来实现。



Reason Codes
----------------

``WLAN_FW_SUCCESSFUL``

WiFi连接成功

``WLAN_FW_TX_AUTH_FRAME_ALLOCATE_FAIILURE``

发送验证帧分配失败

``WLAN_FW_AUTHENTICATION_FAIILURE``

验证失败

``WLAN_FW_AUTH_ALGO_FAIILURE``

身份验证响应但身份验证算法失败

``WLAN_FW_TX_ASSOC_FRAME_ALLOCATE_FAIILURE``

发送关联帧分配失败

``WLAN_FW_ASSOCIATE_FAIILURE``

关联错误

``WLAN_FW_DEAUTH_BY_AP_WHEN_NOT_CONNECTION``

AP取消验证，但状态错误

``WLAN_FW_DEAUTH_BY_AP_WHEN_CONNECTION``

连接时由AP取消验证

``WLAN_FW_4WAY_HANDSHAKE_ERROR_PSK_TIMEOUT_FAILURE``

密码错误，四次握手超时

``WLAN_FW_4WAY_HANDSHAKE_TX_DEAUTH_FRAME_TRANSMIT_FAILURE``

密码错误，发送取消验证帧传输失败

``WLAN_FW_4WAY_HANDSHAKE_TX_DEAUTH_FRAME_ALLOCATE_FAIILURE``

密码错误，发送取消验证帧分配失败

``WLAN_FW_TX_AUTH_OR_ASSOC_FRAME_TRANSMIT_FAILURE``

发送授权或关联帧传输失败

``WLAN_FW_SCAN_NO_BSSID_AND_CHANNEL``

SSID错误，扫描不到bssid和频道

``WLAN_FW_CREATE_CHANNEL_CTX_FAILURE_WHEN_JOIN_NETWORK``

加入网络时创建通道上下文失败

``WLAN_FW_JOIN_NETWORK_FAILURE``

加入网络失败

``WLAN_FW_ADD_STA_FAILURE``

加入sta模式失败

``WLAN_FW_BEACON_LOSS``

信标丢失

API参考
----------

- 头文件

`wifi_mgmr_ext.h <../../../../components/bl602/bl602_wifidrv/bl60x_wifi_driver/include/wifi_mgmr_ext.h>`__

- API介绍

``void wifi_mgmr_start_background(wifi_conf_t *conf)``

::

    /**
    * function      初始化并开启WiFi Manager。（wifi相关的操作第一步首先调用此接口）
    *
    * @param[in]    conf    WiFi配置
    *
    * @return       无
    */

``wifi_interface_t wifi_mgmr_sta_enable(void)``

::

    /**
    * function      wifi sta 使能
    *
    * @param[in]    无
    *
    * @return       指向wifi sta的结构体信息的指针
    */


``int wifi_mgmr_sta_disable(wifi_interface_t *interface)``

::

    /**
    * function    wifi sta 失能
    *
    * @param[in]  interface    wifi sta结构体指针
    *
    * @return     0：成功, 其他：失败
    */

``int wifi_mgmr_sta_mac_set(uint8_t mac[6])``

::

    /**
    * function    设置 wifi mac 地址
    *
    * @param[in]  mac[6]    存储mac地址的数组
    *
    * @return     0：成功, 其他：失败
    */

``int wifi_mgmr_sta_mac_get(uint8_t mac[6])``

::

    /**
    * function    获取 wifi mac 地址
    *
    * @param[in]  mac[6]    存储mac地址的数组
    *
    * @return     0：成功, 其他：失败
    */

``int wifi_mgmr_sta_ip_get(uint32_t *ip, uint32_t *gw, uint32_t *mask)``

::

    /**
    * function    获取 wifi ip
    *
    * @param[in]  ip    指向ip的指针
    * @param[in]  gw    指向gateway的指针
    * @param[in]  mask  指向mask的指针
    *
    * @return     0：成功, 其他：失败
    */

``int wifi_mgmr_sta_connect(wifi_interface_t *wifi_interface, char *ssid, char *psk, char *pmk, uint8_t *mac, uint8_t band, uint16_t freq)``

::

    /**
    * function    wifi sta 连接
    *
    * @param[in]  wifi_interface    wifi sta结构体指针（wifi_mgmr_sta_enable的返回值）
    * @param[in]  ssid              wifi名
    * @param[in]  psk               密码
    * @param[in]  pmk               pmk
    * @param[in]  mac               mac地址
    * @param[in]  band              band
    * @param[in]  freq              freq
    * @return     0：成功, 其他：失败
    */

``int wifi_mgmr_sta_disconnect(void)``

::

    /**
    * function    wifi sta 断开（调用此接口后需要TaskDelay 1s左右，再调用wifi_mgmr_sta_disable接口才能实现wifi disconnect）
    *
    * @param[in]  无
    *
    * @return     0：成功, 其他：失败
    */

``wifi_interface_t wifi_mgmr_ap_enable(void)``

::

    /**
    * function    wifi ap模式使能
    *
    * @param[in]  无
    *
    * @return     指向wifi ap的结构体信息的指针
    */

``int wifi_mgmr_ap_start(wifi_interface_t *interface, char *ssid, int md, char *passwd, int channel)``

::

    /**
    * function    开启wifi ap模式
    *
    * @param[in]  interface     wifi ap结构体指针
    * @param[in]  ssid          wifi名
    * @param[in]  md            md
    * @param[in]  passwd        密码
    * @param[in]  channel       wifi信道
    *
    * @return     0：成功, 其他：失败
    */

``int wifi_mgmr_ap_stop(wifi_interface_t *interface)``

::

    /**
    * function    关闭wifi ap模式
    *
    * @param[in]  interface     wifi ap结构体指针
    *
    * @return     0：成功, 其他：失败
    */

``int wifi_mgmr_scan(void *data, scan_complete_cb_t cb)``

::

    /**
    * function    开启wifi扫描
    *
    * @param[in]  data     scan data
    * @param[in]  cb       sacn cb
    *
    * @return     0：成功, 其他：失败
    */

``int wifi_mgmr_sta_autoconnect_enable(void)``

::

    /**
    * function    开启wifi重连
    *
    * @param[in]  无
    *
    * @return     0：成功, 其他：失败
    */

``int wifi_mgmr_sta_autoconnect_disable(void)``

::

    /**
    * function    关闭wifi重连
    *
    * @param[in]  无
    *
    * @return     0：成功, 其他：失败
    */

``int wifi_mgmr_sta_powersaving(int ps)``

::

    /**
    * function    开启wifi sta省电模式
    *
    * @param[in]  ps     0：关闭省电模式
                         1：开启省电模式
                         2：动态切换模式
    *
    * @return     0：成功, 其他：失败
    */

``int wifi_mgmr_ap_sta_cnt_get(uint8_t *sta_cnt)``

::

    /**
    * function    获取ap模式下允许连接的sta个数
    *
    * @param[in]  sta_cnt     sta的个数
    *
    * @return     0：成功, 其他：失败
    */

``int wifi_mgmr_ap_sta_info_get(struct wifi_sta_basic_info *sta_info, uint8_t idx)``

::

    /**
    * function    获取ap模式下sta连接的信息
    *
    * @param[in]  sta_info     存储sta的信息
    * @param[in]  idx          sta编号
    *
    * @return     0：成功, 其他：失败
    */

``int wifi_mgmr_sniffer_enable(void)``

::

    /**
    * function    使能sniffer
    *
    * @param[in]  无
    *
    * @return     0：成功, 其他：失败
    */

``int wifi_mgmr_sniffer_disable(void)``

::

    /**
    * function    失能sniffer
    *
    * @param[in]  无
    *
    * @return     0：成功, 其他：失败
    */

``int wifi_mgmr_sniffer_register(void *env, sniffer_cb_t cb)``

::

    /**
    * function    注册sniffer（注册之前需enable sniffer）
    *
    * @param[in]  env    env
    * @param[in]  cb     sniffer回调函数
    *
    * @return     0：成功, 其他：失败
    */

``int wifi_mgmr_sniffer_unregister(void *env)``

::

    /**
    * function    注销sniffer（注销之前需disable sniffer）
    *
    * @param[in]  env    env
    *
    * @return     0：成功, 其他：失败
    */
