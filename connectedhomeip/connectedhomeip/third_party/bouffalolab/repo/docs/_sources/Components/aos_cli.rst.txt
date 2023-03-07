cli
===

概述
----

linux下有强大的shell工具，可以让用户和片上系统进行交互，而在传统的单片机系统中，用户往往需要自行实现一套类似的交互工具。AliOS-Things原生带有一套名为cli（command-line
interface）的命令行交互工具，在提供基本的系统交互命令的基础上，也支持用户自定义命令。我们已经将其移植到我们到系统中，
同时对其做出了很多完善， 下面将介绍如何使用cli命令并执行

示例代码
--------

用户在只需在程序中调用\ ``test_cli_init()``\ 接口之后，就可以在shell中\ ``test``,就可以打印\ ``hello world.``\ 如下图所示

::

    #
    #
    # test
    hello world.
    #
    #

还有很多常用命令，请参考\ `常用命令<../helper/helper.html>`_

.. code:: c

    static void cmd_test_func(char *buf, int len, int argc, char **argv)
    {
        printf("hello world.\r\n");
        return;
    }

    const static struct cli_command cmds_user[] = {
        {"test", "it's test func ", cmd_test_func}
    };

    int test_cli_init(void)
    {
        return aos_cli_register_commands(cmds_user, sizeof(cmds_user)/sizeof(cmds_user[0]));
    }

