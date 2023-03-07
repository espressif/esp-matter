.. _httpc-index:

Httpc client
==================

总览
------

本示例主要介绍如何通过tcp的方式访问Http服务器获取数据的过程。

准备和使用步骤
----------------

- 使用之前需要准备可用的URL以及网络。

- 使用步骤：
    
  - 在终端输入开启wifi的命令： ``stack_wifi`` ，会打印下图一log（部分），然后输入连接wifi的命令： ``wifi_sta_connect <name> <key>`` ，并确认wifi连接成功（如输入：wifi_sta_connect bl_test_005 12345678）；

    .. figure:: imgs/image1.png
       :alt: 

    .. figure:: imgs/image2.png
       :alt: 

  - 使用 ``httpc`` 命令进行下载。
    
    .. figure:: imgs/image3.png
       :alt: 

应用实例
---------

- 主要实现过程

::    
    
    settings.use_proxy = 0;
    settings.result_fn = cb_httpc_result;
    settings.headers_done_fn = cb_httpc_headers_done_fn;
    httpc_get_file_dns(
            "nf.cr.dandanman.com",
            80,
            "/ddm/ContentResource/music/204.mp3",
            &settings,
            cb_altcp_recv_fn,
            &req,
            &req
   );

cb_httpc_result()次回调函数会在http传输完成后调用，打印获取的消息内容长度；cb_httpc_headers_done_fn()回调函数会在接收到http headers时调用，打印headers的大小；用户可以在cb_altcp_recv_fn()回调函数中处理接收到的消息。