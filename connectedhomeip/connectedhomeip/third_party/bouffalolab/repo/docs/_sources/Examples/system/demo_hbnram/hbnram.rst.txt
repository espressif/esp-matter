.. _hbnram-index:

HBNRAM
==================

总览
------

本示例主要介绍如何使用hbnram的api

使用步骤
-----------

1.首先进行初始化.调用hal_hbnram_init(void)，进行初始化hbnram。如果是断电重启，则会清空hbnram,如果是不断点的reboot,则仅校验数据。
2.hal_hbnram_alloc(const char *key, int len),key 必须是const char 类型的字符串，且必须小于4个字符。len是要申请的内存的长度。
3.hal_hbnram_buffer_set(const char *key, uint8_t *buf, int length)；把数据一次写入申请的buff.
4.hal_hbnram_buffer_get(const char *key, uint8_t *buf, int length)；把数据一次从buff读出
5.hal_hbnram_handle_get_fromkey(const char *key, hbnram_handle_t *handle);如果需要流读写，需要首先根据key，获取hanlde.
6.hal_hbnram_copy_from_stream(hbnram_handle_t *handle, uint8_t *buf, int len);流读出
7.hal_hbnram_copy_to_stream(hbnram_handle_t *handle, uint8_t *buf, int len); 流写入

具体使用方式参考 sdk_app_hbnram下的demo.c 

 
应用实例:
将工程sdk_app_hbnram 编译，烧写进入板子。重启板子。 
