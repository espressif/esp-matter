#ifndef _AWS_LIB_H_
#define _AWS_LIB_H_

/* auth type */
enum AWS_AUTH_TYPE {
	AWS_AUTH_TYPE_OPEN,
	AWS_AUTH_TYPE_SHARED,
	AWS_AUTH_TYPE_WPAPSK,
	AWS_AUTH_TYPE_WPA8021X,
	AWS_AUTH_TYPE_WPA2PSK,
	AWS_AUTH_TYPE_WPA28021X,
	AWS_AUTH_TYPE_WPAPSKWPA2PSK,
	AWS_AUTH_TYPE_MAX = AWS_AUTH_TYPE_WPAPSKWPA2PSK,
	AWS_AUTH_TYPE_INVALID = 0xff,
};

/* encry type */
enum AWS_ENC_TYPE {
	AWS_ENC_TYPE_NONE,
	AWS_ENC_TYPE_WEP,
	AWS_ENC_TYPE_TKIP,
	AWS_ENC_TYPE_AES,
	AWS_ENC_TYPE_TKIPAES,
	AWS_ENC_TYPE_MAX = AWS_ENC_TYPE_TKIPAES,
	AWS_ENC_TYPE_INVALID = 0xff,
};

/* link type */
enum AWS_LINK_TYPE {
	AWS_LINK_TYPE_NONE,
	AWS_LINK_TYPE_PRISM,
	AWS_LINK_TYPE_80211_RADIO,
	AWS_LINK_TYPE_80211_RADIO_AVS
};

//用于打印aws配网库版本
const char *aws_version(void);

//将monitor模式下抓到的包传入该函数进行处理
//参数：
//	buf: frame buffer
//	length: frame len
//	link_type: see enum AWS_LINK_TYPE
//	with_fcs: frame include 80211 fcs field, the tailing 4bytes
//
//说明：
//	适配前执行以下命令, 检查link_type和with_fcs参数
//	a) iwconfig wlan0 mode monitor	#进入monitor模式
//	b) iwconfig wlan0 channel 6	#切换到信道6(以路由器信道为准)
//	c) tcpdump -i wlan0 -s0 -w file.pacp	#抓包保存文件
//	d) 用wireshark或者omnipeek打开，检查包头格式，及包尾是否包含FCS 4字节
//
//	常见的包头类型为：
//	无额外的包头：AWS_LINK_TYPE_NONE
//	radio header: hdr_len = *(unsigned short *)(buf + 2)
//	avs header: hdr_len = *(unsigned long *)(buf + 4)
//	prism header: hdr_len = 144
//
void aws_80211_frame_handler(char *buf, int length,
		enum AWS_LINK_TYPE link_type, int with_fcs);

//启动一键配网服务, 该函数会block，直到配网成功或者超时退出,
//	超时时间由aws_timeout_period_ms设置
//参数：
//	model: 产品model, 如
//	secret: 产品secret, 如
//	mac: 产品mac地址，如11:22:33:44:55:66
//	sn: 产品sn条码，通常填NULL
void aws_start(char *model, char *secret, char *mac, char *sn);
//{该函数大致流程如下:
//	init();
//	vendor_monitor_open();
//	aws_main_thread_func();
//	vendor_monitor_close();
//	destroy();
//}

//aws_start返回后，调用该函数，获取ssid和passwd等信息
//aws成功时，ssid & passwd一定会返回非NULL字符串, 但bssid和auth, encry, channel
//	有可能会返回NULL或者INVALID值(取决于是否能在wifi列表里搜索命中)
//aws失败超时后，该函数会返回0, 且所有参数为NULL或INVALID VALUE
//
//auth defined by enum AWS_AUTH_TYPE
//encry defined by enum AWS_ENC_TYPE
//
//返回值：1--成功，0--失败
int aws_get_ssid_passwd(char *ssid, char *passwd, char *bssid,
	char *auth, char *encry, char *channel);

//发送广播通知APP，配网成功。
//默认会广播2min, 广播过程中收到APP应答后，提前终止
void aws_notify_app(void);

//配网结束（成功或失败）后，调用该函数，释放配网库占用的资源
void aws_destroy(void);
#endif
