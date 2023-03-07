yloop
=====

--------------

-  `Yloop概要`_

-  `Yloop上下文`_

-  `Yloop调度`_

-  `Yloop实现原理`_

-  `主要api介绍`_

-  `示例代码`_

-  `注意事项`_

Yloop概要
---------

Yloop 是AliOS
Things的异步事件框架。Yloop借鉴了，libuv及嵌入式业界常见的event
loop，综合考虑使用复杂性，性能，及footprint，实现了一个适合于MCU的事件调度机制。我们移植了相关的插件。其主要优势是所有的处理都是在主任务中执行的，不需要额外的创建任务，从而节省内存使用。同时，由于所有处理都是在主任务进行，不需要复杂的互斥操作。

Yloop上下文
-----------

每个Yloop实例（aos\_loop\_t）与特定的任务上下文绑定，AliOS
Things的程序入口application\_start
所在的上下文与系统的主Yloop实例绑定，该上下文也称为主任务。主任务以外的任务也可以创建自己的Yloop实例。

Yloop调度
---------

Yloop实现了对IO，timer，callback，event的统一调度管理：

-  ``IO``\ ：最常见的是Socket，也可以是AliOS Things的vfs管理的设备
-  ``timer``\ ：即常见的定时器
-  ``callback``\ ：特定的执行函数
-  ``event``\ ：包括系统事件，用户自定义事件
   当调用aos\_loop\_run后，当前任务将会等待上述的各类事件发生。

Yloop实现原理
-------------

Yloop利用协议栈的select接口实现了对IO及timer的调度。AliOS
Things自带的协议栈又暴露一个特殊的eventfd接口，Yloop利用此接口把VFS的设备文件，和eventfd关联起来，实现了对整个系统的事件的统一调度。

主要api介绍
-----------

-  注册事件监听函数

.. code:: c

    /**
     * Register system event filter callback.
     *
     @param[in]  type  event type interested.
     * @param[in]  cb    system event callback.
     * @param[in]  priv  private data past to cb.
     *
     * @return  the operation status, 0 is OK, others is error.
     */
    int aos_register_event_filter(uint16_t type, aos_event_cb cb, void *priv);

    /**
     * Unregister native event callback.
     *
     * @param[in]  type  event type interested.
     * @param[in]  cb    system event callback.
     * @param[in]  priv  private data past to cb.
     *
     * @return  the operation status, 0 is OK, others is error.
     */
    int aos_unregister_event_filter(uint16_t type, aos_event_cb cb, void *priv);

-  发布一个 event

.. code:: c

    /**
     * Post local event.
     *
     * @param[in]  type   event type.
     * @param[in]  code   event code.
     * @param[in]  value  event value.
     *
     * @return  the operation status, 0 is OK,others is error.
     */
    int aos_post_event(uint16_t type, uint16_t code, unsigned long  value);

-  注册和取消一个 poll event

.. code:: c

    /**
     * Register a poll event in main loop.
     *
     * @param[in]  fd      poll fd.
     * @param[in]  action  action to be executed.
     * @param[in]  param   private data past to action.
     *
     * @return  the operation status, 0 is OK,others is error.
     */
    int aos_poll_read_fd(int fd, aos_poll_call_t action, void *param);

    /**
     * Cancel a poll event to be executed in main loop.
     *
     * @param[in]  fd      poll fd.
     * @param[in]  action  action to be executed.
     * @param[in]  param   private data past to action.
     */
    void aos_cancel_poll_read_fd(int fd, aos_poll_call_t action, void *param);

-  发布和取消一个延迟执行的 action

.. code:: c

    /**static void adc_cb_read(int fd, void *param)
    {
        aos_post_event(EV_ADCKEY, CODE_ADCKEY_INT_TRIGGER, fd);
    }
     * Post a delayed action to be executed in main loop.
     *
     * @param[in]  ms      milliseconds to wait.
     * @param[in]  action  action to be executed.
     * @param[in]  arg     private data past to action.
     *
     * @return  the operation status, 0 is OK,others is error.
     */
    int aos_post_delayed_action(int ms, aos_call_t action, void *arg);

    /**
     * Cancel a delayed action to be executed in main loop.
     *
     * @param[in]  ms      milliseconds to wait, -1 means don't care.
     * @param[in]  action  action to be executed.
     * @param[in]  arg     private data past to action.
     */
    void aos_cancel_delayed_action(int ms, aos_call_t action, void *arg);

-  安排一次回调

.. code:: c

    /**
     * Schedule a callback in next event loop.
     * Unlike aos_post_delayed_action,
     * this function can be called from non-aos-main-loop context.

     * @param[in]  action  action to be executed.
     * @param[in]  arg     private data past to action.
     *
     * @return  the operation status, <0 is error,others is OK.
     */
    int aos_schedule_call(aos_call_t action, void *arg);

示例代码
--------

这里会介绍\ `事件注册、通知、回调、取消流程 <#事件注册、通知、回调、取消流程>`__\ 、\ `poll事件的注册取消 <#poll事件的注册取消>`__\ 、\ `延迟执行一个actio <#延迟执行一个action>`__)以及\ `安排一次回调 <#安排一次回调>`__\ 的使用方法

事件注册、通知、回调、取消流程
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: c

    aos_register_event_filter(EV_WIFI, event_cb_wifi_event, NULL);

用户首先调用\ ``aos_register_event_filter``\ 注册事件监听函数，例如首先显注册一个\ ``EV_WIFI``\ 事件的监听函数\ ``event_cb_wifi_event``

.. code:: c

    aos_post_event(EV_WIFI, CODE_WIFI_ON_INIT_DONE, 0);

当有任务调用\ ``aos_post_event``\ 接口，发布\ ``CODE_WIFI_ON_INIT_DONE``\ 事件之后

.. code:: c

    static void event_cb_wifi_event(input_event_t *event, void *private_data)
    {
        switch (
            case CODE_WIFI_ON_INIT_DONE:
            {
                printf("[APP] [EVT] CODE_WIFI_ON_INIT_DONE %lld\r\n", aos_now_ms());
            }
            break;
            case CODE_WIFI_ON_PRE_GOT_IP:
            {
                printf("[APP] [EVT] connected %lld\r\n", aos_now_ms());
            }
            break;
            case CODE_WIFI_ON_GOT_IP:
            {
                printf("[APP] [EVT] GOT IP %lld\r\n", aos_now_ms());
            }
            break;
            default:
            {
                /*nothing*/
            }
        }
    }

``event_cb_wifi_event``\ 会被调用，并进入case\ ``CODE_WIFI_ON_INIT_DONE``\ 分支

.. code:: c

    aos_unregister_event_filter(EV_WIFI, event_cb_wifi_event, NULL);

如果用户不需要事件的监听，用户可以主动调用\ ``aos_unregister_event_filter``\ 取消监听

poll事件的注册取消
~~~~~~~~~~~~~~~~~~

.. code:: c

    /*uart*/
    fd_console = aos_open("/dev/ttyS0", 0); 
    if (fd_console >= 0) {
        printf("Init CLI with event Driven\r\n");
        aos_cli_init(0);
        aos_poll_read_fd(fd_console, aos_cli_event_cb_read_get(), (void*)0x12345678);
        _cli_init();
    }   

这里以 ``uart0`` 为例，用户首先注册一个\ ``aos_poll_read_fd``\ poll事件

.. code:: c

    aos_cancel_poll_read_fd(fd_console, action, (void*)0x12345678);

如果用户不需要事件的poll，用户可以调用\ ``aos_cancel_poll_read_fd`` \ 取消poll

延迟执行一个action
~~~~~~~~~~~~~~~~~~

.. code:: c

    aos_post_delayed_action(1000, app_delayed_action_print, NULL);

用户可以调用\ ``aos_post_delayed_action``\ 做一个延迟\ ``1s``\ 执行的事件

.. code:: c

    static void app_delayed_action_print(void *arg)
    {
        printf("test.\r\n");
    }

那过\ ``1s``\ 之后会主动调用\ ``app_delayed_action_print``\ 函数

.. code:: c

    aos_cancel_delayed_action(1000, app_delayed_action_print, NULL);

当用户想直接取消一个延迟动作可以调用\ ``aos_cancel_delayed_action``\ ，其第一个\ ``ms``\ 参数,
当\ ``ms == -1``\ 时，表示无需关心时间是否一致

安排一次回调
~~~~~~~~~~~~

.. code:: c

    aos_schedule_call(app_action_print, NULL);

用户主动调用\ ``aos_schedule_call``\ 函数

.. code:: c

    static app_action_print(void *arg)
    {
        printf("test\r\n");
    }

那么会在下一次循环中主动调用\ ``app_action_print``\ 函数

注意事项
--------

Yloop的API(include/aos/yloop.h)除了下述API，都必须在Yloop实例所绑定的任务的上下文执行：

-  aos\_schedule\_call
-  aos\_loop\_schedule\_call
-  aos\_loop\_schedule\_work
-  aos\_cancel\_work
-  aos\_post\_event

