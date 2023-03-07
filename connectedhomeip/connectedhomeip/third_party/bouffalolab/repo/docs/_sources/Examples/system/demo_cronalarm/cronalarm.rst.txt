.. _cronalarm-index:

cronalarm
==================

总览
------

本示例主要介绍如何配置cronalarm

使用步骤
----------------

- 使用步骤：
   
  - 编译 ``customer_app/sdk_app_cronalarm`` 工程并下载工程；
  - 在main.c中有相关头文件的使用，main.c中aos_loop_proc 中有cronalarm相关的初始化。使用cronalarm 需要包含头文件cronalarms.h 。


应用实例
---------

- 命令解析
- .. image:: imgs/01.png
   :alt:

::

    cron_alarm_create("0 30 8 * * *", MorningAlarm, 0);
    每日 8:30:0 调用MorningAlarm ;
  
    cron_alarm_create("0 30 8 * * *", MorningAlarm, 1)
    仅在将要到来的8:30:0 调用一次MorningAlarm. ;

    cron_alarm_create("0 15 9 * * 6", WeeklyAlarm, 0)
    每周六的9:15:0  调用WeeklyAlarm。;

    cron_alarm_create("*/15 * * * * *", Repeats, 0);
    每15s调用一次Repeats;

    cron_alarm_create("*/10 * * * * *", OnceOnly, 1);
    在创建之后，时钟的下一个秒达到10的倍数时，将调用afterOnly（）函数。 且仅执行一次。

    create("0 0 12 4 7 *", Celebration, 0);
    每年的 7月4日 12:0:0 调用Celebration函数。

    cron_alarm_create("0 */1 * * * *", test_min02, 0);
    每分钟的 0s 调用test_min02

    cron_alarm_create("15 15 */4 * * *", test_hour02, 0);
    每4小时 15分15秒调用test_hour02

    cron_alarm_create("1-10/1 * * * * *", Repeats, 0);
    每分钟的前10s,每秒钟执行一次Repeats.

    cron_alarm_create("0 0 0 */1 * ?", test_day01, 0);
    每天的 0:0:0 执行一次test_day01

    cron_alarm_create("0 0 0 * * MON,WED,FRI", test_week01, 0); 
    每周的周一，周三，周五，触发一次test_week01

    cron_alarm_create("11 23 1 1 JAN-FEB *", test_month02, 0); 
    每年的一月二月 day1 01:23:11 触发test_month02.

