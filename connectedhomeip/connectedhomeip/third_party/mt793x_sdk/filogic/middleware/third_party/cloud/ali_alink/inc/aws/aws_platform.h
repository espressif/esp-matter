#ifndef _AWS_PLATFORM_H_
#define _AWS_PLATFORM_H_
/*
 * platform porting API
 */

//一键配置超时时间, 建议超时时间1-3min, APP侧一键配置1min超时
extern int aws_timeout_period_ms;

//一键配置每个信道停留时间, 建议200ms-400ms
extern int aws_chn_scanning_period_ms;

//系统自boot起来后的时间, 用于判断收包之间的间隔时间
unsigned int vendor_get_time_ms(void);

//aws库会调用该函数用于信道切换之间的sleep
void vendor_msleep(int ms);

//系统malloc/free函数
void *vendor_malloc(int size);
void vendor_free(void *ptr);

//系统打印函数, 可不实现
void vendor_printf(int log_level, const char* log_tag, const char* file,
	const char* fun, int line, const char* fmt, ...);

char *vendor_get_model(void);
char *vendor_get_secret(void);
char *vendor_get_mac(void);
char *vendor_get_sn(void);
int vendor_alink_version(void);

//aws库调用该函数来接收80211无线包
//若平台上通过注册回调函数aws_recv_80211_frame()来收包时，
//将该函数填为vendor_msleep(100)
int vendor_recv_80211_frame(void);

//进入monitor模式, 并做好一些准备工作，如
//设置wifi工作在默认信道6
//若是linux平台，初始化socket句柄，绑定网卡，准备收包
//若是rtos的平台，注册收包回调函数aws_recv_80211_frame()到系统接口
void vendor_monitor_open(void);

//退出monitor模式，回到station模式, 其他资源回收
void vendor_monitor_close(void);

//wifi信道切换，信道1-13
void vendor_channel_switch(char primary_channel,char secondary_channel,
		char bssid[6]);

//通过以下函数发送配网成功通知给APP, 端口定义如下
#define UDP_TX_PORT			(65123)
#define UDP_RX_PORT			(65126)
int vendor_broadcast_notification(char *msg, int msg_num);

#endif
