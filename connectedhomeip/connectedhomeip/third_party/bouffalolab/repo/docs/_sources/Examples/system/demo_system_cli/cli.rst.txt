cli
===

总览
------

本实例主要介绍如何增加一个cli（command-line interface）。

使用步骤
----------

- 编译 ``customer_app/sdk_app_cli`` 工程并下载对应的bin文件。
- 使用 ``test`` 命令即可在终端看到打印的 ``hello world``。

应用实例
--------

.. code:: c

    static void cmd_cli(char *buf, int len, int argc, char **argv)
    {
        printf("hello world\r\n");
    }

    const static struct cli_command cmds_user[] STATIC_CLI_CMD_ATTRIBUTE = {
        {"test", "cli test", cmd_cli}, 
    };

    int test_cli_init(void)
    {
        // static command(s) do NOT need to call aos_cli_register_command(s) to register.
        // However, calling aos_cli_register_command(s) here is OK but is of no effect as cmds_user are included in cmds list.
        // XXX NOTE: Calling this *empty* function is necessary to make cmds_user in this file to be kept in the final link.
        //return aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));
        return 0;
    }     

用户在只需在程序中调用\ ``test_cli_init()``\ 接口之后，就可以在shell中\ ``test``,就可以打印\ ``hello world.``\ 如下所示：

::

    #
    #
    # test
    hello world.
    #
    #


