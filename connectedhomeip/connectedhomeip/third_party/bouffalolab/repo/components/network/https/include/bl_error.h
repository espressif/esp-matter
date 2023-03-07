
#ifndef __BL_ERROR_H__
#define __BL_ERROR_H__

#define BL_TCP_NO_ERROR                 (0)      /**< 无错误 */
#define BL_TCP_ARG_INVALID             (-2)      /**< 无效的参数 */
#define BL_TCP_CREATE_CONNECT_ERR     (-21)      /**< 创建TCP连接错误 */
#define BL_TCP_SEND_ERR               (-23)      /**< TCP发送失败 */
#define BL_TCP_READ_ERR               (-25)      /**< TCP读取失败 */
#define BL_TCP_CONNECT_TIMEOUT        (-26)      /**< TCP连接超时 */
#define BL_TCP_CONNECT_ERR            (-27)      /**< TCP连接失败 */
#define BL_TCP_CONNECTING             (-28)      /**< TCP连接中    */
#define BL_TCP_READ_INCOMPLETED       (-29)      /**< TCP读包不完整 */
#define BL_TCP_CREATE_SERVER_ERR      (-31)      /**< 创建TCP服务错误 */
#define BL_TCP_SERVER_WAIT_CONNECT    (-32)      /**< tcp等待客户端连接 */
#define BL_TCP_DNS_PARSING            (-35)      /**< DNS解析中 */
#define BL_TCP_DNS_PARSE_ERR          (-36)      /**< DNS解析失败 */
#define BL_TCP_DNS_PARSE_TIMEOUT      (-37)      /**< DNS解析超时 */

#endif /* #ifndef __BL_ERROR_H__ */
