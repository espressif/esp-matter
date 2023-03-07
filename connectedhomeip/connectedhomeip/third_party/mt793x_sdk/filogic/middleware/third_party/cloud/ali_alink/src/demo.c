#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alink_export.h>
#include <platform_porting.h>
#include "smt_conn.h"
#define DEV_VERSION "1.0.0;APP2.0;OTA1.0"


#define TEST_ALINK_CERT
#ifdef TEST_ALINK_CERT
#define TEST_IOT
#endif
	
void * func2(void * arg);

void * ssl_conn = NULL;
int remote_fd = -1 ;
#ifdef TEST_ALINK_CERT
#ifdef TEST_IOT
char ssl_request[] = "GET /checkpreload.htm HTTP/1.1\r\nAccept: */*\r\nAccept-Language: zh-cn\r\n\
User-Agent: Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)\r\n\
Host: iot.aliyun.com:443\r\nConnection: Close\r\n\r\n";
#else
char ssl_request[] = "GET /checkpreload.htm HTTP/1.1\r\nAccept: */*\r\nAccept-Language: zh-cn\r\n\
User-Agent: Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)\r\n\
Host: alink.tcp.aliyun.com:443\r\nConnection: Close\r\n\r\n";
#endif

#else
char ssl_request[] = "GET www.baidu.com HTTP/1.1\r\nHost: /\r\n\r\n";
#endif


//static const char global_ca_str[] ="-----BEGIN CERTIFICATE-----MIICjjCCAfcCCQCCfnwiKXV0bDANBgkqhkiG9w0BAQUFADBRMQswCQYDVQQGEwJDTjERMA8GA1UECAwIWmhlamlhbmcxETAPBgNVBAcMCEhhbmd6aG91MRwwGgYDVQQKDBNEZWZhdWx0IENvbXBhbnkgTHRkMB4XDTE2MDIxNjA2MzUwNFoXDTE4MDIxNTA2MzUwNFowQjELMAkGA1UEBhMCQ04xETAPBgNVBAgMCFpoZWppYW5nMREwDwYDVQQHDAhIYW5nemhvdTENMAsGA1UECgwEdGVzdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKZziXoq5qaCt1ZXT/L0nv04GkasbSUBUS+VuWHHfJr621zMd/JOthyB/Ju5slSJTYa6x9eh6ZOYaWsTuwg8hNTDmWWUCU59PcqejS6Kuk+ANNhvRdc4wlOxfu7GhwCkB+VAW6EUta62CxunVoyiQ5RD/QHCGflsos/G/OgCAMcxFHFzEYLhvh1fMc08gV1xNCny9Gtsv/nho56HBrIx+76soBkvR4aMX9eIhfY3OEJz9xwjMXCw8P/TnfCuNenQmz2TDh4snbV5OMOBjkV/tLgDO37vq4jAe1Kc6UyKiK2v9MQpNndX+umi0iuGB7CHrVjSGoM/kV6uakdnKZMz2fUCAwEAATANBgkqhkiG9w0BAQUFAAOBgQBUAQ2OHkItOdd+4hzUKUHcEATKG89L4ChDQT3LHa2mJBqqXx9psElpVFgAHrEwV8ykJUav5xCY12Rkz3uqsN0qK/5+0pR+cjbZUOnAi2f30vtrOhvxtEjywPwhPrHksGGY/uqZV+hjpUxThdedoxmiBLW6ZwM7c1w/fqt6URMyBw==-----END CERTIFICATE-----";

#define ALINK_CERT_TEST_NUM	2

#if ALINK_CERT_TEST_NUM == 1
#define ALINK_CERT1 \
"-----BEGIN CERTIFICATE-----\r\n" \
"MIIE0zCCA7ugAwIBAgIQGNrRniZ96LtKIVjNzGs7SjANBgkqhkiG9w0BAQUFADCB\r\n" \
"yjELMAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQL\r\n" \
"ExZWZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJp\r\n" \
"U2lnbiwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxW\r\n" \
"ZXJpU2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0\r\n" \
"aG9yaXR5IC0gRzUwHhcNMDYxMTA4MDAwMDAwWhcNMzYwNzE2MjM1OTU5WjCByjEL\r\n" \
"MAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQLExZW\r\n" \
"ZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJpU2ln\r\n" \
"biwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxWZXJp\r\n" \
"U2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0aG9y\r\n" \
"aXR5IC0gRzUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCvJAgIKXo1\r\n" \
"nmAMqudLO07cfLw8RRy7K+D+KQL5VwijZIUVJ/XxrcgxiV0i6CqqpkKzj/i5Vbex\r\n" \
"t0uz/o9+B1fs70PbZmIVYc9gDaTY3vjgw2IIPVQT60nKWVSFJuUrjxuf6/WhkcIz\r\n" \
"SdhDY2pSS9KP6HBRTdGJaXvHcPaz3BJ023tdS1bTlr8Vd6Gw9KIl8q8ckmcY5fQG\r\n" \
"BO+QueQA5N06tRn/Arr0PO7gi+s3i+z016zy9vA9r911kTMZHRxAy3QkGSGT2RT+\r\n" \
"rCpSx4/VBEnkjWNHiDxpg8v+R70rfk/Fla4OndTRQ8Bnc+MUCH7lP59zuDMKz10/\r\n" \
"NIeWiu5T6CUVAgMBAAGjgbIwga8wDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8E\r\n" \
"BAMCAQYwbQYIKwYBBQUHAQwEYTBfoV2gWzBZMFcwVRYJaW1hZ2UvZ2lmMCEwHzAH\r\n" \
"BgUrDgMCGgQUj+XTGoasjY5rw8+AatRIGCx7GS4wJRYjaHR0cDovL2xvZ28udmVy\r\n" \
"aXNpZ24uY29tL3ZzbG9nby5naWYwHQYDVR0OBBYEFH/TZafC3ey78DAJ80M5+gKv\r\n" \
"MzEzMA0GCSqGSIb3DQEBBQUAA4IBAQCTJEowX2LP2BqYLz3q3JktvXf2pXkiOOzE\r\n" \
"p6B4Eq1iDkVwZMXnl2YtmAl+X6/WzChl8gGqCBpH3vn5fJJaCGkgDdk+bW48DW7Y\r\n" \
"5gaRQBi5+MHt39tBquCWIMnNZBU4gcmU7qKEKQsTb47bDN0lAtukixlE0kF6BWlK\r\n" \
"WE9gyn6CagsCqiUXObXbf+eEZSqVir2G3l6BFoMtEMze/aiCKm0oHw0LxOXnGiYZ\r\n" \
"4fQRbxC1lfznQgUy286dUV4otp6F01vvpX1FQHKOtw5rDgb7MzVIcbidJ4vEZV8N\r\n" \
"hnacRHr2lVz2XTIIM6RUthg/aFzyQkqFOFSDX9HoLPKsEdao7WNq\r\n" \
"-----END CERTIFICATE-----\r\n";
const char alink_cert1[] = ALINK_CERT1;

#elif ALINK_CERT_TEST_NUM == 2
#define ALINK_CERT2 \
"-----BEGIN CERTIFICATE-----\r\n"\
"MIIDtzCCAp+gAwIBAgIJAOxbLdldR1+SMA0GCSqGSIb3DQEBBQUAMHIxCzAJBgNV\r\n" \
"BAYTAkNOMREwDwYDVQQIDAh6aGVqaWFuZzERMA8GA1UEBwwIaGFuZ3pob3UxEDAO\r\n" \
"BgNVBAoMB2FsaWJhYmExDjAMBgNVBAsMBXl1bm9zMRswGQYDVQQDDBIqLnNtYXJ0\r\n" \
"LmFsaXl1bi5jb20wHhcNMTQwOTE3MDI0OTM0WhcNMjQwOTE0MDI0OTM0WjByMQsw\r\n" \
"CQYDVQQGEwJDTjERMA8GA1UECAwIemhlamlhbmcxETAPBgNVBAcMCGhhbmd6aG91\r\n" \
"MRAwDgYDVQQKDAdhbGliYWJhMQ4wDAYDVQQLDAV5dW5vczEbMBkGA1UEAwwSKi5z\r\n" \
"bWFydC5hbGl5dW4uY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA\r\n" \
"pwFEj4onz7YZ0ESpG7BNZmuK4KJgGMlFHnEL3AT1YtvB7DGePTNsW9hX3WYsaqy7\r\n" \
"+4PyzJgfNJY3WQr6uPv/EMwqlYMO0F8kg9AmFepuicHh5JvCeTJciG7OH/8qHhEB\r\n" \
"3b3w35un5YxUXuffw3SiFTj+vnFdc3Yj9pBv++0nsDvl6l8TSkJMnRLY8lRzzi1T\r\n" \
"rbdsDeNXLnfeThElMPFeI1h+s7amt2ktBGnv6HAg7a9OehUI8uTpFZ7559Yf8Dpm\r\n" \
"MDijYc6LLLSE6OO5C7im0pg8IRu6oZo1F5raK5gbRU/QI7K58IuIo+k4+clcvtko\r\n" \
"Ck4RkwdvC8cc0u5mJ8mXJwIDAQABo1AwTjAdBgNVHQ4EFgQUw6RWDo81JEoy+Vnf\r\n" \
"vMTvRsLkZ30wHwYDVR0jBBgwFoAUw6RWDo81JEoy+VnfvMTvRsLkZ30wDAYDVR0T\r\n" \
"BAUwAwEB/zANBgkqhkiG9w0BAQUFAAOCAQEAS2UQtfguKHnlxH9jpfJUyGKFoaTT\r\n" \
"8/XhPW3CW9c++zDrgNq920iQ42Yl9zB58iJ/v0w6n9quTtIta6QD72ssEJtWc2v2\r\n" \
"rwu14WJB5tGhBRagMvtF7p/56KYxib0p3fqjaE3Neer5r8Mgb6oI13tHbf0WT4JM\r\n" \
"GJCLsDoz4ZwvemLISeonZVSVIezs0BDU/TeEK2kIeUDB14FR6fY/U4ovS/v+han8\r\n" \
"NLhWorEpB1p2sgnSPgSVc6ZPHHyjIQOcWdn56vnOf41rLF/zqjD0Sn7YgceAd5OT\r\n" \
"rJ/t7PbiC/sn8SR7+0ATOMh0vRSA9HuuvoDz0adMhoFnba2FwiENfsLlhw==\r\n" \
"-----END CERTIFICATE-----\r\n";
const char alink_cert2[] = ALINK_CERT2;

#elif ALINK_CERT_TEST_NUM == 3
#define ALINK_CERT3 \
"-----BEGIN CERTIFICATE-----\r\n"\
"MIICjjCCAfcCCQCCfnwiKXV0bDANBgkqhkiG9w0BAQUFADBRMQswCQYDVQQGEwJD\r\n" \
"TjERMA8GA1UECAwIWmhlamlhbmcxETAPBgNVBAcMCEhhbmd6aG91MRwwGgYDVQQK\r\n" \
"DBNEZWZhdWx0IENvbXBhbnkgTHRkMB4XDTE2MDIxNjA2MzUwNFoXDTE4MDIxNTA2\r\n" \
"MzUwNFowQjELMAkGA1UEBhMCQ04xETAPBgNVBAgMCFpoZWppYW5nMREwDwYDVQQH\r\n" \
"DAhIYW5nemhvdTENMAsGA1UECgwEdGVzdDCCASIwDQYJKoZIhvcNAQEBBQADggEP\r\n" \
"ADCCAQoCggEBAKZziXoq5qaCt1ZXT/L0nv04GkasbSUBUS+VuWHHfJr621zMd/JO\r\n" \
"thyB/Ju5slSJTYa6x9eh6ZOYaWsTuwg8hNTDmWWUCU59PcqejS6Kuk+ANNhvRdc4\r\n" \
"wlOxfu7GhwCkB+VAW6EUta62CxunVoyiQ5RD/QHCGflsos/G/OgCAMcxFHFzEYLh\r\n" \
"vh1fMc08gV1xNCny9Gtsv/nho56HBrIx+76soBkvR4aMX9eIhfY3OEJz9xwjMXCw\r\n" \
"8P/TnfCuNenQmz2TDh4snbV5OMOBjkV/tLgDO37vq4jAe1Kc6UyKiK2v9MQpNndX\r\n" \
"+umi0iuGB7CHrVjSGoM/kV6uakdnKZMz2fUCAwEAATANBgkqhkiG9w0BAQUFAAOB\r\n" \
"gQBUAQ2OHkItOdd+4hzUKUHcEATKG89L4ChDQT3LHa2mJBqqXx9psElpVFgAHrEw\r\n" \
"V8ykJUav5xCY12Rkz3uqsN0qK/5+0pR+cjbZUOnAi2f30vtrOhvxtEjywPwhPrHk\r\n" \
"sGGY/uqZV+hjpUxThdedoxmiBLW6ZwM7c1w/fqt6URMyBw==\r\n" \
"-----END CERTIFICATE-----\r\n";
const char alink_cert3[] = ALINK_CERT3;

#elif ALINK_CERT_TEST_NUM == 4
#define ALINK_CERT4 \
"-----BEGIN CERTIFICATE-----\n"\
"MIIEoTCCA4mgAwIBAgIJAMQSGd3GuNGVMA0GCSqGSIb3DQEBBQUAMIGRMQswCQYD\n"\
"VQQGEwJDTjERMA8GA1UECBMIWmhlamlhbmcxETAPBgNVBAcTCEhhbmd6aG91MRYw\n"\
"FAYDVQQKEw1BbGliYWJhIEdyb3VwMQ4wDAYDVQQLEwVBbGluazEOMAwGA1UEAxMF\n"\
"QWxpbmsxJDAiBgkqhkiG9w0BCQEWFWFsaW5rQGFsaWJhYmEtaW5jLmNvbTAeFw0x\n"\
"NDA4MjkwMzA5NDhaFw0yNDA4MjYwMzA5NDhaMIGRMQswCQYDVQQGEwJDTjERMA8G\n"\
"A1UECBMIWmhlamlhbmcxETAPBgNVBAcTCEhhbmd6aG91MRYwFAYDVQQKEw1BbGli\n"\
"YWJhIEdyb3VwMQ4wDAYDVQQLEwVBbGluazEOMAwGA1UEAxMFQWxpbmsxJDAiBgkq\n"\
"hkiG9w0BCQEWFWFsaW5rQGFsaWJhYmEtaW5jLmNvbTCCASIwDQYJKoZIhvcNAQEB\n"\
"BQADggEPADCCAQoCggEBAMHr21qKVy3g1GKWdeGQj3by+lN7dMjGoPquLxiJHzEs\n"\
"6auxiAiWez8pFktlekIL7FwK5F7nH1px5W5G8s3cTSqRvunex/Cojw8LbNAStpXy\n"\
"HrqUuDhL3DYF7x87M/7H3lqFuIlucSKNC60Yc03yuIR1I/W0di40rDNeXYuCzXIv\n"\
"yheg+b7zD939HOe+RS878hDW5/p75FY+ChI8GA4dPEQb5fyRrjHAXneo+S8jdnqr\n"\
"SCjHSS7+jI36dyEfZ72rkLNJ3v1WboH02Rchj1fFIfagn+Ij4v0ruejOTIvc/ngD\n"\
"OLZNTUyF4B3EG4IAZRlO12jDECc4Com0yfFQ0IxkNVMCAwEAAaOB+TCB9jAdBgNV\n"\
"HQ4EFgQU9iyOWx+oGSOhdlpHeWMYsHXRwwkwgcYGA1UdIwSBvjCBu4AU9iyOWx+o\n"\
"GSOhdlpHeWMYsHXRwwmhgZekgZQwgZExCzAJBgNVBAYTAkNOMREwDwYDVQQIEwha\n"\
"aGVqaWFuZzERMA8GA1UEBxMISGFuZ3pob3UxFjAUBgNVBAoTDUFsaWJhYmEgR3Jv\n"\
"dXAxDjAMBgNVBAsTBUFsaW5rMQ4wDAYDVQQDEwVBbGluazEkMCIGCSqGSIb3DQEJ\n"\
"ARYVYWxpbmtAYWxpYmFiYS1pbmMuY29tggkAxBIZ3ca40ZUwDAYDVR0TBAUwAwEB\n"\
"/zANBgkqhkiG9w0BAQUFAAOCAQEAO7u7ozEES2TgTepq3ZTk1VD5qh2zhcSLLr+b\n"\
"yDQlkbm0lnah/GOGmpr/Wlr8JSXUJEWhsLcbnG1dhbP72DzFHri8ME4wO8hbeyXU\n"\
"7cFOHmP4DZi+Ia2gyy/GZ66P6L9df89MJzMOt46NYn+A3rI12M0qTJ6GNdUHz2R9\n"\
"VGkahs6bfGQGi0Se24jj4Es+MeAlrG5U0d0wVY5Dt4jpS9wHLupyAiANbj4Ls5x2\n"\
"6cwS4+Q4ErezFMnKiQ5VKc6S88ohYszt82CYMPEqIiZRkCfjsVz/r6vw2DFwN0Ox\n"\
"8Cb9myZwXypcOZbI7M+9W8909Z+TSHW1UlNkyrIsqDGuzE866w==\n"\
"-----END CERTIFICATE-----\n";
const char alink_cert4[] = ALINK_CERT4;
#endif


#ifndef TEST_ALINK_CERT
#define BAIDUIE_CER														\
"-----BEGIN CERTIFICATE-----\r\n"										\
"MIIE0zCCA7ugAwIBAgIQGNrRniZ96LtKIVjNzGs7SjANBgkqhkiG9w0BAQUFADCB\r\n"	\
"yjELMAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQL\r\n"	\
"ExZWZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJp\r\n"	\
"U2lnbiwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxW\r\n"	\
"ZXJpU2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0\r\n"	\
"aG9yaXR5IC0gRzUwHhcNMDYxMTA4MDAwMDAwWhcNMzYwNzE2MjM1OTU5WjCByjEL\r\n"	\
"MAkGA1UEBhMCVVMxFzAVBgNVBAoTDlZlcmlTaWduLCBJbmMuMR8wHQYDVQQLExZW\r\n"	\
"ZXJpU2lnbiBUcnVzdCBOZXR3b3JrMTowOAYDVQQLEzEoYykgMjAwNiBWZXJpU2ln\r\n"	\
"biwgSW5jLiAtIEZvciBhdXRob3JpemVkIHVzZSBvbmx5MUUwQwYDVQQDEzxWZXJp\r\n"	\
"U2lnbiBDbGFzcyAzIFB1YmxpYyBQcmltYXJ5IENlcnRpZmljYXRpb24gQXV0aG9y\r\n"	\
"aXR5IC0gRzUwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCvJAgIKXo1\r\n"	\
"nmAMqudLO07cfLw8RRy7K+D+KQL5VwijZIUVJ/XxrcgxiV0i6CqqpkKzj/i5Vbex\r\n"	\
"t0uz/o9+B1fs70PbZmIVYc9gDaTY3vjgw2IIPVQT60nKWVSFJuUrjxuf6/WhkcIz\r\n"	\
"SdhDY2pSS9KP6HBRTdGJaXvHcPaz3BJ023tdS1bTlr8Vd6Gw9KIl8q8ckmcY5fQG\r\n"	\
"BO+QueQA5N06tRn/Arr0PO7gi+s3i+z016zy9vA9r911kTMZHRxAy3QkGSGT2RT+\r\n"	\
"rCpSx4/VBEnkjWNHiDxpg8v+R70rfk/Fla4OndTRQ8Bnc+MUCH7lP59zuDMKz10/\r\n"	\
"NIeWiu5T6CUVAgMBAAGjgbIwga8wDwYDVR0TAQH/BAUwAwEB/zAOBgNVHQ8BAf8E\r\n"	\
"BAMCAQYwbQYIKwYBBQUHAQwEYTBfoV2gWzBZMFcwVRYJaW1hZ2UvZ2lmMCEwHzAH\r\n"	\
"BgUrDgMCGgQUj+XTGoasjY5rw8+AatRIGCx7GS4wJRYjaHR0cDovL2xvZ28udmVy\r\n"	\
"aXNpZ24uY29tL3ZzbG9nby5naWYwHQYDVR0OBBYEFH/TZafC3ey78DAJ80M5+gKv\r\n"	\
"MzEzMA0GCSqGSIb3DQEBBQUAA4IBAQCTJEowX2LP2BqYLz3q3JktvXf2pXkiOOzE\r\n"	\
"p6B4Eq1iDkVwZMXnl2YtmAl+X6/WzChl8gGqCBpH3vn5fJJaCGkgDdk+bW48DW7Y\r\n"	\
"5gaRQBi5+MHt39tBquCWIMnNZBU4gcmU7qKEKQsTb47bDN0lAtukixlE0kF6BWlK\r\n"	\
"WE9gyn6CagsCqiUXObXbf+eEZSqVir2G3l6BFoMtEMze/aiCKm0oHw0LxOXnGiYZ\r\n"	\
"4fQRbxC1lfznQgUy286dUV4otp6F01vvpX1FQHKOtw5rDgb7MzVIcbidJ4vEZV8N\r\n"	\
"hnacRHr2lVz2XTIIM6RUthg/aFzyQkqFOFSDX9HoLPKsEdao7WNq\r\n"				\
"-----END CERTIFICATE-----\r\n"	
const char baiduie_cer[] = BAIDUIE_CER;
#endif

int ssl_establish_connection()
{
	in_addr_t ip;
	alink_sockaddr_t addr;
	int err, ssl_err;
    
	#ifdef TEST_ALINK_CERT
	#ifdef TEST_IOT
	err = alink_resolve_ip("iot.aliyun.com", &ip);
	#else
	err = alink_resolve_ip("alink.tcp.aliyun.com", &ip);
	#endif
	#else
	err = alink_resolve_ip("www.baidu.com", &ip);
	#endif
	if (err != ALINK_OK) {
		printf("resolve_ip err\n");
		return ALINK_ERR;
	}

	#ifdef TEST_IOT
	printf("connect to %s(%x) %d \n", "iot.aliyun.com", ip, 443);
	#else
	printf("connect to %s(%x) %d \n", "alink.tcp.aliyun.com", ip, 443);
	#endif

	remote_fd = alink_create_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	alink_create_addr(&addr, ip, 443);

	err = connect(remote_fd, (alink_sockaddr *) & addr, sizeof(addr));
	if (err != 0) {
		printf("socket connect err\n");
		return ALINK_ERR;
	}else {
	    printf("socket connect success!\n");
	}

	#ifdef TEST_ALINK_CERT
	#if ALINK_CERT_TEST_NUM == 1
	ssl_conn = (void *)alink_ssl_connect(remote_fd, strlen(alink_cert1),alink_cert1, &ssl_err);	
	#elif ALINK_CERT_TEST_NUM == 2
	ssl_conn = (void *)alink_ssl_connect(remote_fd, strlen(alink_cert2),alink_cert2, &ssl_err);
	#endif
	#else
	ssl_conn = (void *)alink_ssl_connect(remote_fd, strlen(baiduie_cer),baiduie_cer, &ssl_err);
	#endif

	if (ssl_conn == NULL) {
        printf("ssl connect fail!\n");
		return ALINK_ERR;
	}else {
	    printf("ssl connect success!\n");
	}

	return ALINK_OK;
}
void alink_fill_deviceinfo(struct device_info *deviceinfo)
{
    char mac[STR_MAC_LEN] = {0};
    /*fill main device info here */
    printf("%s %d \n",__func__,__LINE__);
    if(alink_get_device_sn())
	    strcpy(deviceinfo->sn, alink_get_device_sn());
	strcpy(deviceinfo->version,DEV_VERSION);
    if(alink_get_device_mac())
	    strcpy(deviceinfo->mac, alink_get_device_mac());
    if(alink_get_device_chipid())
	    strcpy(deviceinfo->cid, alink_get_device_chipid());
    if(alink_get_device_ip()) {
	    strcpy(deviceinfo->ip, alink_get_device_ip());
    }
}
#define OTAURL http://otalink.alicdn.com/ALINKTEST_LIVING_LIGHT_SMARTLED_LUA/1.0.1/alink_demo.bin

char getFile[] = { "GET /ALINKTEST_LIVING_LIGHT_SMARTLED_LUA/1.0.1/alink_demo.bin HTTP/1.1\r\n\
Host:otalink.alicdn.com:80\r\n\
Connection: keep-alive\r\n\r\n" };

//#define filesize 199105 // get file size from ota server.
#define filesize 5000 // get file size from ota server.
// test OTA
void * func3(void * arg)
{
    in_addr_t ip;
    alink_sockaddr_t addr;
    int sockFd = 0;
    char buffer[512];
    static int receive_size = 0;
    int ret = 0;
    
    printf("%s %d \n",__func__,__LINE__);
    if (alink_resolve_ip("otalink.alicdn.com", &ip) != 0) {
        printf("resolve ip fail!\n");        
        alink_exit_thread("exit func3");
        return "fail!";
    }
    alink_create_addr(&addr, ip, 80);
    sockFd = alink_create_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockFd <= 0 || connect(sockFd, (alink_sockaddr *) & addr, sizeof(addr)) != 0) {
	    printf("Socket Connect:failed!\n");        
        alink_exit_thread("exit func3");
        return "fail!";
    }
    if (alink_send(sockFd, getFile, strlen(getFile), 0) < 0) {
	    printf("send: failed !\n");
	    goto END;
    }
    
    while (1) {
	    int len = 0;
	    memset(buffer, 0, 512);
	    len = alink_recv(sockFd, buffer, 512, 0);
	    if (len <= 0) {
		    printf("recv error!!!!\n");
            ret = -1;
		    goto END;
	    }
        receive_size = receive_size + len;
        printf("len =%d \n",len);

        // remove http head
        // do ...


        // write bin data to flash
        if(alink_fw_download(buffer,len)) {
            printf("write flash fail!\n");
            ret = -1;
            break;
        }

        if(receive_size >= filesize) {
            if(alink_fw_check()) {
                ret = -1;
                break;
            }
            if(alink_fw_upgrade()) {
                ret = -1;
                break;
            }
            break;
        }
    }
  END:
    if (sockFd) {
    	alink_socket_close(sockFd);
    }

    printf("%s %d \n",__func__,__LINE__);
    
    alink_exit_thread("exit func3");
    //return ret;
}

#define CONFIG_ADDR 0X100000 // FLASH ADDR

// test system api
void * func1(void * arg)
{
    struct device_info main_dev;
    pthread_t threadid_2;
    
    printf("creat thread1 success!\n");
    // get device info
    if(! alink_read_flash((char*)&main_dev,sizeof(struct device_info)) ) {
        memset(&main_dev, 0, sizeof(main_dev));
	    alink_fill_deviceinfo(&main_dev);
        alink_write_flash((char *)&main_dev,sizeof(struct device_info));
    }

    // get system info
    printf("alink_get_free_heap_size %d\n",alink_get_free_heap_size());
    printf("alink_get_os_time_sec() - %ld \n",alink_get_os_time_sec());   
    alink_sleep(5000); // millisecond 
    printf("alink_get_os_time_sec() - %ld \n",alink_get_os_time_sec());

    // device wifi connect AP
    // zconfig
    // mtk_smart_connect();
    alink_wait_network_up();

    printf("%d \n",alink_get_wifi_station_rssi());

    // test tls
    if (!!alink_create_thread(&threadid_2, "threadid_2", (void *)func2, NULL, 0x1000))
        return "fail!";
    
    //pthread_join(threadid_2,NULL);
    
    // ssl connect
    
    alink_exit_thread("exit func1");
    
   // return NULL;
}

// pthread_cond_wait
// alink_pthread_cond_timedwait

int recv_buf_pos;
#define recv_buf_len 2048
char recv_buf[recv_buf_len];
int handler_socket_event( )
{
	int max_fd;
	fd_set rfds;
	alink_timeval_t t;

	FD_ZERO(&rfds);

	max_fd = remote_fd;
	max_fd += 1;
	FD_SET(remote_fd, &rfds);

	t.tv_sec = 5;
	t.tv_usec = 0;

	int err = select(max_fd, &rfds, NULL, NULL, &t);
	printf("select out");
	if (!err) {
		/* recv nothing in t times */
		printf("recv nothing \n");
	} else if (err < 0) {
		printf("select return err:%d\n", err);
		return ALINK_ERR;
	}
	/*recv wlan data using remote client fd */
	int len = 0;

	if (FD_ISSET(remote_fd, &rfds)) {
		do {
			len = alink_ssl_recv(ssl_conn,recv_buf + recv_buf_pos, recv_buf_len - recv_buf_pos);
			if (len < 0) {
				printf("Remote client closed, fd: %d", remote_fd);
				return ALINK_ERR;
			} 

			recv_buf_pos += len;
            if(NULL != strstr(recv_buf,"app success"))
			    printf("process_received_buf %d %s\n", len,recv_buf);
		} while (ssl_conn && alink_ssl_pending(ssl_conn));
	}

	return ALINK_OK;
}

static int ssl_flag = 10;
// test tls
void * func2(void * arg)
{
    while(ssl_flag--) {
        printf("ssl_flag =%d\n",ssl_flag);
		if (remote_fd == -1) {
			if (ssl_establish_connection(ssl_conn) != ALINK_OK)
				goto ReConnWithDelay;
		}
        // ssl send:
        alink_ssl_send(ssl_conn,ssl_request, strlen(ssl_request));
        
		int ret = handler_socket_event(ssl_conn);
		if (ret <= ALINK_ERR) {
			goto ReConnWithDelay;
		} 

		alink_sleep(0);	/* give a chance to schedule out */
		continue;

        ReConnWithDelay:
		printf("ReConnWithDelay...\n");
		if (ssl_conn != NULL) {
			alink_ssl_force_close(ssl_conn);
			ssl_conn = NULL;
		}

		if (remote_fd >= 0) {
			close(remote_fd);
			remote_fd = -1;
		}
		alink_wait_network_up();
		alink_sleep(100);
    }
      
    // test OTA
    pthread_t threadid_3;
    if (!!alink_create_thread(&threadid_3, "threadid_3", (void *)func3, NULL, 0x800))
        return -1;
    //pthread_join(threadid_3,NULL);
    
    alink_exit_thread("exit func2");
}

//int main()
int alink_demo()
{
    pthread_t threadid_1;
    printf("alink demo 1.0\n");
    //test thread
    if (!!alink_create_thread(&threadid_1, "threadid_1", (void *)func1, NULL, 0x800))
        return -1;
    //pthread_join(threadid_1,NULL);

    return 0;
}
