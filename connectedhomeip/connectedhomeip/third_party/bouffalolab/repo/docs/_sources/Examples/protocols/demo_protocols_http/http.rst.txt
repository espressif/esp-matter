.. _http-index:

Http client
==================

总览
------

本示例主要介绍如何创建一个socket并连接到指定的Http服务器获取数据的过程。

准备和使用步骤
----------------

- 使用之前需要准备可用的URL以及网络。

- 使用步骤：
   
  - 编译 ``customer_app/sdk_app_http_client_socket`` 工程并下载工程；
  - 在终端输入开启wifi的命令： ``stack_wifi`` ，输入连接wifi的命令： ``wifi_sta_connect <name> <key>`` ，并确认wifi连接成功（如输入：wifi_sta_connect bl_test_005 12345678）；

    .. figure:: imgs/image1.png
       :alt: 

    .. figure:: imgs/image2.png
       :alt: 


  - 使用 ``http`` 命令通过socket的方式进行下载。

    .. figure:: imgs/image3.png
       :alt: 


应用实例
---------

- 通过 ``hostname`` 获取 ``hostinfo``

::

    struct hostent *hostinfo = gethostbyname(hostname);
    if (!hostinfo) {
        printf("gethostbyname Failed\r\n");
        return -1;
    }
  
- 创建一个 ``socket`` 连接并发送http请求。

::

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error in socket\r\n");
        return -1;
    }
    /*---Initialize server address/port struct---*/
    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(PORT);
    dest.sin_addr = *((struct in_addr *) hostinfo->h_addr);
    //char ip[16];
    uint32_t address = dest.sin_addr.s_addr;
    char *ip = inet_ntoa(address);

    printf("Server ip Address : %s\r\n", ip);
    /*---Connect to server---*/
    if (connect(sockfd,
             (struct sockaddr *)&dest,
             sizeof(dest)) != 0) {
        printf("Error in connect\r\n");
        return -1;
    }
    /*---Get "Hello?"---*/
    memset(buffer, 0, MAXBUF);
    char wbuf[]
        = "GET /ddm/ContentResource/music/204.mp3 HTTP/1.1\r\nHost: nf.cr.dandanman.com\r\nUser-Agent: wmsdk\r\nAccept: */*\r\n\r\n";
    write(sockfd, wbuf, sizeof(wbuf) - 1);

- 获取http响应的数据，完成时打印获取数据花费的时间和传输的速度。

::

    while (1) {
        ret = read(sockfd, recv_buffer, BUFFER_SIZE);
        if (ret == 0) {
            printf("eof\n\r");
            break;
        } else if (ret < 0) {
            printf("ret = %d, err = %d\n\r", ret, errno);
            break;
        } else {
            total += ret;
            /*use less debug*/
            if (0 == ((debug_counter++) & 0xFF)) {
                printf("total = %d, ret = %d\n\r", total, ret);
            }
            //vTaskDelay(2);
            if (total > 82050000) {
                ticks_end = xTaskGetTickCount();
                time_consumed = ((uint32_t)(((int32_t)ticks_end) - ((int32_t)ticks_start))) / 1000;
                printf("Download comlete, total time %u s, speed %u Kbps\r\n",
                        (unsigned int)time_consumed,
                        (unsigned int)(total / time_consumed * 8 / 1000)
                );
                break;
            }
        }

- 关闭 ``socket`` 。

:: 

    close(sockfd);

- 在 ``customer_app/sdk_app_http_client_socket/sdk_app_http_client_socket/demo.c`` 中 ``static void _cli_init()`` 的函数里调用 ``http_client_cli_init()`` 初始化http命令
