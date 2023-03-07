.. _helloworld-index:

Helloword
==================

总览
------

该工程介绍将iot板子启动起来并打印相关log

应用实例
----------------
::

    bl_uart_init(0, 16, 7, 255, 255, 2 * 1000 * 1000);
    helloworld();

设置串口波特率，此处设置为2000000bps。在串口终端可以看到打印的 ``start`` ， ``helloworld`` ， ``end`` 。

.. figure:: imgs/image1.png
   :alt: 
